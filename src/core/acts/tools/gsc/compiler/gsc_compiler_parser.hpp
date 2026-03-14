#pragma once

#include <tools/gsc/compiler/gsc_compiler_grammar.hpp>

namespace tool::gsc::compiler {
    class CompileObject;
    class FunctionObject;

	std::string ParseString(TerminalNode* term, size_t start = 0);

    bool ParseFieldNode(ParseTree* exp, gscParser& parser, CompileObject& obj, FunctionObject& fobj);
    bool ParseExpressionNode(ParseTree* exp, gscParser& parser, CompileObject& obj, FunctionObject& fobj, bool expressVal);
    bool ParseClassDef(RuleContext* func, gscParser& parser, CompileObject& obj);
    FunctionObject* ParseFunction(RuleContext* func, gscParser& parser, CompileObject& obj, byte forceFlags = 0);
    bool ParsePrecache(RuleContext* prec, gscParser& parser, CompileObject& obj);
    bool ParseAnimTree(RuleContext* atr, gscParser& parser, CompileObject& obj);
    bool ParseInclude(RuleContext* nsp, gscParser& parser, CompileObject& obj);
    bool ParseNamespace(RuleContext* nsp, gscParser& parser, CompileObject& obj);
    bool ParseFileNamespace(RuleContext* nsp, gscParser& parser, CompileObject& obj);
    bool ParseConstExpr(RuleContext* exp, gscParser& parser, CompileObject& obj);
    bool ParseProg(gscParser::ProgContext* prog, gscParser& parser, CompileObject& obj);
}