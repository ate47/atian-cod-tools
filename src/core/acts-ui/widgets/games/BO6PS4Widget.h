#pragma once

#include <QWidget>
#include "ui_BO6PS4Widget.h"

class BO6PS4Widget : public QWidget
{
	Q_OBJECT

public:
	BO6PS4Widget(QWidget *parent = nullptr);
	~BO6PS4Widget();

private:
	Ui::BO6PS4WidgetClass ui;
};

