
// Generated from .\grammar\gsc.g4 by ANTLR 4.13.0

#pragma once


#include "antlr4-runtime.h"
#include "gscVisitor.h"


/**
 * This class provides an empty implementation of gscVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class  gscBaseVisitor : public gscVisitor {
public:

  virtual std::any visitProg(gscParser::ProgContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInclude(gscParser::IncludeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNamespace(gscParser::NamespaceContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFunction(gscParser::FunctionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitParam_list(gscParser::Param_listContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitParam_val(gscParser::Param_valContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExpression_list(gscParser::Expression_listContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStatement_block(gscParser::Statement_blockContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStatement(gscParser::StatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStatement_for(gscParser::Statement_forContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStatement_foreach(gscParser::Statement_foreachContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStatement_if(gscParser::Statement_ifContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStatement_inst(gscParser::Statement_instContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFunction_call(gscParser::Function_callContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitOperator_inst(gscParser::Operator_instContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExpression(gscParser::ExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitConst_expr(gscParser::Const_exprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFunction_ref(gscParser::Function_refContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNumber(gscParser::NumberContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVector_value(gscParser::Vector_valueContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitArray_def(gscParser::Array_defContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStruct_def(gscParser::Struct_defContext *ctx) override {
    return visitChildren(ctx);
  }


};

