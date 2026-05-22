#pragma once

#include <QMdiArea>
#include <QMdiSubWindow>

class UI3MdiArea  : public QMdiArea {
	Q_OBJECT

public:
	using QMdiArea::QMdiArea;

    void AddSubWindow(QWidget* widget);

    template<typename T>
    QMdiSubWindow* findMdiInstance() {
        for (QMdiSubWindow* sub : subWindowList()) {
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

        T* t{ new T(this) };

        AddSubWindow(t);
        return t;
    }

protected:
    void dragEnterEvent(QDragEnterEvent* e) override;
	void dropEvent(QDropEvent* e) override;
signals:
    void fileDropped(const QString& path);
};

UI3MdiArea* GetMainArea();