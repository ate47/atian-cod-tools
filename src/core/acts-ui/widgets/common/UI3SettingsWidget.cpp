#include <ui_includes.hpp>
#include "UI3SettingsWidget.h"
#include <config_ui.hpp>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QLabel>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <limits>

namespace {
    // Bind a spinbox if the entry holds an integer of type T, the ConfigVal type must match the entry type
    template<typename T>
    bool TryBindInteger(ui3::config::PropertyEntry& entry, QHBoxLayout* row) {
        if (*entry.type != typeid(T)) {
            return false;
        }
        constexpr int64_t minVal{
            std::numeric_limits<T>::min() < 0
                ? std::max<int64_t>(std::numeric_limits<int>::min(), std::numeric_limits<T>::min())
                : 0
        };
        constexpr int maxVal{ (uint64_t)std::numeric_limits<T>::max() > (uint64_t)std::numeric_limits<int>::max()
                                  ? std::numeric_limits<int>::max()
                                  : (int)std::numeric_limits<T>::max() };

        QSpinBox* spin{ new QSpinBox };
        spin->setRange((int)minVal, maxVal);
        ui3::config::ConfigVal<T>(entry.path).Bind(spin);
        row->addWidget(spin);
        return true;
    }

    // Bind a double spinbox if the entry holds a floating point of type T
    template<typename T>
    bool TryBindFloating(ui3::config::PropertyEntry& entry, QHBoxLayout* row) {
        if (*entry.type != typeid(T)) {
            return false;
        }
        QDoubleSpinBox* spin{ new QDoubleSpinBox };
        spin->setDecimals(6);
        spin->setRange(-1e12, 1e12);
        ui3::config::ConfigVal<T>(entry.path).Bind(spin);
        row->addWidget(spin);
        return true;
    }

    QHBoxLayout* CreateEditor(ui3::config::PropertyEntry& entry) {
        QHBoxLayout* row{ new QHBoxLayout() };
        QLabel* label{ new QLabel(
            QString::asprintf(
                "%s (%s)",
                entry.description && *entry.description ? entry.description : entry.path,
                entry.path
            )
        ) };
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
        case ui3::config::PropertyType::PT_INT: {
            // the integer type is only known at runtime, try the supported ones
            TryBindInteger<int>(entry, row) || TryBindInteger<unsigned int>(entry, row) ||
                TryBindInteger<int16_t>(entry, row) || TryBindInteger<uint16_t>(entry, row) ||
                TryBindInteger<long>(entry, row) || TryBindInteger<unsigned long>(entry, row) ||
                TryBindInteger<int64_t>(entry, row) || TryBindInteger<uint64_t>(entry, row);
            break;
        }
        case ui3::config::PropertyType::PT_DOUBLE: {
            TryBindFloating<double>(entry, row) || TryBindFloating<float>(entry, row);
            break;
        }
        default: {
            break;
        }
        }
        return row;
    }
} // namespace
UI3SettingsWidget::UI3SettingsWidget(QWidget* parent) : QWidget(parent) {
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