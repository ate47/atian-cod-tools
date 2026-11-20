#include <ui_includes.hpp>
#include "McpServersWidget.h"
#include <http_server_ui.hpp>
#include <acts_api/http_server.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QSplitter>
#include <QHeaderView>
#include <QJsonDocument>
#include <QFontDatabase>
#include <QApplication>
#include <QClipboard>
#include <map>

namespace {
    // text shown in the details panel
    constexpr int ROLE_DETAILS = Qt::UserRole;
    // url of the endpoint, empty for the tools
    constexpr int ROLE_URL = Qt::UserRole + 1;

    // copy of a tool sent by ActsAPIHttpServer_ListMcpTools
    struct McpToolInfo {
        QString name{};
        QString category{};
        QString description{};
        // JSON schema of the arguments, empty if there is none
        QByteArray schema{};
    };

    using McpToolList = std::vector<const McpToolInfo*>;

    bool CompareToolNames(const McpToolInfo* a, const McpToolInfo* b) { return a->name < b->name; }

    QString ToolDetails(const McpToolInfo& tool) {
        QString text{ tool.name };
        text += QString("\nCategory: %1").arg(tool.category);
        text += QString("\nEndpoints: /mcp, /mcp/%1").arg(tool.category);
        text += QString("\n\n%1").arg(tool.description);
        text += "\n\nArguments (JSON schema):\n";

        if (tool.schema.isEmpty()) {
            return text + "none";
        }
        QJsonParseError err{};
        QJsonDocument doc{ QJsonDocument::fromJson(tool.schema, &err) };
        if (err.error != QJsonParseError::NoError) {
            return text + QString::fromUtf8(tool.schema); // show it as is
        }
        return text + QString::fromUtf8(doc.toJson(QJsonDocument::Indented));
    }

    QTreeWidgetItem* AddEndpoint(QTreeWidget* tree, const QString& path, const QString& url, const McpToolList& tools) {
        QTreeWidgetItem* endpoint{ new QTreeWidgetItem(tree) };
        endpoint->setText(0, path);
        endpoint->setText(1, QString("%1 (%2 tool(s))").arg(url).arg(tools.size()));
        endpoint->setData(0, ROLE_URL, url);

        QString details{ QString("Endpoint: %1\nURL: %2\nTools (%3):").arg(path, url).arg(tools.size()) };
        for (const McpToolInfo* tool : tools) {
            details += QString("\n- %1").arg(tool->name);

            QTreeWidgetItem* item{ new QTreeWidgetItem(endpoint) };
            item->setText(0, tool->name);
            item->setText(1, tool->description);
            item->setData(0, ROLE_DETAILS, ToolDetails(*tool));
        }
        endpoint->setData(0, ROLE_DETAILS, details);
        return endpoint;
    }
} // namespace

McpServersWidget::McpServersWidget(QWidget* parent) : QWidget(parent) {
    setObjectName("McpServersWidgetClass");
    resize(800, 500);
    setWindowTitle("MCP Servers");

    statusLabel = new QLabel();
    statusLabel->setWordWrap(true);
    statusLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    tree = new QTreeWidget();
    tree->setColumnCount(2);
    tree->setHeaderLabels({ "Endpoint / Tool", "Description" });
    tree->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);

    restartButton = new QPushButton("Restart http server");
    copyButton = new QPushButton("Copy endpoint URL");
    copyButton->setEnabled(false);

    details = new QPlainTextEdit();
    details->setReadOnly(true);
    details->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

    QSplitter* splitter{ new QSplitter(Qt::Vertical) };
    splitter->addWidget(tree);
    splitter->addWidget(details);
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 2);

    QHBoxLayout* footer{ new QHBoxLayout() };
    footer->addWidget(restartButton);
    footer->addStretch();
    footer->addWidget(copyButton);

    // http server config, the values are saved when they are modified
    QCheckBox* enableHttpCheck{ new QCheckBox("Enable http server") };
    QCheckBox* enableMcpCheck{ new QCheckBox("Enable MCP tools") };
    QLineEdit* hostEdit{ new QLineEdit() };
    QSpinBox* portSpin{ new QSpinBox() };
    portSpin->setRange(1, UINT16_MAX);
    hostEdit->setPlaceholderText(ACTS_HTTP_DEFAULT_HOST);

    cfgHttpServerEnabled.Bind(enableHttpCheck);
    cfgHttpServerMcpTools.Bind(enableMcpCheck);
    cfgHttpServerHost.Bind(hostEdit);
    cfgHttpServerPort.Bind(portSpin);

    QHBoxLayout* enableLayout{ new QHBoxLayout() };
    enableLayout->addWidget(enableHttpCheck);
    enableLayout->addWidget(enableMcpCheck);
    enableLayout->addStretch();

    QHBoxLayout* addressLayout{ new QHBoxLayout() };
    addressLayout->addWidget(new QLabel("Host"));
    addressLayout->addWidget(hostEdit, 1);
    addressLayout->addWidget(new QLabel("Port"));
    addressLayout->addWidget(portSpin);

    QVBoxLayout* layout{ new QVBoxLayout() };
    layout->addLayout(enableLayout);
    layout->addLayout(addressLayout);
    layout->addWidget(statusLabel);
    layout->addWidget(splitter, 1);
    layout->addLayout(footer);
    setLayout(layout);
    setMinimumWidth(600);
    setMinimumHeight(400);

    connect(tree, &QTreeWidget::currentItemChanged, this, [this]() { UpdateSelection(); });
    connect(copyButton, &QPushButton::clicked, this, [this]() {
        QTreeWidgetItem* item{ tree->currentItem() };
        if (item) {
            QApplication::clipboard()->setText(item->data(0, ROLE_URL).toString());
        }
    });
    connect(restartButton, &QPushButton::clicked, this, [this]() {
        ui3::http_server::Restart();
        Refresh();
    });

    // the status and the endpoints depend on the http server config
    cfgHttpServerHost.OnUpdate(this, [this]() { Refresh(); });
    cfgHttpServerPort.OnUpdate(this, [this]() { Refresh(); });
    cfgHttpServerEnabled.OnUpdate(this, [this]() { Refresh(); });
    cfgHttpServerMcpTools.OnUpdate(this, [this]() { Refresh(); });

    Refresh();
}

