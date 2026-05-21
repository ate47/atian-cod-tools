#include <ui_includes.hpp>
#include <core/hashes/hash_lookup.hpp>
#include <utils/hash_mini.hpp>
#include "HashWidget.h"
#include <acts_api/hash.h>
#include <QLayout>
#include <QLineEdit>
#include <QLabel>
#include <QCoreApplication>

namespace {
	QPushButton* CreateQPushButtonRow(const char* desc, QVBoxLayout* parent) {
		QHBoxLayout* row{ new QHBoxLayout() };

		QLabel* label{ new QLabel() };
		QPushButton* button{ new QPushButton(desc) };
		button->setMinimumWidth(300);

		row->addWidget(button);
		row->addWidget(label);
		row->addSpacing(10);
		row->addStretch();

		parent->addLayout(row);
		return button;
	}
	QLineEdit* CreateQLineEditRow(const char* desc, bool editable, QVBoxLayout* parent) {
		QHBoxLayout* row{ new QHBoxLayout() };

		QLabel* label{ new QLabel(desc) };
		QLineEdit* edit{ new QLineEdit() };
		edit->setMinimumWidth(300);
		if (!editable) {
			edit->setReadOnly(true);
		}

		row->addWidget(edit);
		row->addWidget(label);
		row->addSpacing(10);
		row->addStretch();

		parent->addLayout(row);
		return edit;
	}
}

HashWidget::HashWidget(QWidget* parent) : QWidget(parent) {
	setObjectName("HashWidgetClass");
	resize(600, 400);
	setWindowTitle(QCoreApplication::translate("HashWidgetClass", "Hash ", nullptr));
	QMetaObject::connectSlotsByName(this);

	QHBoxLayout* layout{ new QHBoxLayout(this) };
	layout->setContentsMargins(0, 0, 0, 0); // optional, removes padding
	layout->setSpacing(30);

	QVBoxLayout* funcLayout{ new QVBoxLayout() };
	QVBoxLayout* customLayout{ new QVBoxLayout() };

	// functions
	ActsAPIHash_HashTypeList* types{ ActsAPIHash_GetHashesList() };

	stringValue = CreateQLineEditRow("String", true, funcLayout);
	for (size_t i = 0; i < types->count; i++) {
		ActsAPIHash_HashType& type{ types->values[i] };
		QLineEdit* edit{ CreateQLineEditRow(type.desc, false, funcLayout) };

		stringValue->connect(stringValue, &QLineEdit::textChanged, [this, type, edit](const QString& text) {
			QString str{ stringValue->text() };
			if (str.isEmpty()) {
				edit->setText("");
			}
			else {
				QByteArray ba{ str.toUtf8() };
				edit->setText(QString::asprintf("%llx", type.hashFunc(ba.constData())));
			}
		});
	}

	// custom
	customLayout->addWidget(new QLabel("Custom hash"));
	customOffset = CreateQLineEditRow("Offset", true, customLayout);
	customPrime = CreateQLineEditRow("Prime", true, customLayout);
	customPrime->setText(QString::asprintf("%llx", hash::IV_DEFAULT));
	customReverseOffset = CreateQLineEditRow("Reverse Offset", false, customLayout);
	customDJB2ReverseOffset = CreateQLineEditRow("DJB2 Reverse Offset", false, customLayout);
	customHash = CreateQLineEditRow("Custom Hash", false, customLayout);
	customDJB2Hash = CreateQLineEditRow("DJB2 Custom Hash", false, customLayout);

	customLayout->addWidget(new QLabel("Hash lookup"));
	hashLookupIn = CreateQLineEditRow("Hashed value", true, customLayout);
	hashLookupOut = CreateQLineEditRow("Lookup", false, customLayout);

	stringValue->connect(stringValue, &QLineEdit::textChanged, [this]() { ComputeCustom(); });
	customOffset->connect(customOffset, &QLineEdit::textChanged, [this]() { ComputeCustom(); });
	customPrime->connect(customPrime, &QLineEdit::textChanged, [this]() { ComputeCustom(); });
	hashLookupIn->connect(hashLookupIn, &QLineEdit::textChanged, [this]() { ComputeLookup(); });

	funcLayout->addStretch();
	layout->addLayout(funcLayout, 1);
	layout->setSpacing(10);
	customLayout->addStretch();
	customLayout->setSpacing(10);
	layout->addLayout(customLayout, 1);
	layout->addStretch();

	setLayout(layout);
}

