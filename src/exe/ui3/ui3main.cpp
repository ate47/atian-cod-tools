#include <ui_includes.hpp>
#include <acts_api/internal.h>
#include <core/actsinfo.hpp>
#include <core/shared_cfg_data.hpp>
#include <QApplication>
#include <QWidget>
#include <MainWindow.h>

namespace {
    static MainWindow* mainWindow{};

    void ActsQTMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg) {
        core::logs::loglevel lvl;
        switch (type) {
        case QtDebugMsg: lvl = core::logs::LVL_DEBUG; break;
        case QtInfoMsg: lvl = core::logs::LVL_INFO; break;
        case QtWarningMsg: lvl = core::logs::LVL_WARNING; break;
        case QtCriticalMsg:
        case QtFatalMsg:
        default: lvl = core::logs::LVL_ERROR; break;
        }

        if (!HAS_LOG_LEVEL(lvl)) {
            return;
        }

        QByteArray localMsg{ msg.toLocal8Bit() };

        const char* category;

        if (context.category) {
            char* pcat = utils::va("QT/%s", context.category);
			utils::MapString(pcat + 3, [](char c) -> char { return c == '.' ? ':' : c; });
			category = pcat;
        }
        else {
			category = "QT/default";
        }
        if (!std::strcmp(category, "QT/qt:core:plugin:factoryloader")) {
            LOG_TRACE("test");
        }
        core::logs::log(lvl, category, context.line, localMsg.constData());
    }
}

MainWindow* GetMainWindow() {
    return mainWindow;
}

int main(int argc, char* argv[]) {
    core::shared_cfg::SetSharedConfigPtr(GetActsSharedConfig());
    // load acts api data
    int r{ InitActsAPI(false, &argc, (const char***)&argv, core::actsinfo::BUILD_VERSION_ID) };
    if (r) {
		MessageBoxA(NULL, "Failed to initialize ACTS API! Check the acts-ui.logs for more information.", "ACTS API Error", MB_OK | MB_ICONERROR);
        return r;
    }
    if (core::logs::getlevel() <= core::logs::LVL_TRACE) {
		// logs from QT are really verbose, so we only enable them if we're in trace mode
        QLoggingCategory::setFilterRules(
            "qt.core.library\n"
            //"qt.*=true\n"
        );
    }

    qInstallMessageHandler(ActsQTMessageHandler);

	qCDebug(lcActs) << "Starting acts-ui3 " << core::actsinfo::VERSION << " with QT " QT_VERSION_STR << " (q" << qVersion() << ") mscv version" << _MSC_VER;

    std::filesystem::path workDir{ utils::GetProgDir() / "platforms" };
	std::string workDirStr{ workDir.string() };

    const char* argv2[]{
        argv[0],
        "--platformpluginpath", workDirStr.data()
    };
    int argc2 = ARRAYSIZE(argv2);
    QApplication app{ argc2, (char**)argv2};

    MainWindow window;
    mainWindow = &window;
    window.show();
    window.setWindowTitle(QString::asprintf("Atian Tools (%s)", core::actsinfo::VERSION));
    return app.exec();
}
