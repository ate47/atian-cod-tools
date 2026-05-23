#pragma once

#include <QWidget>
#include "ui_GscInjectorWidget.h"

class GscInjectorWidget : public QWidget
{
	Q_OBJECT

public:
	GscInjectorWidget(QWidget *parent = nullptr);
	~GscInjectorWidget();

private:
	void UpdateConfig();

	Ui::GscInjectorWidgetClass ui;
};

