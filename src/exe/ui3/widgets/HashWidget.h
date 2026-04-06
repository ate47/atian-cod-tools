#pragma once

#include <QWidget>
#include <QLineEdit>

class HashWidget : public QWidget
{
	Q_OBJECT

public:
	HashWidget(QWidget* parent = nullptr);
	~HashWidget();

	void ComputeCustom();
private:
	QLineEdit* stringValue;
	QLineEdit* customOffset;
	QLineEdit* customPrime;
	QLineEdit* customReverseOffset;
	QLineEdit* customHash;
};

