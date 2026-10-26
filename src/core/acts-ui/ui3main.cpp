#include <ui_includes.hpp>
#include <acts_api/internal.h>
#include <acts_api/internal_ui.h>
#include <acts_api/http_server.h>
#include <core/actsinfo.hpp>
#include <core/shared_cfg_data.hpp>
#include <QApplication>
#include <QWidget>
#include <QFile>
#include <QTimer>
#include <MainWindow.h>
#include <config_ui.hpp>
#include <platform/platform_windows.hpp>
#include <QMessageBox>

UI_CONFIG_VAL(cfgHttpServerHost, "http.server.host", QString(ACTS_HTTP_DEFAULT_HOST), "http server host");
UI_CONFIG_VAL(cfgHttpServerPort, "http.server.port", (uint16_t)ACTS_HTTP_DEFAULT_PORT, "http server port");
UI_CONFIG_VAL(cfgHttpServerEnabled, "http.server.enabled", false, "enable http server");
UI_CONFIG_VAL(cfgHttpServerMcpTools, "http.server.mcpTools", true, "enable mcp server");

namespace {
    static MainWindow* mainWindow{};

    void ActsQTMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg) {
        core::logs::loglevel lvl;
        switch (type) {
        case QtDebugMsg:
            lvl = core::logs::LVL_DEBUG;
            break;
        case QtInfoMsg:
            lvl = core::logs::LVL_INFO;
            break;
        case QtWarningMsg:
            lvl = core::logs::LVL_WARNING;
            break;
        case QtCriticalMsg:
        case QtFatalMsg:
        default:
            lvl = core::logs::LVL_ERROR;
            break;
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
        } else {
            category = "QT/default";
        }
        core::logs::log(lvl, category, context.line, localMsg.constData());
    }
} // namespace

MainWindow* GetMainWindow() { return mainWindow; }

UI3MdiArea* GetMainArea() { return mainWindow->GetMdiArea(); }

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    return TRUE; // ignore
}

int MainActsUI(int argc, const char** argv) {
    core::shared_cfg::SetSharedConfigPtr(GetActsSharedConfig());
    // load acts api data
    int r{ InitActsAPI(false, &argc, (const char***)&argv, core::actsinfo::BUILD_VERSION_ID) };
    if (r) {
        platform::InfoMessageBox(
            "ACTS API Error",
            "Failed to initialize ACTS API! Check the acts-ui.logs for more information.",
            false,
            true
        );
        return r;
    }
    if (core::logs::getlevel() <= core::logs::LVL_TRACE) {
        // logs from QT are really verbose, so we only enable them if we're in trace mode
        QLoggingCategory::setFilterRules(
            "qt.core.library\n"
            //"qt.*=true\n"
        );
    }

    ui3::config::SyncConfigVals();

    qInstallMessageHandler(ActsQTMessageHandler);

    qCDebug(lcActs) << "Starting acts-ui3 " << core::actsinfo::VERSION << " with QT " QT_VERSION_STR << "("
                    << QT_VERSION << ", q" << qVersion() << ") mscv version" << _MSC_VER;

    std::filesystem::path workDir{ utils::GetProgDir() / "platforms" };
    std::string workDirStr{ workDir.string() };

    const char* argv2[]{ argv[0], "--platformpluginpath", workDirStr.data() };
    int argc2 = ACTS_ARRAYSIZE(argv2);
    QApplication app{ argc2, (char**)argv2 };

    QFile file(":/theme.qss");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        app.setStyleSheet(file.readAll());
    }

    MainWindow window;
    mainWindow = &window;
    window.show();
    window.setWindowTitle(QString::asprintf("Atian Tools (%s)", core::actsinfo::VERSION));

    if (argc) {
        for (size_t i = 1; i < argc; i++) {
            window.OpenFile(argv[i]);
        }
    }

    // sync the http server loop to qt
    QTimer httpTimer{};
    ActsHandle httpServerHandle{ INVALID_ACTS_HANDLE_VALUE };
    if (cfgHttpServerEnabled) {
        ActsAPIHttpServer_Options httpOpts{};
        httpOpts.structSize = sizeof(httpOpts);
        QByteArray host{ cfgHttpServerHost.Get().toUtf8() };
        httpOpts.host = host.constData();
        httpOpts.port = cfgHttpServerPort.Get();
        httpOpts.noMCP = !cfgHttpServerMcpTools.Get();

        httpServerHandle = ActsAPIHttpServer_Start(&httpOpts);
        if (IS_ACTS_HANDLE_VALID(httpServerHandle)) {
            QObject::connect(&httpTimer, &QTimer::timeout, [httpServerHandle]() {
                ActsAPIHttpServer_Poll(httpServerHandle);
            });
            httpTimer.start(10);
        } else {
            LOG_ERROR("Can't start the http server: {}", ActsGetAPILastMessage());
        }
    }

    int exitCode{ app.exec() };

    httpTimer.stop();
    ActsAPICloseHandle(httpServerHandle);
    return exitCode;
}
