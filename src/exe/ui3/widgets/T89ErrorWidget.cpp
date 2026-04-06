#include <ui_includes.hpp>
#include "T89ErrorWidget.h"
#include <api/hash.hpp>
#include <QLabel>
#include <QLayout>
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


T89ErrorWidget::T89ErrorWidget(QWidget* parent) : QWidget(parent) {
	setObjectName("T89ErrorWidgetClass");
	resize(600, 400);
	setWindowTitle(QCoreApplication::translate("T89ErrorWidgetClass", "BO4/CW Errors", nullptr));
	QMetaObject::connectSlotsByName(this);

	QVBoxLayout* layout{ new QVBoxLayout(this) };
	layout->setContentsMargins(0, 0, 0, 0);

	layout->addWidget(new QLabel("Error Encoder"));
	errorCode = CreateQLineEditRow("Error code", true, layout);
	errorCodeOut = CreateQLineEditRow("Encoded", false, layout);
	layout->addWidget(alternative = new QCheckBox("Alternative"));
	layout->addWidget(new QLabel("Error Decoder"));
	errorMessage = CreateQLineEditRow("Error message", true, layout);
	errorMessageOut = CreateQLineEditRow("Decoded code", false, layout);

	errorCode->connect(errorCode, &QLineEdit::textChanged, [this]() { ComputeEncode(); });
	alternative->connect(alternative, &QCheckBox::checkStateChanged, [this]() { ComputeEncode(); });
	errorMessage->connect(errorMessage, &QLineEdit::textChanged, [this]() { ComputeDecode(); });

	layout->addSpacing(10);
	layout->addStretch();
	setLayout(layout);
}

T89ErrorWidget::~T89ErrorWidget(){}

void T89ErrorWidget::ComputeEncode() {
	QString text{ this->errorCode->text() };
	QByteArray ba{ text.toUtf8() };
	const char* cstr{ ba.constData() };

	if (!*cstr) {
		errorCodeOut->setText("");
		return;
	}

	try {
		char* end;
		uint32_t err = std::strtoul(cstr, &end, 10);
		if (*end) {
			throw std::runtime_error("Invalid code");
		}

		acts::api::ErrorCode code{};

		acts::api::ActsAPIHash& apiHash{ ActsAPIHash() };

		apiHash.EncodeErrorCode(code, err, alternative->isChecked());

		std::string res = apiHash.ErrorCodeToStr(code);
		errorCodeOut->setText(res.c_str());
	}
	catch (std::exception& e) {
		errorCodeOut->setText(e.what());
	}
}
void T89ErrorWidget::ComputeDecode() {
	QString text{ this->errorMessage->text() };
	QByteArray ba{ text.toUtf8() };
	const char* cstr{ ba.constData() };
	if (!*cstr) {
		errorMessageOut->setText("");
	}

	std::string_view decodeStr{ cstr };
	std::string words[4]{};

	try {
		size_t idx{};
		for (int i = 0; i < 4; i++) {
			size_t f = decodeStr.find(' ', idx);
			if (f == std::string::npos) {
				if (i != 3) {
					errorMessageOut->setText("Not enough components");
					return;
				}
				f = decodeStr.length();
			}
			if (f - idx == 0) {
				// empty word
				i--;
				idx = f + 1;
				continue;
			}

			words[i] = decodeStr.substr(idx, f - idx);
			idx = f + 1;
		}
		while (idx < decodeStr.length() && decodeStr[idx] == ' ') {
			idx++; // ignore end spaces
		}
		if (idx < decodeStr.length()) {
			errorMessageOut->setText("Too many components");
			return;
		}

		acts::api::ErrorCode code{ words[0].c_str(), words[1].c_str(), words[2].c_str(), words[3].c_str() };

		uint32_t err = ActsAPIHash().DecodeErrorCode(code);

		errorMessageOut->setText(QString::asprintf("%llu", err));
	}
	catch (std::exception& e) {
		errorMessageOut->setText(e.what());
	}
}