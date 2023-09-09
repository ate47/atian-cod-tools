
// Generated from .\grammar\gsc.g4 by ANTLR 4.13.0

#pragma once


#include "antlr4-runtime.h"
#include "gscParser.h"


/**
 * This interface defines an abstract listener for a parse tree produced by gscParser.
 */
class  gscListener : public antlr4::tree::ParseTreeListener {
public:

  virtual void enterProg(gscParser::ProgContext *ctx) = 0;
  virtual void exitProg(gscParser::ProgContext *ctx) = 0;

  virtual void enterInclude(gscParser::IncludeContext *ctx) = 0;
  virtual void exitInclude(gscParser::IncludeContext *ctx) = 0;

  virtual void enterFunction(gscParser::FunctionContext *ctx) = 0;
  virtual void exitFunction(gscParser::FunctionContext *ctx) = 0;

  virtual void enterParam_list(gscParser::Param_listContext *ctx) = 0;
  virtual void exitParam_list(gscParser::Param_listContext *ctx) = 0;

  virtual void enterBlock(gscParser::BlockContext *ctx) = 0;
  virtual void exitBlock(gscParser::BlockContext *ctx) = 0;


};

