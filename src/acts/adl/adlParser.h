
// Generated from .\grammar\adl.g4 by ANTLR 4.13.0

#pragma once


#include "antlr4-runtime.h"




class  adlParser : public antlr4::Parser {
public:
  enum {
    T__0 = 1, T__1 = 2, T__2 = 3, T__3 = 4, T__4 = 5, T__5 = 6, T__6 = 7, 
    T__7 = 8, T__8 = 9, T__9 = 10, T__10 = 11, T__11 = 12, T__12 = 13, T__13 = 14, 
    T__14 = 15, T__15 = 16, T__16 = 17, T__17 = 18, T__18 = 19, T__19 = 20, 
    T__20 = 21, NEWLINE = 22, WHITESPACE = 23, INTEGER10 = 24, INTEGER16 = 25, 
    INTEGER8 = 26, INTEGER2 = 27, BOOL_VALUE = 28, IDENTIFIER = 29, STRING = 30
  };

  enum {
    RuleProg = 0, RuleDef = 1, RuleRoot_def = 2, RuleAlign_def = 3, RuleType_def = 4, 
    RuleStruct_def = 5, RuleFlag_def = 6, RuleEnum_def = 7, RuleStruct_members = 8, 
    RuleEnum_members = 9, RuleStruct_member = 10, RuleData_member = 11, 
    RuleData_operator = 12, RuleEnum_member = 13, RuleIdf = 14, RuleNumber = 15
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
  class Align_defContext;
  class Type_defContext;
  class Struct_defContext;
  class Flag_defContext;
  class Enum_defContext;
  class Struct_membersContext;
  class Enum_membersContext;
  class Struct_memberContext;
  class Data_memberContext;
  class Data_operatorContext;
  class Enum_memberContext;
  class IdfContext;
  class NumberContext; 

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

  class  IdfContext : public antlr4::ParserRuleContext {
  public:
    IdfContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENTIFIER();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  IdfContext* idf();

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


  // By default the static state used to implement the parser is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:
};

