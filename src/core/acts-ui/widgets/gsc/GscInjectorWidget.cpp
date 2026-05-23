#include <ui_includes.hpp>
#include "GscInjectorWidget.h"
#include <acts_api/config.h>
#include <acts_api/internal.h>
#include <QFileDialog>

namespace {
	const char* defaultHooks[]{
		"scripts\\zm_common\\load.gsc",
		"scripts\\mp_common\\bb.gsc",
		"scripts\\core_common\\load_shared.gsc"
	};
}

GscInjectorWidget::GscInjectorWidget(QWidget *parent)
	: QWidget(parent) {
	ui.setupUi(this);
	ui.mainLayout->setAlignment(Qt::AlignTop);
	for (const char* hook : defaultHooks) {
		ui.hookBox->addItem(hook, hook);
	}
	ui.hookBox->setEditable(true);


	connect(ui.gsccPathButton, &QPushButton::clicked, this, [this]() {
		QString path = QFileDialog::getOpenFileName(
			this,
			tr("Compiled GSC File"),
			QString(),
			tr("Compiled GSC file (*.gscc *.cscc);;All Files (*.*)")
		);
		if (!path.isEmpty()) {
			ui.gsccPathEdit->setText(path);
		}
	});
	connect(ui.decompButton, &QPushButton::clicked, this, [this]() {
		QByteArray gscPath{ ui.gsccPathEdit->text().toUtf8() };
		tools::ui::OpenMenu("GscDecompiler", gscPath.constData());
	});
	connect(ui.gsccPathEdit, &QLineEdit::textChanged, this, [this]() { UpdateConfig(); });
	connect(ui.hookBox, &QComboBox::currentTextChanged, this, [this]() { UpdateConfig(); });
	connect(ui.ps4IpEdit, &QLineEdit::textChanged, this, [this]() { UpdateConfig(); });

	ui.logLabel->setText("");

	connect(ui.injectButton, &QPushButton::clicked, this, [this]() {
		QByteArray gscPath{ ui.gsccPathEdit->text().toUtf8() };
		QByteArray hookPath{ ui.hookBox->currentText().toUtf8() };

		ui.logLabel->setText(ActsAPIGscInjection_InjectPC(gscPath.constData(), hookPath.constData()));
	});

	connect(ui.injectPS4Button, &QPushButton::clicked, this, [this]() {
		QByteArray gscPath{ ui.gsccPathEdit->text().toUtf8() };
		QByteArray hookPath{ ui.hookBox->currentText().toUtf8() };
		QByteArray ps4Ip{ ui.ps4IpEdit->text().toUtf8() };

		ui.logLabel->setText(ActsAPIGscInjection_InjectPS4(gscPath.constData(), hookPath.constData(), ps4Ip.constData()));
	});
	connect(ui.patchEEButton, &QPushButton::clicked, this, [this]() {
		ui.logLabel->setText(ActsAPIGscInjection_PatchEE());
	});


	const char* injGsc{ ActsAPIConfig_GetString("ui.injector.path", "") };
	const char* injHook{ ActsAPIConfig_GetString("ui.injector.hook", defaultHooks[0]) };
	const char* injPS4{ ActsAPIConfig_GetString("ui.ps4.ipd", "") };
	ui.gsccPathEdit->setText(injGsc);
	ui.hookBox->setCurrentText(injHook);
	ui.ps4IpEdit->setText(injPS4);

	setLayout(ui.mainLayout);
}

GscInjectorWidget::~GscInjectorWidget() {}

void GscInjectorWidget::UpdateConfig() {
	QByteArray gscPath{ ui.gsccPathEdit->text().toUtf8() };
	QByteArray hookPath{ ui.hookBox->currentText().toUtf8() };
	QByteArray ps4Ip{ ui.ps4IpEdit->text().toUtf8() };

	ActsAPIConfig_SetString("ui.injector.path", gscPath.constData());
	ActsAPIConfig_SetString("ui.injector.hook", hookPath.constData());
	ActsAPIConfig_SetString("ui.ps4.ipd", ps4Ip.constData());

	bool hasGscPath = !gscPath.isEmpty();
	bool hasHook = !hookPath.isEmpty();
	bool hasPS4Ip = !ps4Ip.isEmpty();

	ui.decompButton->setEnabled(hasGscPath);
	ui.injectButton->setEnabled(hasGscPath && hasHook);
	ui.injectPS4Button->setEnabled(hasGscPath && hasHook && hasPS4Ip);

	ActsAPIConfig_SaveConfig();
}

ADD_UI_TOOL(GscInjectorWidget, "GSC Injector", "GSC");