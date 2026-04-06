#pragma once

#include <QWidget>
#include <QLineEdit>
#include <MainWindow.h>
#include "ui_HashWidget.h"

class HashWidget : public QWidget
{
	Q_OBJECT

public:
	HashWidget(QMainWindow* main, QWidget* parent = nullptr);
	~HashWidget();

	void ComputeCustom();
private:
	Ui::HashWidgetClass ui;
	QMainWindow* main;

	QLineEdit* stringValue;
	QLineEdit* customOffset;
	QLineEdit* customPrime;
	QLineEdit* customReverseOffset;
	QLineEdit* customHash;
};

