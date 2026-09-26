#include <ui_includes.hpp>
#include "FastFileAssetPoolConfigWidget.h"
#include "FastFileAssetPoolWidget.h"
#include <config_ui.hpp>
#include <MainWindow.h>
#include <acts_api/fastfile_loader.h>

#include <QCoreApplication>
#include <QCheckBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <widgets/common/UI3MdiArea.h>

UI_CONFIG_VAL(cfgUiFFPath, "ui.ff.path", QString(""), "fastfile path");
UI_CONFIG_VAL(cfgUiFFHandler, "ui.ff.handler", QString(""), "fastfile handler");
UI_CONFIG_VAL(cfgUiFFOutput, "ui.ff.output", QString("output_ff"), "fastfile output");
UI_CONFIG_VAL(cfgUiFFPatch, "ui.ff.patch", true, "fastfile apply patch");

FastFileAssetPoolConfigWidget::FastFileAssetPoolConfigWidget(QWidget* parent) : QWidget(parent) {
    setObjectName("FastFileAssetPoolConfigWidget");
    resize(600, 400);
    setWindowTitle(
        QCoreApplication::translate("FastFileAssetPoolConfigWidget", "FastFile Asset Pool Configuration", nullptr)
    );

    auto* layout{ new QVBoxLayout(this) };
    auto* formLayout{ new QFormLayout() };

    auto* gamePathLayout{ new QHBoxLayout() };
    gamePathEdit = new QLineEdit(this);
    gamePathEdit->setMinimumWidth(300);
    auto* gamePathButton{ new QPushButton("Browse...", this) };
    gamePathLayout->addWidget(gamePathEdit);
    gamePathLayout->addWidget(gamePathButton);
    formLayout->addRow("Game path", gamePathLayout);

    handlerEdit = new QComboBox(this);
    handlerEdit->setEditable(false);
    handlerEdit->setMinimumWidth(300);
    ActsStatus handlersStatus{ ActsAPIFastFile_ListHandlers(
        [](const ActsAPIFastFile_FastFileHandlerEntry* entry, void* ud) {
            if (entry && entry->id) {
                static_cast<QComboBox*>(ud)->addItem(entry->description);
            }
        },
        handlerEdit
    ) };
    if (ACTS_NOT_OK(handlersStatus)) {
        QMessageBox::warning(this, "FastFile handlers", ActsGetAPILastMessage());
    }
    formLayout->addRow("Handler", handlerEdit);

    auto* outputPathLayout{ new QHBoxLayout() };
    outputPathEdit = new QLineEdit(this);
    outputPathEdit->setMinimumWidth(300);
    auto* outputPathButton{ new QPushButton("Browse...", this) };
    outputPathLayout->addWidget(outputPathEdit);
    outputPathLayout->addWidget(outputPathButton);
    formLayout->addRow("Output path", outputPathLayout);

    patchCheck = new QCheckBox("Patch", this);
    formLayout->addRow(patchCheck);
    layout->addLayout(formLayout);

    auto* createButton{ new QPushButton("Create asset pool", this) };
    layout->addWidget(createButton);
    layout->addStretch();
    GetMainWindow()->RequiresInitialization(createButton);

    connect(gamePathButton, &QPushButton::clicked, this, &FastFileAssetPoolConfigWidget::BrowseGamePath);
    connect(outputPathButton, &QPushButton::clicked, this, &FastFileAssetPoolConfigWidget::BrowseOutputPath);
    connect(createButton, &QPushButton::clicked, this, &FastFileAssetPoolConfigWidget::CreateAssetPool);

    cfgUiFFPath.Bind(gamePathEdit);
    cfgUiFFHandler.Bind(handlerEdit);
    cfgUiFFOutput.Bind(outputPathEdit);
    cfgUiFFPatch.Bind(patchCheck);
}

void FastFileAssetPoolConfigWidget::BrowseGamePath() {
    QString path{ QFileDialog::getExistingDirectory(this, "Select game path", cfgUiFFPath.Get()) };
    if (!path.isEmpty()) {
        cfgUiFFPath.Set(path);
    }
}

void FastFileAssetPoolConfigWidget::BrowseOutputPath() {
    QString path{ QFileDialog::getExistingDirectory(this, "Select output path", cfgUiFFOutput.Get()) };
    if (!path.isEmpty()) {
        cfgUiFFOutput.Set(path);
    }
}

void FastFileAssetPoolConfigWidget::CreateAssetPool() {
    std::string path{ cfgUiFFPath.Get().toStdString() };
    std::string handler{ cfgUiFFHandler.Get().toStdString() };
    std::string output{ cfgUiFFOutput.Get().toStdString() };

    if (path.empty() || handler.empty() || output.empty()) {
        QMessageBox::warning(this, "Invalid configuration", "Game path, handler, and output path are required.");
        return;
    }

    ActsAPIFastFile_AssetPoolOptions opts{};
    opts.structSize = sizeof(opts);
    opts.gamePath = path.c_str();
    opts.handler = handler.c_str();
    opts.outputPath = output.c_str();
    opts.patch = cfgUiFFPatch.Get();

    ActsHandle ctx{ ActsAPIFastFile_CreateAssetPoolContext(&opts) };

    if (!IS_ACTS_HANDLE_VALID(ctx)) {
        QMessageBox::warning(
            this,
            "Can't create handler",
            QString::asprintf("Can't create handler %s", ActsGetAPILastMessage())
        );
        return;
    }

    if (ActsAPIFastFile_AssetPoolInit(ctx) != ACTS_STATUS_OK) {
        QMessageBox::warning(
            this,
            "Can't init handler",
            QString::asprintf("Can't init handler %s", ActsGetAPILastMessage())
        );
        ActsAPICloseHandle(ctx);
        return;
    }

    UI3MdiArea* mdi{ GetMainArea() };
    // Pass ownership of the initialized context to the widget; do not close ctx here.
    FastFileAssetPoolWidget* widget{ new FastFileAssetPoolWidget(ctx, mdi) };
    mdi->AddSubWindow(widget);

    // Remove and delete this configuration widget
    mdi->removeSubWindow(parentWidget());
}

ADD_UI_TOOL(FastFileAssetPoolConfigWidget, "FastFile Asset Pool", "FastFile");
