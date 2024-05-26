
// Generated from .\grammar\adl.g4 by ANTLR 4.13.0

#pragma once


#include "antlr4-runtime.h"
#include "adlVisitor.h"


/**
 * This class provides an empty implementation of adlVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class  adlBaseVisitor : public adlVisitor {
public:

  virtual std::any visitProg(adlParser::ProgContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDef(adlParser::DefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRoot_def(adlParser::Root_defContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInclude_def(adlParser::Include_defContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAlign_def(adlParser::Align_defContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitType_def(adlParser::Type_defContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStruct_def(adlParser::Struct_defContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFlag_def(adlParser::Flag_defContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEnum_def(adlParser::Enum_defContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStruct_members(adlParser::Struct_membersContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEnum_members(adlParser::Enum_membersContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStruct_member(adlParser::Struct_memberContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitData_member(adlParser::Data_memberContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitData_operator(adlParser::Data_operatorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEnum_member(adlParser::Enum_memberContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIdf(adlParser::IdfContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNumber(adlParser::NumberContext *ctx) override {
    return visitChildren(ctx);
  }


};

