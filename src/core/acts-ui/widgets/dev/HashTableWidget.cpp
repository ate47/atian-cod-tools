#include <ui_includes.hpp>
#include <deps/dzporter_cdb.hpp>
#include <deps/scobalula_wni.hpp>
#include "HashTableWidget.h"
#include "HashTableModel.h"
#include <QTimer>

HashTableWidget::HashTableWidget(QWidget *parent)
	: QWidget(parent) {
	ui.setupUi(this);
	setLayout(ui.verticalLayout);

	model = new HashTableModel(hashMap, this);

	proxy = new QSortFilterProxyModel(this);
	proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
	proxy->setFilterKeyColumn(-1);
	

	ui.tableView->setModel(proxy);
	ui.tableView->setSortingEnabled(true);

	debounceTimer = new QTimer(this);
	debounceTimer->setSingleShot(true);
	debounceTimer->setInterval(500);

	connect(debounceTimer, &QTimer::timeout, this, [this]() {
		proxy->setFilterFixedString(ui.searchEdit->text());
	});

	connect(ui.searchEdit, &QLineEdit::textChanged, this, [this]() {
		debounceTimer->start();
	});
}

HashTableWidget::~HashTableWidget() {}



void HashTableWidget::LoadFile(const QString& path) {
	
	hashMap.clear();
	alloc.FreeAll();

	if (path.endsWith(".wni", Qt::CaseInsensitive)) {
		deps::scobalula::wni::ReadWNIFile(path.toStdString(), [this](uint64_t hash, const char* str) {
			hashMap.emplace_back(hash, str);
		}, [this](size_t len) -> void* {
			return alloc.Alloc<byte>(len);
		});
	}
	else if (path.endsWith(".cdb", Qt::CaseInsensitive)) {
		deps::dzporter::cdb::ReadCDBFile(path.toStdString(), [this](uint64_t hash, const char* str) {
			hashMap.emplace_back(hash, str);
		}, [this](size_t len) -> void* {
			return alloc.Alloc<byte>(len);
		});
	}

	setWindowTitle(QString("%1 - %2 hash(es)").arg(path).arg(hashMap.size()));

	proxy->setSourceModel(model);
	ui.tableView->resizeColumnsToContents();
}

ADD_UI_TOOL(HashTableWidget, "Hash Table", nullptr, ".wni,.cdb", true);