#include <ui_includes.hpp>
#include "UI3MdiArea.h"

#include <QIcon>
#include <QPainter>
#include <QPixmap>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QMimeData>

void UI3MdiArea::paintEvent(QPaintEvent* e) {
    // draw the background, then the logo under the sub windows
    QMdiArea::paintEvent(e);

    constexpr int logoSize{ 96 };
    constexpr qreal logoOpacity{ 0.12 };

    static const QIcon logo{ ":/images/logo.png" };
    QPixmap pix{ logo.pixmap(logoSize, logoSize) };
    if (pix.isNull()) {
        return;
    }

    QPainter painter{ viewport() };
    painter.setOpacity(logoOpacity);
    QSize size{ pix.size() / pix.devicePixelRatio() };
    QRect logoRect{ QPoint(viewport()->width() - size.width(), viewport()->height() - size.height()), size };
    painter.drawPixmap(logoRect.topLeft(), pix);

    // title at the left of the logo, the font height is a fraction of the logo height
    constexpr qreal textHeightRatio{ 0.3 };
    constexpr int textGap{ 8 };

    QFont font{ painter.font() };
    font.setPixelSize(qMax(1, (int)(size.height() * textHeightRatio)));
    painter.setFont(font);

    QRect textRect{ 0, logoRect.top(), logoRect.left() - textGap, logoRect.height() };
    painter.drawText(textRect, Qt::AlignRight | Qt::AlignVCenter, "Atian Tools");
}

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
        sub->setWindowFlags((sub->windowFlags() | Qt::MSWindowsFixedSizeDialogHint) & ~Qt::WindowMaximizeButtonHint);
        sub->updateGeometry();
        sub->update();
    }
    sub->show();
}