McpServersWidget::~McpServersWidget() {}

void McpServersWidget::Refresh() {
    bool running{ ui3::http_server::IsRunning() };

    // the endpoints of the running server, or the ones the server would have with the current config
    QString base{ running ? ui3::http_server::GetRunningUrl()
                          : QString("http://%1:%2").arg(cfgHttpServerHost.Get()).arg(cfgHttpServerPort.Get()) };

    QString status{};
    if (running) {
        if (ui3::http_server::IsRunningMcp()) {
            status = QString("The http server is running, the MCP server is available on %1/mcp").arg(base);
        } else {
            status = QString("The http server is running on %1, the MCP tools are disabled.").arg(base);
        }
    } else if (!cfgHttpServerEnabled.Get()) {
        status = "The http server is disabled.";
    } else if (!ui3::http_server::GetLastError().isEmpty()) {
        status = QString("The http server can't start: %1").arg(ui3::http_server::GetLastError());
    } else {
        status = "The http server isn't running.";
    }
    if (ui3::http_server::NeedsRestart()) {
        status += "\nThe config doesn't match the server, restart the http server to apply it.";
    }
    statusLabel->setText(status);

    // copy the tools, the pointers sent by the callback are only valid during the call
    std::vector<McpToolInfo> tools{};
    ActsStatus listStatus{ ActsAPIHttpServer_ListMcpTools(
        [](const ActsAPIHttpServer_McpTool* tool, void* ud) -> bool {
            std::vector<McpToolInfo>& list{ *(std::vector<McpToolInfo>*)ud };
            McpToolInfo& info{ list.emplace_back() };
            info.name = QString::fromUtf8(tool->name);
            info.category = QString::fromUtf8(tool->category);
            info.description = QString::fromUtf8(tool->description);
            if (tool->inputSchema) {
                info.schema = tool->inputSchema;
            }
            return true;
        },
        &tools
    ) };
    if (listStatus != ACTS_STATUS_OK) {
        statusLabel->setText(
            QString("%1\nCan't list the MCP tools: %2").arg(status, QString::fromUtf8(ActsGetAPILastMessage()))
        );
    }

    // the tools are sorted by name, the categories are sorted by the map
    McpToolList all{};
    std::map<QString, McpToolList> categories{};
    for (const McpToolInfo& tool : tools) {
        all.push_back(&tool);
        categories[tool.category].push_back(&tool);
    }
    std::sort(all.begin(), all.end(), CompareToolNames);

    tree->clear();
    AddEndpoint(tree, "/mcp", base + "/mcp", all);
    for (std::pair<const QString, McpToolList>& category : categories) {
        std::sort(category.second.begin(), category.second.end(), CompareToolNames);
        AddEndpoint(tree, "/mcp/" + category.first, QString("%1/mcp/%2").arg(base, category.first), category.second);
    }
    tree->expandToDepth(0);
    UpdateSelection();
}

void McpServersWidget::UpdateSelection() {
    QTreeWidgetItem* item{ tree->currentItem() };
    if (!item) {
        details->clear();
        copyButton->setEnabled(false);
        return;
    }
    details->setPlainText(item->data(0, ROLE_DETAILS).toString());
    copyButton->setEnabled(!item->data(0, ROLE_URL).toString().isEmpty());
}

ADD_UI_TOOL(McpServersWidget, "MCP Servers", "Utilities");
