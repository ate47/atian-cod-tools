#include <ui_includes.hpp>
#include <config_ui.hpp>
#include "BO6PS4Widget.h"
#include <acts_api/internal.h>
#include <QFileDialog>

UI_CONFIG_VAL(cfgUiBo6Path, "ui.bo6.path", QString(""), "BO6BO6 GSC Path");
UI_CONFIG_VAL(cfgUiBo6Cbuf, "ui.bo6.cbuf", QString(""), "BO6BO6 Command");
UI_CONFIG_VAL(cfgUiPs4Ipd, "ui.ps4.ipd", QString(""), "PS4 IP");

BO6PS4Widget::BO6PS4Widget(QWidget *parent)
	: QWidget(parent) {
	ui.setupUi(this);
	ui.mainLayout->setAlignment(Qt::AlignTop);

	cfgUiPs4Ipd.Bind(ui.ps4ipEdit);
	cfgUiBo6Path.Bind(ui.gscPathEdit);
	cfgUiBo6Cbuf.Bind(ui.cbuffEdit);

	connect(ui.openPathButton, &QPushButton::clicked, this, [this]() {
		QString path = QFileDialog::getOpenFileName(
			this,
			tr("Compiled GSC File"),
			QString(),
			tr("Compiled GSC file (*.gscc);;All Files (*.*)")
		);
		if (!path.isEmpty()) {
			ui.gscPathEdit->setText(path);
		}
	});

	connect(ui.injectButton, &QPushButton::clicked, this, [this]() {
		QByteArray gsc{ cfgUiBo6Path.Get().toUtf8() };
		QByteArray ps4{ cfgUiPs4Ipd.Get().toUtf8() };
		ui.logLabel->setText(ActsAPIBO6PS4_InjectScript(gsc.constData(), ps4.constData()));
	});

	connect(ui.cbuffButton, &QPushButton::clicked, this, [this]() {
		QByteArray cmd{ cfgUiBo6Cbuf.Get().toUtf8() };
		QByteArray ps4{ cfgUiPs4Ipd.Get().toUtf8() };
		ui.logLabel->setText(ActsAPIBO6PS4_Cbuff(cmd.constData(), ps4.constData()));
	});

	setLayout(ui.mainLayout);
}

BO6PS4Widget::~BO6PS4Widget() {}


ADD_UI_TOOL(BO6PS4Widget, "PS4 Tool", "Games/Black Ops 6");