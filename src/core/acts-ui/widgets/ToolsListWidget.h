#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QCheckBox>

class ToolsListWidget : public QWidget
{
	Q_OBJECT

public:
	ToolsListWidget(QWidget* parent = nullptr);
	~ToolsListWidget();

private:
	QTableWidget* table;
};

