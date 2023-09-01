
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

  virtual std::any visitFunction(gscParser::FunctionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBlock(gscParser::BlockContext *ctx) override {
    return visitChildren(ctx);
  }


};

