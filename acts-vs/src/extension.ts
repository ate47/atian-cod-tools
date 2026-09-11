import * as vscode from "vscode";
import {
  LanguageClient,
  type LanguageClientOptions,
  type ServerOptions,
} from "vscode-languageclient/node";

let client: LanguageClient | undefined;

export function activate(context: vscode.ExtensionContext): void {
  const configuration = vscode.workspace.getConfiguration("acts");
  const command = configuration.get<string>("languageServerPath", "acts");
  const args = configuration.get<string[]>("languageServerArgs", [
    "--no-title", // remove title
    "--noUpdater", // disable updater
    "--log", "w", // only warnings and error, the rest isn't in logs
    "language_server", // tool
    "gsc" // language
  ]);
  const configuredCwd = configuration.get<string>("languageServerCwd", "");
  const serverOptions: ServerOptions = {
    run: {
      command,
      args,
      options: configuredCwd ? { cwd: configuredCwd } : undefined,
    },
    debug: {
      command,
      args,
      options: configuredCwd ? { cwd: configuredCwd } : undefined,
    },
  };
  const clientOptions: LanguageClientOptions = {
    documentSelector: [{ scheme: "file", language: "gsc" }],
  };

  client = new LanguageClient(
    "gscLanguageServer",
    "GSC Language Server",
    serverOptions,
    clientOptions,
  );
  context.subscriptions.push(client);
  void client.start();
}

export function deactivate(): Thenable<void> | undefined {
  return client?.stop();
}
