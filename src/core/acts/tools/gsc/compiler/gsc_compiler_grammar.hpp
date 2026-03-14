#pragma once
#include <tools/gsc/compiler/grammar/gscBaseVisitor.h>
#include <tools/gsc/compiler/grammar/gscLexer.h>
#include <tools/gsc/compiler/grammar/gscParser.h>
#include <tools/gsc/compiler/grammar/gscVisitor.h>
#include <core/preprocessor.hpp>

namespace tool::gsc::compiler {
    using namespace antlr4;
    using namespace antlr4::tree;

    constexpr ParseTreeType TREE_ERROR = ParseTreeType::ERROR;
    constexpr ParseTreeType TREE_RULE = ParseTreeType::RULE;
    constexpr ParseTreeType TREE_TERMINAL = ParseTreeType::TERMINAL;

    struct InputInfo {
        core::preprocessor::StringContainer container{};


        Token* GetToken(ParseTree* tree) const {
            while (tree && tree->getTreeType() != TREE_TERMINAL && tree->children.size()) {
                tree = tree->children[0];
            }

            if (tree && tree->getTreeType() == TREE_TERMINAL) {
                return dynamic_cast<TerminalNode*>(tree)->getSymbol();
            }
            return nullptr;
        }
        void PrintLineMessage(core::logs::loglevel lvl, ParseTree* tree, const std::string& msg) const {
            Token* token = GetToken(tree);
            if (token) {
                container.PrintLineMessage(lvl, token->getLine(), token->getCharPositionInLine(), msg);
                return;
            }

            LOG_LVL(lvl, msg);
        }
    };
}

#define IS_RULE_TYPE(rule, index) (rule && rule->getTreeType() == TREE_RULE && dynamic_cast<RuleContext*>(rule)->getRuleIndex() == index)
#define IS_TERMINAL_TYPE(term, index) (term && term->getTreeType() == TREE_TERMINAL && dynamic_cast<TerminalNode*>(term)->getSymbol()->getType() == index)
#define IS_IDF(rule) (IS_RULE_TYPE((rule), gscParser::RuleIdf) || IS_TERMINAL_TYPE((rule), gscParser::IDENTIFIER))