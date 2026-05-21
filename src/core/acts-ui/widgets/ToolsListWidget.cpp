#include <ui_includes.hpp>
#include "ToolsListWidget.h"
#include <QVBoxLayout>
#include <QStandardItem>


ToolsListWidget::ToolsListWidget(QWidget* parent) : QWidget(parent) {
	setObjectName("ToolsListWidgetClass");
	resize(600, 400);
	setWindowTitle("Tools List");
	QMetaObject::connectSlotsByName(this);

	QVBoxLayout* layout{ new QVBoxLayout() };

	table = new QTableWidget();

	layout->addWidget(table);
	layout->setContentsMargins(0, 0, 0, 0);
	setLayout(layout);
	
	std::vector<tools::ui::AbstractUITool*>& tools{ tools::ui::GetTools() };

	table->setRowCount((int)tools.size());
	table->setColumnCount(5);
	QStringList headers{ "Name", "Path", "Extensions", "Allow Duplicates", "Needs Initialization" };
	table->setHorizontalHeaderLabels(headers);
	for (size_t i = 0; i < tools.size(); i++) {
		tools::ui::AbstractUITool* tool{ tools[i] };

		QTableWidgetItem* nameItem{ new QTableWidgetItem(tool->name ? tool->name : "") };
		QTableWidgetItem* pathItem{ new QTableWidgetItem(tool->path ? tool->path : "") };
		QTableWidgetItem* extItem{ new QTableWidgetItem(tool->extensions ? tool->extensions : "") };
		QTableWidgetItem* allowDupeItem{ new QTableWidgetItem(tool->allowDupe ? "true" : "false") };
		QTableWidgetItem* initItem{ new QTableWidgetItem(tool->needsInitialization ? "true" : "false") };

		table->setItem((int)i, 0, nameItem);
		table->setItem((int)i, 1, pathItem);
		table->setItem((int)i, 2, extItem);
		table->setItem((int)i, 3, allowDupeItem);
		table->setItem((int)i, 4, initItem);
	}

	table->resizeColumnsToContents();
}

ToolsListWidget::~ToolsListWidget() {}

ADD_UI_TOOL(ToolsListWidget, "Tools List", "Utilities/Developper");