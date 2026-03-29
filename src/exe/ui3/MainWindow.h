#pragma once
#include <QMainWindow>
#include "ui_MainWindow.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        ui.setupUi(this);
        setWindowIcon(QIcon(":/icons/icon.ico"));
		connect(ui.actionExit, &QAction::triggered, this, &MainWindow::close);
		setCentralWidget(ui.mdiArea);
        
    }
    ~MainWindow() = default;

private:
    Ui::MainWindow ui;
};