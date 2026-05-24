#include <ui_includes.hpp>
#include "UI3SettingsWidget.h"
#include <config_ui.hpp>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QLabel>


namespace {
    QHBoxLayout* CreateEditor(ui3::config::PropertyEntry& entry) {
        QHBoxLayout* row{ new QHBoxLayout() };
        QLabel* label{ new QLabel(QString::asprintf("%s (%s)", entry.description && *entry.description ? entry.description : entry.path, entry.path)) };
        label->setMinimumWidth(300);
        row->addWidget(label);
        
        switch (entry.ptype) {
        case ui3::config::PropertyType::PT_BOOL: {
            QCheckBox* box = new QCheckBox;
            auto* cnt = static_cast<ui3::config::PropertyContainer<bool>*>(entry.ptr);
            ui3::config::ConfigVal<bool>(entry.path).Bind(box);
            row->addWidget(box);
            break;
        }
        case ui3::config::PropertyType::PT_STRING: {
            QLineEdit* edit = new QLineEdit;
            auto* cnt = static_cast<ui3::config::PropertyContainer<QString>*>(entry.ptr);
            ui3::config::ConfigVal<QString>(entry.path).Bind(edit);
            row->addWidget(edit);
            break;
        }
        default: {
            break;
        }
        }
        return row;
    }
}
UI3SettingsWidget::UI3SettingsWidget(QWidget *parent)
	: QWidget(parent) {
	ui.setupUi(this);

    QScrollArea* scroll{ new QScrollArea(this) };
    scroll->setWidgetResizable(true);

    QWidget* content{ new QWidget(scroll) };
    QVBoxLayout* layout{ new QVBoxLayout(content) };

    for (ui3::config::PropertyEntry* entry : ui3::config::GetPropertyList()) {
        layout->addLayout(CreateEditor(*entry));
    }

    layout->addStretch();
    scroll->setWidget(content);

    QVBoxLayout* main{ new QVBoxLayout(this) };
    main->setContentsMargins(0, 0, 0, 0);
    main->addWidget(scroll);
}

UI3SettingsWidget::~UI3SettingsWidget() {}


ADD_UI_TOOL(UI3SettingsWidget, "Settings", "Utilities");