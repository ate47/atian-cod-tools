#include <ui_includes.hpp>
#include "GscDecompilerWidget.h"
#include <acts_api/gsc_decompiler.h>
#include <acts_api/config.h>
#include <QSyntaxHighlighter>
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include <config_ui.hpp>

UI_CONFIG_VAL(cfgGscDecompilerGsc, "gsc.decompiler.gsc", true, "GSC Decompile Decompilation");
UI_CONFIG_VAL(cfgGscDecompilerAsm, "gsc.decompiler.asm", false, "GSC Decompile Assembly");
UI_CONFIG_VAL(cfgGscDecompilerHeader, "gsc.decompiler.header", false, "GSC Decompile Header");

GscDecompilerWidget::GscDecompilerWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	QMenuBar* bar{ new QMenuBar(this) };

	QMenu* fileMenu{ bar->addMenu("File") };
	QAction* openAction{ fileMenu->addAction("Open") };
	QAction* saveAction{ fileMenu->addAction("Save") };
	QAction* reloadAction{ fileMenu->addAction("Reload") };
	QMenu* editMenu{ bar->addMenu("Edit") };
	QAction* copyAction{ editMenu->addAction("Copy") };
	ui.mainLayout->insertWidget(0, bar);

	/*
	ActsAPIGscDecompiler_Formatters* formatters{ ActsAPIGscDecompiler_GetFormatters() };

	for (size_t i = 0; i < formatters->count; i++) {
		const char* name{ formatters->formatters[i] };
		ui.formattersList->addItem(name, name);
	}
	ActsAPIConfig_SaveConfig();
	*/


	setLayout(ui.mainLayout);
    highlighter = new GscHighlighter(ui.textEdit->document());
	ui.textEdit->setReadOnly(true);
	QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
	ui.textEdit->setFont(font);



	connect(openAction, &QAction::triggered, this, [this]() {
		QString path{ QFileDialog::getOpenFileName(
			this,
			tr("Gsc File"),
			QString(),
			tr("Gsc File (*.gscc *.cscc *.gscbin)")
		) };
		if (!path.isEmpty()) {
			LoadFile(path);
		}
	});
	connect(saveAction, &QAction::triggered, this, [this]() {
		QString path{ QFileDialog::getSaveFileName(
			this,
			tr("Gsc File"),
			QString("file.gsc")
		) };

		if (!path.isEmpty()) {
			QString text{ ui.textEdit->document()->toPlainText() };
			QFile file(path);
			if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
				QTextStream out(&file);
				out << text;
				file.close();
			}
			else {
				QMessageBox::warning(this, "Error", "Failed to save file.");
			}
		}
	});
	connect(reloadAction, &QAction::triggered, this, [this]() { ReloadFile(); });
	connect(copyAction, &QAction::triggered, this, [this]() { ui.textEdit->copy(); });

	cfgGscDecompilerGsc.OnUpdate(this, [this]() { ReloadFile(); });
	cfgGscDecompilerAsm.OnUpdate(this, [this]() { ReloadFile(); });
	cfgGscDecompilerHeader.OnUpdate(this, [this]() { ReloadFile(); });
}

GscDecompilerWidget::~GscDecompilerWidget() {}

void GscDecompilerWidget::LoadFile(const QString& filePath) {
	lastFilePath = filePath;
	fileData.clear();
	ReloadFile();
}

void GscDecompilerWidget::LoadData(const byte* data, size_t len, const char* fileName) {
	lastFilePath = fileName ? fileName : "Custom GSC File";
	fileData.assign(data, data + len);
	ReloadFile();
}

void GscDecompilerWidget::ReloadFile() {
	setWindowTitle(lastFilePath);
	ui.textEdit->setPlainText("");

	if (lastFilePath.isEmpty() && fileData.empty()) {
		return;
	}

	ActsAPIGscDecompiler_OptionalConfig config{};

	config.writeDecompilation = cfgGscDecompilerGsc;
	config.writeHeader = cfgGscDecompilerHeader;
	config.writeAsm = cfgGscDecompilerAsm;
	std::string str{};
	config.WriteAsmFunctionUserData = &str;
	config.WriteAsmFunction = [](const char* str, size_t len, void* ud) {
		std::string* widget = static_cast<std::string*>(ud);
		widget->append(str, len);
	};
	//QString fmt{ ui.formattersList->currentData().toString() };
	//std::string fmtChars{ fmt.toStdString() };
	//config.formatter = fmtChars.data();

	ActsHandle context{ ActsAPIGscDecompiler_CreateDecompilerContext(ActsAPIGsc_Platform::PLATFORM_PC, &config) };

	if (!IS_ACTS_HANDLE_VALID(context)) {
		ui.textEdit->setPlainText(QString("// Can't create decompiler context: %1").arg(ActsGetAPILastMessage()));
		return;
	}

	utils::CloseEnd cce([&]() { ActsAPICloseHandle(context); });

	ActsStatus status;

	if (!fileData.empty()) {
		status = ActsAPIGscDecompiler_DecompileObject(context, fileData.data(), fileData.size());
	}
	else {
		QByteArray filePathBytes = lastFilePath.toUtf8();
		status = ActsAPIGscDecompiler_DecompileFile(context, filePathBytes.constData());
	}

	if (status != ACTS_STATUS_OK) {
		ui.textEdit->setPlainText(QString("// Failed to decompile file: %1").arg(ActsGetAPILastMessage()));
	}
	else {
		ui.textEdit->setPlainText(QString::fromStdString(str));
	}
}


ADD_UI_TOOL(GscDecompilerWidget, "GSC Decompiler", "GSC", ".gscc,.cscc,.gscbin,.csic,.gsic", true, true);
