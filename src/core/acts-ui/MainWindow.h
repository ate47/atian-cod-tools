#pragma once
#include <QMainWindow>
#include <QMdiSubWindow>
#include <QProgressBar>
#include <QFutureWatcher>
#include <widgets/common/UI3MdiArea.h>
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
	void RequiresInitialization(std::function<void()> func);

	UI3MdiArea* GetMdiArea() const { return mdiArea; }
private:
    Ui::MainWindow ui;
	UI3MdiArea* mdiArea;
    QProgressBar* progressBar;
    QFutureWatcher<void>* hashWatcher;
	ActsProgressHandler progressHandler;
};

MainWindow* GetMainWindow();