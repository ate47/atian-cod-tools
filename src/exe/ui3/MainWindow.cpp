#include <ui_includes.hpp>
#include <ui-tools/InfoWidget.h>
#include <ui-tools/HashWidget.h>
#include <ui-tools/T89ErrorWidget.h>
#include <QTimer>
#include "MainWindow.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    ui.setupUi(this);
    setWindowIcon(QIcon(":/images/icon.ico"));
    connect(ui.actionExit, &QAction::triggered, this, &MainWindow::close);
    connect(ui.actionAbout, &QAction::triggered, this, [this]() { LoadToolUi<InfoWidget>(); });
    connect(ui.actionHash, &QAction::triggered, this, [this]() { LoadToolUi<HashWidget>(); });
    connect(ui.actionT89Error, &QAction::triggered, this, [this]() { LoadToolUi<T89ErrorWidget>(); });
    setCentralWidget(ui.mdiArea);

}
MainWindow::~MainWindow() = default;

void MainWindow::AddSubWindow(QWidget* widget) {
    QMdiSubWindow* sub{ ui.mdiArea->addSubWindow(widget) };
    sub->setContentsMargins(0, 0, 0, 0);

    if (widget->minimumSize() == widget->maximumSize()) {
        QSize s{ widget->size() };
            sub->setMinimumSize(s);
            sub->setMaximumSize(s);
            sub->setWindowFlags(
                (sub->windowFlags() | Qt::MSWindowsFixedSizeDialogHint)
                & ~Qt::WindowMaximizeButtonHint
            );
            sub->updateGeometry();
            sub->update();
    }
    sub->show();
}