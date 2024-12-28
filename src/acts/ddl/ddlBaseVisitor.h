
// Generated from .\grammar\ddl.g4 by ANTLR 4.13.0

#pragma once


#include "antlr4-runtime.h"
#include "ddlVisitor.h"


/**
 * This class provides an empty implementation of ddlVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class  ddlBaseVisitor : public ddlVisitor {
public:

  virtual std::any visitProg(ddlParser::ProgContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBuffer_data(ddlParser::Buffer_dataContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEnum(ddlParser::EnumContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStruct(ddlParser::StructContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStruct_def(ddlParser::Struct_defContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNumber(ddlParser::NumberContext *ctx) override {
    return visitChildren(ctx);
  }


};

