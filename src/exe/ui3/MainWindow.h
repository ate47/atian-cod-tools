#pragma once
#include <QMainWindow>
#include <QMdiSubWindow>
#include <QProgressBar>
#include "ui_MainWindow.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    void AddSubWindow(QWidget* widget);

    template<typename T>
    QMdiSubWindow* findMdiInstance() {
        for (QMdiSubWindow* sub : ui.mdiArea->subWindowList()) {
            if (qobject_cast<T*>(sub->widget())) {
                return sub;
            }
        }
        return nullptr;
    }

    template<typename T>
    void LoadToolUi() {
        QMdiSubWindow* instance{ findMdiInstance<T>() };
        if (instance) {
            // already added, we raise it
            instance->raise();
            return;
        }

        AddSubWindow(new T(ui.mdiArea));
    }
private:
    Ui::MainWindow ui;
};

MainWindow* GetMainWindow();