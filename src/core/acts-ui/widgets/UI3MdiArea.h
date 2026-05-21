#pragma once

#include <QMdiArea>

class UI3MdiArea  : public QMdiArea {
	Q_OBJECT

public:
	using QMdiArea::QMdiArea;

protected:
    void dragEnterEvent(QDragEnterEvent* e) override;
	void dropEvent(QDropEvent* e) override;
signals:
    void fileDropped(const QString& path);
};

