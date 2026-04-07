#include <ui_includes.hpp>
#include "ExeDumperWidget.h"
#include <QFileDialog>
#include <acts_api/process.hpp>

ExeDumperWidget::ExeDumperWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	setLayout(ui.verticalLayout);

    ui.returnLabel->setText(""); // remove placeholder
    ui.dumpButton->setEnabled(false); // activated using link

    connect(ui.execPathEdit, &QLineEdit::textChanged, this, [this](const QString& text) {
        ui.dumpButton->setEnabled(!text.isEmpty());
    });

    connect(ui.execPathButton, &QPushButton::clicked, this, [this] {
        QString path = QFileDialog::getOpenFileName(
            this,
            tr("Executable File"),
            QString(),
            tr("Executable File (*.exe)")
        );

        if (!path.isEmpty()) {
            ui.execPathEdit->setText(path);
        }
    });

    connect(ui.dumpPathButton, &QPushButton::clicked, this, [this] {
        QString path = QFileDialog::getSaveFileName(
            this,
            tr("Dump File"),
            QString(),
            tr("Executable File (*.exe)")
        );

        if (!path.isEmpty()) {
            ui.dumpPathEdit->setText(path);
        }
    });

    connect(ui.dumpButton, &QPushButton::clicked, this, [this] {
        QString qexecPath{ ui.execPathEdit->text() };
        QString qdumpPath{ ui.dumpPathEdit->text() };

        QByteArray execPath{ qexecPath.toUtf8() };
        QByteArray dumpPath{ qdumpPath.toUtf8() };

        bool rebuildIAT{ ui.rebuildIATCheck->isChecked() };

        char message[0x400]{};
        acts::api::DumpProcessOption options{};
        options.rebuildIAT = rebuildIAT;
        options.outMessage = message;
        options.outMessageSize = sizeof(message);

        ActsAPIProcess().DumpProcessExe(execPath.constData(), dumpPath.constData(), &options);

        ui.returnLabel->setText(message);
    });
}

ExeDumperWidget::~ExeDumperWidget()
{}

