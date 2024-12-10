
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

    virtual std::any visitFilenamespace(gscParser::FilenamespaceContext *context) = 0;

    virtual std::any visitConstexpr(gscParser::ConstexprContext *context) = 0;

    virtual std::any visitClass_def(gscParser::Class_defContext *context) = 0;

    virtual std::any visitClass_var(gscParser::Class_varContext *context) = 0;

    virtual std::any visitFunction(gscParser::FunctionContext *context) = 0;

    virtual std::any visitDetour_info(gscParser::Detour_infoContext *context) = 0;

    virtual std::any visitParam_list(gscParser::Param_listContext *context) = 0;

    virtual std::any visitParam_val(gscParser::Param_valContext *context) = 0;

    virtual std::any visitExpression_list(gscParser::Expression_listContext *context) = 0;

    virtual std::any visitStatement_block(gscParser::Statement_blockContext *context) = 0;

    virtual std::any visitStatement(gscParser::StatementContext *context) = 0;

    virtual std::any visitStatement_dev_block(gscParser::Statement_dev_blockContext *context) = 0;

    virtual std::any visitStatement_for(gscParser::Statement_forContext *context) = 0;

    virtual std::any visitStatement_while(gscParser::Statement_whileContext *context) = 0;

    virtual std::any visitStatement_dowhile(gscParser::Statement_dowhileContext *context) = 0;

    virtual std::any visitStatement_foreach(gscParser::Statement_foreachContext *context) = 0;

    virtual std::any visitStatement_if(gscParser::Statement_ifContext *context) = 0;

    virtual std::any visitStatement_switch(gscParser::Statement_switchContext *context) = 0;

    virtual std::any visitStatement_inst(gscParser::Statement_instContext *context) = 0;

    virtual std::any visitNop_def(gscParser::Nop_defContext *context) = 0;

    virtual std::any visitDevop_def(gscParser::Devop_defContext *context) = 0;

    virtual std::any visitFunction_component(gscParser::Function_componentContext *context) = 0;

    virtual std::any visitOperator_inst(gscParser::Operator_instContext *context) = 0;

    virtual std::any visitExpression(gscParser::ExpressionContext *context) = 0;

    virtual std::any visitSet_expression(gscParser::Set_expressionContext *context) = 0;

    virtual std::any visitExpression0(gscParser::Expression0Context *context) = 0;

    virtual std::any visitExpression1(gscParser::Expression1Context *context) = 0;

    virtual std::any visitExpression2(gscParser::Expression2Context *context) = 0;

    virtual std::any visitExpression3(gscParser::Expression3Context *context) = 0;

    virtual std::any visitExpression4(gscParser::Expression4Context *context) = 0;

    virtual std::any visitExpression5(gscParser::Expression5Context *context) = 0;

    virtual std::any visitExpression6(gscParser::Expression6Context *context) = 0;

    virtual std::any visitExpression7(gscParser::Expression7Context *context) = 0;

    virtual std::any visitExpression8(gscParser::Expression8Context *context) = 0;

    virtual std::any visitExpression9(gscParser::Expression9Context *context) = 0;

    virtual std::any visitExpression10(gscParser::Expression10Context *context) = 0;

    virtual std::any visitExpression11(gscParser::Expression11Context *context) = 0;

    virtual std::any visitExpression12(gscParser::Expression12Context *context) = 0;

    virtual std::any visitExpression13(gscParser::Expression13Context *context) = 0;

    virtual std::any visitExpression14(gscParser::Expression14Context *context) = 0;

    virtual std::any visitExpression15(gscParser::Expression15Context *context) = 0;

    virtual std::any visitIs_expression(gscParser::Is_expressionContext *context) = 0;

    virtual std::any visitFunction_call_exp(gscParser::Function_call_expContext *context) = 0;

    virtual std::any visitFunction_call(gscParser::Function_callContext *context) = 0;

    virtual std::any visitLeft_value(gscParser::Left_valueContext *context) = 0;

    virtual std::any visitConst_expr(gscParser::Const_exprContext *context) = 0;

    virtual std::any visitConst_expr_static(gscParser::Const_expr_staticContext *context) = 0;

    virtual std::any visitFunction_ref(gscParser::Function_refContext *context) = 0;

    virtual std::any visitData_ref(gscParser::Data_refContext *context) = 0;

    virtual std::any visitNumber(gscParser::NumberContext *context) = 0;

    virtual std::any visitVector_value(gscParser::Vector_valueContext *context) = 0;

    virtual std::any visitArray_def(gscParser::Array_defContext *context) = 0;

    virtual std::any visitStruct_def(gscParser::Struct_defContext *context) = 0;

    virtual std::any visitClass_init(gscParser::Class_initContext *context) = 0;

    virtual std::any visitIdf(gscParser::IdfContext *context) = 0;


};

