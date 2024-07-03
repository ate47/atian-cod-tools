
// Generated from .\grammar\adl.g4 by ANTLR 4.13.0

#pragma once


#include "antlr4-runtime.h"




class  adlParser : public antlr4::Parser {
public:
  enum {
    T__0 = 1, T__1 = 2, T__2 = 3, T__3 = 4, T__4 = 5, T__5 = 6, T__6 = 7, 
    T__7 = 8, T__8 = 9, T__9 = 10, T__10 = 11, T__11 = 12, T__12 = 13, T__13 = 14, 
    T__14 = 15, T__15 = 16, T__16 = 17, T__17 = 18, T__18 = 19, T__19 = 20, 
    T__20 = 21, T__21 = 22, T__22 = 23, T__23 = 24, T__24 = 25, T__25 = 26, 
    T__26 = 27, T__27 = 28, T__28 = 29, T__29 = 30, T__30 = 31, T__31 = 32, 
    T__32 = 33, NEWLINE = 34, WHITESPACE = 35, INTEGER10 = 36, INTEGER16 = 37, 
    INTEGER8 = 38, INTEGER2 = 39, BOOL_VALUE = 40, IDENTIFIER = 41, STRING = 42
  };

  enum {
    RuleProg = 0, RuleDef = 1, RuleRoot_def = 2, RuleInclude_def = 3, RuleAlign_def = 4, 
    RuleType_def = 5, RuleStruct_def = 6, RuleFlag_def = 7, RuleEnum_def = 8, 
    RuleCustomtype_def = 9, RuleStruct_members = 10, RuleEnum_members = 11, 
    RuleStruct_member = 12, RuleData_member = 13, RuleData_operator = 14, 
    RuleEnum_member = 15, RuleNumber = 16, RuleNumber_op1 = 17, RuleNumber_op2 = 18, 
    RuleNumber_op3 = 19, RuleNumber_op4 = 20, RuleNumber_op5 = 21, RuleNumber_op6 = 22, 
    RuleIdf = 23, RuleNumber_raw = 24
  };

  explicit adlParser(antlr4::TokenStream *input);

  adlParser(antlr4::TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options);

  ~adlParser() override;

  std::string getGrammarFileName() const override;

  const antlr4::atn::ATN& getATN() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;


  class ProgContext;
  class DefContext;
  class Root_defContext;
  class Include_defContext;
  class Align_defContext;
  class Type_defContext;
  class Struct_defContext;
  class Flag_defContext;
  class Enum_defContext;
  class Customtype_defContext;
  class Struct_membersContext;
  class Enum_membersContext;
  class Struct_memberContext;
  class Data_memberContext;
  class Data_operatorContext;
  class Enum_memberContext;
  class NumberContext;
  class Number_op1Context;
  class Number_op2Context;
  class Number_op3Context;
  class Number_op4Context;
  class Number_op5Context;
  class Number_op6Context;
  class IdfContext;
  class Number_rawContext; 

