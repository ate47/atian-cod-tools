#pragma once

#include <QWidget>

class QCheckBox;
class QComboBox;
class QLineEdit;

class FastFileAssetPoolConfigWidget : public QWidget {
	Q_OBJECT

  public:
	explicit FastFileAssetPoolConfigWidget(QWidget* parent = nullptr);

  private:
	void BrowseGamePath();
	void BrowseOutputPath();
	void CreateAssetPool();

	QLineEdit* gamePathEdit{};
	QComboBox* handlerEdit{};
	QLineEdit* outputPathEdit{};
	QCheckBox* patchCheck{};
};
