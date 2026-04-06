#include <ui_includes.hpp>
#include <ui-tools/InfoWidget.h>
#include <QTimer>
#include "MainWindow.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    ui.setupUi(this);
    setWindowIcon(QIcon(":/images/icon.ico"));
    connect(ui.actionExit, &QAction::triggered, this, &MainWindow::close);
    connect(ui.actionAbout, &QAction::triggered, this, [this]() { LoadToolUi<InfoWidget>(); });
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