HashWidget::~HashWidget() {}


void HashWidget::ComputeCustom() {
	QString str{ stringValue->text() };
	QString costr{ customOffset->text() };
	QString cpstr{ customPrime->text() };
	if (str.isEmpty() || costr.isEmpty() || cpstr.isEmpty()) {
		customHash->setText("");
		customDJB2Hash->setText("");
		customReverseOffset->setText("");
		customDJB2ReverseOffset->setText("");
	}
	else {
		QByteArray ba{ str.toUtf8() };
		QByteArray baco{ costr.toUtf8() };
		QByteArray bacp{ cpstr.toUtf8() };
		const char* cstr{ ba.constData() };
		const char* cstrco{ baco.constData() };
		const char* cstrcp{ bacp.constData() };


		uint64_t val{};
		uint64_t iv{};
		try {
			val = std::strtoull(cstrco, nullptr, 16);
			iv = std::strtoull(cstrcp, nullptr, 16);

			if (val && iv) {
				customHash->setText(QString::asprintf("%llx", hash::Hash64A(cstr, val, iv)));
				customDJB2Hash->setText(QString::asprintf("%llx", hash::HashDJB2(cstr, val, iv)));

				uint64_t disc{ core::hashes::lookup::LookupFNV1A64(cstr, val, iv) };
				uint64_t disc2{ core::hashes::lookup::LookupDJB2(cstr, val, iv) };

				customReverseOffset->setText(QString::asprintf("%llx", disc));
				customDJB2ReverseOffset->setText(QString::asprintf("%llx", disc2));
			}
			else {
				customReverseOffset->setText("N/A");
				customDJB2ReverseOffset->setText("N/A");
			}
		}
		catch (std::runtime_error& e) {
			if (!val || !iv) {
				customReverseOffset->setText("N/A");
				customDJB2ReverseOffset->setText("N/A");
			}
			else {
				customReverseOffset->setText(e.what());
				customDJB2ReverseOffset->setText(e.what());
			}
		}

	}
}

void HashWidget::ComputeLookup() {
	QString str{ hashLookupIn->text() };
	hashLookupOut->setText("");
	if (str.isEmpty()) {
		return;
	}
	QByteArray ba{ str.toUtf8() };
	char* cstr{ ba.data() };

	uint64_t val{};
	try {
		while (*cstr && std::isspace(*cstr)) {
			cstr++;
		}
		size_t len{ std::strlen(cstr) };
		while (len && std::isspace(cstr[len - 1])) {
			cstr[len - 1] = '\0';
			len--;
		}
		if (len && !hash::TryHashPattern(cstr, val)) {
			if (cstr[0] == '0' && cstr[1] == 'x') {
				// allow 0x prefix
				cstr += 2;
			}
			if (len > 3 && cstr[len - 1] == '4' && cstr[len - 2] == '6' && cstr[len - 3] == 'i') {
				cstr[len - 3] = '\0';
				len -= 3;
				if (len && cstr[len - 1] == 'u') {
					cstr[len - 1] = '\0';
					len--;
				}
			}

			val = std::strtoull(cstr, nullptr, 16);
		}
	}
	catch (std::runtime_error& e) {
		hashLookupOut->setText(e.what());
		return;
	}
	if (val) {
		const char* extracted{ ActsAPIHash_ExtractPtr(val) };
		hashLookupOut->setText(extracted ? extracted : "no found");
	}
}