#include <ui_includes.hpp>
#include <widgets/common/InfoWidget.h>
#include <tools_ui.hpp>
#include <QLayout>
#include <QProgressBar>
#include <QLabel>
#include <QTimer>
#include <QDesktopServices>
#include <QMessageBox>
#include <QFileDialog>
#include "MainWindow.h"
#include <QtConcurrent>
#include <acts_api/hash.h>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    ui.setupUi(this);
	mdiArea = new UI3MdiArea(this);

    progressBar = new QProgressBar(this);
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    progressBar->setTextVisible(false);
    progressBar->setFixedWidth(150);

    setWindowIcon(QIcon(":/images/icon.ico"));
    statusBar()->addPermanentWidget(progressBar);

    progressHandler.ud = this;
	progressHandler.handler = [](float progress, const char* message, void* ud) {
		MainWindow* w{ (MainWindow*)ud };
		QMetaObject::invokeMethod(w, [w, progress, message]() {
            int percent{ (int)(progress) };
			w->statusBar()->showMessage(QString("%1 (%2%)").arg(message).arg(percent), 5000);
			if (percent >= 0 && percent <= 100) {
                w->progressBar->setValue(percent);
			}
		});
	};


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

    QMenu* menu{};

    std::vector<ui3::tools::AbstractUITool*>& tools{ ui3::tools::GetTools() };

	std::sort(tools.begin(), tools.end(), [](ui3::tools::AbstractUITool* a, ui3::tools::AbstractUITool* b) {
		int c{ _strcmpi(a->path ? a->path : "", b->path ? b->path : "") };
		if (c != 0) return c < 0;
		return _strcmpi(a->name ? a->name : "", b->name ? b->name : "") < 0;
	});

	for (ui3::tools::AbstractUITool* tool : tools) {
        if (!tool->path) {
            continue;
        }

		tool->action = new QAction(tool->name, this);
		tool->action->setObjectName(QString::asprintf("%s/%s", tool->path, tool->name));
		if (tool->needsInitialization) {
			tool->action->setEnabled(false);
		}
		CreateMenu(tool->path)->addAction(tool->action);
        connect(tool->action, &QAction::triggered, this, [this, tool]() {
            tool->Activate();
        });
	}

    // Help
    connect(ui.actionWiki, &QAction::triggered, this, [this]() { QDesktopServices::openUrl(QUrl::fromLocalFile("https://github.com/ate47/atian-cod-tools/wiki")); });
    connect(ui.actionDonate, &QAction::triggered, this, [this]() { QDesktopServices::openUrl(QUrl::fromLocalFile("https://ko-fi.com/ate47")); });
    connect(ui.actionAbout, &QAction::triggered, this, [this]() { mdiArea->LoadToolUi<InfoWidget>(); });
    setCentralWidget(mdiArea);

    mdiArea->setAcceptDrops(true);
    mdiArea->connect(mdiArea, &UI3MdiArea::fileDropped, this, [this](const QString& path) { OpenFile(path); });


    QFuture<void> future = QtConcurrent::run([this] {
        ActsAPIHash_ReadDefaultHashFilesP(&this->progressHandler);
    });
	hashWatcher = new QFutureWatcher<void>(this);
	hashWatcher->setFuture(future);

	RequiresInitialization([this]() {
		QMetaObject::invokeMethod(this, [this]() {
			menuBar()->setEnabled(true);

			for (ui3::tools::AbstractUITool* tool : ui3::tools::GetTools()) {
				if (tool->needsInitialization) {
					tool->action->setEnabled(true);
				}
			}
		});
	});
}
MainWindow::~MainWindow() = default;

void MainWindow::RequiresInitialization(std::function<void()> func) {
	if (hashWatcher->isRunning()) {
		connect(hashWatcher, &QFutureWatcher<void>::finished, this, [this, func]() { func(); }, Qt::SingleShotConnection);
	}
	else {
		func();
	}
}

QMenu* MainWindow::CreateMenu(const char* path) {
	QMenuBar* bar{ menuBar() };
	// create or load a menu based on the path, we split by '/' and create submenus as needed
	QStringList parts{ QString(path).split('/') };
	QMenu* menu{ nullptr };
	for (const QString& part : parts) {
		QMenu* sub{ nullptr };
		if (menu) {
			// find submenu with this name
			for (QAction* action : menu->actions()) {
				if (action->menu() && action->text() == part) {
					sub = action->menu();
					break;
				}
			}
			if (!sub) {
				sub = menu->addMenu(part);
			}
		}
		else {
			QAction* action{ nullptr };
			QList<QAction*> actions{ bar->actions() };

			for (QAction* a : actions) {
				if (a->menu() && a->text() == part) {
					action = a;
					break;
				}
			}
			if (action) {
				sub = action->menu();
			}
			else {
				sub = new QMenu(part, bar);
				bar->insertMenu(actions[actions.size() - 1], sub);
			}
		}
		menu = sub;
	}
	return menu;
}

void MainWindow::OpenFile(const QString& path) {
    QByteArray dd{ path.toUtf8() };
    std::filesystem::path p{ dd.data() };

    for (ui3::tools::AbstractUITool* tool : ui3::tools::GetTools()) {

		if (tool->path && tool->name && (path.compare(tool->name, Qt::CaseInsensitive) == 0 || path.compare(tool->id, Qt::CaseInsensitive) == 0)) {
			if (tool->needsInitialization) {
				RequiresInitialization([tool, path, this]() {
					tool->Activate();
				});
			}
			else {
				tool->Activate();
			}
			return;
		}
        if (!tool->extensions) {
            continue;
        }

        QStringList extensions{ QString(tool->extensions).split(',') };

		for (const QString& ext : extensions) {
			if (!ext.isEmpty() && path.endsWith(ext, Qt::CaseInsensitive)) {
				if (tool->needsInitialization) {
					RequiresInitialization([tool, path, this]() {
						tool->OpenFile(path);
					});
				}
				else {
					tool->OpenFile(path);
				}

				return;
			}
		}
    }

    QTimer::singleShot(0, this, [this, path]() {
        QMessageBox::warning(this, "Unsupported File", QString("The file '%1' is not supported.").arg(path));
    });
}
