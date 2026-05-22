#pragma once

#include <QWidget>
#include "GscHighlighter.h"
#include "ui_GscDecompilerWidget.h"

class GscHighlighter;

class GscDecompilerWidget : public QWidget
{
	Q_OBJECT

public:
	GscDecompilerWidget(QWidget *parent = nullptr);
	~GscDecompilerWidget();

	void LoadFile(const QString& filePath);
	void ReloadFile();
private:
	Ui::GscDecompilerWidgetClass ui;
	GscHighlighter* highlighter;
	QString lastFilePath;
};

