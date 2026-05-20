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
