
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

  virtual std::any visitFilenamespace(gscParser::FilenamespaceContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitConstexpr(gscParser::ConstexprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitClass_def(gscParser::Class_defContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitClass_var(gscParser::Class_varContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFunction(gscParser::FunctionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDetour_info(gscParser::Detour_infoContext *ctx) override {
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

  virtual std::any visitStatement_dev_block(gscParser::Statement_dev_blockContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStatement_for(gscParser::Statement_forContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStatement_while(gscParser::Statement_whileContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStatement_dowhile(gscParser::Statement_dowhileContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStatement_foreach(gscParser::Statement_foreachContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStatement_if(gscParser::Statement_ifContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStatement_switch(gscParser::Statement_switchContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStatement_inst(gscParser::Statement_instContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNop_def(gscParser::Nop_defContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDevop_def(gscParser::Devop_defContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFunction_component(gscParser::Function_componentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitOperator_inst(gscParser::Operator_instContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExpression(gscParser::ExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSet_expression(gscParser::Set_expressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExpression0(gscParser::Expression0Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExpression1(gscParser::Expression1Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExpression2(gscParser::Expression2Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExpression3(gscParser::Expression3Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExpression4(gscParser::Expression4Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExpression5(gscParser::Expression5Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExpression6(gscParser::Expression6Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExpression7(gscParser::Expression7Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExpression8(gscParser::Expression8Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExpression9(gscParser::Expression9Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExpression10(gscParser::Expression10Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExpression11(gscParser::Expression11Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExpression12(gscParser::Expression12Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExpression13(gscParser::Expression13Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExpression14(gscParser::Expression14Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExpression15(gscParser::Expression15Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIs_expression(gscParser::Is_expressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFunction_call_exp(gscParser::Function_call_expContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFunction_call(gscParser::Function_callContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLeft_value(gscParser::Left_valueContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitConst_expr(gscParser::Const_exprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitConst_expr_static(gscParser::Const_expr_staticContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFunction_ref(gscParser::Function_refContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitData_ref(gscParser::Data_refContext *ctx) override {
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

  virtual std::any visitClass_init(gscParser::Class_initContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIdf(gscParser::IdfContext *ctx) override {
    return visitChildren(ctx);
  }


};

