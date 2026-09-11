import * as assert from "node:assert/strict";
import * as vscode from "vscode";

suite("Extension Test Suite", () => {
  test("registers the GSC language", async () => {
    const languages = await vscode.languages.getLanguages();
    assert.ok(languages.includes("gsc"));
  });
});
