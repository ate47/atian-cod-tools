#include <ui_includes.hpp>
#include <core/actsinfo.hpp>
#include <core/updater_endpoint.hpp>
#include <mstch/mstch.hpp>
#include <acts_api/version.h>
#include "InfoWidget.h"
#include <QFile>
#include <QTextBrowser>
#include <QCoreApplication>

InfoWidget::InfoWidget(QWidget *parent) : QWidget(parent) {
	setObjectName("InfoWidgetClass");
	resize(600, 400);
	QSizePolicy sizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	sizePolicy.setHeightForWidth(hasHeightForWidth());
	setSizePolicy(sizePolicy);
	QTextBrowser* text{ new QTextBrowser(this) };
	text->setObjectName("text");
	text->setGeometry(QRect(0, 0, 600, 400));
	setWindowTitle(QCoreApplication::translate("InfoWidgetClass", "About", nullptr));

	QMetaObject::connectSlotsByName(this);

	setFixedSize(600, 400);

	text->setAcceptRichText(true);
	text->setTextInteractionFlags(Qt::TextInteractionFlag::TextBrowserInteraction);
	text->setOpenExternalLinks(true);

	QFile aboutTemplate{ ":/templates/about.mustache" };

	QString aboutData;
	if (aboutTemplate.open(QIODevice::ReadOnly | QIODevice::Text)) {
		aboutData = QString::fromUtf8(aboutTemplate.readAll());
	}
	else {
		aboutData = "missing about information";
	}
	aboutTemplate.close();

	std::string data{ aboutData.toStdString() };


	mstch::map templateData{};

	templateData["version"] = core::actsinfo::VERSION;
	templateData["build"] = std::format("0x{:x}", core::actsinfo::BUILD_VERSION_ID);

	templateData["api_version"] = ActsAPIVersion_GetVersion();
	templateData["api_build"] = std::format("0x{:x}", ActsAPIVersion_GetBuildVersion());

#ifdef GPL_BUILD
	templateData["license_type"] = "GNU General Public License v3";
	templateData["license_url"] = std::format("{}/blob/main/licenses/gpl3.md", core::updater::PROJECT_URL);
#else
	templateData["license_type"] = "MIT License";
	templateData["license_url"] = std::format("{}/blob/main/licenses/mit.md", core::updater::PROJECT_URL);
#endif

	templateData["qt_version"] = QT_VERSION_STR;
	templateData["project"] = core::updater::PROJECT;
	templateData["project_url"] = core::updater::PROJECT_URL;


	mstch::node context{ mstch::map_wrapper{ templateData } };
	data = mstch::render(data, context);
	aboutData = data.data();
	text->setHtml(aboutData);
}

InfoWidget::~InfoWidget() {}

