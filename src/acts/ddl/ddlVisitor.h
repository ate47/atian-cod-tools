
// Generated from .\grammar\ddl.g4 by ANTLR 4.13.0

#pragma once


#include "antlr4-runtime.h"
#include "ddlParser.h"



/**
 * This class defines an abstract visitor for a parse tree
 * produced by ddlParser.
 */
class  ddlVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by ddlParser.
   */
    virtual std::any visitProg(ddlParser::ProgContext *context) = 0;

    virtual std::any visitBuffer_data(ddlParser::Buffer_dataContext *context) = 0;

    virtual std::any visitEnum(ddlParser::EnumContext *context) = 0;

    virtual std::any visitStruct(ddlParser::StructContext *context) = 0;

    virtual std::any visitStruct_def(ddlParser::Struct_defContext *context) = 0;

    virtual std::any visitNumber(ddlParser::NumberContext *context) = 0;


};

