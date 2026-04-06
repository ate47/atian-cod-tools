#include <ui_includes.hpp>
#include <core/actsinfo.hpp>
#include <core/updater_endpoint.hpp>
#include <mstch/mstch.hpp>
#include <QFile>
#include "InfoWidget.h"

InfoWidget::InfoWidget(QMainWindow* main, QWidget *parent)
	: main(main), QWidget(parent) {
	ui.setupUi(this);
	setFixedSize(600, 400);

	ui.text->setAcceptRichText(true);
	ui.text->setTextInteractionFlags(Qt::TextInteractionFlag::TextBrowserInteraction);
	ui.text->setOpenExternalLinks(true);

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
	ui.text->setHtml(aboutData);
}

InfoWidget::~InfoWidget() {}

