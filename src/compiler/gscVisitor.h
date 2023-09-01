
// Generated from .\grammar\gsc.g4 by ANTLR 4.13.0

#pragma once


#include "antlr4-runtime.h"
#include "gscParser.h"



/**
 * This class defines an abstract visitor for a parse tree
 * produced by gscParser.
 */
class  gscVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by gscParser.
   */
    virtual std::any visitProg(gscParser::ProgContext *context) = 0;

    virtual std::any visitInclude(gscParser::IncludeContext *context) = 0;

    virtual std::any visitFunction(gscParser::FunctionContext *context) = 0;

    virtual std::any visitBlock(gscParser::BlockContext *context) = 0;


};

