#pragma once

#include <QWidget>
#include <MainWindow.h>
#include "ui_T89ErrorWidget.h"

class T89ErrorWidget : public QWidget
{
	Q_OBJECT

public:
	T89ErrorWidget(QMainWindow* main, QWidget* parent = nullptr);
	~T89ErrorWidget();

private:
	Ui::T89ErrorWidgetClass ui;
	QMainWindow* main;
};

