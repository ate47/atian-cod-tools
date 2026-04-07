#include <ui_includes.hpp>
#include <utils/hash_mini.hpp>
#include "HashWidget.h"
#include <acts_api/hash.hpp>
#include <QLayout>
#include <QLineEdit>
#include <QLabel>
#include <QCoreApplication>

namespace {
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
	acts::api::ActsAPIHash& hash{ ActsAPIHash() };
	acts::api::HashType* types{ hash.GetHashes() };
	size_t typesCount{ hash.GetHashesCount() };

	stringValue = CreateQLineEditRow("String", true, funcLayout);
	for (size_t i = 0; i < typesCount; i++) {
		acts::api::HashType& type{ types[i] };
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
	customReverseOffset = CreateQLineEditRow("Reverse Offset", false, customLayout);
	customHash = CreateQLineEditRow("Custom Hash", false, customLayout);

	stringValue->connect(stringValue, &QLineEdit::textChanged, [this]() { ComputeCustom(); });
	customOffset->connect(customOffset, &QLineEdit::textChanged, [this]() { ComputeCustom(); });
	customPrime->connect(customPrime, &QLineEdit::textChanged, [this]() { ComputeCustom(); });

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
		customReverseOffset->setText("");
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

				uint64_t mask = 0xFFFF;
				uint64_t found = 0;
				uint64_t disc{};

				while (found < 64) {
					uint64_t k;
					for (k = 0; k < 0x10000; k++) {
						uint64_t v = (k << found) | disc;
						if ((hash::Hash64A(cstr, v, iv) & mask) == (val & mask)) {
							break;
						}
					}
					if (k == 0x10000) {
						throw std::runtime_error("Invalid value");
					}

					disc |= (k & 0xF) << found;

					found += 4;
					mask = (mask << 4) | 0xF;
				}

				customReverseOffset->setText(QString::asprintf("%llx", disc));
			}
			else {
				customReverseOffset->setText("N/A");
				customReverseOffset->setText("N/A");
			}
		}
		catch (std::runtime_error& e) {
			customReverseOffset->setText(e.what());
			if (!val || !iv) {
				customReverseOffset->setText("N/A");
			}
		}

	}
}