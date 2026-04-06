#pragma once

#include <QWidget>
#include <MainWindow.h>
#include "ui_InfoWidget.h"

class InfoWidget : public QWidget
{
	Q_OBJECT

public:
	InfoWidget(QMainWindow* main, QWidget *parent = nullptr);
	~InfoWidget();

private:
	QMainWindow* main;
	Ui::InfoWidgetClass ui;
};

