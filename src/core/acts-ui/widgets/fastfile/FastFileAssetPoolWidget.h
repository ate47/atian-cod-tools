#pragma once

#include <QString>
#include <QList>
#include <QWidget>
#include <acts_api/api.h>
#include <acts_api/fastfile_loader.h>

class UI3MdiArea;
class QListWidget;
class QListWidgetItem;

class FastFileAssetPoolWidget : public QWidget {
	Q_OBJECT

  public:
	FastFileAssetPoolWidget(ActsHandle ctx, UI3MdiArea* mdi);
	~FastFileAssetPoolWidget() override;

  private:
	void RefreshFastFiles(const QString& search);
	void LoadFastFileEntries(const QList<QListWidgetItem*>& items);

	ActsHandle ctx{ INVALID_ACTS_HANDLE_VALUE };
	QListWidget* fastFileList{};
	QString currentSearch{};
};
