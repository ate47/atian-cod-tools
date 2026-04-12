#include <ui_includes.hpp>
#include "ExeDllInjectorWidget.h"
#include <QFileDialog>
#include <QListWidgetItem>
#include <QVariant>
#include <acts_api/config.h>
#include <utils/memapi.hpp>

static struct InjectGame {
    const char* name;
    const char* exeName;
    const char* dllName;
} injects[]{
    { "None", "", "" },
    { "Black Ops 3", "BlackOps3.exe", "acts-bo3.dll" },
    { "COD2020", "COD2020.exe", "acts-cod2020.dll" },
    { "Black Ops ColdWar", "BlackOpsColdWar.exe", "acts-bocw.dll" },
};

ExeDllInjectorWidget::ExeDllInjectorWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	setLayout(ui.commonLayout);
	injectorTimer = new QTimer(this);

    ui.dllEdit->setText(ActsAPIConfig_GetString("ui.autoinjector.dll", ""));
    ui.exeEdit->setText(ActsAPIConfig_GetString("ui.autoinjector.exe", ""));

    for (size_t i = 0; i < ARRAYSIZE(injects); i++) {
        InjectGame& game{ injects[i] };

        QListWidgetItem* item{ new QListWidgetItem(game.name) };
        item->setData(Qt::UserRole, i);
        ui.configsList->addItem(item);
    }

    connect(ui.dllOpen, &QPushButton::clicked, this, [this] {
        QString path = QFileDialog::getOpenFileName(
            this,
            tr("Dynamic Link Library"),
            QString(),
            tr("Dynamic Link Library (*.dll)")
        );

        if (!path.isEmpty()) {
            ui.dllEdit->setText(path);
        }
    });
    connect(ui.exeOpen, &QPushButton::clicked, this, [this] {
        QString path = QFileDialog::getOpenFileName(
            this,
            tr("Executable File"),
            QString(),
            tr("Executable File (*.exe)")
        );


        if (path.isEmpty()) {
            return;
        }
        std::filesystem::path pp{ path.toStdString() };
        pp = pp.filename();
        std::string stdpp{ pp.string() };

        ui.exeEdit->setText(stdpp.data());
    });
    connect(ui.configsList, &QListWidget::currentItemChanged, [this](QListWidgetItem* current, QListWidgetItem* prev) {
        InjectGame& game{ injects[current->data(Qt::UserRole).value<size_t>()] };
        ui.dllEdit->setText(game.dllName);
        ui.exeEdit->setText(game.exeName);
    });

    connect(ui.dllEdit, &QLineEdit::textChanged, [this](const QString& text) {
        std::string val{ text.toStdString() };
        ActsAPIConfig_SetString("ui.autoinjector.dll", val.data());
        ActsAPIConfig_SaveConfig();
    });

    connect(ui.exeEdit, &QLineEdit::textChanged, [this](const QString& text) {
        std::string val{ text.toStdString() };
        ActsAPIConfig_SetString("ui.autoinjector.exe", val.data());
        ActsAPIConfig_SaveConfig();
    });

	connect(injectorTimer, &QTimer::timeout, this, [this]() {
        QString qdll{ ui.dllEdit->text() };
        QString qexe{ ui.exeEdit->text() };

        if (qdll.isEmpty() || qexe.isEmpty()) {
            return; // nothing to run
        }

        QByteArray bqdll{ qdll.toUtf8() };
        QByteArray bqexe{ qexe.toUtf8() };
        const char* dll{ bqdll.constData() };
        const char* exe{ bqexe.constData() };


        static std::unordered_set<DWORD> loadedPids{};

        Process game{ exe };

        if (!game || loadedPids.contains(game.GetProcessId())) {
            return; // no new exe
        }

        if (!game.Open()) {
            ui.logLabel->setText(QString::asprintf("Failed to open game %s", exe));
            LOG_ERROR("Failed to open game {}", exe);
            return;
        }

        std::filesystem::path dllPath{ dll };
        if (!dllPath.is_absolute()) {
            dllPath = utils::GetProgDir() / dll;
        }

        std::string dllPathStr{ dllPath.string() };

        LOG_DEBUG("Injecting {}...", dllPathStr);

        game.ComputeModules();

        if (game.LoadDll(dllPathStr.c_str())) {
            ui.logLabel->setText(QString::asprintf("Injected %s", dll));
            loadedPids.insert(game.GetProcessId());
        }
        else {
            ui.logLabel->setText(QString::asprintf("Failed to inject %s", dll));
            LOG_ERROR("Failed to inject {}", dll);
        }
	});
	injectorTimer->start(1000);
}

ExeDllInjectorWidget::~ExeDllInjectorWidget() {
	injectorTimer->stop();
}

