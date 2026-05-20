#pragma once

#include <QWidget>
#include "ui_ExeDumperWidget.h"

class ExeDumperWidget : public QWidget
{
	Q_OBJECT

public:
	ExeDumperWidget(QWidget *parent = nullptr);
	~ExeDumperWidget();

	void LoadFile(const QString& path);
private:
	Ui::ExeDumperWidgetClass ui;
};

