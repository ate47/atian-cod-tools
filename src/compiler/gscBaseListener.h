
// Generated from .\grammar\gsc.g4 by ANTLR 4.13.0

#pragma once


#include "antlr4-runtime.h"
#include "gscListener.h"


/**
 * This class provides an empty implementation of gscListener,
 * which can be extended to create a listener which only needs to handle a subset
 * of the available methods.
 */
class  gscBaseListener : public gscListener {
public:

  virtual void enterProg(gscParser::ProgContext * /*ctx*/) override { }
  virtual void exitProg(gscParser::ProgContext * /*ctx*/) override { }

  virtual void enterInclude(gscParser::IncludeContext * /*ctx*/) override { }
  virtual void exitInclude(gscParser::IncludeContext * /*ctx*/) override { }

  virtual void enterFunction(gscParser::FunctionContext * /*ctx*/) override { }
  virtual void exitFunction(gscParser::FunctionContext * /*ctx*/) override { }

  virtual void enterParam_list(gscParser::Param_listContext * /*ctx*/) override { }
  virtual void exitParam_list(gscParser::Param_listContext * /*ctx*/) override { }

  virtual void enterBlock(gscParser::BlockContext * /*ctx*/) override { }
  virtual void exitBlock(gscParser::BlockContext * /*ctx*/) override { }


  virtual void enterEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override { }
  virtual void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override { }

};

