#pragma once

#include <QWidget>
#include "ui_UI3SettingsWidget.h"

class UI3SettingsWidget : public QWidget
{
	Q_OBJECT

public:
	UI3SettingsWidget(QWidget *parent = nullptr);
	~UI3SettingsWidget();

private:
	Ui::UI3SettingsWidgetClass ui;
};

