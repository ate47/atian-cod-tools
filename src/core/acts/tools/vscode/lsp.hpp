#pragma once

namespace tool::vscode::lsp {

    enum JsonRPCError {
        RPC_ParseError = -32700,
        RPC_InvalidRequest = -32600,
        RPC_MethodNotFound = -32601,
        RPC_InvalidParams = -32602,
        RPC_InternalError = -32603,
    };

    enum TextDocumentSyncKind {
        TDSCK_None = 0,
        TDSCK_Full = 1,
        TDSCK_Incremental = 2,
    };

    enum SymbolKind {
        SK_File = 1,
        SK_Module = 2,
        SK_Namespace = 3,
        SK_Package = 4,
        SK_Class = 5,
        SK_Method = 6,
        SK_Property = 7,
        SK_Field = 8,
        SK_Constructor = 9,
        SK_Enum = 10,
        SK_Interface = 11,
        SK_Function = 12,
        SK_Variable = 13,
        SK_Constant = 14,
        SK_String = 15,
        SK_Number = 16,
        SK_Boolean = 17,
        SK_Array = 18,
        SK_Object = 19,
        SK_Key = 20,
        SK_Null = 21,
        SK_EnumMember = 22,
        SK_Struct = 23,
        SK_Event = 24,
        SK_Operator = 25,
        SK_TypeParameter = 26,
    };
} // namespace tool::vscode::lsp