#pragma once

#include <QList>
#include <QString>
#include <string>
#include <unordered_map>
#include <vector>
#include <QWidget>
#include <acts_api/api.h>
#include <acts_api/fastfile_loader.h>

class UI3MdiArea;
class QListWidget;
class QListWidgetItem;
class QTableWidget;
class QTreeWidget;

class FastFileAssetPoolWidget : public QWidget {
	Q_OBJECT

  public:
	FastFileAssetPoolWidget(ActsHandle ctx, UI3MdiArea* mdi);
	~FastFileAssetPoolWidget() override;

  private:
	void RefreshFastFiles(const QString& search);
	void RefreshAssets(const QString& search);
	void RefreshStrings(const QString& search);
	void FilterAssets(const QString& search);
	void FilterStrings(const QString& search);
	void LoadFastFileEntries(const QList<QListWidgetItem*>& items);
	void CopyStringSelection(int column);

	struct AssetEntry {
		uint64_t poolId{};
		std::string poolName{};
		uint64_t hash{};
		std::string name{};
	};
	struct AssetKey {
		uint64_t poolId{};
		uint64_t hash{};

		bool operator==(const AssetKey& other) const {
			return poolId == other.poolId && hash == other.hash;
		}
	};

	struct AssetKeyHash {
		size_t operator()(const AssetKey& key) const {
			return std::hash<uint64_t>{}(key.poolId) ^ (std::hash<uint64_t>{}(key.hash) << 1);
		}
	};

	ActsHandle ctx{ INVALID_ACTS_HANDLE_VALUE };
	QListWidget* fastFileList{};
	QTreeWidget* assetList{};
	QTableWidget* stringTable{};
	QString currentSearch{};
	QString currentAssetSearch{};
	QString currentStringSearch{};
	std::unordered_map<AssetKey, AssetEntry, AssetKeyHash> assets{};
	std::unordered_map<uint64_t, std::string> strings{};
};
