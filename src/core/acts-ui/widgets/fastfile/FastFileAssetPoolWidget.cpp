#include <ui_includes.hpp>
#include "FastFileAssetPoolWidget.h"
#include <widgets/common/UI3MdiArea.h>

#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMenu>
#include <QSplitter>
#include <QMenuBar>
#include <QVBoxLayout>

FastFileAssetPoolWidget::FastFileAssetPoolWidget(ActsHandle ctx, UI3MdiArea* mdi) : QWidget(mdi), ctx(ctx) {
    setWindowTitle("FastFile Asset Pool");

    QMenuBar* bar{ new QMenuBar(this) };

    QMenu* fileMenu{ bar->addMenu("File") };
    QAction* loadCommonAction{ fileMenu->addAction("Load common fastfiles") };

    QSplitter* splitter{ new QSplitter(Qt::Horizontal, this) };
    QWidget* filesWidget{ new QWidget(splitter) };
    QVBoxLayout* filesLayout{ new QVBoxLayout(filesWidget) };
    fastFileList = new QListWidget(filesWidget);
    fastFileList->setMinimumWidth(200);
    fastFileList->setSelectionMode(QAbstractItemView::ExtendedSelection);
    QLineEdit* searchEdit{ new QLineEdit(filesWidget) };
    searchEdit->setPlaceholderText("Search fastfiles");
    filesLayout->addWidget(fastFileList);
    filesLayout->addWidget(searchEdit);

    QWidget* todoWidget{ new QWidget(splitter) };
    QVBoxLayout* todoLayout{ new QVBoxLayout(todoWidget) };
    todoWidget->setMinimumWidth(600);
    todoLayout->addWidget(new QLabel("todo", todoWidget));
    todoLayout->addStretch();

    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 4);
    splitter->setSizes({ 200, 800 });

    QVBoxLayout* layout{ new QVBoxLayout(this) };
    layout->insertWidget(0, bar);
    layout->addWidget(splitter);
    layout->setContentsMargins(0, 0, 0, 0);

    connect(loadCommonAction, &QAction::triggered, this, [this]() {
        ActsStatus status{ ActsAPIFastFile_AssetPoolLoadCommonFastFiles(this->ctx) };
        if (ACTS_NOT_OK(status)) {
            qWarning("Failed to load common fastfiles: %s", ActsGetAPILastMessage());
            return;
        }
        RefreshFastFiles(currentSearch);
    });
    connect(searchEdit, &QLineEdit::textChanged, this, &FastFileAssetPoolWidget::RefreshFastFiles);
    fastFileList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(fastFileList, &QListWidget::customContextMenuRequested, this, [this](const QPoint& position) {
        QListWidgetItem* item{ fastFileList->itemAt(position) };
        if (!item)
            return;

        if (!item->isSelected()) {
            fastFileList->clearSelection();
            item->setSelected(true);
        }

        QList<QListWidgetItem*> selectedItems{ fastFileList->selectedItems() };
        QMenu menu{ this };
        QAction* loadAction{ menu.addAction(selectedItems.size() == 1 ? "Load file" : "Load files") };
        if (menu.exec(fastFileList->viewport()->mapToGlobal(position)) == loadAction)
            LoadFastFileEntries(selectedItems);
    });
    RefreshFastFiles({});
}

FastFileAssetPoolWidget::~FastFileAssetPoolWidget() {
    if (IS_ACTS_HANDLE_VALID(ctx)) {
        ActsAPICloseHandle(ctx);
        ctx = INVALID_ACTS_HANDLE_VALUE;
    }
}

void FastFileAssetPoolWidget::RefreshFastFiles(const QString& search) {
    currentSearch = search;
    fastFileList->clear();

    QByteArray wildcard{};
    const char* wildcardPtr{};
    if (!search.isEmpty()) {
        QString pattern{ search };
        if (!pattern.startsWith('^'))
            pattern.prepend(".*");
        if (!pattern.endsWith('$'))
            pattern.append(".*");
        wildcard = pattern.toUtf8();
        wildcardPtr = wildcard.constData();
    }

    ActsStatus status{ ActsAPIFastFile_ListFastFile(
        ctx,
        "",
        wildcardPtr,
        nullptr,
        [](const ActsAPIFastFile_FastFileEntry* entry, void* ud) {
            if (!entry || !entry->name)
                return true;

            QListWidget* list{ static_cast<QListWidget*>(ud) };
            std::filesystem::path filePath{ entry->name };
            std::string cfilePath{ filePath.filename().string() };
            QListWidgetItem* item{
                new QListWidgetItem(QString::asprintf("%s%s", cfilePath.data(), entry->loaded ? " (loaded)" : ""), list)
            };
            item->setData(Qt::UserRole, QVariant::fromValue<quintptr>(reinterpret_cast<quintptr>(entry)));
            item->setToolTip(QString::fromUtf8(entry->name));
            return true;
        },
        fastFileList
    ) };

    if (ACTS_NOT_OK(status))
        qWarning("Failed to list fastfiles: %s", ActsGetAPILastMessage());
}

void FastFileAssetPoolWidget::LoadFastFileEntries(const QList<QListWidgetItem*>& items) {
    for (QListWidgetItem* item : items) {
        quintptr value{ item->data(Qt::UserRole).value<quintptr>() };
        const ActsAPIFastFile_FastFileEntry* entry{ reinterpret_cast<const ActsAPIFastFile_FastFileEntry*>(value) };
        if (!entry)
            continue;

        ActsStatus status{ ActsAPIFastFile_AssetPoolLoadFastFileEntry(ctx, entry, false) };
        if (ACTS_NOT_OK(status)) {
            qWarning("Failed to load fastfile entry: %s", ActsGetAPILastMessage());
        }
    }

    RefreshFastFiles(currentSearch);
}
