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
#include <QApplication>
#include <QClipboard>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTabWidget>
#include <QTreeWidget>
#include <QHeaderView>
#include <QRegularExpression>
#include <QProgressDialog>
#include <QVBoxLayout>
#include <filesystem>
#include <sstream>

namespace {
    QString MakeWildcard(const QString& search) {
        if (search.isEmpty()) {
            return {};
        }

        QString pattern{ search };
        if (!pattern.startsWith('^')) {
            pattern.prepend(".*");
        }
        if (!pattern.endsWith('$')) {
            pattern.append(".*");
        }
        return pattern;
    }

    QString FormatHash(uint64_t hash) {
        std::ostringstream stream;
        stream << "0x" << std::hex << std::uppercase << hash;
        return QString::fromStdString(stream.str());
    }
} // namespace

FastFileAssetPoolWidget::FastFileAssetPoolWidget(ActsHandle ctx, UI3MdiArea* mdi) : QWidget(mdi), ctx(ctx) {
    setWindowTitle("FastFile Asset Pool");

    QMenuBar* bar{ new QMenuBar(this) };
    bar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    QMenu* fileMenu{ bar->addMenu("File") };
    QAction* loadCommonAction{ fileMenu->addAction("Load common fastfiles") };

    QTabWidget* tabs{ new QTabWidget(this) };
    QWidget* fastFilesTab{ new QWidget(tabs) };
    QWidget* assetsTab{ new QWidget(tabs) };
    QWidget* stringsTab{ new QWidget(tabs) };
    tabs->addTab(fastFilesTab, "Fast Files");
    tabs->addTab(assetsTab, "Assets");
    tabs->addTab(stringsTab, "Strings");

    QSplitter* splitter{ new QSplitter(Qt::Horizontal, fastFilesTab) };
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

    QVBoxLayout* fastFilesLayout{ new QVBoxLayout(fastFilesTab) };
    fastFilesLayout->addWidget(splitter);
    fastFilesLayout->setContentsMargins(0, 0, 0, 0);

    QSplitter* assetsSplitter{ new QSplitter(Qt::Horizontal, assetsTab) };
    QWidget* assetsLeft{ new QWidget(assetsSplitter) };
    QVBoxLayout* assetsLeftLayout{ new QVBoxLayout(assetsLeft) };
    assetList = new QTreeWidget(assetsLeft);
    assetList->setColumnCount(2);
    assetList->setHeaderLabels({ "Pool", "Asset" });
    assetList->header()->setStretchLastSection(true);
    QLineEdit* assetSearch{ new QLineEdit(assetsLeft) };
    assetSearch->setPlaceholderText("Search assets");
    assetsLeftLayout->addWidget(assetList);
    assetsLeftLayout->addWidget(assetSearch);
    QWidget* assetsTodo{ new QWidget(assetsSplitter) };
    QVBoxLayout* assetsTodoLayout{ new QVBoxLayout(assetsTodo) };
    assetsTodoLayout->addWidget(new QLabel("todo", assetsTodo));
    assetsTodoLayout->addStretch();
    assetsSplitter->setSizes({ 200, 800 });
    QVBoxLayout* assetsLayout{ new QVBoxLayout(assetsTab) };
    assetsLayout->addWidget(assetsSplitter);
    assetsLayout->setContentsMargins(0, 0, 0, 0);

    stringTable = new QTableWidget(stringsTab);
    stringTable->setColumnCount(2);
    stringTable->setHorizontalHeaderLabels({ "Hash", "String" });
    stringTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    stringTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    stringTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    stringTable->setContextMenuPolicy(Qt::CustomContextMenu);
    QLineEdit* stringSearch{ new QLineEdit(stringsTab) };
    stringSearch->setPlaceholderText("Search strings");
    QVBoxLayout* stringsLayout{ new QVBoxLayout(stringsTab) };
    stringsLayout->addWidget(stringTable);
    stringsLayout->addWidget(stringSearch);
    stringsLayout->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout* layout{ new QVBoxLayout(this) };
    layout->addWidget(bar);
    layout->addWidget(tabs);
    layout->setContentsMargins(0, 0, 0, 0);

    connect(loadCommonAction, &QAction::triggered, this, [this]() {
        QProgressDialog progress{ "Loading common files", QString{}, 0, 0, this };
        progress.setWindowTitle("FastFile Asset Pool");
        progress.setCancelButton(nullptr);
        progress.show();
        QApplication::processEvents();

        ActsStatus status{ ActsAPIFastFile_AssetPoolLoadCommonFastFiles(this->ctx) };
        if (ACTS_NOT_OK(status)) {
            qWarning("Failed to load common fastfiles: %s", ActsGetAPILastMessage());
            return;
        }
        progress.setLabelText("Refreshing fastfiles");
        QApplication::processEvents();
        RefreshFastFiles(currentSearch);
        progress.setLabelText("Refreshing assets");
        QApplication::processEvents();
        RefreshAssets(currentAssetSearch);
        progress.setLabelText("Refreshing strings");
        QApplication::processEvents();
        RefreshStrings(currentStringSearch);
    });
    connect(searchEdit, &QLineEdit::textChanged, this, &FastFileAssetPoolWidget::RefreshFastFiles);
    connect(assetSearch, &QLineEdit::textChanged, this, &FastFileAssetPoolWidget::FilterAssets);
    connect(stringSearch, &QLineEdit::textChanged, this, &FastFileAssetPoolWidget::FilterStrings);
    connect(fastFileList, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem* item) {
        QList<QListWidgetItem*> items{ item };
        LoadFastFileEntries(items);
    });
    fastFileList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(fastFileList, &QListWidget::customContextMenuRequested, this, [this](const QPoint& position) {
        QListWidgetItem* item{ fastFileList->itemAt(position) };
        if (!item) {
            return;
        }

        if (!item->isSelected()) {
            fastFileList->clearSelection();
            item->setSelected(true);
        }

        QList<QListWidgetItem*> selectedItems{ fastFileList->selectedItems() };
        QMenu menu{ this };
        QAction* loadAction{ menu.addAction(selectedItems.size() == 1 ? "Load file" : "Load files") };
        if (menu.exec(fastFileList->viewport()->mapToGlobal(position)) == loadAction) {
            LoadFastFileEntries(selectedItems);
        }
    });
    connect(stringTable, &QTableWidget::customContextMenuRequested, this, [this](const QPoint& position) {
        if (!stringTable->itemAt(position)) {
            return;
        }
        QMenu menu{ this };
        QAction* copyLine{ menu.addAction("Copy line") };
        QAction* copyHash{ menu.addAction("Copy hash") };
        QAction* copyString{ menu.addAction("Copy string") };
        QAction* selected{ menu.exec(stringTable->viewport()->mapToGlobal(position)) };
        if (selected == copyLine) {
            CopyStringSelection(-1);
        } else if (selected == copyHash) {
            CopyStringSelection(0);
        } else if (selected == copyString) {
            CopyStringSelection(1);
        }
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

    QByteArray wildcard{ MakeWildcard(search).toUtf8() };
    const char* wildcardPtr{ wildcard.isEmpty() ? nullptr : wildcard.constData() };

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

    if (ACTS_NOT_OK(status)) {
        qWarning("Failed to list fastfiles: %s", ActsGetAPILastMessage());
    }
}

void FastFileAssetPoolWidget::RefreshAssets(const QString& search) {
    currentAssetSearch = search;
    ActsStatus status{ ActsAPIFastFile_ListAssets(
        ctx,
        nullptr,
        [](const ActsAPIFastFile_FastFileAssetEntry* entry, void* ud) {
            if (!entry) {
                return;
            }
            AssetEntry copy{};
            copy.poolId = entry->poolId;
            copy.hash = entry->hash;
            if (entry->poolName) {
                copy.poolName = entry->poolName;
            }
            if (entry->name) {
                copy.name = entry->name;
            }
            std::unordered_map<AssetKey, AssetEntry, AssetKeyHash>* entries{
                static_cast<std::unordered_map<AssetKey, AssetEntry, AssetKeyHash>*>(ud)
            };
            entries->try_emplace({ entry->poolId, entry->hash }, std::move(copy));
        },
        &assets
    ) };
    if (ACTS_NOT_OK(status)) {
        qWarning("Failed to list assets: %s", ActsGetAPILastMessage());
    }
    FilterAssets(search);
}

void FastFileAssetPoolWidget::RefreshStrings(const QString& search) {
    currentStringSearch = search;
    ActsStatus status{ ActsAPIFastFile_ListStrings(
        ctx,
        nullptr,
        [](const char* string, uint64_t hash, void* ud) {
            std::unordered_map<uint64_t, std::string>* entries{
                static_cast<std::unordered_map<uint64_t, std::string>*>(ud)
            };
            entries->try_emplace(hash, string ? string : "");
        },
        &strings
    ) };
    if (ACTS_NOT_OK(status)) {
        qWarning("Failed to list strings: %s", ActsGetAPILastMessage());
    }
    FilterStrings(search);
}

void FastFileAssetPoolWidget::FilterAssets(const QString& search) {
    currentAssetSearch = search;
    assetList->clear();
    QRegularExpression expression{ MakeWildcard(search) };
    for (const auto& pair : assets) {
        const AssetEntry& asset{ pair.second };
        QString poolName{ QString::fromStdString(asset.poolName) };
        QString assetName{ QString::fromStdString(asset.name) };
        if (!search.isEmpty() && !expression.match(poolName + " " + assetName).hasMatch()) {
            continue;
        }
        QTreeWidgetItem* item{ new QTreeWidgetItem(assetList) };
        item->setText(0, poolName);
        item->setText(1, assetName);
    }
}

void FastFileAssetPoolWidget::FilterStrings(const QString& search) {
    currentStringSearch = search;
    stringTable->setRowCount(0);
    QRegularExpression expression{ MakeWildcard(search) };
    for (const auto& value : strings) {
        QString stringValue{ QString::fromStdString(value.second) };
        if (!search.isEmpty() && !expression.match(stringValue).hasMatch()) {
            continue;
        }

        int row{ stringTable->rowCount() };
        stringTable->insertRow(row);
        stringTable->setItem(row, 0, new QTableWidgetItem(FormatHash(value.first)));
        stringTable->setItem(row, 1, new QTableWidgetItem(stringValue));
    }
}

void FastFileAssetPoolWidget::LoadFastFileEntries(const QList<QListWidgetItem*>& items) {
    bool loaded{ false };
    int count{ static_cast<int>(items.size()) };
    QProgressDialog progress{ this };
    progress.setWindowTitle("FastFile Asset Pool");
    progress.setCancelButton(nullptr);
    progress.setMinimumDuration(0);
    progress.setRange(0, count + 3);
    progress.setValue(0);
    progress.show();
    QApplication::processEvents();

    int current{ 0 };
    for (QListWidgetItem* item : items) {
        ++current;
        QByteArray name{ item->toolTip().toUtf8() };
        if (name.isEmpty()) {
            continue;
        }

        progress.setLabelText(
            QString("Loading fastfile \"%1\" (%2 / %3)").arg(QString::fromUtf8(name)).arg(current).arg(count)
        );
        ActsStatus status{ ActsAPIFastFile_AssetPoolLoadFastFile(ctx, name.constData(), nullptr, nullptr) };
        if (ACTS_NOT_OK(status)) {
            qWarning("Failed to load fastfile: %s", ActsGetAPILastMessage());
        } else {
            loaded = true;
        }
        progress.setValue(current);
        QApplication::processEvents();
    }

    progress.setLabelText("Refreshing fastfiles");
    QApplication::processEvents();
    RefreshFastFiles(currentSearch);
    progress.setValue(count + 1);
    if (loaded) {
        progress.setLabelText("Refreshing assets");
        QApplication::processEvents();
        RefreshAssets(currentAssetSearch);
        progress.setValue(count + 2);
        QApplication::processEvents();
        progress.setLabelText("Refreshing strings");
        QApplication::processEvents();
        RefreshStrings(currentStringSearch);
        progress.setValue(count + 3);
        QApplication::processEvents();
    } else {
        progress.setValue(count + 3);
    }
}

void FastFileAssetPoolWidget::CopyStringSelection(int column) {
    QStringList lines{};
    for (QTableWidgetItem* item : stringTable->selectedItems()) {
        int row{ item->row() };
        if (column < 0) {
            QString line{ stringTable->item(row, 0)->text() + " " + stringTable->item(row, 1)->text() };
            lines.append(line);
        } else {
            lines.append(stringTable->item(row, column)->text());
        }
    }
    QApplication::clipboard()->setText(lines.join("\n"));
}
