#pragma once
#include <core/memory_allocator.hpp>
#include <QWidget>
#include <QAbstractTableModel>
#include <QSortFilterProxyModel>
#include "ui_HashTableWidget.h"
#include "HashTableModel.h"

class HashTableWidget : public QWidget {
	Q_OBJECT

public:
	HashTableWidget(QWidget *parent = nullptr);
	~HashTableWidget();

	void LoadFile(const QString& path);

private:
	Ui::HashTableWidgetClass ui;

	core::memory_allocator::MemoryAllocator alloc{};
	std::vector<HashTableKVP> hashMap{};
	HashTableModel* model;
	QSortFilterProxyModel* proxy;
	QTimer* debounceTimer;
};

