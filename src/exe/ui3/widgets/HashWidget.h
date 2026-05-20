#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>

class HashWidget : public QWidget
{
	Q_OBJECT

public:
	HashWidget(QWidget* parent = nullptr);
	~HashWidget();

	void ComputeCustom();
	void ComputeLookup();
private:
	QLineEdit* stringValue;
	QLineEdit* customOffset;
	QLineEdit* customPrime;
	QLineEdit* customReverseOffset;
	QLineEdit* customDJB2ReverseOffset;
	QLineEdit* customHash;
	QLineEdit* customDJB2Hash;
	QPushButton* hashLookupLoad;
	QLineEdit* hashLookupIn;
	QLineEdit* hashLookupOut;
};

