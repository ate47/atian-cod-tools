#include <ui_includes.hpp>
#include "UI3MdiArea.h"


#include <QDropEvent>
#include <QDragEnterEvent>
#include <QMimeData>

void UI3MdiArea::dragEnterEvent(QDragEnterEvent* e) {
    if (e->mimeData()->hasUrls()) {
        e->acceptProposedAction();
    }
}

void UI3MdiArea::dropEvent(QDropEvent* e) {
    for (const QUrl& url : e->mimeData()->urls()) {
        if (url.isLocalFile()) {
            emit fileDropped(url.toLocalFile());
        }
    }
}

void UI3MdiArea::AddSubWindow(QWidget* widget) {
    QMdiSubWindow* sub{ addSubWindow(widget) };
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
