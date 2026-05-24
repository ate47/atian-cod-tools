#pragma once
#include <utils/hash_mini.hpp>
#include <acts_api/config.h>
#include <utils/utils.hpp>
#include <QProperty>
#include <QLineEdit>
#include <QCheckBox>

namespace ui3::config {
	template<class>
	inline constexpr bool always_false_v = false;

	enum PropertyType {
		PT_BOOL,
		PT_INT,
		PT_DOUBLE,
		PT_STRING,
	};

	struct PropertyEntry {
		void* ptr{};
		const char* path;
		const char* description;
		const std::type_info* type;
		PropertyType ptype;
		void (*Load)(void* ptr);
	};

	template<typename T>
	constexpr PropertyType GetPropertyType() {
		if constexpr (std::is_same_v<T, bool>) {
			return PT_BOOL;
		}
		else if constexpr (std::is_same_v<T, int64_t>) {
			return PT_INT;
		}
		else if constexpr (std::is_same_v<T, double>) {
			return PT_DOUBLE;
		}
		else if constexpr (std::is_same_v<T, QString>) {
			return PT_STRING;
		}
		else {
			static_assert(always_false_v<T>, "Unsupported type for Config");
		}
	}

	void SyncConfigVals();
	void SaveConfig();
	std::unordered_map<uint64_t, PropertyEntry>& GetPropertyMap();

	class PropertyNotifier : public QObject {
		Q_OBJECT
		using QObject::QObject;
	signals:
		void changed();
	};

	template<typename T>
	class PropertyContainer{
		PropertyEntry& entry;
		PropertyNotifier notifier{ nullptr };
		QProperty<T> prop;
	public:
		PropertyContainer(PropertyEntry& entry, T defaultVal)
			: entry(entry), prop(defaultVal) {
		}

		void Load() {
			T v{ prop.value() };

			constexpr PropertyType pt = GetPropertyType<T>();

			if constexpr (pt == PropertyType::PT_BOOL) {
				v = ActsAPIConfig_GetBool(entry.path, v);
			}
			else if constexpr (pt == PropertyType::PT_INT) {
				v = (T)ActsAPIConfig_GetInteger(entry.path, (int64_t)v);
			}
			else if constexpr (pt == PropertyType::PT_DOUBLE) {
				v = (T)ActsAPIConfig_GetDouble(entry.path, v);
			}
			else if constexpr (pt == PropertyType::PT_STRING) {
				QByteArray arr{ v.toUtf8() };
				v = ActsAPIConfig_GetString(entry.path, arr.constData());
			}
			else {
				static_assert(always_false_v<T>, "Unsupported type for ConfigVal::Load");
			}

			prop.setValue(v);
			emit notifier.changed();
		}
		void Set(T v) {
			constexpr PropertyType pt = GetPropertyType<T>();

			if constexpr (pt == PropertyType::PT_BOOL) {
				ActsAPIConfig_SetBool(entry.path, v);
			}
			else if constexpr (pt == PropertyType::PT_INT) {
				ActsAPIConfig_SetInteger(entry.path, (int64_t)v);
			}
			else if constexpr (pt == PropertyType::PT_DOUBLE) {
				ActsAPIConfig_SetDouble(entry.path, v);
			}
			else if constexpr (pt == PropertyType::PT_STRING) {
				QByteArray arr{ v.toUtf8() };
				ActsAPIConfig_SetString(entry.path, arr.constData());
			}
			else {
				static_assert(always_false_v<T>, "Unsupported type for ConfigVal::Set");
			}
			SaveConfig();

			prop.setValue(v);
			emit notifier.changed();
		}
		constexpr QProperty<T>& GetProperty() { return prop; }
		constexpr PropertyNotifier& GetNotifier() { return notifier; }
		T Get() { return prop.value(); }
	};

	template<typename T>
	class ConfigVal {
		PropertyContainer<T>& container;
	public:
		ConfigVal(const char* path, T v = {}, const char* description = "") 
			: container(CreateContainer(path, description, v)) {
		}

		void Set(T v) { container.Set(v); }
		T Get() { return container.Get(); }

		template<typename Widget>
		requires std::same_as<Widget, QLineEdit>&& std::same_as<T, QString>
		void Bind(Widget* edit) {
			PropertyContainer<T>& cc{ container };
			QObject::connect(edit, &QLineEdit::textChanged,
				&container.GetNotifier(), [&cc](const QString& s) { cc.Set(s); });
			QObject::connect(&container.GetNotifier(), &PropertyNotifier::changed,
				edit, [&cc, edit]() { edit->setText(cc.Get()); });

			edit->setText(Get());
		}

		template<typename Widget>
		requires std::same_as<Widget, QCheckBox>&& std::same_as<T, bool>
		void Bind(Widget* box) {
			PropertyContainer<T>& cc{ container };
			QObject::connect(box, &QCheckBox::toggled,
				&container.GetNotifier(), [&cc, box](bool val) { cc.Set(val); });

			QObject::connect(&container.GetNotifier(), &PropertyNotifier::changed,
				box, [box, &cc] { box->setChecked(cc.Get()); });

			box->setChecked(Get());
		}

		void OnUpdate(QObject* receiver, std::function<void(T)> action) {
			PropertyContainer<T>& cc{ container };
			QObject::connect(&container.GetNotifier(), &PropertyNotifier::changed,
				receiver, [action, &cc]() { action(cc.Get()); });
		}

		void OnUpdate(QObject* receiver, std::function<void()> action) {
			QObject::connect(&container.GetNotifier(), &PropertyNotifier::changed,
				receiver, [action]() { action(); });
		}

		operator T() {
			return Get();
		}

		T operator=(T other) {
			Set(other);
			return other;
		}
	private:
		PropertyContainer<T>& CreateContainer(const char* path, const char* description, T defaultValue) {
			std::unordered_map<uint64_t, PropertyEntry>& map{ GetPropertyMap() };

			uint64_t ph{ hash::Hash64A(path) };
			PropertyEntry& entry{ map[ph] };

			if (entry.ptr) {
				if (*(entry.type) != typeid(T)) {
					throw std::runtime_error(
						std::format(
							"Config path'{}' type mismatch: {} != {}", 
							path, entry.type->name(), typeid(T).name()
						)
					);
				}
				return *static_cast<PropertyContainer<T>*>(entry.ptr);
			}

			PropertyContainer<T>* cnt{ new PropertyContainer<T>(entry, defaultValue) };
			entry.ptr = cnt;
			entry.type = &typeid(T);
			entry.ptype = GetPropertyType<T>();
			entry.path = path;
			entry.description = description;
			entry.Load = [](void* p) {((PropertyContainer<T>*)p)->Load(); };
			return *cnt;
		}
	};
}

// name, path, defaultValue, description
#define UI_CONFIG_VAL(name, path, defaultVal, description) \
static ui3::config::ConfigVal<decltype(defaultVal)> name{ path, defaultVal, description }
