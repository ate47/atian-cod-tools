
// Generated from .\grammar\gsc.g4 by ANTLR 4.13.0

#pragma once


#include "antlr4-runtime.h"




class  gscParser : public antlr4::Parser {
public:
  enum {
    T__0 = 1, T__1 = 2, T__2 = 3, T__3 = 4, T__4 = 5, T__5 = 6, T__6 = 7, 
    T__7 = 8, T__8 = 9, T__9 = 10, T__10 = 11, T__11 = 12, T__12 = 13, T__13 = 14, 
    T__14 = 15, T__15 = 16, T__16 = 17, T__17 = 18, T__18 = 19, T__19 = 20, 
    T__20 = 21, T__21 = 22, T__22 = 23, T__23 = 24, T__24 = 25, T__25 = 26, 
    T__26 = 27, T__27 = 28, T__28 = 29, T__29 = 30, T__30 = 31, T__31 = 32, 
    T__32 = 33, T__33 = 34, T__34 = 35, T__35 = 36, T__36 = 37, T__37 = 38, 
    T__38 = 39, T__39 = 40, T__40 = 41, T__41 = 42, T__42 = 43, T__43 = 44, 
    T__44 = 45, T__45 = 46, T__46 = 47, T__47 = 48, T__48 = 49, T__49 = 50, 
    T__50 = 51, T__51 = 52, T__52 = 53, T__53 = 54, T__54 = 55, T__55 = 56, 
    T__56 = 57, T__57 = 58, T__58 = 59, T__59 = 60, T__60 = 61, T__61 = 62, 
    T__62 = 63, T__63 = 64, T__64 = 65, T__65 = 66, T__66 = 67, T__67 = 68, 
    T__68 = 69, T__69 = 70, T__70 = 71, T__71 = 72, NEWLINE = 73, WHITESPACE = 74, 
    INTEGER10 = 75, INTEGER16 = 76, INTEGER8 = 77, INTEGER2 = 78, FLOATVAL = 79, 
    BUILTIN = 80, BOOL_VALUE = 81, UNDEFINED_VALUE = 82, IDENTIFIER = 83, 
    STRUCT_IDENTIFIER = 84, PATH = 85, STRING = 86, HASHSTRING = 87
  };

  enum {
    RuleProg = 0, RuleInclude = 1, RuleNamespace = 2, RuleFunction = 3, 
    RuleParam_list = 4, RuleParam_val = 5, RuleExpression_list = 6, RuleStatement_block = 7, 
    RuleStatement = 8, RuleStatement_for = 9, RuleStatement_while = 10, 
    RuleStatement_dowhile = 11, RuleStatement_foreach = 12, RuleStatement_if = 13, 
    RuleStatement_switch = 14, RuleStatement_inst = 15, RuleFunction_call = 16, 
    RuleFunction_component = 17, RuleOperator_inst = 18, RuleExpression = 19, 
    RuleSet_expression = 20, RuleExpression1 = 21, RuleExpression2 = 22, 
    RuleExpression3 = 23, RuleExpression4 = 24, RuleExpression5 = 25, RuleExpression6 = 26, 
    RuleExpression7 = 27, RuleExpression8 = 28, RuleExpression9 = 29, RuleExpression10 = 30, 
    RuleExpression11 = 31, RuleExpression12 = 32, RuleExpression13 = 33, 
    RuleExpression14 = 34, RuleLeft_value = 35, RuleConst_expr = 36, RuleFunction_ref = 37, 
    RuleNumber = 38, RuleVector_value = 39, RuleArray_def = 40, RuleStruct_def = 41
  };

  explicit gscParser(antlr4::TokenStream *input);

  gscParser(antlr4::TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options);

  ~gscParser() override;

  std::string getGrammarFileName() const override;

  const antlr4::atn::ATN& getATN() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;


