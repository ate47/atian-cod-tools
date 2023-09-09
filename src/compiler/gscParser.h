
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
    T__26 = 27, T__27 = 28, T__28 = 29, NEWLINE = 30, WHITESPACE = 31, INTEGER10 = 32, 
    INTEGER16 = 33, INTEGER8 = 34, INTEGER2 = 35, FLOATVAL = 36, IDENTIFIER = 37, 
    STRUCT_IDENTIFIER = 38, PATH = 39, STRING = 40, HASHSTRING = 41, BOOL_VALUE = 42, 
    UNDEFINED_VALUE = 43
  };

  enum {
    RuleProg = 0, RuleInclude = 1, RuleNamespace = 2, RuleFunction = 3, 
    RuleParam_list = 4, RuleParam_val = 5, RuleExpression_list = 6, RuleStatement_block = 7, 
    RuleStatement = 8, RuleStatement_for = 9, RuleStatement_foreach = 10, 
    RuleStatement_if = 11, RuleStatement_inst = 12, RuleFunction_call = 13, 
    RuleOperator_inst = 14, RuleExpression = 15, RuleConst_expr = 16, RuleFunction_ref = 17, 
    RuleNumber = 18, RuleVector_value = 19, RuleArray_def = 20, RuleStruct_def = 21
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
  class Statement_foreachContext;
  class Statement_ifContext;
  class Statement_instContext;
  class Function_callContext;
  class Operator_instContext;
  class ExpressionContext;
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
    Statement_foreachContext *statement_foreach();
    Statement_instContext *statement_inst();


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

  class  Statement_instContext : public antlr4::ParserRuleContext {
  public:
    Statement_instContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Function_callContext *function_call();
    Operator_instContext *operator_inst();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Statement_instContext* statement_inst();

  class  Function_callContext : public antlr4::ParserRuleContext {
  public:
    Function_callContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> IDENTIFIER();
    antlr4::tree::TerminalNode* IDENTIFIER(size_t i);
    Expression_listContext *expression_list();
    ExpressionContext *expression();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Function_callContext* function_call();

  class  Operator_instContext : public antlr4::ParserRuleContext {
  public:
    Operator_instContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENTIFIER();
    ExpressionContext *expression();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Operator_instContext* operator_inst();

  class  ExpressionContext : public antlr4::ParserRuleContext {
  public:
    ExpressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENTIFIER();
    Const_exprContext *const_expr();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ExpressionContext* expression();

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
    std::vector<antlr4::tree::TerminalNode *> HASHSTRING();
    antlr4::tree::TerminalNode* HASHSTRING(size_t i);
    std::vector<NumberContext *> number();
    NumberContext* number(size_t i);


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


  // By default the static state used to implement the parser is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:
};

