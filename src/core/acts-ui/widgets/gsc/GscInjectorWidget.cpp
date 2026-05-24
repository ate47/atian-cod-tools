#include <ui_includes.hpp>
#include "GscInjectorWidget.h"
#include <acts_api/config.h>
#include <acts_api/internal.h>
#include <config_ui.hpp>
#include <QFileDialog>

namespace {
	const char* defaultHooks[]{
		"scripts\\zm_common\\load.gsc",
		"scripts\\mp_common\\bb.gsc",
		"scripts\\core_common\\load_shared.gsc"
	};
}

const char* injGsc{ ActsAPIConfig_GetString("", "") };
const char* injHook{ ActsAPIConfig_GetString("", defaultHooks[0]) };
const char* injPS4{ ActsAPIConfig_GetString("", "") };


UI_CONFIG_VAL(cfgUiInjectorPath, "ui.injector.path", QString(""), "GSC Injector Path");
UI_CONFIG_VAL(cfgUiInjectorHook, "ui.injector.hook", QString("scripts\\zm_common\\load.gsc"), "GSC Injector Hook");
UI_CONFIG_VAL(cfgUiPs4Ipd, "ui.ps4.ipd", QString(""), "PS4 IP");

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
		ui3::tools::OpenMenu("GscDecompiler", gscPath.constData());
	});

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

	cfgUiInjectorPath.OnUpdate(this, [this] { UpdateConfig(); });
	cfgUiInjectorHook.OnUpdate(this, [this] { UpdateConfig(); });
	cfgUiPs4Ipd.OnUpdate(this, [this] { UpdateConfig(); });

	cfgUiInjectorPath.Bind(ui.gsccPathEdit);
	cfgUiInjectorHook.Bind(ui.hookBox);
	cfgUiPs4Ipd.Bind(ui.ps4IpEdit);

	setLayout(ui.mainLayout);
}

GscInjectorWidget::~GscInjectorWidget() {}

void GscInjectorWidget::UpdateConfig() {
	QString gscPath{ cfgUiInjectorPath };
	QString hookPath{ cfgUiInjectorHook };
	QString ps4Ip{ cfgUiPs4Ipd };

	bool hasGscPath = !gscPath.isEmpty();
	bool hasHook = !hookPath.isEmpty();
	bool hasPS4Ip = !ps4Ip.isEmpty();

	ui.decompButton->setEnabled(hasGscPath);
	ui.injectButton->setEnabled(hasGscPath && hasHook);
	ui.injectPS4Button->setEnabled(hasGscPath && hasHook && hasPS4Ip);
}

ADD_UI_TOOL(GscInjectorWidget, "GSC Injector", "GSC");