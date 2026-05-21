#pragma once
#include <QMainWindow>
#include <QMdiSubWindow>
#include <QProgressBar>
#include <widgets/UI3MdiArea.h>
#include <acts_api/api.h>
#include "ui_MainWindow.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    void AddSubWindow(QWidget* widget);
    QMenu* CreateMenu(const char* path);
    void OpenFile(const QString& path);

	UI3MdiArea* GetMdiArea() const { return mdiArea; }
private:
    Ui::MainWindow ui;
	UI3MdiArea* mdiArea;
    QProgressBar* progressBar;
	ActsProgressHandler progressHandler;
};

MainWindow* GetMainWindow();