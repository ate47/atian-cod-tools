#include <ui_includes.hpp>
#include "T89ErrorWidget.h"

T89ErrorWidget::T89ErrorWidget(QMainWindow* main, QWidget* parent)
	: main(main), QWidget(parent) {
	ui.setupUi(this);
}

T89ErrorWidget::~T89ErrorWidget(){}

