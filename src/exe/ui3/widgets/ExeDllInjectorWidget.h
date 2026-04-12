#pragma once

#include <QWidget>
#include <QTimer>
#include "ui_ExeDllInjectorWidget.h"

class ExeDllInjectorWidget : public QWidget
{
	Q_OBJECT

public:
	ExeDllInjectorWidget(QWidget *parent = nullptr);
	~ExeDllInjectorWidget();

private:
	Ui::ExeDllInjectorWidgetClass ui;
	QTimer* injectorTimer;
};

