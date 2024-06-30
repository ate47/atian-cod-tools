
// Generated from .\grammar\adl.g4 by ANTLR 4.13.0

#pragma once


#include "antlr4-runtime.h"
#include "adlParser.h"



/**
 * This class defines an abstract visitor for a parse tree
 * produced by adlParser.
 */
class  adlVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by adlParser.
   */
    virtual std::any visitProg(adlParser::ProgContext *context) = 0;

    virtual std::any visitDef(adlParser::DefContext *context) = 0;

    virtual std::any visitRoot_def(adlParser::Root_defContext *context) = 0;

    virtual std::any visitInclude_def(adlParser::Include_defContext *context) = 0;

    virtual std::any visitAlign_def(adlParser::Align_defContext *context) = 0;

    virtual std::any visitType_def(adlParser::Type_defContext *context) = 0;

    virtual std::any visitStruct_def(adlParser::Struct_defContext *context) = 0;

    virtual std::any visitFlag_def(adlParser::Flag_defContext *context) = 0;

    virtual std::any visitEnum_def(adlParser::Enum_defContext *context) = 0;

    virtual std::any visitCustomtype_def(adlParser::Customtype_defContext *context) = 0;

    virtual std::any visitStruct_members(adlParser::Struct_membersContext *context) = 0;

    virtual std::any visitEnum_members(adlParser::Enum_membersContext *context) = 0;

    virtual std::any visitStruct_member(adlParser::Struct_memberContext *context) = 0;

    virtual std::any visitData_member(adlParser::Data_memberContext *context) = 0;

    virtual std::any visitData_operator(adlParser::Data_operatorContext *context) = 0;

    virtual std::any visitEnum_member(adlParser::Enum_memberContext *context) = 0;

    virtual std::any visitIdf(adlParser::IdfContext *context) = 0;

    virtual std::any visitNumber(adlParser::NumberContext *context) = 0;


};

