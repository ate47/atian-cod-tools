
// Generated from .\grammar\ddl.g4 by ANTLR 4.13.0

#pragma once


#include "antlr4-runtime.h"




class  ddlParser : public antlr4::Parser {
public:
  enum {
    T__0 = 1, T__1 = 2, T__2 = 3, T__3 = 4, T__4 = 5, T__5 = 6, T__6 = 7, 
    T__7 = 8, T__8 = 9, T__9 = 10, NEWLINE = 11, WHITESPACE = 12, INTEGER10 = 13, 
    INTEGER16 = 14, INTEGER8 = 15, INTEGER2 = 16, IDENTIFIER = 17, PATH = 18, 
    STRING = 19
  };

  enum {
    RuleProg = 0, RuleBuffer_data = 1, RuleEnum = 2, RuleStruct = 3, RuleStruct_def = 4, 
    RuleNumber = 5
  };

  explicit ddlParser(antlr4::TokenStream *input);

  ddlParser(antlr4::TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options);

  ~ddlParser() override;

  std::string getGrammarFileName() const override;

  const antlr4::atn::ATN& getATN() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;


  class ProgContext;
  class Buffer_dataContext;
  class EnumContext;
  class StructContext;
  class Struct_defContext;
  class NumberContext; 

  class  ProgContext : public antlr4::ParserRuleContext {
  public:
    ProgContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *EOF();
    std::vector<Buffer_dataContext *> buffer_data();
    Buffer_dataContext* buffer_data(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ProgContext* prog();

  class  Buffer_dataContext : public antlr4::ParserRuleContext {
  public:
    Buffer_dataContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    NumberContext *number();
    std::vector<EnumContext *> enum_();
    EnumContext* enum_(size_t i);
    std::vector<StructContext *> struct_();
    StructContext* struct_(size_t i);
    std::vector<Struct_defContext *> struct_def();
    Struct_defContext* struct_def(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Buffer_dataContext* buffer_data();

  class  EnumContext : public antlr4::ParserRuleContext {
  public:
    EnumContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> IDENTIFIER();
    antlr4::tree::TerminalNode* IDENTIFIER(size_t i);
    std::vector<antlr4::tree::TerminalNode *> STRING();
    antlr4::tree::TerminalNode* STRING(size_t i);
    std::vector<NumberContext *> number();
    NumberContext* number(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  EnumContext* enum_();

  class  StructContext : public antlr4::ParserRuleContext {
  public:
    StructContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENTIFIER();
    std::vector<Struct_defContext *> struct_def();
    Struct_defContext* struct_def(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  StructContext* struct_();

  class  Struct_defContext : public antlr4::ParserRuleContext {
  public:
    Struct_defContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> IDENTIFIER();
    antlr4::tree::TerminalNode* IDENTIFIER(size_t i);
    std::vector<NumberContext *> number();
    NumberContext* number(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Struct_defContext* struct_def();

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

