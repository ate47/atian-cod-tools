
// Generated from .\grammar\gsc.g4 by ANTLR 4.13.0

#pragma once


#include "antlr4-runtime.h"
#include "gscParser.h"



/**
 * This class defines an abstract visitor for a parse tree
 * produced by gscParser.
 */
class  gscVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by gscParser.
   */
    virtual std::any visitProg(gscParser::ProgContext *context) = 0;

    virtual std::any visitInclude(gscParser::IncludeContext *context) = 0;

    virtual std::any visitNamespace(gscParser::NamespaceContext *context) = 0;

    virtual std::any visitFunction(gscParser::FunctionContext *context) = 0;

    virtual std::any visitParam_list(gscParser::Param_listContext *context) = 0;

    virtual std::any visitParam_val(gscParser::Param_valContext *context) = 0;

    virtual std::any visitExpression_list(gscParser::Expression_listContext *context) = 0;

    virtual std::any visitStatement_block(gscParser::Statement_blockContext *context) = 0;

    virtual std::any visitStatement(gscParser::StatementContext *context) = 0;

    virtual std::any visitStatement_for(gscParser::Statement_forContext *context) = 0;

    virtual std::any visitStatement_foreach(gscParser::Statement_foreachContext *context) = 0;

    virtual std::any visitStatement_if(gscParser::Statement_ifContext *context) = 0;

    virtual std::any visitStatement_inst(gscParser::Statement_instContext *context) = 0;

    virtual std::any visitFunction_call(gscParser::Function_callContext *context) = 0;

    virtual std::any visitOperator_inst(gscParser::Operator_instContext *context) = 0;

    virtual std::any visitExpression(gscParser::ExpressionContext *context) = 0;

    virtual std::any visitConst_expr(gscParser::Const_exprContext *context) = 0;

    virtual std::any visitFunction_ref(gscParser::Function_refContext *context) = 0;

    virtual std::any visitNumber(gscParser::NumberContext *context) = 0;

    virtual std::any visitVector_value(gscParser::Vector_valueContext *context) = 0;

    virtual std::any visitArray_def(gscParser::Array_defContext *context) = 0;

    virtual std::any visitStruct_def(gscParser::Struct_defContext *context) = 0;


};