  class  ProgContext : public antlr4::ParserRuleContext {
  public:
    ProgContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *EOF();
    std::vector<DefContext *> def();
    DefContext* def(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ProgContext* prog();

  class  DefContext : public antlr4::ParserRuleContext {
  public:
    DefContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Struct_defContext *struct_def();
    Flag_defContext *flag_def();
    Enum_defContext *enum_def();
    Root_defContext *root_def();
    Align_defContext *align_def();
    Type_defContext *type_def();
    Include_defContext *include_def();
    Customtype_defContext *customtype_def();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DefContext* def();

  class  Root_defContext : public antlr4::ParserRuleContext {
  public:
    Root_defContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdfContext *idf();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Root_defContext* root_def();

  class  Include_defContext : public antlr4::ParserRuleContext {
  public:
    Include_defContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *STRING();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Include_defContext* include_def();

  class  Align_defContext : public antlr4::ParserRuleContext {
  public:
    Align_defContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    NumberContext *number();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Align_defContext* align_def();

  class  Type_defContext : public antlr4::ParserRuleContext {
  public:
    Type_defContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<IdfContext *> idf();
    IdfContext* idf(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Type_defContext* type_def();

  class  Struct_defContext : public antlr4::ParserRuleContext {
  public:
    Struct_defContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdfContext *idf();
    Struct_membersContext *struct_members();
    std::vector<NumberContext *> number();
    NumberContext* number(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Struct_defContext* struct_def();

  class  Flag_defContext : public antlr4::ParserRuleContext {
  public:
    Flag_defContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<IdfContext *> idf();
    IdfContext* idf(size_t i);
    Enum_membersContext *enum_members();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Flag_defContext* flag_def();

  class  Enum_defContext : public antlr4::ParserRuleContext {
  public:
    Enum_defContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<IdfContext *> idf();
    IdfContext* idf(size_t i);
    Enum_membersContext *enum_members();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Enum_defContext* enum_def();

  class  Customtype_defContext : public antlr4::ParserRuleContext {
  public:
    Customtype_defContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    NumberContext *number();
    IdfContext *idf();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Customtype_defContext* customtype_def();

  class  Struct_membersContext : public antlr4::ParserRuleContext {
  public:
    Struct_membersContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Struct_memberContext *> struct_member();
    Struct_memberContext* struct_member(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Struct_membersContext* struct_members();

  class  Enum_membersContext : public antlr4::ParserRuleContext {
  public:
    Enum_membersContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Enum_memberContext *> enum_member();
    Enum_memberContext* enum_member(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Enum_membersContext* enum_members();

  class  Struct_memberContext : public antlr4::ParserRuleContext {
  public:
    Struct_memberContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Data_memberContext *data_member();
    Data_operatorContext *data_operator();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Struct_memberContext* struct_member();

  class  Data_memberContext : public antlr4::ParserRuleContext {
  public:
    Data_memberContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<IdfContext *> idf();
    IdfContext* idf(size_t i);
    NumberContext *number();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Data_memberContext* data_member();

  class  Data_operatorContext : public antlr4::ParserRuleContext {
  public:
    Data_operatorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    NumberContext *number();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Data_operatorContext* data_operator();

  class  Enum_memberContext : public antlr4::ParserRuleContext {
  public:
    Enum_memberContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *STRING();
    IdfContext *idf();
    NumberContext *number();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Enum_memberContext* enum_member();

  class  NumberContext : public antlr4::ParserRuleContext {
  public:
    NumberContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Number_op1Context *number_op1();
    NumberContext *number();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  NumberContext* number();
  NumberContext* number(int precedence);
  class  Number_op1Context : public antlr4::ParserRuleContext {
  public:
    Number_op1Context(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Number_op2Context *number_op2();
    Number_op1Context *number_op1();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Number_op1Context* number_op1();
  Number_op1Context* number_op1(int precedence);
  class  Number_op2Context : public antlr4::ParserRuleContext {
  public:
    Number_op2Context(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Number_op3Context *number_op3();
    Number_op2Context *number_op2();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Number_op2Context* number_op2();
  Number_op2Context* number_op2(int precedence);
  class  Number_op3Context : public antlr4::ParserRuleContext {
  public:
    Number_op3Context(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Number_op4Context *number_op4();
    Number_op3Context *number_op3();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Number_op3Context* number_op3();
  Number_op3Context* number_op3(int precedence);
  class  Number_op4Context : public antlr4::ParserRuleContext {
  public:
    Number_op4Context(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Number_op5Context *number_op5();
    Number_op4Context *number_op4();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Number_op4Context* number_op4();
  Number_op4Context* number_op4(int precedence);
  class  Number_op5Context : public antlr4::ParserRuleContext {
  public:
    Number_op5Context(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Number_op6Context *number_op6();
    Number_op5Context *number_op5();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Number_op5Context* number_op5();
  Number_op5Context* number_op5(int precedence);
  class  Number_op6Context : public antlr4::ParserRuleContext {
  public:
    Number_op6Context(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Number_rawContext *number_raw();
    NumberContext *number();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Number_op6Context* number_op6();

  class  IdfContext : public antlr4::ParserRuleContext {
  public:
    IdfContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENTIFIER();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  IdfContext* idf();

  class  Number_rawContext : public antlr4::ParserRuleContext {
  public:
    Number_rawContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *INTEGER10();
    antlr4::tree::TerminalNode *INTEGER16();
    antlr4::tree::TerminalNode *INTEGER8();
    antlr4::tree::TerminalNode *INTEGER2();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Number_rawContext* number_raw();


  bool sempred(antlr4::RuleContext *_localctx, size_t ruleIndex, size_t predicateIndex) override;

  bool numberSempred(NumberContext *_localctx, size_t predicateIndex);
  bool number_op1Sempred(Number_op1Context *_localctx, size_t predicateIndex);
  bool number_op2Sempred(Number_op2Context *_localctx, size_t predicateIndex);
  bool number_op3Sempred(Number_op3Context *_localctx, size_t predicateIndex);
  bool number_op4Sempred(Number_op4Context *_localctx, size_t predicateIndex);
  bool number_op5Sempred(Number_op5Context *_localctx, size_t predicateIndex);

  // By default the static state used to implement the parser is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:
};

