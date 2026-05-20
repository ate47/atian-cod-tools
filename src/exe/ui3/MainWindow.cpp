#include <ui_includes.hpp>
#include <widgets/InfoWidget.h>
#include <widgets/HashWidget.h>
#include <widgets/T89ErrorWidget.h>
#include <widgets/ExeDumperWidget.h>
#include <widgets/ExeDllInjectorWidget.h>
#include <QLayout>
#include <QProgressBar>
#include <QLabel>
#include <QTimer>
#include <QDesktopServices>
#include <QMessageBox>
#include <QFileDialog>
#include "MainWindow.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    ui.setupUi(this);
	mdiArea = new UI3MdiArea(this);

    setWindowIcon(QIcon(":/images/icon.ico"));
    // File
    connect(ui.actionOpen, &QAction::triggered, this, [this]() {
        QString path = QFileDialog::getOpenFileName(
            this,
            tr("File"),
            QString(),
            tr("File (*.*)")
        );

        if (!path.isEmpty()) {
            OpenFile(path);
        }
    });
    connect(ui.actionExit, &QAction::triggered, this, &MainWindow::close);

    // Dev
    connect(ui.actionHash, &QAction::triggered, this, [this]() { LoadToolUi<HashWidget>(); });
    connect(ui.actionT89Error, &QAction::triggered, this, [this]() { LoadToolUi<T89ErrorWidget>(); });
    connect(ui.actionExecutable_Dumper, &QAction::triggered, this, [this]() { LoadToolUi<ExeDumperWidget>(); });
    connect(ui.actionDllInjector, &QAction::triggered, this, [this]() { LoadToolUi<ExeDllInjectorWidget>(); });

    // Help
    connect(ui.actionWiki, &QAction::triggered, this, [this]() { QDesktopServices::openUrl(QUrl::fromLocalFile("https://github.com/ate47/atian-cod-tools/wiki")); });
    connect(ui.actionDonate, &QAction::triggered, this, [this]() { QDesktopServices::openUrl(QUrl::fromLocalFile("https://ko-fi.com/ate47")); });
    connect(ui.actionAbout, &QAction::triggered, this, [this]() { LoadToolUi<InfoWidget>(); });
    setCentralWidget(mdiArea);

    ui.menubar->addSeparator();

    mdiArea->setAcceptDrops(true);
    mdiArea->connect(mdiArea, &UI3MdiArea::fileDropped, this, [this](const QString& path) { OpenFile(path); });
}
MainWindow::~MainWindow() = default;

void MainWindow::AddSubWindow(QWidget* widget) {
    QMdiSubWindow* sub{ mdiArea->addSubWindow(widget) };
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
void MainWindow::OpenFile(const QString& path) {
    QByteArray dd{ path.toUtf8() };
    std::filesystem::path p{ dd.data() };

    if (path.endsWith(".exe", Qt::CaseInsensitive)) {
        LoadToolUi<ExeDumperWidget>()->LoadFile(path);
    }
    else if (path.endsWith(".dll", Qt::CaseInsensitive)) {
        LoadToolUi<ExeDllInjectorWidget>()->LoadFile(path);
    }
    else {
        QTimer::singleShot(0, this, [this, path]() {
            QMessageBox::warning(this, "Unsupported File", QString("The file '%1' is not supported.").arg(path));
        });
    }
}
