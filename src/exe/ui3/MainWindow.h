#pragma once
#include <QMainWindow>
#include <QMdiSubWindow>
#include <QProgressBar>
#include <widgets/UI3MdiArea.h>
#include "ui_MainWindow.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    void AddSubWindow(QWidget* widget);

    template<typename T>
    QMdiSubWindow* findMdiInstance() {
        for (QMdiSubWindow* sub : mdiArea->subWindowList()) {
            if (qobject_cast<T*>(sub->widget())) {
                return sub;
            }
        }
        return nullptr;
    }

    template<typename T>
    T* LoadToolUi(bool allowDupe = false) {
        if (!allowDupe) {
            QMdiSubWindow* instance{ findMdiInstance<T>() };
            if (instance) {
                // already added, we raise it
                instance->raise();
                return qobject_cast<T*>(instance->widget());
            }

        }

        T* t{ new T(mdiArea) };
        AddSubWindow(t);
        return t;
    }

    void OpenFile(const QString& path);
private:
    Ui::MainWindow ui;
	UI3MdiArea* mdiArea;
};

MainWindow* GetMainWindow();