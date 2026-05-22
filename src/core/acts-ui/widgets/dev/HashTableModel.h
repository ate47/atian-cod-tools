#pragma once
#include <QAbstractTableModel>

struct HashTableKVP {
	uint64_t hash;
	const char* str;
};

class HashTableModel : public QAbstractTableModel {
	const std::vector<HashTableKVP>& hashMap;

	Q_OBJECT

public:
	explicit HashTableModel(const std::vector<HashTableKVP>& hashMap,
		QObject* parent = nullptr)
		: QAbstractTableModel(parent), hashMap(hashMap) {}

	int rowCount(const QModelIndex& parent = QModelIndex()) const override {
		return parent.isValid() ? 0 : (int)hashMap.size();
	}

	int columnCount(const QModelIndex& parent = QModelIndex()) const override {
		return parent.isValid() ? 0 : 2;
	}

	QVariant data(const QModelIndex& index, int role) const override {
		if (!index.isValid() || role != Qt::DisplayRole) {
			return {};
		}

		const HashTableKVP& kv{ hashMap[index.row()] };

		switch (index.column()) {
		case 0: return QString::number(kv.hash, 16);        // hex hash
		case 1: return QString::fromUtf8(kv.str);           // string
		}
		return {};
	}

	QVariant headerData(int section, Qt::Orientation orientation,
		int role) const override {
		if (role != Qt::DisplayRole || orientation != Qt::Horizontal) {
			return {};
		}

		return (section == 0) ? "Hash" : "String";
	}

	Qt::ItemFlags flags(const QModelIndex& index) const override {
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	}
};