  class ProgContext;
  class IncludeContext;
  class NamespaceContext;
  class FunctionContext;
  class Param_listContext;
  class Param_valContext;
  class Expression_listContext;
  class Statement_blockContext;
  class StatementContext;
  class Statement_forContext;
  class Statement_whileContext;
  class Statement_dowhileContext;
  class Statement_foreachContext;
  class Statement_ifContext;
  class Statement_switchContext;
  class Statement_instContext;
  class Function_callContext;
  class Function_componentContext;
  class Operator_instContext;
  class ExpressionContext;
  class Set_expressionContext;
  class Expression1Context;
  class Expression2Context;
  class Expression3Context;
  class Expression4Context;
  class Expression5Context;
  class Expression6Context;
  class Expression7Context;
  class Expression8Context;
  class Expression9Context;
  class Expression10Context;
  class Expression11Context;
  class Expression12Context;
  class Expression13Context;
  class Expression14Context;
  class Left_valueContext;
  class Const_exprContext;
  class Function_refContext;
  class NumberContext;
  class Vector_valueContext;
  class Array_defContext;
  class Struct_defContext; 

  class  ProgContext : public antlr4::ParserRuleContext {
  public:
    ProgContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *EOF();
    std::vector<FunctionContext *> function();
    FunctionContext* function(size_t i);
    std::vector<IncludeContext *> include();
    IncludeContext* include(size_t i);
    std::vector<NamespaceContext *> namespace_();
    NamespaceContext* namespace_(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ProgContext* prog();

  class  IncludeContext : public antlr4::ParserRuleContext {
  public:
    IncludeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENTIFIER();
    antlr4::tree::TerminalNode *PATH();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  IncludeContext* include();

  class  NamespaceContext : public antlr4::ParserRuleContext {
  public:
    NamespaceContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENTIFIER();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  NamespaceContext* namespace_();

  class  FunctionContext : public antlr4::ParserRuleContext {
  public:
    FunctionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> IDENTIFIER();
    antlr4::tree::TerminalNode* IDENTIFIER(size_t i);
    Param_listContext *param_list();
    Statement_blockContext *statement_block();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FunctionContext* function();

  class  Param_listContext : public antlr4::ParserRuleContext {
  public:
    Param_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Param_valContext *> param_val();
    Param_valContext* param_val(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Param_listContext* param_list();

  class  Param_valContext : public antlr4::ParserRuleContext {
  public:
    Param_valContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENTIFIER();
    ExpressionContext *expression();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Param_valContext* param_val();

  class  Expression_listContext : public antlr4::ParserRuleContext {
  public:
    Expression_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<ExpressionContext *> expression();
    ExpressionContext* expression(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Expression_listContext* expression_list();

  class  Statement_blockContext : public antlr4::ParserRuleContext {
  public:
    Statement_blockContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<StatementContext *> statement();
    StatementContext* statement(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Statement_blockContext* statement_block();

  class  StatementContext : public antlr4::ParserRuleContext {
  public:
    StatementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Statement_blockContext *statement_block();
    Statement_forContext *statement_for();
    Statement_ifContext *statement_if();
    Statement_whileContext *statement_while();
    Statement_foreachContext *statement_foreach();
    Statement_instContext *statement_inst();
    Statement_switchContext *statement_switch();
    antlr4::tree::TerminalNode *IDENTIFIER();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  StatementContext* statement();

  class  Statement_forContext : public antlr4::ParserRuleContext {
  public:
    Statement_forContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    StatementContext *statement();
    std::vector<ExpressionContext *> expression();
    ExpressionContext* expression(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Statement_forContext* statement_for();

  class  Statement_whileContext : public antlr4::ParserRuleContext {
  public:
    Statement_whileContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExpressionContext *expression();
    StatementContext *statement();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Statement_whileContext* statement_while();

  class  Statement_dowhileContext : public antlr4::ParserRuleContext {
  public:
    Statement_dowhileContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    StatementContext *statement();
    ExpressionContext *expression();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Statement_dowhileContext* statement_dowhile();

  class  Statement_foreachContext : public antlr4::ParserRuleContext {
  public:
    Statement_foreachContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> IDENTIFIER();
    antlr4::tree::TerminalNode* IDENTIFIER(size_t i);
    ExpressionContext *expression();
    StatementContext *statement();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Statement_foreachContext* statement_foreach();

  class  Statement_ifContext : public antlr4::ParserRuleContext {
  public:
    Statement_ifContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExpressionContext *expression();
    std::vector<StatementContext *> statement();
    StatementContext* statement(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Statement_ifContext* statement_if();

  class  Statement_switchContext : public antlr4::ParserRuleContext {
  public:
    Statement_switchContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExpressionContext *expression();
    std::vector<Const_exprContext *> const_expr();
    Const_exprContext* const_expr(size_t i);
    std::vector<StatementContext *> statement();
    StatementContext* statement(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Statement_switchContext* statement_switch();

  class  Statement_instContext : public antlr4::ParserRuleContext {
  public:
    Statement_instContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Operator_instContext *operator_inst();
    Statement_dowhileContext *statement_dowhile();
    Function_callContext *function_call();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Statement_instContext* statement_inst();

  class  Function_callContext : public antlr4::ParserRuleContext {
  public:
    Function_callContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Function_componentContext *function_component();
    Expression_listContext *expression_list();
    Expression14Context *expression14();
    Function_callContext *function_call();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Function_callContext* function_call();
  Function_callContext* function_call(int precedence);
  class  Function_componentContext : public antlr4::ParserRuleContext {
  public:
    Function_componentContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> IDENTIFIER();
    antlr4::tree::TerminalNode* IDENTIFIER(size_t i);
    ExpressionContext *expression();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Function_componentContext* function_component();

  class  Operator_instContext : public antlr4::ParserRuleContext {
  public:
    Operator_instContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *BUILTIN();
    antlr4::tree::TerminalNode *IDENTIFIER();
    ExpressionContext *expression();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Operator_instContext* operator_inst();

  class  ExpressionContext : public antlr4::ParserRuleContext {
  public:
    ExpressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Set_expressionContext *set_expression();
    Expression1Context *expression1();
    std::vector<ExpressionContext *> expression();
    ExpressionContext* expression(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ExpressionContext* expression();

  class  Set_expressionContext : public antlr4::ParserRuleContext {
  public:
    Set_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Left_valueContext *left_value();
    ExpressionContext *expression();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Set_expressionContext* set_expression();

  class  Expression1Context : public antlr4::ParserRuleContext {
  public:
    Expression1Context(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Expression2Context *expression2();
    Expression1Context *expression1();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Expression1Context* expression1();
  Expression1Context* expression1(int precedence);
  class  Expression2Context : public antlr4::ParserRuleContext {
  public:
    Expression2Context(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Expression3Context *expression3();
    Expression2Context *expression2();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Expression2Context* expression2();
  Expression2Context* expression2(int precedence);
  class  Expression3Context : public antlr4::ParserRuleContext {
  public:
    Expression3Context(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Expression4Context *expression4();
    Expression3Context *expression3();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Expression3Context* expression3();
  Expression3Context* expression3(int precedence);
  class  Expression4Context : public antlr4::ParserRuleContext {
  public:
    Expression4Context(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Expression5Context *expression5();
    Expression4Context *expression4();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Expression4Context* expression4();
  Expression4Context* expression4(int precedence);
  class  Expression5Context : public antlr4::ParserRuleContext {
  public:
    Expression5Context(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Expression6Context *expression6();
    Expression5Context *expression5();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Expression5Context* expression5();
  Expression5Context* expression5(int precedence);
  class  Expression6Context : public antlr4::ParserRuleContext {
  public:
    Expression6Context(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Expression7Context *expression7();
    Expression6Context *expression6();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Expression6Context* expression6();
  Expression6Context* expression6(int precedence);
  class  Expression7Context : public antlr4::ParserRuleContext {
  public:
    Expression7Context(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Expression8Context *expression8();
    Expression7Context *expression7();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Expression7Context* expression7();
  Expression7Context* expression7(int precedence);
  class  Expression8Context : public antlr4::ParserRuleContext {
  public:
    Expression8Context(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Expression9Context *expression9();
    Expression8Context *expression8();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Expression8Context* expression8();
  Expression8Context* expression8(int precedence);
  class  Expression9Context : public antlr4::ParserRuleContext {
  public:
    Expression9Context(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Expression10Context *expression10();
    Expression9Context *expression9();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Expression9Context* expression9();
  Expression9Context* expression9(int precedence);
  class  Expression10Context : public antlr4::ParserRuleContext {
  public:
    Expression10Context(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Expression11Context *expression11();
    Expression10Context *expression10();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Expression10Context* expression10();
  Expression10Context* expression10(int precedence);
  class  Expression11Context : public antlr4::ParserRuleContext {
  public:
    Expression11Context(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Expression12Context *expression12();
    Expression11Context *expression11();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Expression11Context* expression11();
  Expression11Context* expression11(int precedence);
  class  Expression12Context : public antlr4::ParserRuleContext {
  public:
    Expression12Context(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Expression13Context *expression13();
    Left_valueContext *left_value();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Expression12Context* expression12();

  class  Expression13Context : public antlr4::ParserRuleContext {
  public:
    Expression13Context(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Function_callContext *function_call();
    Expression14Context *expression14();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Expression13Context* expression13();

  class  Expression14Context : public antlr4::ParserRuleContext {
  public:
    Expression14Context(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Const_exprContext *const_expr();
    ExpressionContext *expression();
    Left_valueContext *left_value();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Expression14Context* expression14();

  class  Left_valueContext : public antlr4::ParserRuleContext {
  public:
    Left_valueContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENTIFIER();
    std::vector<ExpressionContext *> expression();
    ExpressionContext* expression(size_t i);
    Const_exprContext *const_expr();
    Left_valueContext *left_value();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Left_valueContext* left_value();
  Left_valueContext* left_value(int precedence);
  class  Const_exprContext : public antlr4::ParserRuleContext {
  public:
    Const_exprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Vector_valueContext *vector_value();
    Array_defContext *array_def();
    Struct_defContext *struct_def();
    NumberContext *number();
    antlr4::tree::TerminalNode *BOOL_VALUE();
    antlr4::tree::TerminalNode *FLOATVAL();
    antlr4::tree::TerminalNode *STRING();
    antlr4::tree::TerminalNode *HASHSTRING();
    antlr4::tree::TerminalNode *UNDEFINED_VALUE();
    Function_refContext *function_ref();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Const_exprContext* const_expr();

  class  Function_refContext : public antlr4::ParserRuleContext {
  public:
    Function_refContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> IDENTIFIER();
    antlr4::tree::TerminalNode* IDENTIFIER(size_t i);
    antlr4::tree::TerminalNode *PATH();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Function_refContext* function_ref();

  class  NumberContext : public antlr4::ParserRuleContext {
  public:
    NumberContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *INTEGER10();
    antlr4::tree::TerminalNode *INTEGER16();
    antlr4::tree::TerminalNode *INTEGER8();
    antlr4::tree::TerminalNode *INTEGER2();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  NumberContext* number();

  class  Vector_valueContext : public antlr4::ParserRuleContext {
  public:
    Vector_valueContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<ExpressionContext *> expression();
    ExpressionContext* expression(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Vector_valueContext* vector_value();

  class  Array_defContext : public antlr4::ParserRuleContext {
  public:
    Array_defContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<ExpressionContext *> expression();
    ExpressionContext* expression(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Array_defContext* array_def();

  class  Struct_defContext : public antlr4::ParserRuleContext {
  public:
    Struct_defContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> STRUCT_IDENTIFIER();
    antlr4::tree::TerminalNode* STRUCT_IDENTIFIER(size_t i);
    std::vector<ExpressionContext *> expression();
    ExpressionContext* expression(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Struct_defContext* struct_def();


  bool sempred(antlr4::RuleContext *_localctx, size_t ruleIndex, size_t predicateIndex) override;

  bool function_callSempred(Function_callContext *_localctx, size_t predicateIndex);
  bool expression1Sempred(Expression1Context *_localctx, size_t predicateIndex);
  bool expression2Sempred(Expression2Context *_localctx, size_t predicateIndex);
  bool expression3Sempred(Expression3Context *_localctx, size_t predicateIndex);
  bool expression4Sempred(Expression4Context *_localctx, size_t predicateIndex);
  bool expression5Sempred(Expression5Context *_localctx, size_t predicateIndex);
  bool expression6Sempred(Expression6Context *_localctx, size_t predicateIndex);
  bool expression7Sempred(Expression7Context *_localctx, size_t predicateIndex);
  bool expression8Sempred(Expression8Context *_localctx, size_t predicateIndex);
  bool expression9Sempred(Expression9Context *_localctx, size_t predicateIndex);
  bool expression10Sempred(Expression10Context *_localctx, size_t predicateIndex);
  bool expression11Sempred(Expression11Context *_localctx, size_t predicateIndex);
  bool left_valueSempred(Left_valueContext *_localctx, size_t predicateIndex);

  // By default the static state used to implement the parser is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:
};

