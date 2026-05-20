#pragma once
#include <ui_includes.hpp>
#include <QWidget>
#include <QTimer>
#include "ui_ExeDllInjectorWidget.h"

class ExeDllInjectorWidget : public QWidget
{
	Q_OBJECT

public:
	ExeDllInjectorWidget(QWidget *parent = nullptr);
	~ExeDllInjectorWidget();

	void LoadFile(const QString& path);
private:
	Ui::ExeDllInjectorWidgetClass ui;
	QTimer* injectorTimer;
};

