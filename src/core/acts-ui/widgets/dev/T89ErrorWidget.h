#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QCheckBox>

class T89ErrorWidget : public QWidget
{
	Q_OBJECT

public:
	T89ErrorWidget(QWidget* parent = nullptr);
	~T89ErrorWidget();

	void ComputeEncode();
	void ComputeDecode();

private:
	QLineEdit* errorCode;
	QLineEdit* errorCodeOut;
	QCheckBox* alternative;
	QLineEdit* errorMessage;
	QLineEdit* errorMessageOut;
};

