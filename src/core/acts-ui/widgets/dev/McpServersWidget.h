#pragma once

#include <QWidget>
#include <QTreeWidget>
#include <QLabel>
#include <QPushButton>
#include <QPlainTextEdit>

/*
 * List the MCP endpoints (all the tools and one by category) of the http server with their tools
 */
class McpServersWidget : public QWidget {
    Q_OBJECT

  public:
    McpServersWidget(QWidget* parent = nullptr);
    ~McpServersWidget();

  private:
    // rebuild the status and the endpoints, called when the http server config changes
    void Refresh();
    // update the details of the selected item
    void UpdateSelection();

    QLabel* statusLabel;
    QTreeWidget* tree;
    QPushButton* restartButton;
    QPushButton* copyButton;
    QPlainTextEdit* details;
};
