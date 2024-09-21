// precompiled header
#include <includes.hpp>


// Generated from .\grammar\gsc.g4 by ANTLR 4.13.0


#include "gscVisitor.h"

#include "gscParser.h"


using namespace antlrcpp;

using namespace antlr4;

namespace {

struct GscParserStaticData final {
  GscParserStaticData(std::vector<std::string> ruleNames,
                        std::vector<std::string> literalNames,
                        std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  GscParserStaticData(const GscParserStaticData&) = delete;
  GscParserStaticData(GscParserStaticData&&) = delete;
  GscParserStaticData& operator=(const GscParserStaticData&) = delete;
  GscParserStaticData& operator=(GscParserStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

::antlr4::internal::OnceFlag gscParserOnceFlag;
#if ANTLR4_USE_THREAD_LOCAL_CACHE
static thread_local
#endif
GscParserStaticData *gscParserStaticData = nullptr;

void gscParserInitialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  if (gscParserStaticData != nullptr) {
    return;
  }
#else
  assert(gscParserStaticData == nullptr);
#endif
  auto staticData = std::make_unique<GscParserStaticData>(
    std::vector<std::string>{
      "prog", "include", "namespace", "filenamespace", "constexpr", "function", 
      "detour_info", "param_list", "param_val", "expression_list", "statement_block", 
      "statement", "statement_dev_block", "statement_for", "statement_while", 
      "statement_dowhile", "statement_foreach", "statement_if", "statement_switch", 
      "statement_inst", "nop_def", "devop_def", "function_component", "operator_inst", 
      "expression", "set_expression", "expression0", "expression1", "expression2", 
      "expression3", "expression4", "expression5", "expression6", "expression7", 
      "expression8", "expression9", "expression10", "expression11", "expression12", 
      "expression13", "expression14", "expression15", "is_expression", "function_call_exp", 
      "function_call", "left_value", "const_expr", "function_ref", "number", 
      "vector_value", "array_def", "struct_def", "idf"
    },
    std::vector<std::string>{
      "", "'/#'", "'#/'", "'#include'", "'#using'", "';'", "'#namespace'", 
      "'#file'", "'#constexpr'", "'#define'", "'='", "'function'", "'private'", 
      "'autoexec'", "'('", "')'", "'event_handler'", "'['", "']'", "'=>'", 
      "'detour'", "'<'", "'>'", "'::'", "','", "'*'", "'&'", "'...'", "'{'", 
      "'}'", "':'", "'for'", "'while'", "'do'", "'foreach'", "'in'", "'if'", 
      "'else'", "'switch'", "'case'", "'default'", "'nop'", "'Nop'", "'DevOp'", 
      "'devop'", "'Devop'", "'->'", "'\\u003F'", "'+='", "'-='", "'/='", 
      "'*='", "'%='", "'&='", "'|='", "'^='", "'<<='", "'>>='", "'~='", 
      "'\\u003F\\u003F'", "'||'", "'&&'", "'|'", "'^'", "'!='", "'=='", 
      "'!=='", "'==='", "'<='", "'>='", "'<=='", "'>=='", "'<<'", "'>>'", 
      "'+'", "'-'", "'/'", "'%'", "'!'", "'~'", "'++'", "'--'", "'is'", 
      "'not'", "'thread'", "'childthread'", "'threadendon'", "'builtin'", 
      "'.'", "'\\u003F.'", "'@'", "", "", "", "", "", "", "", "", "", "'undefined'"
    },
    std::vector<std::string>{
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
      "", "", "", "", "", "", "NEWLINE", "WHITESPACE", "INTEGER10", "INTEGER16", 
      "INTEGER8", "INTEGER2", "FLOATVAL", "BUILTIN", "BOOL_VALUE", "UNDEFINED_VALUE", 
      "IDENTIFIER", "STRUCT_IDENTIFIER", "PATH", "STRING", "HASHSTRING"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,105,732,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,7,
  	14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,7,
  	21,2,22,7,22,2,23,7,23,2,24,7,24,2,25,7,25,2,26,7,26,2,27,7,27,2,28,7,
  	28,2,29,7,29,2,30,7,30,2,31,7,31,2,32,7,32,2,33,7,33,2,34,7,34,2,35,7,
  	35,2,36,7,36,2,37,7,37,2,38,7,38,2,39,7,39,2,40,7,40,2,41,7,41,2,42,7,
  	42,2,43,7,43,2,44,7,44,2,45,7,45,2,46,7,46,2,47,7,47,2,48,7,48,2,49,7,
  	49,2,50,7,50,2,51,7,51,2,52,7,52,1,0,1,0,1,0,1,0,1,0,1,0,1,0,5,0,114,
  	8,0,10,0,12,0,117,9,0,1,0,1,0,1,1,1,1,1,1,1,1,1,2,1,2,1,2,1,2,1,3,1,3,
  	1,3,1,3,1,4,1,4,1,4,1,4,1,4,1,4,1,5,3,5,140,8,5,1,5,3,5,143,8,5,1,5,1,
  	5,1,5,1,5,1,5,3,5,150,8,5,3,5,152,8,5,1,5,1,5,1,5,1,5,3,5,158,8,5,1,5,
  	3,5,161,8,5,1,5,3,5,164,8,5,1,5,1,5,1,5,1,5,3,5,170,8,5,1,5,1,5,1,6,1,
  	6,1,6,1,6,1,6,1,6,1,6,3,6,181,8,6,1,7,1,7,1,7,5,7,186,8,7,10,7,12,7,189,
  	9,7,3,7,191,8,7,1,8,3,8,194,8,8,1,8,1,8,1,8,3,8,199,8,8,1,8,3,8,202,8,
  	8,1,9,1,9,1,9,5,9,207,8,9,10,9,12,9,210,9,9,3,9,212,8,9,1,10,1,10,5,10,
  	216,8,10,10,10,12,10,219,9,10,1,10,1,10,1,11,1,11,3,11,225,8,11,1,11,
  	1,11,1,11,1,11,1,11,1,11,1,11,1,11,3,11,235,8,11,1,12,1,12,5,12,239,8,
  	12,10,12,12,12,242,9,12,1,12,1,12,1,13,1,13,1,13,3,13,249,8,13,1,13,1,
  	13,3,13,253,8,13,1,13,1,13,3,13,257,8,13,1,13,1,13,1,13,1,14,1,14,1,14,
  	1,14,1,14,1,14,1,15,1,15,1,15,1,15,1,15,1,15,1,15,1,16,1,16,1,16,1,16,
  	1,16,3,16,280,8,16,1,16,1,16,1,16,1,16,1,16,1,17,1,17,1,17,1,17,1,17,
  	1,17,1,17,3,17,294,8,17,1,18,1,18,1,18,1,18,1,18,1,18,1,18,1,18,3,18,
  	304,8,18,1,18,1,18,5,18,308,8,18,10,18,12,18,311,9,18,4,18,313,8,18,11,
  	18,12,18,314,1,18,1,18,1,19,1,19,1,19,1,19,1,19,1,19,3,19,325,8,19,1,
  	19,1,19,1,20,1,20,1,20,1,20,1,20,3,20,334,8,20,1,21,1,21,1,21,1,21,1,
  	21,1,22,1,22,3,22,343,8,22,1,22,1,22,1,22,1,22,1,22,1,22,1,22,1,22,1,
  	22,1,22,1,22,1,22,1,22,1,22,1,22,3,22,360,8,22,1,23,1,23,1,23,3,23,365,
  	8,23,1,24,1,24,1,24,1,24,1,24,1,24,1,24,1,24,3,24,375,8,24,1,25,1,25,
  	1,25,1,25,1,26,1,26,1,26,1,26,1,26,1,26,5,26,387,8,26,10,26,12,26,390,
  	9,26,1,27,1,27,1,27,1,27,1,27,1,27,5,27,398,8,27,10,27,12,27,401,9,27,
  	1,28,1,28,1,28,1,28,1,28,1,28,5,28,409,8,28,10,28,12,28,412,9,28,1,29,
  	1,29,1,29,1,29,1,29,1,29,5,29,420,8,29,10,29,12,29,423,9,29,1,30,1,30,
  	1,30,1,30,1,30,1,30,5,30,431,8,30,10,30,12,30,434,9,30,1,31,1,31,1,31,
  	1,31,1,31,1,31,5,31,442,8,31,10,31,12,31,445,9,31,1,32,1,32,1,32,1,32,
  	1,32,1,32,5,32,453,8,32,10,32,12,32,456,9,32,1,33,1,33,1,33,1,33,1,33,
  	1,33,5,33,464,8,33,10,33,12,33,467,9,33,1,34,1,34,1,34,1,34,1,34,1,34,
  	5,34,475,8,34,10,34,12,34,478,9,34,1,35,1,35,1,35,1,35,1,35,1,35,5,35,
  	486,8,35,10,35,12,35,489,9,35,1,36,1,36,1,36,1,36,1,36,1,36,5,36,497,
  	8,36,10,36,12,36,500,9,36,1,37,1,37,1,37,1,37,1,37,1,37,5,37,508,8,37,
  	10,37,12,37,511,9,37,1,38,1,38,1,38,1,38,1,38,1,38,1,38,1,38,1,38,3,38,
  	522,8,38,1,39,1,39,3,39,526,8,39,1,40,1,40,1,40,3,40,531,8,40,1,41,1,
  	41,1,41,1,41,1,42,1,42,1,42,3,42,540,8,42,1,42,1,42,1,43,1,43,1,43,3,
  	43,547,8,43,1,43,1,43,1,43,1,43,1,43,3,43,554,8,43,1,44,1,44,3,44,558,
  	8,44,1,44,1,44,1,44,1,44,1,44,1,44,1,44,3,44,567,8,44,1,44,3,44,570,8,
  	44,1,44,1,44,1,44,1,44,1,44,3,44,577,8,44,1,44,1,44,3,44,581,8,44,1,44,
  	1,44,1,44,1,44,1,44,5,44,588,8,44,10,44,12,44,591,9,44,1,45,1,45,1,45,
  	1,45,1,45,3,45,598,8,45,1,45,1,45,1,45,1,45,1,45,1,45,3,45,606,8,45,1,
  	45,1,45,1,45,3,45,611,8,45,1,45,1,45,1,45,1,45,3,45,617,8,45,1,45,1,45,
  	1,45,1,45,1,45,1,45,1,45,3,45,626,8,45,1,45,1,45,1,45,1,45,1,45,5,45,
  	633,8,45,10,45,12,45,636,9,45,1,46,1,46,1,46,1,46,1,46,1,46,1,46,1,46,
  	1,46,1,46,1,46,3,46,649,8,46,1,47,1,47,1,47,3,47,654,8,47,1,47,1,47,1,
  	47,1,47,1,47,1,47,1,47,1,47,1,47,1,47,3,47,666,8,47,1,48,1,48,1,49,1,
  	49,1,49,1,49,1,49,1,49,1,49,1,49,1,50,1,50,1,50,1,50,3,50,682,8,50,1,
  	50,1,50,1,50,1,50,1,50,3,50,689,8,50,1,50,5,50,692,8,50,10,50,12,50,695,
  	9,50,1,50,3,50,698,8,50,3,50,700,8,50,1,50,1,50,1,51,1,51,1,51,3,51,707,
  	8,51,1,51,1,51,1,51,1,51,1,51,3,51,714,8,51,1,51,1,51,5,51,718,8,51,10,
  	51,12,51,721,9,51,1,51,3,51,724,8,51,3,51,726,8,51,1,51,1,51,1,52,1,52,
  	1,52,0,14,52,54,56,58,60,62,64,66,68,70,72,74,88,90,53,0,2,4,6,8,10,12,
  	14,16,18,20,22,24,26,28,30,32,34,36,38,40,42,44,46,48,50,52,54,56,58,
  	60,62,64,66,68,70,72,74,76,78,80,82,84,86,88,90,92,94,96,98,100,102,104,
  	0,19,1,0,3,4,2,0,101,101,103,103,1,0,8,9,1,0,25,26,1,0,41,42,1,0,43,45,
  	2,0,10,10,48,58,1,0,64,67,2,0,21,22,68,71,2,0,21,21,68,68,1,0,72,73,1,
  	0,74,75,2,0,25,25,76,77,1,0,78,79,1,0,80,81,2,0,11,11,99,101,1,0,84,87,
  	1,0,88,89,2,0,93,96,101,101,793,0,115,1,0,0,0,2,120,1,0,0,0,4,124,1,0,
  	0,0,6,128,1,0,0,0,8,132,1,0,0,0,10,139,1,0,0,0,12,173,1,0,0,0,14,190,
  	1,0,0,0,16,201,1,0,0,0,18,211,1,0,0,0,20,213,1,0,0,0,22,224,1,0,0,0,24,
  	236,1,0,0,0,26,245,1,0,0,0,28,261,1,0,0,0,30,267,1,0,0,0,32,274,1,0,0,
  	0,34,286,1,0,0,0,36,295,1,0,0,0,38,324,1,0,0,0,40,328,1,0,0,0,42,335,
  	1,0,0,0,44,359,1,0,0,0,46,361,1,0,0,0,48,374,1,0,0,0,50,376,1,0,0,0,52,
  	380,1,0,0,0,54,391,1,0,0,0,56,402,1,0,0,0,58,413,1,0,0,0,60,424,1,0,0,
  	0,62,435,1,0,0,0,64,446,1,0,0,0,66,457,1,0,0,0,68,468,1,0,0,0,70,479,
  	1,0,0,0,72,490,1,0,0,0,74,501,1,0,0,0,76,521,1,0,0,0,78,525,1,0,0,0,80,
  	530,1,0,0,0,82,532,1,0,0,0,84,536,1,0,0,0,86,553,1,0,0,0,88,576,1,0,0,
  	0,90,616,1,0,0,0,92,648,1,0,0,0,94,665,1,0,0,0,96,667,1,0,0,0,98,669,
  	1,0,0,0,100,677,1,0,0,0,102,703,1,0,0,0,104,729,1,0,0,0,106,114,5,1,0,
  	0,107,114,5,2,0,0,108,114,3,10,5,0,109,114,3,2,1,0,110,114,3,4,2,0,111,
  	114,3,6,3,0,112,114,3,8,4,0,113,106,1,0,0,0,113,107,1,0,0,0,113,108,1,
  	0,0,0,113,109,1,0,0,0,113,110,1,0,0,0,113,111,1,0,0,0,113,112,1,0,0,0,
  	114,117,1,0,0,0,115,113,1,0,0,0,115,116,1,0,0,0,116,118,1,0,0,0,117,115,
  	1,0,0,0,118,119,5,0,0,1,119,1,1,0,0,0,120,121,7,0,0,0,121,122,7,1,0,0,
  	122,123,5,5,0,0,123,3,1,0,0,0,124,125,5,6,0,0,125,126,5,101,0,0,126,127,
  	5,5,0,0,127,5,1,0,0,0,128,129,5,7,0,0,129,130,7,1,0,0,130,131,5,5,0,0,
  	131,7,1,0,0,0,132,133,7,2,0,0,133,134,5,101,0,0,134,135,5,10,0,0,135,
  	136,3,48,24,0,136,137,5,5,0,0,137,9,1,0,0,0,138,140,5,11,0,0,139,138,
  	1,0,0,0,139,140,1,0,0,0,140,142,1,0,0,0,141,143,5,12,0,0,142,141,1,0,
  	0,0,142,143,1,0,0,0,143,151,1,0,0,0,144,149,5,13,0,0,145,146,5,14,0,0,
  	146,147,3,96,48,0,147,148,5,15,0,0,148,150,1,0,0,0,149,145,1,0,0,0,149,
  	150,1,0,0,0,150,152,1,0,0,0,151,144,1,0,0,0,151,152,1,0,0,0,152,157,1,
  	0,0,0,153,154,5,16,0,0,154,155,5,17,0,0,155,156,5,101,0,0,156,158,5,18,
  	0,0,157,153,1,0,0,0,157,158,1,0,0,0,158,160,1,0,0,0,159,161,3,12,6,0,
  	160,159,1,0,0,0,160,161,1,0,0,0,161,163,1,0,0,0,162,164,5,101,0,0,163,
  	162,1,0,0,0,163,164,1,0,0,0,164,165,1,0,0,0,165,166,5,14,0,0,166,167,
  	3,14,7,0,167,169,5,15,0,0,168,170,5,19,0,0,169,168,1,0,0,0,169,170,1,
  	0,0,0,170,171,1,0,0,0,171,172,3,20,10,0,172,11,1,0,0,0,173,174,5,20,0,
  	0,174,180,5,101,0,0,175,176,5,21,0,0,176,177,5,103,0,0,177,178,5,22,0,
  	0,178,179,5,23,0,0,179,181,5,101,0,0,180,175,1,0,0,0,180,181,1,0,0,0,
  	181,13,1,0,0,0,182,187,3,16,8,0,183,184,5,24,0,0,184,186,3,16,8,0,185,
  	183,1,0,0,0,186,189,1,0,0,0,187,185,1,0,0,0,187,188,1,0,0,0,188,191,1,
  	0,0,0,189,187,1,0,0,0,190,182,1,0,0,0,190,191,1,0,0,0,191,15,1,0,0,0,
  	192,194,7,3,0,0,193,192,1,0,0,0,193,194,1,0,0,0,194,195,1,0,0,0,195,198,
  	5,101,0,0,196,197,5,10,0,0,197,199,3,48,24,0,198,196,1,0,0,0,198,199,
  	1,0,0,0,199,202,1,0,0,0,200,202,5,27,0,0,201,193,1,0,0,0,201,200,1,0,
  	0,0,202,17,1,0,0,0,203,208,3,48,24,0,204,205,5,24,0,0,205,207,3,48,24,
  	0,206,204,1,0,0,0,207,210,1,0,0,0,208,206,1,0,0,0,208,209,1,0,0,0,209,
  	212,1,0,0,0,210,208,1,0,0,0,211,203,1,0,0,0,211,212,1,0,0,0,212,19,1,
  	0,0,0,213,217,5,28,0,0,214,216,3,22,11,0,215,214,1,0,0,0,216,219,1,0,
  	0,0,217,215,1,0,0,0,217,218,1,0,0,0,218,220,1,0,0,0,219,217,1,0,0,0,220,
  	221,5,29,0,0,221,21,1,0,0,0,222,223,5,101,0,0,223,225,5,30,0,0,224,222,
  	1,0,0,0,224,225,1,0,0,0,225,234,1,0,0,0,226,235,3,20,10,0,227,235,3,24,
  	12,0,228,235,3,26,13,0,229,235,3,34,17,0,230,235,3,28,14,0,231,235,3,
  	32,16,0,232,235,3,38,19,0,233,235,3,36,18,0,234,226,1,0,0,0,234,227,1,
  	0,0,0,234,228,1,0,0,0,234,229,1,0,0,0,234,230,1,0,0,0,234,231,1,0,0,0,
  	234,232,1,0,0,0,234,233,1,0,0,0,235,23,1,0,0,0,236,240,5,1,0,0,237,239,
  	3,22,11,0,238,237,1,0,0,0,239,242,1,0,0,0,240,238,1,0,0,0,240,241,1,0,
  	0,0,241,243,1,0,0,0,242,240,1,0,0,0,243,244,5,2,0,0,244,25,1,0,0,0,245,
  	246,5,31,0,0,246,248,5,14,0,0,247,249,3,48,24,0,248,247,1,0,0,0,248,249,
  	1,0,0,0,249,250,1,0,0,0,250,252,5,5,0,0,251,253,3,48,24,0,252,251,1,0,
  	0,0,252,253,1,0,0,0,253,254,1,0,0,0,254,256,5,5,0,0,255,257,3,48,24,0,
  	256,255,1,0,0,0,256,257,1,0,0,0,257,258,1,0,0,0,258,259,5,15,0,0,259,
  	260,3,22,11,0,260,27,1,0,0,0,261,262,5,32,0,0,262,263,5,14,0,0,263,264,
  	3,48,24,0,264,265,5,15,0,0,265,266,3,22,11,0,266,29,1,0,0,0,267,268,5,
  	33,0,0,268,269,3,22,11,0,269,270,5,32,0,0,270,271,5,14,0,0,271,272,3,
  	48,24,0,272,273,5,15,0,0,273,31,1,0,0,0,274,275,5,34,0,0,275,276,5,14,
  	0,0,276,279,5,101,0,0,277,278,5,24,0,0,278,280,5,101,0,0,279,277,1,0,
  	0,0,279,280,1,0,0,0,280,281,1,0,0,0,281,282,5,35,0,0,282,283,3,48,24,
  	0,283,284,5,15,0,0,284,285,3,22,11,0,285,33,1,0,0,0,286,287,5,36,0,0,
  	287,288,5,14,0,0,288,289,3,48,24,0,289,290,5,15,0,0,290,293,3,22,11,0,
  	291,292,5,37,0,0,292,294,3,22,11,0,293,291,1,0,0,0,293,294,1,0,0,0,294,
  	35,1,0,0,0,295,296,5,38,0,0,296,297,5,14,0,0,297,298,3,48,24,0,298,299,
  	5,15,0,0,299,312,5,28,0,0,300,301,5,39,0,0,301,304,3,92,46,0,302,304,
  	5,40,0,0,303,300,1,0,0,0,303,302,1,0,0,0,304,305,1,0,0,0,305,309,5,30,
  	0,0,306,308,3,22,11,0,307,306,1,0,0,0,308,311,1,0,0,0,309,307,1,0,0,0,
  	309,310,1,0,0,0,310,313,1,0,0,0,311,309,1,0,0,0,312,303,1,0,0,0,313,314,
  	1,0,0,0,314,312,1,0,0,0,314,315,1,0,0,0,315,316,1,0,0,0,316,317,5,29,
  	0,0,317,37,1,0,0,0,318,325,3,48,24,0,319,325,3,46,23,0,320,325,3,30,15,
  	0,321,325,3,86,43,0,322,325,3,40,20,0,323,325,3,42,21,0,324,318,1,0,0,
  	0,324,319,1,0,0,0,324,320,1,0,0,0,324,321,1,0,0,0,324,322,1,0,0,0,324,
  	323,1,0,0,0,325,326,1,0,0,0,326,327,5,5,0,0,327,39,1,0,0,0,328,333,7,
  	4,0,0,329,330,5,14,0,0,330,331,3,96,48,0,331,332,5,15,0,0,332,334,1,0,
  	0,0,333,329,1,0,0,0,333,334,1,0,0,0,334,41,1,0,0,0,335,336,7,5,0,0,336,
  	337,5,14,0,0,337,338,3,96,48,0,338,339,5,15,0,0,339,43,1,0,0,0,340,341,
  	5,101,0,0,341,343,5,23,0,0,342,340,1,0,0,0,342,343,1,0,0,0,343,344,1,
  	0,0,0,344,360,5,101,0,0,345,346,5,17,0,0,346,347,5,17,0,0,347,348,3,48,
  	24,0,348,349,5,18,0,0,349,350,5,18,0,0,350,360,1,0,0,0,351,352,5,17,0,
  	0,352,353,5,17,0,0,353,354,3,48,24,0,354,355,5,18,0,0,355,356,5,18,0,
  	0,356,357,5,46,0,0,357,358,5,101,0,0,358,360,1,0,0,0,359,342,1,0,0,0,
  	359,345,1,0,0,0,359,351,1,0,0,0,360,45,1,0,0,0,361,364,5,98,0,0,362,365,
  	5,101,0,0,363,365,3,48,24,0,364,362,1,0,0,0,364,363,1,0,0,0,364,365,1,
  	0,0,0,365,47,1,0,0,0,366,375,3,50,25,0,367,368,3,52,26,0,368,369,5,47,
  	0,0,369,370,3,48,24,0,370,371,5,30,0,0,371,372,3,48,24,0,372,375,1,0,
  	0,0,373,375,3,52,26,0,374,366,1,0,0,0,374,367,1,0,0,0,374,373,1,0,0,0,
  	375,49,1,0,0,0,376,377,3,90,45,0,377,378,7,6,0,0,378,379,3,48,24,0,379,
  	51,1,0,0,0,380,381,6,26,-1,0,381,382,3,54,27,0,382,388,1,0,0,0,383,384,
  	10,2,0,0,384,385,5,59,0,0,385,387,3,54,27,0,386,383,1,0,0,0,387,390,1,
  	0,0,0,388,386,1,0,0,0,388,389,1,0,0,0,389,53,1,0,0,0,390,388,1,0,0,0,
  	391,392,6,27,-1,0,392,393,3,56,28,0,393,399,1,0,0,0,394,395,10,2,0,0,
  	395,396,5,60,0,0,396,398,3,56,28,0,397,394,1,0,0,0,398,401,1,0,0,0,399,
  	397,1,0,0,0,399,400,1,0,0,0,400,55,1,0,0,0,401,399,1,0,0,0,402,403,6,
  	28,-1,0,403,404,3,58,29,0,404,410,1,0,0,0,405,406,10,2,0,0,406,407,5,
  	61,0,0,407,409,3,58,29,0,408,405,1,0,0,0,409,412,1,0,0,0,410,408,1,0,
  	0,0,410,411,1,0,0,0,411,57,1,0,0,0,412,410,1,0,0,0,413,414,6,29,-1,0,
  	414,415,3,60,30,0,415,421,1,0,0,0,416,417,10,2,0,0,417,418,5,62,0,0,418,
  	420,3,60,30,0,419,416,1,0,0,0,420,423,1,0,0,0,421,419,1,0,0,0,421,422,
  	1,0,0,0,422,59,1,0,0,0,423,421,1,0,0,0,424,425,6,30,-1,0,425,426,3,62,
  	31,0,426,432,1,0,0,0,427,428,10,2,0,0,428,429,5,63,0,0,429,431,3,62,31,
  	0,430,427,1,0,0,0,431,434,1,0,0,0,432,430,1,0,0,0,432,433,1,0,0,0,433,
  	61,1,0,0,0,434,432,1,0,0,0,435,436,6,31,-1,0,436,437,3,64,32,0,437,443,
  	1,0,0,0,438,439,10,2,0,0,439,440,5,26,0,0,440,442,3,64,32,0,441,438,1,
  	0,0,0,442,445,1,0,0,0,443,441,1,0,0,0,443,444,1,0,0,0,444,63,1,0,0,0,
  	445,443,1,0,0,0,446,447,6,32,-1,0,447,448,3,66,33,0,448,454,1,0,0,0,449,
  	450,10,2,0,0,450,451,7,7,0,0,451,453,3,66,33,0,452,449,1,0,0,0,453,456,
  	1,0,0,0,454,452,1,0,0,0,454,455,1,0,0,0,455,65,1,0,0,0,456,454,1,0,0,
  	0,457,458,6,33,-1,0,458,459,3,68,34,0,459,465,1,0,0,0,460,461,10,2,0,
  	0,461,462,7,8,0,0,462,464,3,68,34,0,463,460,1,0,0,0,464,467,1,0,0,0,465,
  	463,1,0,0,0,465,466,1,0,0,0,466,67,1,0,0,0,467,465,1,0,0,0,468,469,6,
  	34,-1,0,469,470,3,70,35,0,470,476,1,0,0,0,471,472,10,2,0,0,472,473,7,
  	9,0,0,473,475,3,70,35,0,474,471,1,0,0,0,475,478,1,0,0,0,476,474,1,0,0,
  	0,476,477,1,0,0,0,477,69,1,0,0,0,478,476,1,0,0,0,479,480,6,35,-1,0,480,
  	481,3,72,36,0,481,487,1,0,0,0,482,483,10,2,0,0,483,484,7,10,0,0,484,486,
  	3,72,36,0,485,482,1,0,0,0,486,489,1,0,0,0,487,485,1,0,0,0,487,488,1,0,
  	0,0,488,71,1,0,0,0,489,487,1,0,0,0,490,491,6,36,-1,0,491,492,3,74,37,
  	0,492,498,1,0,0,0,493,494,10,2,0,0,494,495,7,11,0,0,495,497,3,74,37,0,
  	496,493,1,0,0,0,497,500,1,0,0,0,498,496,1,0,0,0,498,499,1,0,0,0,499,73,
  	1,0,0,0,500,498,1,0,0,0,501,502,6,37,-1,0,502,503,3,76,38,0,503,509,1,
  	0,0,0,504,505,10,2,0,0,505,506,7,12,0,0,506,508,3,76,38,0,507,504,1,0,
  	0,0,508,511,1,0,0,0,509,507,1,0,0,0,509,510,1,0,0,0,510,75,1,0,0,0,511,
  	509,1,0,0,0,512,513,7,13,0,0,513,522,3,78,39,0,514,515,7,14,0,0,515,522,
  	3,90,45,0,516,517,3,90,45,0,517,518,7,14,0,0,518,522,1,0,0,0,519,522,
  	3,84,42,0,520,522,3,78,39,0,521,512,1,0,0,0,521,514,1,0,0,0,521,516,1,
  	0,0,0,521,519,1,0,0,0,521,520,1,0,0,0,522,77,1,0,0,0,523,526,3,86,43,
  	0,524,526,3,80,40,0,525,523,1,0,0,0,525,524,1,0,0,0,526,79,1,0,0,0,527,
  	531,3,92,46,0,528,531,3,82,41,0,529,531,3,90,45,0,530,527,1,0,0,0,530,
  	528,1,0,0,0,530,529,1,0,0,0,531,81,1,0,0,0,532,533,5,14,0,0,533,534,3,
  	48,24,0,534,535,5,15,0,0,535,83,1,0,0,0,536,537,3,78,39,0,537,539,5,82,
  	0,0,538,540,5,83,0,0,539,538,1,0,0,0,539,540,1,0,0,0,540,541,1,0,0,0,
  	541,542,7,15,0,0,542,85,1,0,0,0,543,554,3,88,44,0,544,546,3,80,40,0,545,
  	547,7,16,0,0,546,545,1,0,0,0,546,547,1,0,0,0,547,548,1,0,0,0,548,549,
  	3,44,22,0,549,550,5,14,0,0,550,551,3,18,9,0,551,552,5,15,0,0,552,554,
  	1,0,0,0,553,543,1,0,0,0,553,544,1,0,0,0,554,87,1,0,0,0,555,557,6,44,-1,
  	0,556,558,7,16,0,0,557,556,1,0,0,0,557,558,1,0,0,0,558,559,1,0,0,0,559,
  	560,3,44,22,0,560,561,5,14,0,0,561,562,3,18,9,0,562,563,5,15,0,0,563,
  	577,1,0,0,0,564,567,3,92,46,0,565,567,3,82,41,0,566,564,1,0,0,0,566,565,
  	1,0,0,0,567,569,1,0,0,0,568,570,7,16,0,0,569,568,1,0,0,0,569,570,1,0,
  	0,0,570,571,1,0,0,0,571,572,3,44,22,0,572,573,5,14,0,0,573,574,3,18,9,
  	0,574,575,5,15,0,0,575,577,1,0,0,0,576,555,1,0,0,0,576,566,1,0,0,0,577,
  	589,1,0,0,0,578,580,10,1,0,0,579,581,7,16,0,0,580,579,1,0,0,0,580,581,
  	1,0,0,0,581,582,1,0,0,0,582,583,3,44,22,0,583,584,5,14,0,0,584,585,3,
  	18,9,0,585,586,5,15,0,0,586,588,1,0,0,0,587,578,1,0,0,0,588,591,1,0,0,
  	0,589,587,1,0,0,0,589,590,1,0,0,0,590,89,1,0,0,0,591,589,1,0,0,0,592,
  	593,6,45,-1,0,593,617,3,104,52,0,594,598,3,88,44,0,595,598,3,92,46,0,
  	596,598,3,82,41,0,597,594,1,0,0,0,597,595,1,0,0,0,597,596,1,0,0,0,598,
  	599,1,0,0,0,599,605,7,17,0,0,600,606,3,104,52,0,601,602,5,14,0,0,602,
  	603,3,48,24,0,603,604,5,15,0,0,604,606,1,0,0,0,605,600,1,0,0,0,605,601,
  	1,0,0,0,606,617,1,0,0,0,607,611,3,88,44,0,608,611,3,92,46,0,609,611,3,
  	82,41,0,610,607,1,0,0,0,610,608,1,0,0,0,610,609,1,0,0,0,611,612,1,0,0,
  	0,612,613,5,17,0,0,613,614,3,48,24,0,614,615,5,18,0,0,615,617,1,0,0,0,
  	616,592,1,0,0,0,616,597,1,0,0,0,616,610,1,0,0,0,617,634,1,0,0,0,618,619,
  	10,4,0,0,619,625,7,17,0,0,620,626,3,104,52,0,621,622,5,14,0,0,622,623,
  	3,48,24,0,623,624,5,15,0,0,624,626,1,0,0,0,625,620,1,0,0,0,625,621,1,
  	0,0,0,626,633,1,0,0,0,627,628,10,3,0,0,628,629,5,17,0,0,629,630,3,48,
  	24,0,630,631,5,18,0,0,631,633,1,0,0,0,632,618,1,0,0,0,632,627,1,0,0,0,
  	633,636,1,0,0,0,634,632,1,0,0,0,634,635,1,0,0,0,635,91,1,0,0,0,636,634,
  	1,0,0,0,637,649,3,98,49,0,638,649,3,100,50,0,639,649,3,102,51,0,640,649,
  	3,96,48,0,641,649,3,10,5,0,642,649,5,99,0,0,643,649,5,97,0,0,644,649,
  	5,104,0,0,645,649,5,105,0,0,646,649,5,100,0,0,647,649,3,94,47,0,648,637,
  	1,0,0,0,648,638,1,0,0,0,648,639,1,0,0,0,648,640,1,0,0,0,648,641,1,0,0,
  	0,648,642,1,0,0,0,648,643,1,0,0,0,648,644,1,0,0,0,648,645,1,0,0,0,648,
  	646,1,0,0,0,648,647,1,0,0,0,649,93,1,0,0,0,650,653,5,26,0,0,651,652,5,
  	101,0,0,652,654,5,23,0,0,653,651,1,0,0,0,653,654,1,0,0,0,654,655,1,0,
  	0,0,655,666,5,101,0,0,656,657,5,90,0,0,657,658,5,101,0,0,658,659,5,21,
  	0,0,659,660,5,103,0,0,660,661,5,22,0,0,661,662,5,23,0,0,662,666,5,101,
  	0,0,663,664,5,26,0,0,664,666,3,90,45,0,665,650,1,0,0,0,665,656,1,0,0,
  	0,665,663,1,0,0,0,666,95,1,0,0,0,667,668,7,18,0,0,668,97,1,0,0,0,669,
  	670,5,14,0,0,670,671,3,48,24,0,671,672,5,24,0,0,672,673,3,48,24,0,673,
  	674,5,24,0,0,674,675,3,48,24,0,675,676,5,15,0,0,676,99,1,0,0,0,677,699,
  	5,17,0,0,678,679,3,48,24,0,679,680,5,30,0,0,680,682,1,0,0,0,681,678,1,
  	0,0,0,681,682,1,0,0,0,682,683,1,0,0,0,683,693,3,48,24,0,684,688,5,24,
  	0,0,685,686,3,48,24,0,686,687,5,30,0,0,687,689,1,0,0,0,688,685,1,0,0,
  	0,688,689,1,0,0,0,689,690,1,0,0,0,690,692,3,48,24,0,691,684,1,0,0,0,692,
  	695,1,0,0,0,693,691,1,0,0,0,693,694,1,0,0,0,694,697,1,0,0,0,695,693,1,
  	0,0,0,696,698,5,24,0,0,697,696,1,0,0,0,697,698,1,0,0,0,698,700,1,0,0,
  	0,699,681,1,0,0,0,699,700,1,0,0,0,700,701,1,0,0,0,701,702,5,18,0,0,702,
  	101,1,0,0,0,703,725,5,28,0,0,704,707,5,102,0,0,705,707,3,48,24,0,706,
  	704,1,0,0,0,706,705,1,0,0,0,707,708,1,0,0,0,708,709,5,30,0,0,709,719,
  	3,48,24,0,710,713,5,24,0,0,711,714,5,102,0,0,712,714,3,48,24,0,713,711,
  	1,0,0,0,713,712,1,0,0,0,714,715,1,0,0,0,715,716,5,30,0,0,716,718,3,48,
  	24,0,717,710,1,0,0,0,718,721,1,0,0,0,719,717,1,0,0,0,719,720,1,0,0,0,
  	720,723,1,0,0,0,721,719,1,0,0,0,722,724,5,24,0,0,723,722,1,0,0,0,723,
  	724,1,0,0,0,724,726,1,0,0,0,725,706,1,0,0,0,725,726,1,0,0,0,726,727,1,
  	0,0,0,727,728,5,29,0,0,728,103,1,0,0,0,729,730,5,101,0,0,730,105,1,0,
  	0,0,80,113,115,139,142,149,151,157,160,163,169,180,187,190,193,198,201,
  	208,211,217,224,234,240,248,252,256,279,293,303,309,314,324,333,342,359,
  	364,374,388,399,410,421,432,443,454,465,476,487,498,509,521,525,530,539,
  	546,553,557,566,569,576,580,589,597,605,610,616,625,632,634,648,653,665,
  	681,688,693,697,699,706,713,719,723,725
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  gscParserStaticData = staticData.release();
}

}

gscParser::gscParser(TokenStream *input) : gscParser(input, antlr4::atn::ParserATNSimulatorOptions()) {}

gscParser::gscParser(TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options) : Parser(input) {
  gscParser::initialize();
  _interpreter = new atn::ParserATNSimulator(this, *gscParserStaticData->atn, gscParserStaticData->decisionToDFA, gscParserStaticData->sharedContextCache, options);
}

gscParser::~gscParser() {
  delete _interpreter;
}

const atn::ATN& gscParser::getATN() const {
  return *gscParserStaticData->atn;
}

std::string gscParser::getGrammarFileName() const {
  return "gsc.g4";
}

const std::vector<std::string>& gscParser::getRuleNames() const {
  return gscParserStaticData->ruleNames;
}

const dfa::Vocabulary& gscParser::getVocabulary() const {
  return gscParserStaticData->vocabulary;
}

antlr4::atn::SerializedATNView gscParser::getSerializedATN() const {
  return gscParserStaticData->serializedATN;
}


//----------------- ProgContext ------------------------------------------------------------------

gscParser::ProgContext::ProgContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* gscParser::ProgContext::EOF() {
  return getToken(gscParser::EOF, 0);
}

std::vector<gscParser::FunctionContext *> gscParser::ProgContext::function() {
  return getRuleContexts<gscParser::FunctionContext>();
}

gscParser::FunctionContext* gscParser::ProgContext::function(size_t i) {
  return getRuleContext<gscParser::FunctionContext>(i);
}

std::vector<gscParser::IncludeContext *> gscParser::ProgContext::include() {
  return getRuleContexts<gscParser::IncludeContext>();
}

gscParser::IncludeContext* gscParser::ProgContext::include(size_t i) {
  return getRuleContext<gscParser::IncludeContext>(i);
}

std::vector<gscParser::NamespaceContext *> gscParser::ProgContext::namespace_() {
  return getRuleContexts<gscParser::NamespaceContext>();
}

gscParser::NamespaceContext* gscParser::ProgContext::namespace_(size_t i) {
  return getRuleContext<gscParser::NamespaceContext>(i);
}

std::vector<gscParser::FilenamespaceContext *> gscParser::ProgContext::filenamespace() {
  return getRuleContexts<gscParser::FilenamespaceContext>();
}

gscParser::FilenamespaceContext* gscParser::ProgContext::filenamespace(size_t i) {
  return getRuleContext<gscParser::FilenamespaceContext>(i);
}

std::vector<gscParser::ConstexprContext *> gscParser::ProgContext::constexpr_() {
  return getRuleContexts<gscParser::ConstexprContext>();
}

gscParser::ConstexprContext* gscParser::ProgContext::constexpr_(size_t i) {
  return getRuleContext<gscParser::ConstexprContext>(i);
}


size_t gscParser::ProgContext::getRuleIndex() const {
  return gscParser::RuleProg;
}


std::any gscParser::ProgContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitProg(this);
  else
    return visitor->visitChildren(this);
}

gscParser::ProgContext* gscParser::prog() {
  ProgContext *_localctx = _tracker.createInstance<ProgContext>(_ctx, getState());
  enterRule(_localctx, 0, gscParser::RuleProg);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(115);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 1145822) != 0) || _la == gscParser::IDENTIFIER) {
      setState(113);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case gscParser::T__0: {
          setState(106);
          match(gscParser::T__0);
          break;
        }

        case gscParser::T__1: {
          setState(107);
          match(gscParser::T__1);
          break;
        }

        case gscParser::T__10:
        case gscParser::T__11:
        case gscParser::T__12:
        case gscParser::T__13:
        case gscParser::T__15:
        case gscParser::T__19:
        case gscParser::IDENTIFIER: {
          setState(108);
          function();
          break;
        }

        case gscParser::T__2:
        case gscParser::T__3: {
          setState(109);
          include();
          break;
        }

        case gscParser::T__5: {
          setState(110);
          namespace_();
          break;
        }

        case gscParser::T__6: {
          setState(111);
          filenamespace();
          break;
        }

        case gscParser::T__7:
        case gscParser::T__8: {
          setState(112);
          constexpr_();
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(117);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(118);
    match(gscParser::EOF);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- IncludeContext ------------------------------------------------------------------

gscParser::IncludeContext::IncludeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* gscParser::IncludeContext::IDENTIFIER() {
  return getToken(gscParser::IDENTIFIER, 0);
}

tree::TerminalNode* gscParser::IncludeContext::PATH() {
  return getToken(gscParser::PATH, 0);
}


size_t gscParser::IncludeContext::getRuleIndex() const {
  return gscParser::RuleInclude;
}


std::any gscParser::IncludeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitInclude(this);
  else
    return visitor->visitChildren(this);
}

gscParser::IncludeContext* gscParser::include() {
  IncludeContext *_localctx = _tracker.createInstance<IncludeContext>(_ctx, getState());
  enterRule(_localctx, 2, gscParser::RuleInclude);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(120);
    _la = _input->LA(1);
    if (!(_la == gscParser::T__2

    || _la == gscParser::T__3)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(121);
    _la = _input->LA(1);
    if (!(_la == gscParser::IDENTIFIER

    || _la == gscParser::PATH)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(122);
    match(gscParser::T__4);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- NamespaceContext ------------------------------------------------------------------

gscParser::NamespaceContext::NamespaceContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* gscParser::NamespaceContext::IDENTIFIER() {
  return getToken(gscParser::IDENTIFIER, 0);
}


size_t gscParser::NamespaceContext::getRuleIndex() const {
  return gscParser::RuleNamespace;
}


std::any gscParser::NamespaceContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitNamespace(this);
  else
    return visitor->visitChildren(this);
}

gscParser::NamespaceContext* gscParser::namespace_() {
  NamespaceContext *_localctx = _tracker.createInstance<NamespaceContext>(_ctx, getState());
  enterRule(_localctx, 4, gscParser::RuleNamespace);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(124);
    match(gscParser::T__5);
    setState(125);
    match(gscParser::IDENTIFIER);
    setState(126);
    match(gscParser::T__4);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FilenamespaceContext ------------------------------------------------------------------

gscParser::FilenamespaceContext::FilenamespaceContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* gscParser::FilenamespaceContext::IDENTIFIER() {
  return getToken(gscParser::IDENTIFIER, 0);
}

tree::TerminalNode* gscParser::FilenamespaceContext::PATH() {
  return getToken(gscParser::PATH, 0);
}


size_t gscParser::FilenamespaceContext::getRuleIndex() const {
  return gscParser::RuleFilenamespace;
}


std::any gscParser::FilenamespaceContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitFilenamespace(this);
  else
    return visitor->visitChildren(this);
}

gscParser::FilenamespaceContext* gscParser::filenamespace() {
  FilenamespaceContext *_localctx = _tracker.createInstance<FilenamespaceContext>(_ctx, getState());
  enterRule(_localctx, 6, gscParser::RuleFilenamespace);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(128);
    match(gscParser::T__6);
    setState(129);
    _la = _input->LA(1);
    if (!(_la == gscParser::IDENTIFIER

    || _la == gscParser::PATH)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(130);
    match(gscParser::T__4);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ConstexprContext ------------------------------------------------------------------

gscParser::ConstexprContext::ConstexprContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* gscParser::ConstexprContext::IDENTIFIER() {
  return getToken(gscParser::IDENTIFIER, 0);
}

gscParser::ExpressionContext* gscParser::ConstexprContext::expression() {
  return getRuleContext<gscParser::ExpressionContext>(0);
}


size_t gscParser::ConstexprContext::getRuleIndex() const {
  return gscParser::RuleConstexpr;
}


std::any gscParser::ConstexprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitConstexpr(this);
  else
    return visitor->visitChildren(this);
}

gscParser::ConstexprContext* gscParser::constexpr_() {
  ConstexprContext *_localctx = _tracker.createInstance<ConstexprContext>(_ctx, getState());
  enterRule(_localctx, 8, gscParser::RuleConstexpr);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(132);
    _la = _input->LA(1);
    if (!(_la == gscParser::T__7

    || _la == gscParser::T__8)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(133);
    match(gscParser::IDENTIFIER);
    setState(134);
    match(gscParser::T__9);
    setState(135);
    expression();
    setState(136);
    match(gscParser::T__4);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FunctionContext ------------------------------------------------------------------

gscParser::FunctionContext::FunctionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

gscParser::Param_listContext* gscParser::FunctionContext::param_list() {
  return getRuleContext<gscParser::Param_listContext>(0);
}

gscParser::Statement_blockContext* gscParser::FunctionContext::statement_block() {
  return getRuleContext<gscParser::Statement_blockContext>(0);
}

std::vector<tree::TerminalNode *> gscParser::FunctionContext::IDENTIFIER() {
  return getTokens(gscParser::IDENTIFIER);
}

tree::TerminalNode* gscParser::FunctionContext::IDENTIFIER(size_t i) {
  return getToken(gscParser::IDENTIFIER, i);
}

gscParser::Detour_infoContext* gscParser::FunctionContext::detour_info() {
  return getRuleContext<gscParser::Detour_infoContext>(0);
}

gscParser::NumberContext* gscParser::FunctionContext::number() {
  return getRuleContext<gscParser::NumberContext>(0);
}


size_t gscParser::FunctionContext::getRuleIndex() const {
  return gscParser::RuleFunction;
}


std::any gscParser::FunctionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitFunction(this);
  else
    return visitor->visitChildren(this);
}

gscParser::FunctionContext* gscParser::function() {
  FunctionContext *_localctx = _tracker.createInstance<FunctionContext>(_ctx, getState());
  enterRule(_localctx, 10, gscParser::RuleFunction);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(139);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__10) {
      setState(138);
      match(gscParser::T__10);
    }
    setState(142);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__11) {
      setState(141);
      match(gscParser::T__11);
    }
    setState(151);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__12) {
      setState(144);
      match(gscParser::T__12);
      setState(149);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 4, _ctx)) {
      case 1: {
        setState(145);
        match(gscParser::T__13);
        setState(146);
        number();
        setState(147);
        match(gscParser::T__14);
        break;
      }

      default:
        break;
      }
    }
    setState(157);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__15) {
      setState(153);
      match(gscParser::T__15);
      setState(154);
      match(gscParser::T__16);
      setState(155);
      match(gscParser::IDENTIFIER);
      setState(156);
      match(gscParser::T__17);
    }
    setState(160);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__19) {
      setState(159);
      detour_info();
    }
    setState(163);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::IDENTIFIER) {
      setState(162);
      match(gscParser::IDENTIFIER);
    }
    setState(165);
    match(gscParser::T__13);
    setState(166);
    param_list();
    setState(167);
    match(gscParser::T__14);
    setState(169);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__18) {
      setState(168);
      match(gscParser::T__18);
    }
    setState(171);
    statement_block();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Detour_infoContext ------------------------------------------------------------------

gscParser::Detour_infoContext::Detour_infoContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> gscParser::Detour_infoContext::IDENTIFIER() {
  return getTokens(gscParser::IDENTIFIER);
}

tree::TerminalNode* gscParser::Detour_infoContext::IDENTIFIER(size_t i) {
  return getToken(gscParser::IDENTIFIER, i);
}

tree::TerminalNode* gscParser::Detour_infoContext::PATH() {
  return getToken(gscParser::PATH, 0);
}


size_t gscParser::Detour_infoContext::getRuleIndex() const {
  return gscParser::RuleDetour_info;
}


std::any gscParser::Detour_infoContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitDetour_info(this);
  else
    return visitor->visitChildren(this);
}

gscParser::Detour_infoContext* gscParser::detour_info() {
  Detour_infoContext *_localctx = _tracker.createInstance<Detour_infoContext>(_ctx, getState());
  enterRule(_localctx, 12, gscParser::RuleDetour_info);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(173);
    match(gscParser::T__19);
    setState(174);
    match(gscParser::IDENTIFIER);
    setState(180);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__20) {
      setState(175);
      match(gscParser::T__20);
      setState(176);
      match(gscParser::PATH);
      setState(177);
      match(gscParser::T__21);
      setState(178);
      match(gscParser::T__22);
      setState(179);
      match(gscParser::IDENTIFIER);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Param_listContext ------------------------------------------------------------------

gscParser::Param_listContext::Param_listContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<gscParser::Param_valContext *> gscParser::Param_listContext::param_val() {
  return getRuleContexts<gscParser::Param_valContext>();
}

gscParser::Param_valContext* gscParser::Param_listContext::param_val(size_t i) {
  return getRuleContext<gscParser::Param_valContext>(i);
}


size_t gscParser::Param_listContext::getRuleIndex() const {
  return gscParser::RuleParam_list;
}


std::any gscParser::Param_listContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitParam_list(this);
  else
    return visitor->visitChildren(this);
}

gscParser::Param_listContext* gscParser::param_list() {
  Param_listContext *_localctx = _tracker.createInstance<Param_listContext>(_ctx, getState());
  enterRule(_localctx, 14, gscParser::RuleParam_list);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(190);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 234881024) != 0) || _la == gscParser::IDENTIFIER) {
      setState(182);
      param_val();
      setState(187);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == gscParser::T__23) {
        setState(183);
        match(gscParser::T__23);
        setState(184);
        param_val();
        setState(189);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Param_valContext ------------------------------------------------------------------

gscParser::Param_valContext::Param_valContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* gscParser::Param_valContext::IDENTIFIER() {
  return getToken(gscParser::IDENTIFIER, 0);
}

gscParser::ExpressionContext* gscParser::Param_valContext::expression() {
  return getRuleContext<gscParser::ExpressionContext>(0);
}


size_t gscParser::Param_valContext::getRuleIndex() const {
  return gscParser::RuleParam_val;
}


std::any gscParser::Param_valContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitParam_val(this);
  else
    return visitor->visitChildren(this);
}

gscParser::Param_valContext* gscParser::param_val() {
  Param_valContext *_localctx = _tracker.createInstance<Param_valContext>(_ctx, getState());
  enterRule(_localctx, 16, gscParser::RuleParam_val);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(201);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case gscParser::T__24:
      case gscParser::T__25:
      case gscParser::IDENTIFIER: {
        enterOuterAlt(_localctx, 1);
        setState(193);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == gscParser::T__24

        || _la == gscParser::T__25) {
          setState(192);
          _la = _input->LA(1);
          if (!(_la == gscParser::T__24

          || _la == gscParser::T__25)) {
          _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
        }
        setState(195);
        match(gscParser::IDENTIFIER);
        setState(198);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == gscParser::T__9) {
          setState(196);
          match(gscParser::T__9);
          setState(197);
          expression();
        }
        break;
      }

      case gscParser::T__26: {
        enterOuterAlt(_localctx, 2);
        setState(200);
        match(gscParser::T__26);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Expression_listContext ------------------------------------------------------------------

gscParser::Expression_listContext::Expression_listContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<gscParser::ExpressionContext *> gscParser::Expression_listContext::expression() {
  return getRuleContexts<gscParser::ExpressionContext>();
}

gscParser::ExpressionContext* gscParser::Expression_listContext::expression(size_t i) {
  return getRuleContext<gscParser::ExpressionContext>(i);
}


size_t gscParser::Expression_listContext::getRuleIndex() const {
  return gscParser::RuleExpression_list;
}


std::any gscParser::Expression_listContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitExpression_list(this);
  else
    return visitor->visitChildren(this);
}

gscParser::Expression_listContext* gscParser::expression_list() {
  Expression_listContext *_localctx = _tracker.createInstance<Expression_listContext>(_ctx, getState());
  enterRule(_localctx, 18, gscParser::RuleExpression_list);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(211);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 336820224) != 0) || ((((_la - 78) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 78)) & 217027535) != 0)) {
      setState(203);
      expression();
      setState(208);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == gscParser::T__23) {
        setState(204);
        match(gscParser::T__23);
        setState(205);
        expression();
        setState(210);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Statement_blockContext ------------------------------------------------------------------

gscParser::Statement_blockContext::Statement_blockContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<gscParser::StatementContext *> gscParser::Statement_blockContext::statement() {
  return getRuleContexts<gscParser::StatementContext>();
}

gscParser::StatementContext* gscParser::Statement_blockContext::statement(size_t i) {
  return getRuleContext<gscParser::StatementContext>(i);
}


size_t gscParser::Statement_blockContext::getRuleIndex() const {
  return gscParser::RuleStatement_block;
}


std::any gscParser::Statement_blockContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitStatement_block(this);
  else
    return visitor->visitChildren(this);
}

gscParser::Statement_blockContext* gscParser::statement_block() {
  Statement_blockContext *_localctx = _tracker.createInstance<Statement_blockContext>(_ctx, getState());
  enterRule(_localctx, 20, gscParser::RuleStatement_block);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(213);
    match(gscParser::T__27);
    setState(217);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 68545867380738) != 0) || ((((_la - 78) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 78)) & 218076111) != 0)) {
      setState(214);
      statement();
      setState(219);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(220);
    match(gscParser::T__28);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- StatementContext ------------------------------------------------------------------

gscParser::StatementContext::StatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

gscParser::Statement_blockContext* gscParser::StatementContext::statement_block() {
  return getRuleContext<gscParser::Statement_blockContext>(0);
}

gscParser::Statement_dev_blockContext* gscParser::StatementContext::statement_dev_block() {
  return getRuleContext<gscParser::Statement_dev_blockContext>(0);
}

gscParser::Statement_forContext* gscParser::StatementContext::statement_for() {
  return getRuleContext<gscParser::Statement_forContext>(0);
}

gscParser::Statement_ifContext* gscParser::StatementContext::statement_if() {
  return getRuleContext<gscParser::Statement_ifContext>(0);
}

gscParser::Statement_whileContext* gscParser::StatementContext::statement_while() {
  return getRuleContext<gscParser::Statement_whileContext>(0);
}

gscParser::Statement_foreachContext* gscParser::StatementContext::statement_foreach() {
  return getRuleContext<gscParser::Statement_foreachContext>(0);
}

gscParser::Statement_instContext* gscParser::StatementContext::statement_inst() {
  return getRuleContext<gscParser::Statement_instContext>(0);
}

gscParser::Statement_switchContext* gscParser::StatementContext::statement_switch() {
  return getRuleContext<gscParser::Statement_switchContext>(0);
}

tree::TerminalNode* gscParser::StatementContext::IDENTIFIER() {
  return getToken(gscParser::IDENTIFIER, 0);
}


size_t gscParser::StatementContext::getRuleIndex() const {
  return gscParser::RuleStatement;
}


std::any gscParser::StatementContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitStatement(this);
  else
    return visitor->visitChildren(this);
}

gscParser::StatementContext* gscParser::statement() {
  StatementContext *_localctx = _tracker.createInstance<StatementContext>(_ctx, getState());
  enterRule(_localctx, 22, gscParser::RuleStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(224);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 19, _ctx)) {
    case 1: {
      setState(222);
      match(gscParser::IDENTIFIER);
      setState(223);
      match(gscParser::T__29);
      break;
    }

    default:
      break;
    }
    setState(234);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 20, _ctx)) {
    case 1: {
      setState(226);
      statement_block();
      break;
    }

    case 2: {
      setState(227);
      statement_dev_block();
      break;
    }

    case 3: {
      setState(228);
      statement_for();
      break;
    }

    case 4: {
      setState(229);
      statement_if();
      break;
    }

    case 5: {
      setState(230);
      statement_while();
      break;
    }

    case 6: {
      setState(231);
      statement_foreach();
      break;
    }

    case 7: {
      setState(232);
      statement_inst();
      break;
    }

    case 8: {
      setState(233);
      statement_switch();
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Statement_dev_blockContext ------------------------------------------------------------------

gscParser::Statement_dev_blockContext::Statement_dev_blockContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<gscParser::StatementContext *> gscParser::Statement_dev_blockContext::statement() {
  return getRuleContexts<gscParser::StatementContext>();
}

gscParser::StatementContext* gscParser::Statement_dev_blockContext::statement(size_t i) {
  return getRuleContext<gscParser::StatementContext>(i);
}


size_t gscParser::Statement_dev_blockContext::getRuleIndex() const {
  return gscParser::RuleStatement_dev_block;
}


std::any gscParser::Statement_dev_blockContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitStatement_dev_block(this);
  else
    return visitor->visitChildren(this);
}

gscParser::Statement_dev_blockContext* gscParser::statement_dev_block() {
  Statement_dev_blockContext *_localctx = _tracker.createInstance<Statement_dev_blockContext>(_ctx, getState());
  enterRule(_localctx, 24, gscParser::RuleStatement_dev_block);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(236);
    match(gscParser::T__0);
    setState(240);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 68545867380738) != 0) || ((((_la - 78) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 78)) & 218076111) != 0)) {
      setState(237);
      statement();
      setState(242);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(243);
    match(gscParser::T__1);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Statement_forContext ------------------------------------------------------------------

gscParser::Statement_forContext::Statement_forContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

gscParser::StatementContext* gscParser::Statement_forContext::statement() {
  return getRuleContext<gscParser::StatementContext>(0);
}

std::vector<gscParser::ExpressionContext *> gscParser::Statement_forContext::expression() {
  return getRuleContexts<gscParser::ExpressionContext>();
}

gscParser::ExpressionContext* gscParser::Statement_forContext::expression(size_t i) {
  return getRuleContext<gscParser::ExpressionContext>(i);
}


size_t gscParser::Statement_forContext::getRuleIndex() const {
  return gscParser::RuleStatement_for;
}


std::any gscParser::Statement_forContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitStatement_for(this);
  else
    return visitor->visitChildren(this);
}

gscParser::Statement_forContext* gscParser::statement_for() {
  Statement_forContext *_localctx = _tracker.createInstance<Statement_forContext>(_ctx, getState());
  enterRule(_localctx, 26, gscParser::RuleStatement_for);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(245);
    match(gscParser::T__30);
    setState(246);
    match(gscParser::T__13);
    setState(248);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 336820224) != 0) || ((((_la - 78) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 78)) & 217027535) != 0)) {
      setState(247);
      expression();
    }
    setState(250);
    match(gscParser::T__4);
    setState(252);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 336820224) != 0) || ((((_la - 78) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 78)) & 217027535) != 0)) {
      setState(251);
      expression();
    }
    setState(254);
    match(gscParser::T__4);
    setState(256);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 336820224) != 0) || ((((_la - 78) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 78)) & 217027535) != 0)) {
      setState(255);
      expression();
    }
    setState(258);
    match(gscParser::T__14);
    setState(259);
    statement();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Statement_whileContext ------------------------------------------------------------------

gscParser::Statement_whileContext::Statement_whileContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

gscParser::ExpressionContext* gscParser::Statement_whileContext::expression() {
  return getRuleContext<gscParser::ExpressionContext>(0);
}

gscParser::StatementContext* gscParser::Statement_whileContext::statement() {
  return getRuleContext<gscParser::StatementContext>(0);
}


size_t gscParser::Statement_whileContext::getRuleIndex() const {
  return gscParser::RuleStatement_while;
}


std::any gscParser::Statement_whileContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitStatement_while(this);
  else
    return visitor->visitChildren(this);
}

gscParser::Statement_whileContext* gscParser::statement_while() {
  Statement_whileContext *_localctx = _tracker.createInstance<Statement_whileContext>(_ctx, getState());
  enterRule(_localctx, 28, gscParser::RuleStatement_while);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(261);
    match(gscParser::T__31);
    setState(262);
    match(gscParser::T__13);
    setState(263);
    expression();
    setState(264);
    match(gscParser::T__14);
    setState(265);
    statement();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Statement_dowhileContext ------------------------------------------------------------------

gscParser::Statement_dowhileContext::Statement_dowhileContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

gscParser::StatementContext* gscParser::Statement_dowhileContext::statement() {
  return getRuleContext<gscParser::StatementContext>(0);
}

gscParser::ExpressionContext* gscParser::Statement_dowhileContext::expression() {
  return getRuleContext<gscParser::ExpressionContext>(0);
}


size_t gscParser::Statement_dowhileContext::getRuleIndex() const {
  return gscParser::RuleStatement_dowhile;
}


std::any gscParser::Statement_dowhileContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitStatement_dowhile(this);
  else
    return visitor->visitChildren(this);
}

gscParser::Statement_dowhileContext* gscParser::statement_dowhile() {
  Statement_dowhileContext *_localctx = _tracker.createInstance<Statement_dowhileContext>(_ctx, getState());
  enterRule(_localctx, 30, gscParser::RuleStatement_dowhile);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(267);
    match(gscParser::T__32);
    setState(268);
    statement();
    setState(269);
    match(gscParser::T__31);
    setState(270);
    match(gscParser::T__13);
    setState(271);
    expression();
    setState(272);
    match(gscParser::T__14);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Statement_foreachContext ------------------------------------------------------------------

gscParser::Statement_foreachContext::Statement_foreachContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> gscParser::Statement_foreachContext::IDENTIFIER() {
  return getTokens(gscParser::IDENTIFIER);
}

tree::TerminalNode* gscParser::Statement_foreachContext::IDENTIFIER(size_t i) {
  return getToken(gscParser::IDENTIFIER, i);
}

gscParser::ExpressionContext* gscParser::Statement_foreachContext::expression() {
  return getRuleContext<gscParser::ExpressionContext>(0);
}

gscParser::StatementContext* gscParser::Statement_foreachContext::statement() {
  return getRuleContext<gscParser::StatementContext>(0);
}


size_t gscParser::Statement_foreachContext::getRuleIndex() const {
  return gscParser::RuleStatement_foreach;
}


std::any gscParser::Statement_foreachContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitStatement_foreach(this);
  else
    return visitor->visitChildren(this);
}

gscParser::Statement_foreachContext* gscParser::statement_foreach() {
  Statement_foreachContext *_localctx = _tracker.createInstance<Statement_foreachContext>(_ctx, getState());
  enterRule(_localctx, 32, gscParser::RuleStatement_foreach);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(274);
    match(gscParser::T__33);
    setState(275);
    match(gscParser::T__13);
    setState(276);
    match(gscParser::IDENTIFIER);
    setState(279);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__23) {
      setState(277);
      match(gscParser::T__23);
      setState(278);
      match(gscParser::IDENTIFIER);
    }
    setState(281);
    match(gscParser::T__34);
    setState(282);
    expression();
    setState(283);
    match(gscParser::T__14);
    setState(284);
    statement();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Statement_ifContext ------------------------------------------------------------------

gscParser::Statement_ifContext::Statement_ifContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

gscParser::ExpressionContext* gscParser::Statement_ifContext::expression() {
  return getRuleContext<gscParser::ExpressionContext>(0);
}

std::vector<gscParser::StatementContext *> gscParser::Statement_ifContext::statement() {
  return getRuleContexts<gscParser::StatementContext>();
}

gscParser::StatementContext* gscParser::Statement_ifContext::statement(size_t i) {
  return getRuleContext<gscParser::StatementContext>(i);
}


size_t gscParser::Statement_ifContext::getRuleIndex() const {
  return gscParser::RuleStatement_if;
}


std::any gscParser::Statement_ifContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitStatement_if(this);
  else
    return visitor->visitChildren(this);
}

gscParser::Statement_ifContext* gscParser::statement_if() {
  Statement_ifContext *_localctx = _tracker.createInstance<Statement_ifContext>(_ctx, getState());
  enterRule(_localctx, 34, gscParser::RuleStatement_if);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(286);
    match(gscParser::T__35);
    setState(287);
    match(gscParser::T__13);
    setState(288);
    expression();
    setState(289);
    match(gscParser::T__14);
    setState(290);
    statement();
    setState(293);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 26, _ctx)) {
    case 1: {
      setState(291);
      match(gscParser::T__36);
      setState(292);
      statement();
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Statement_switchContext ------------------------------------------------------------------

gscParser::Statement_switchContext::Statement_switchContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

gscParser::ExpressionContext* gscParser::Statement_switchContext::expression() {
  return getRuleContext<gscParser::ExpressionContext>(0);
}

std::vector<gscParser::Const_exprContext *> gscParser::Statement_switchContext::const_expr() {
  return getRuleContexts<gscParser::Const_exprContext>();
}

gscParser::Const_exprContext* gscParser::Statement_switchContext::const_expr(size_t i) {
  return getRuleContext<gscParser::Const_exprContext>(i);
}

std::vector<gscParser::StatementContext *> gscParser::Statement_switchContext::statement() {
  return getRuleContexts<gscParser::StatementContext>();
}

gscParser::StatementContext* gscParser::Statement_switchContext::statement(size_t i) {
  return getRuleContext<gscParser::StatementContext>(i);
}


size_t gscParser::Statement_switchContext::getRuleIndex() const {
  return gscParser::RuleStatement_switch;
}


std::any gscParser::Statement_switchContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitStatement_switch(this);
  else
    return visitor->visitChildren(this);
}

gscParser::Statement_switchContext* gscParser::statement_switch() {
  Statement_switchContext *_localctx = _tracker.createInstance<Statement_switchContext>(_ctx, getState());
  enterRule(_localctx, 36, gscParser::RuleStatement_switch);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(295);
    match(gscParser::T__37);
    setState(296);
    match(gscParser::T__13);
    setState(297);
    expression();
    setState(298);
    match(gscParser::T__14);
    setState(299);
    match(gscParser::T__27);
    setState(312); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(303);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case gscParser::T__38: {
          setState(300);
          match(gscParser::T__38);
          setState(301);
          const_expr();
          break;
        }

        case gscParser::T__39: {
          setState(302);
          match(gscParser::T__39);
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(305);
      match(gscParser::T__29);
      setState(309);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while ((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & 68545867380738) != 0) || ((((_la - 78) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 78)) & 218076111) != 0)) {
        setState(306);
        statement();
        setState(311);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
      setState(314); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while (_la == gscParser::T__38

    || _la == gscParser::T__39);
    setState(316);
    match(gscParser::T__28);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Statement_instContext ------------------------------------------------------------------

gscParser::Statement_instContext::Statement_instContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

gscParser::ExpressionContext* gscParser::Statement_instContext::expression() {
  return getRuleContext<gscParser::ExpressionContext>(0);
}

gscParser::Operator_instContext* gscParser::Statement_instContext::operator_inst() {
  return getRuleContext<gscParser::Operator_instContext>(0);
}

gscParser::Statement_dowhileContext* gscParser::Statement_instContext::statement_dowhile() {
  return getRuleContext<gscParser::Statement_dowhileContext>(0);
}

gscParser::Function_call_expContext* gscParser::Statement_instContext::function_call_exp() {
  return getRuleContext<gscParser::Function_call_expContext>(0);
}

gscParser::Nop_defContext* gscParser::Statement_instContext::nop_def() {
  return getRuleContext<gscParser::Nop_defContext>(0);
}

gscParser::Devop_defContext* gscParser::Statement_instContext::devop_def() {
  return getRuleContext<gscParser::Devop_defContext>(0);
}


size_t gscParser::Statement_instContext::getRuleIndex() const {
  return gscParser::RuleStatement_inst;
}


std::any gscParser::Statement_instContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitStatement_inst(this);
  else
    return visitor->visitChildren(this);
}

gscParser::Statement_instContext* gscParser::statement_inst() {
  Statement_instContext *_localctx = _tracker.createInstance<Statement_instContext>(_ctx, getState());
  enterRule(_localctx, 38, gscParser::RuleStatement_inst);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(324);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 30, _ctx)) {
    case 1: {
      setState(318);
      expression();
      break;
    }

    case 2: {
      setState(319);
      operator_inst();
      break;
    }

    case 3: {
      setState(320);
      statement_dowhile();
      break;
    }

    case 4: {
      setState(321);
      function_call_exp();
      break;
    }

    case 5: {
      setState(322);
      nop_def();
      break;
    }

    case 6: {
      setState(323);
      devop_def();
      break;
    }

    default:
      break;
    }
    setState(326);
    match(gscParser::T__4);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Nop_defContext ------------------------------------------------------------------

gscParser::Nop_defContext::Nop_defContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

gscParser::NumberContext* gscParser::Nop_defContext::number() {
  return getRuleContext<gscParser::NumberContext>(0);
}


size_t gscParser::Nop_defContext::getRuleIndex() const {
  return gscParser::RuleNop_def;
}


std::any gscParser::Nop_defContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitNop_def(this);
  else
    return visitor->visitChildren(this);
}

gscParser::Nop_defContext* gscParser::nop_def() {
  Nop_defContext *_localctx = _tracker.createInstance<Nop_defContext>(_ctx, getState());
  enterRule(_localctx, 40, gscParser::RuleNop_def);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(328);
    _la = _input->LA(1);
    if (!(_la == gscParser::T__40

    || _la == gscParser::T__41)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(333);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__13) {
      setState(329);
      match(gscParser::T__13);
      setState(330);
      number();
      setState(331);
      match(gscParser::T__14);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Devop_defContext ------------------------------------------------------------------

gscParser::Devop_defContext::Devop_defContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

gscParser::NumberContext* gscParser::Devop_defContext::number() {
  return getRuleContext<gscParser::NumberContext>(0);
}


size_t gscParser::Devop_defContext::getRuleIndex() const {
  return gscParser::RuleDevop_def;
}


std::any gscParser::Devop_defContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitDevop_def(this);
  else
    return visitor->visitChildren(this);
}

gscParser::Devop_defContext* gscParser::devop_def() {
  Devop_defContext *_localctx = _tracker.createInstance<Devop_defContext>(_ctx, getState());
  enterRule(_localctx, 42, gscParser::RuleDevop_def);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(335);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 61572651155456) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(336);
    match(gscParser::T__13);
    setState(337);
    number();
    setState(338);
    match(gscParser::T__14);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Function_componentContext ------------------------------------------------------------------

gscParser::Function_componentContext::Function_componentContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> gscParser::Function_componentContext::IDENTIFIER() {
  return getTokens(gscParser::IDENTIFIER);
}

tree::TerminalNode* gscParser::Function_componentContext::IDENTIFIER(size_t i) {
  return getToken(gscParser::IDENTIFIER, i);
}

gscParser::ExpressionContext* gscParser::Function_componentContext::expression() {
  return getRuleContext<gscParser::ExpressionContext>(0);
}


size_t gscParser::Function_componentContext::getRuleIndex() const {
  return gscParser::RuleFunction_component;
}


std::any gscParser::Function_componentContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitFunction_component(this);
  else
    return visitor->visitChildren(this);
}

gscParser::Function_componentContext* gscParser::function_component() {
  Function_componentContext *_localctx = _tracker.createInstance<Function_componentContext>(_ctx, getState());
  enterRule(_localctx, 44, gscParser::RuleFunction_component);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(359);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 33, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(342);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 32, _ctx)) {
      case 1: {
        setState(340);
        match(gscParser::IDENTIFIER);
        setState(341);
        match(gscParser::T__22);
        break;
      }

      default:
        break;
      }
      setState(344);
      match(gscParser::IDENTIFIER);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(345);
      match(gscParser::T__16);
      setState(346);
      match(gscParser::T__16);
      setState(347);
      expression();
      setState(348);
      match(gscParser::T__17);
      setState(349);
      match(gscParser::T__17);
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(351);
      match(gscParser::T__16);
      setState(352);
      match(gscParser::T__16);
      setState(353);
      expression();
      setState(354);
      match(gscParser::T__17);
      setState(355);
      match(gscParser::T__17);
      setState(356);
      match(gscParser::T__45);
      setState(357);
      match(gscParser::IDENTIFIER);
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Operator_instContext ------------------------------------------------------------------

gscParser::Operator_instContext::Operator_instContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* gscParser::Operator_instContext::BUILTIN() {
  return getToken(gscParser::BUILTIN, 0);
}

tree::TerminalNode* gscParser::Operator_instContext::IDENTIFIER() {
  return getToken(gscParser::IDENTIFIER, 0);
}

gscParser::ExpressionContext* gscParser::Operator_instContext::expression() {
  return getRuleContext<gscParser::ExpressionContext>(0);
}


size_t gscParser::Operator_instContext::getRuleIndex() const {
  return gscParser::RuleOperator_inst;
}


std::any gscParser::Operator_instContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitOperator_inst(this);
  else
    return visitor->visitChildren(this);
}

gscParser::Operator_instContext* gscParser::operator_inst() {
  Operator_instContext *_localctx = _tracker.createInstance<Operator_instContext>(_ctx, getState());
  enterRule(_localctx, 46, gscParser::RuleOperator_inst);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(361);
    match(gscParser::BUILTIN);
    setState(364);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 34, _ctx)) {
    case 1: {
      setState(362);
      match(gscParser::IDENTIFIER);
      break;
    }

    case 2: {
      setState(363);
      expression();
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExpressionContext ------------------------------------------------------------------

gscParser::ExpressionContext::ExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

gscParser::Set_expressionContext* gscParser::ExpressionContext::set_expression() {
  return getRuleContext<gscParser::Set_expressionContext>(0);
}

gscParser::Expression0Context* gscParser::ExpressionContext::expression0() {
  return getRuleContext<gscParser::Expression0Context>(0);
}

std::vector<gscParser::ExpressionContext *> gscParser::ExpressionContext::expression() {
  return getRuleContexts<gscParser::ExpressionContext>();
}

gscParser::ExpressionContext* gscParser::ExpressionContext::expression(size_t i) {
  return getRuleContext<gscParser::ExpressionContext>(i);
}


size_t gscParser::ExpressionContext::getRuleIndex() const {
  return gscParser::RuleExpression;
}


std::any gscParser::ExpressionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitExpression(this);
  else
    return visitor->visitChildren(this);
}

gscParser::ExpressionContext* gscParser::expression() {
  ExpressionContext *_localctx = _tracker.createInstance<ExpressionContext>(_ctx, getState());
  enterRule(_localctx, 48, gscParser::RuleExpression);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(374);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 35, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(366);
      set_expression();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(367);
      expression0(0);
      setState(368);
      match(gscParser::T__46);
      setState(369);
      expression();
      setState(370);
      match(gscParser::T__29);
      setState(371);
      expression();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(373);
      expression0(0);
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Set_expressionContext ------------------------------------------------------------------

gscParser::Set_expressionContext::Set_expressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

gscParser::Left_valueContext* gscParser::Set_expressionContext::left_value() {
  return getRuleContext<gscParser::Left_valueContext>(0);
}

gscParser::ExpressionContext* gscParser::Set_expressionContext::expression() {
  return getRuleContext<gscParser::ExpressionContext>(0);
}


size_t gscParser::Set_expressionContext::getRuleIndex() const {
  return gscParser::RuleSet_expression;
}


std::any gscParser::Set_expressionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitSet_expression(this);
  else
    return visitor->visitChildren(this);
}

gscParser::Set_expressionContext* gscParser::set_expression() {
  Set_expressionContext *_localctx = _tracker.createInstance<Set_expressionContext>(_ctx, getState());
  enterRule(_localctx, 50, gscParser::RuleSet_expression);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(376);
    left_value(0);
    setState(377);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 576179277326713856) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(378);
    expression();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Expression0Context ------------------------------------------------------------------

gscParser::Expression0Context::Expression0Context(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

gscParser::Expression1Context* gscParser::Expression0Context::expression1() {
  return getRuleContext<gscParser::Expression1Context>(0);
}

gscParser::Expression0Context* gscParser::Expression0Context::expression0() {
  return getRuleContext<gscParser::Expression0Context>(0);
}


size_t gscParser::Expression0Context::getRuleIndex() const {
  return gscParser::RuleExpression0;
}


std::any gscParser::Expression0Context::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitExpression0(this);
  else
    return visitor->visitChildren(this);
}


gscParser::Expression0Context* gscParser::expression0() {
   return expression0(0);
}

gscParser::Expression0Context* gscParser::expression0(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  gscParser::Expression0Context *_localctx = _tracker.createInstance<Expression0Context>(_ctx, parentState);
  gscParser::Expression0Context *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 52;
  enterRecursionRule(_localctx, 52, gscParser::RuleExpression0, precedence);

    

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(381);
    expression1(0);
    _ctx->stop = _input->LT(-1);
    setState(388);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 36, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression0Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression0);
        setState(383);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(384);
        match(gscParser::T__58);
        setState(385);
        expression1(0); 
      }
      setState(390);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 36, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- Expression1Context ------------------------------------------------------------------

gscParser::Expression1Context::Expression1Context(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

gscParser::Expression2Context* gscParser::Expression1Context::expression2() {
  return getRuleContext<gscParser::Expression2Context>(0);
}

gscParser::Expression1Context* gscParser::Expression1Context::expression1() {
  return getRuleContext<gscParser::Expression1Context>(0);
}


size_t gscParser::Expression1Context::getRuleIndex() const {
  return gscParser::RuleExpression1;
}


std::any gscParser::Expression1Context::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitExpression1(this);
  else
    return visitor->visitChildren(this);
}


gscParser::Expression1Context* gscParser::expression1() {
   return expression1(0);
}

gscParser::Expression1Context* gscParser::expression1(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  gscParser::Expression1Context *_localctx = _tracker.createInstance<Expression1Context>(_ctx, parentState);
  gscParser::Expression1Context *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 54;
  enterRecursionRule(_localctx, 54, gscParser::RuleExpression1, precedence);

    

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(392);
    expression2(0);
    _ctx->stop = _input->LT(-1);
    setState(399);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 37, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression1Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression1);
        setState(394);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(395);
        match(gscParser::T__59);
        setState(396);
        expression2(0); 
      }
      setState(401);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 37, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- Expression2Context ------------------------------------------------------------------

gscParser::Expression2Context::Expression2Context(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

gscParser::Expression3Context* gscParser::Expression2Context::expression3() {
  return getRuleContext<gscParser::Expression3Context>(0);
}

gscParser::Expression2Context* gscParser::Expression2Context::expression2() {
  return getRuleContext<gscParser::Expression2Context>(0);
}


size_t gscParser::Expression2Context::getRuleIndex() const {
  return gscParser::RuleExpression2;
}


std::any gscParser::Expression2Context::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitExpression2(this);
  else
    return visitor->visitChildren(this);
}


gscParser::Expression2Context* gscParser::expression2() {
   return expression2(0);
}

gscParser::Expression2Context* gscParser::expression2(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  gscParser::Expression2Context *_localctx = _tracker.createInstance<Expression2Context>(_ctx, parentState);
  gscParser::Expression2Context *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 56;
  enterRecursionRule(_localctx, 56, gscParser::RuleExpression2, precedence);

    

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(403);
    expression3(0);
    _ctx->stop = _input->LT(-1);
    setState(410);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 38, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression2Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression2);
        setState(405);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(406);
        match(gscParser::T__60);
        setState(407);
        expression3(0); 
      }
      setState(412);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 38, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- Expression3Context ------------------------------------------------------------------

gscParser::Expression3Context::Expression3Context(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

gscParser::Expression4Context* gscParser::Expression3Context::expression4() {
  return getRuleContext<gscParser::Expression4Context>(0);
}

gscParser::Expression3Context* gscParser::Expression3Context::expression3() {
  return getRuleContext<gscParser::Expression3Context>(0);
}


size_t gscParser::Expression3Context::getRuleIndex() const {
  return gscParser::RuleExpression3;
}


std::any gscParser::Expression3Context::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitExpression3(this);
  else
    return visitor->visitChildren(this);
}


gscParser::Expression3Context* gscParser::expression3() {
   return expression3(0);
}

gscParser::Expression3Context* gscParser::expression3(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  gscParser::Expression3Context *_localctx = _tracker.createInstance<Expression3Context>(_ctx, parentState);
  gscParser::Expression3Context *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 58;
  enterRecursionRule(_localctx, 58, gscParser::RuleExpression3, precedence);

    

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(414);
    expression4(0);
    _ctx->stop = _input->LT(-1);
    setState(421);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 39, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression3Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression3);
        setState(416);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(417);
        match(gscParser::T__61);
        setState(418);
        expression4(0); 
      }
      setState(423);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 39, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- Expression4Context ------------------------------------------------------------------

gscParser::Expression4Context::Expression4Context(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

gscParser::Expression5Context* gscParser::Expression4Context::expression5() {
  return getRuleContext<gscParser::Expression5Context>(0);
}

gscParser::Expression4Context* gscParser::Expression4Context::expression4() {
  return getRuleContext<gscParser::Expression4Context>(0);
}


size_t gscParser::Expression4Context::getRuleIndex() const {
  return gscParser::RuleExpression4;
}


std::any gscParser::Expression4Context::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitExpression4(this);
  else
    return visitor->visitChildren(this);
}


gscParser::Expression4Context* gscParser::expression4() {
   return expression4(0);
}

gscParser::Expression4Context* gscParser::expression4(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  gscParser::Expression4Context *_localctx = _tracker.createInstance<Expression4Context>(_ctx, parentState);
  gscParser::Expression4Context *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 60;
  enterRecursionRule(_localctx, 60, gscParser::RuleExpression4, precedence);

    

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(425);
    expression5(0);
    _ctx->stop = _input->LT(-1);
    setState(432);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 40, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression4Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression4);
        setState(427);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(428);
        match(gscParser::T__62);
        setState(429);
        expression5(0); 
      }
      setState(434);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 40, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- Expression5Context ------------------------------------------------------------------

gscParser::Expression5Context::Expression5Context(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

gscParser::Expression6Context* gscParser::Expression5Context::expression6() {
  return getRuleContext<gscParser::Expression6Context>(0);
}

gscParser::Expression5Context* gscParser::Expression5Context::expression5() {
  return getRuleContext<gscParser::Expression5Context>(0);
}


size_t gscParser::Expression5Context::getRuleIndex() const {
  return gscParser::RuleExpression5;
}


std::any gscParser::Expression5Context::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitExpression5(this);
  else
    return visitor->visitChildren(this);
}


gscParser::Expression5Context* gscParser::expression5() {
   return expression5(0);
}

gscParser::Expression5Context* gscParser::expression5(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  gscParser::Expression5Context *_localctx = _tracker.createInstance<Expression5Context>(_ctx, parentState);
  gscParser::Expression5Context *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 62;
  enterRecursionRule(_localctx, 62, gscParser::RuleExpression5, precedence);

    

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(436);
    expression6(0);
    _ctx->stop = _input->LT(-1);
    setState(443);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 41, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression5Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression5);
        setState(438);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(439);
        match(gscParser::T__25);
        setState(440);
        expression6(0); 
      }
      setState(445);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 41, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- Expression6Context ------------------------------------------------------------------

gscParser::Expression6Context::Expression6Context(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

gscParser::Expression7Context* gscParser::Expression6Context::expression7() {
  return getRuleContext<gscParser::Expression7Context>(0);
}

gscParser::Expression6Context* gscParser::Expression6Context::expression6() {
  return getRuleContext<gscParser::Expression6Context>(0);
}


size_t gscParser::Expression6Context::getRuleIndex() const {
  return gscParser::RuleExpression6;
}


std::any gscParser::Expression6Context::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitExpression6(this);
  else
    return visitor->visitChildren(this);
}


gscParser::Expression6Context* gscParser::expression6() {
   return expression6(0);
}

gscParser::Expression6Context* gscParser::expression6(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  gscParser::Expression6Context *_localctx = _tracker.createInstance<Expression6Context>(_ctx, parentState);
  gscParser::Expression6Context *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 64;
  enterRecursionRule(_localctx, 64, gscParser::RuleExpression6, precedence);

    size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(447);
    expression7(0);
    _ctx->stop = _input->LT(-1);
    setState(454);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 42, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression6Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression6);
        setState(449);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(450);
        _la = _input->LA(1);
        if (!(((((_la - 64) & ~ 0x3fULL) == 0) &&
          ((1ULL << (_la - 64)) & 15) != 0))) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(451);
        expression7(0); 
      }
      setState(456);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 42, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- Expression7Context ------------------------------------------------------------------

gscParser::Expression7Context::Expression7Context(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

gscParser::Expression8Context* gscParser::Expression7Context::expression8() {
  return getRuleContext<gscParser::Expression8Context>(0);
}

gscParser::Expression7Context* gscParser::Expression7Context::expression7() {
  return getRuleContext<gscParser::Expression7Context>(0);
}


size_t gscParser::Expression7Context::getRuleIndex() const {
  return gscParser::RuleExpression7;
}


std::any gscParser::Expression7Context::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitExpression7(this);
  else
    return visitor->visitChildren(this);
}


gscParser::Expression7Context* gscParser::expression7() {
   return expression7(0);
}

gscParser::Expression7Context* gscParser::expression7(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  gscParser::Expression7Context *_localctx = _tracker.createInstance<Expression7Context>(_ctx, parentState);
  gscParser::Expression7Context *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 66;
  enterRecursionRule(_localctx, 66, gscParser::RuleExpression7, precedence);

    size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(458);
    expression8(0);
    _ctx->stop = _input->LT(-1);
    setState(465);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 43, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression7Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression7);
        setState(460);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(461);
        _la = _input->LA(1);
        if (!(((((_la - 21) & ~ 0x3fULL) == 0) &&
          ((1ULL << (_la - 21)) & 2111062325329923) != 0))) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(462);
        expression8(0); 
      }
      setState(467);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 43, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- Expression8Context ------------------------------------------------------------------

gscParser::Expression8Context::Expression8Context(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

gscParser::Expression9Context* gscParser::Expression8Context::expression9() {
  return getRuleContext<gscParser::Expression9Context>(0);
}

gscParser::Expression8Context* gscParser::Expression8Context::expression8() {
  return getRuleContext<gscParser::Expression8Context>(0);
}


size_t gscParser::Expression8Context::getRuleIndex() const {
  return gscParser::RuleExpression8;
}


std::any gscParser::Expression8Context::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitExpression8(this);
  else
    return visitor->visitChildren(this);
}


gscParser::Expression8Context* gscParser::expression8() {
   return expression8(0);
}

gscParser::Expression8Context* gscParser::expression8(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  gscParser::Expression8Context *_localctx = _tracker.createInstance<Expression8Context>(_ctx, parentState);
  gscParser::Expression8Context *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 68;
  enterRecursionRule(_localctx, 68, gscParser::RuleExpression8, precedence);

    size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(469);
    expression9(0);
    _ctx->stop = _input->LT(-1);
    setState(476);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 44, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression8Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression8);
        setState(471);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(472);
        _la = _input->LA(1);
        if (!(_la == gscParser::T__20

        || _la == gscParser::T__67)) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(473);
        expression9(0); 
      }
      setState(478);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 44, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- Expression9Context ------------------------------------------------------------------

gscParser::Expression9Context::Expression9Context(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

gscParser::Expression10Context* gscParser::Expression9Context::expression10() {
  return getRuleContext<gscParser::Expression10Context>(0);
}

gscParser::Expression9Context* gscParser::Expression9Context::expression9() {
  return getRuleContext<gscParser::Expression9Context>(0);
}


size_t gscParser::Expression9Context::getRuleIndex() const {
  return gscParser::RuleExpression9;
}


std::any gscParser::Expression9Context::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitExpression9(this);
  else
    return visitor->visitChildren(this);
}


gscParser::Expression9Context* gscParser::expression9() {
   return expression9(0);
}

gscParser::Expression9Context* gscParser::expression9(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  gscParser::Expression9Context *_localctx = _tracker.createInstance<Expression9Context>(_ctx, parentState);
  gscParser::Expression9Context *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 70;
  enterRecursionRule(_localctx, 70, gscParser::RuleExpression9, precedence);

    size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(480);
    expression10(0);
    _ctx->stop = _input->LT(-1);
    setState(487);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 45, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression9Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression9);
        setState(482);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(483);
        _la = _input->LA(1);
        if (!(_la == gscParser::T__71

        || _la == gscParser::T__72)) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(484);
        expression10(0); 
      }
      setState(489);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 45, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- Expression10Context ------------------------------------------------------------------

gscParser::Expression10Context::Expression10Context(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

gscParser::Expression11Context* gscParser::Expression10Context::expression11() {
  return getRuleContext<gscParser::Expression11Context>(0);
}

gscParser::Expression10Context* gscParser::Expression10Context::expression10() {
  return getRuleContext<gscParser::Expression10Context>(0);
}


size_t gscParser::Expression10Context::getRuleIndex() const {
  return gscParser::RuleExpression10;
}


std::any gscParser::Expression10Context::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitExpression10(this);
  else
    return visitor->visitChildren(this);
}


gscParser::Expression10Context* gscParser::expression10() {
   return expression10(0);
}

gscParser::Expression10Context* gscParser::expression10(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  gscParser::Expression10Context *_localctx = _tracker.createInstance<Expression10Context>(_ctx, parentState);
  gscParser::Expression10Context *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 72;
  enterRecursionRule(_localctx, 72, gscParser::RuleExpression10, precedence);

    size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(491);
    expression11(0);
    _ctx->stop = _input->LT(-1);
    setState(498);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 46, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression10Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression10);
        setState(493);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(494);
        _la = _input->LA(1);
        if (!(_la == gscParser::T__73

        || _la == gscParser::T__74)) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(495);
        expression11(0); 
      }
      setState(500);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 46, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- Expression11Context ------------------------------------------------------------------

gscParser::Expression11Context::Expression11Context(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

gscParser::Expression12Context* gscParser::Expression11Context::expression12() {
  return getRuleContext<gscParser::Expression12Context>(0);
}

gscParser::Expression11Context* gscParser::Expression11Context::expression11() {
  return getRuleContext<gscParser::Expression11Context>(0);
}


size_t gscParser::Expression11Context::getRuleIndex() const {
  return gscParser::RuleExpression11;
}


std::any gscParser::Expression11Context::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitExpression11(this);
  else
    return visitor->visitChildren(this);
}


gscParser::Expression11Context* gscParser::expression11() {
   return expression11(0);
}

gscParser::Expression11Context* gscParser::expression11(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  gscParser::Expression11Context *_localctx = _tracker.createInstance<Expression11Context>(_ctx, parentState);
  gscParser::Expression11Context *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 74;
  enterRecursionRule(_localctx, 74, gscParser::RuleExpression11, precedence);

    size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(502);
    expression12();
    _ctx->stop = _input->LT(-1);
    setState(509);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 47, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression11Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression11);
        setState(504);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(505);
        _la = _input->LA(1);
        if (!(((((_la - 25) & ~ 0x3fULL) == 0) &&
          ((1ULL << (_la - 25)) & 6755399441055745) != 0))) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(506);
        expression12(); 
      }
      setState(511);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 47, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- Expression12Context ------------------------------------------------------------------

gscParser::Expression12Context::Expression12Context(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

gscParser::Expression13Context* gscParser::Expression12Context::expression13() {
  return getRuleContext<gscParser::Expression13Context>(0);
}

gscParser::Left_valueContext* gscParser::Expression12Context::left_value() {
  return getRuleContext<gscParser::Left_valueContext>(0);
}

gscParser::Is_expressionContext* gscParser::Expression12Context::is_expression() {
  return getRuleContext<gscParser::Is_expressionContext>(0);
}


size_t gscParser::Expression12Context::getRuleIndex() const {
  return gscParser::RuleExpression12;
}


std::any gscParser::Expression12Context::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitExpression12(this);
  else
    return visitor->visitChildren(this);
}

gscParser::Expression12Context* gscParser::expression12() {
  Expression12Context *_localctx = _tracker.createInstance<Expression12Context>(_ctx, getState());
  enterRule(_localctx, 76, gscParser::RuleExpression12);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(521);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 48, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(512);
      _la = _input->LA(1);
      if (!(_la == gscParser::T__77

      || _la == gscParser::T__78)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(513);
      expression13();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(514);
      _la = _input->LA(1);
      if (!(_la == gscParser::T__79

      || _la == gscParser::T__80)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(515);
      left_value(0);
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(516);
      left_value(0);
      setState(517);
      _la = _input->LA(1);
      if (!(_la == gscParser::T__79

      || _la == gscParser::T__80)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(519);
      is_expression();
      break;
    }

    case 5: {
      enterOuterAlt(_localctx, 5);
      setState(520);
      expression13();
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Expression13Context ------------------------------------------------------------------

gscParser::Expression13Context::Expression13Context(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

gscParser::Function_call_expContext* gscParser::Expression13Context::function_call_exp() {
  return getRuleContext<gscParser::Function_call_expContext>(0);
}

gscParser::Expression14Context* gscParser::Expression13Context::expression14() {
  return getRuleContext<gscParser::Expression14Context>(0);
}


size_t gscParser::Expression13Context::getRuleIndex() const {
  return gscParser::RuleExpression13;
}


std::any gscParser::Expression13Context::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitExpression13(this);
  else
    return visitor->visitChildren(this);
}

gscParser::Expression13Context* gscParser::expression13() {
  Expression13Context *_localctx = _tracker.createInstance<Expression13Context>(_ctx, getState());
  enterRule(_localctx, 78, gscParser::RuleExpression13);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(525);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 49, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(523);
      function_call_exp();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(524);
      expression14();
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Expression14Context ------------------------------------------------------------------

gscParser::Expression14Context::Expression14Context(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

gscParser::Const_exprContext* gscParser::Expression14Context::const_expr() {
  return getRuleContext<gscParser::Const_exprContext>(0);
}

gscParser::Expression15Context* gscParser::Expression14Context::expression15() {
  return getRuleContext<gscParser::Expression15Context>(0);
}

gscParser::Left_valueContext* gscParser::Expression14Context::left_value() {
  return getRuleContext<gscParser::Left_valueContext>(0);
}


size_t gscParser::Expression14Context::getRuleIndex() const {
  return gscParser::RuleExpression14;
}


std::any gscParser::Expression14Context::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitExpression14(this);
  else
    return visitor->visitChildren(this);
}

gscParser::Expression14Context* gscParser::expression14() {
  Expression14Context *_localctx = _tracker.createInstance<Expression14Context>(_ctx, getState());
  enterRule(_localctx, 80, gscParser::RuleExpression14);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(530);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 50, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(527);
      const_expr();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(528);
      expression15();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(529);
      left_value(0);
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Expression15Context ------------------------------------------------------------------

gscParser::Expression15Context::Expression15Context(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

gscParser::ExpressionContext* gscParser::Expression15Context::expression() {
  return getRuleContext<gscParser::ExpressionContext>(0);
}


size_t gscParser::Expression15Context::getRuleIndex() const {
  return gscParser::RuleExpression15;
}


std::any gscParser::Expression15Context::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitExpression15(this);
  else
    return visitor->visitChildren(this);
}

gscParser::Expression15Context* gscParser::expression15() {
  Expression15Context *_localctx = _tracker.createInstance<Expression15Context>(_ctx, getState());
  enterRule(_localctx, 82, gscParser::RuleExpression15);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(532);
    match(gscParser::T__13);
    setState(533);
    expression();
    setState(534);
    match(gscParser::T__14);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Is_expressionContext ------------------------------------------------------------------

gscParser::Is_expressionContext::Is_expressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

gscParser::Expression13Context* gscParser::Is_expressionContext::expression13() {
  return getRuleContext<gscParser::Expression13Context>(0);
}

tree::TerminalNode* gscParser::Is_expressionContext::IDENTIFIER() {
  return getToken(gscParser::IDENTIFIER, 0);
}

tree::TerminalNode* gscParser::Is_expressionContext::BOOL_VALUE() {
  return getToken(gscParser::BOOL_VALUE, 0);
}

tree::TerminalNode* gscParser::Is_expressionContext::UNDEFINED_VALUE() {
  return getToken(gscParser::UNDEFINED_VALUE, 0);
}


size_t gscParser::Is_expressionContext::getRuleIndex() const {
  return gscParser::RuleIs_expression;
}


std::any gscParser::Is_expressionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitIs_expression(this);
  else
    return visitor->visitChildren(this);
}

gscParser::Is_expressionContext* gscParser::is_expression() {
  Is_expressionContext *_localctx = _tracker.createInstance<Is_expressionContext>(_ctx, getState());
  enterRule(_localctx, 84, gscParser::RuleIs_expression);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(536);
    expression13();
    setState(537);
    match(gscParser::T__81);
    setState(539);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__82) {
      setState(538);
      match(gscParser::T__82);
    }
    setState(541);
    _la = _input->LA(1);
    if (!(_la == gscParser::T__10 || ((((_la - 99) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 99)) & 7) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Function_call_expContext ------------------------------------------------------------------

gscParser::Function_call_expContext::Function_call_expContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

gscParser::Function_callContext* gscParser::Function_call_expContext::function_call() {
  return getRuleContext<gscParser::Function_callContext>(0);
}

gscParser::Expression14Context* gscParser::Function_call_expContext::expression14() {
  return getRuleContext<gscParser::Expression14Context>(0);
}

gscParser::Function_componentContext* gscParser::Function_call_expContext::function_component() {
  return getRuleContext<gscParser::Function_componentContext>(0);
}

gscParser::Expression_listContext* gscParser::Function_call_expContext::expression_list() {
  return getRuleContext<gscParser::Expression_listContext>(0);
}


size_t gscParser::Function_call_expContext::getRuleIndex() const {
  return gscParser::RuleFunction_call_exp;
}


std::any gscParser::Function_call_expContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitFunction_call_exp(this);
  else
    return visitor->visitChildren(this);
}

gscParser::Function_call_expContext* gscParser::function_call_exp() {
  Function_call_expContext *_localctx = _tracker.createInstance<Function_call_expContext>(_ctx, getState());
  enterRule(_localctx, 86, gscParser::RuleFunction_call_exp);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(553);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 53, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(543);
      function_call(0);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(544);
      expression14();
      setState(546);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (((((_la - 84) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 84)) & 15) != 0)) {
        setState(545);
        _la = _input->LA(1);
        if (!(((((_la - 84) & ~ 0x3fULL) == 0) &&
          ((1ULL << (_la - 84)) & 15) != 0))) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
      }
      setState(548);
      function_component();
      setState(549);
      match(gscParser::T__13);
      setState(550);
      expression_list();
      setState(551);
      match(gscParser::T__14);
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Function_callContext ------------------------------------------------------------------

gscParser::Function_callContext::Function_callContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

gscParser::Function_componentContext* gscParser::Function_callContext::function_component() {
  return getRuleContext<gscParser::Function_componentContext>(0);
}

gscParser::Expression_listContext* gscParser::Function_callContext::expression_list() {
  return getRuleContext<gscParser::Expression_listContext>(0);
}

gscParser::Const_exprContext* gscParser::Function_callContext::const_expr() {
  return getRuleContext<gscParser::Const_exprContext>(0);
}

gscParser::Expression15Context* gscParser::Function_callContext::expression15() {
  return getRuleContext<gscParser::Expression15Context>(0);
}

gscParser::Function_callContext* gscParser::Function_callContext::function_call() {
  return getRuleContext<gscParser::Function_callContext>(0);
}


size_t gscParser::Function_callContext::getRuleIndex() const {
  return gscParser::RuleFunction_call;
}


std::any gscParser::Function_callContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitFunction_call(this);
  else
    return visitor->visitChildren(this);
}


gscParser::Function_callContext* gscParser::function_call() {
   return function_call(0);
}

gscParser::Function_callContext* gscParser::function_call(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  gscParser::Function_callContext *_localctx = _tracker.createInstance<Function_callContext>(_ctx, parentState);
  gscParser::Function_callContext *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 88;
  enterRecursionRule(_localctx, 88, gscParser::RuleFunction_call, precedence);

    size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(576);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 57, _ctx)) {
    case 1: {
      setState(557);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (((((_la - 84) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 84)) & 15) != 0)) {
        setState(556);
        _la = _input->LA(1);
        if (!(((((_la - 84) & ~ 0x3fULL) == 0) &&
          ((1ULL << (_la - 84)) & 15) != 0))) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
      }
      setState(559);
      function_component();
      setState(560);
      match(gscParser::T__13);
      setState(561);
      expression_list();
      setState(562);
      match(gscParser::T__14);
      break;
    }

    case 2: {
      setState(566);
      _errHandler->sync(this);
      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 55, _ctx)) {
      case 1: {
        setState(564);
        const_expr();
        break;
      }

      case 2: {
        setState(565);
        expression15();
        break;
      }

      default:
        break;
      }
      setState(569);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (((((_la - 84) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 84)) & 15) != 0)) {
        setState(568);
        _la = _input->LA(1);
        if (!(((((_la - 84) & ~ 0x3fULL) == 0) &&
          ((1ULL << (_la - 84)) & 15) != 0))) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
      }
      setState(571);
      function_component();
      setState(572);
      match(gscParser::T__13);
      setState(573);
      expression_list();
      setState(574);
      match(gscParser::T__14);
      break;
    }

    default:
      break;
    }
    _ctx->stop = _input->LT(-1);
    setState(589);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 59, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Function_callContext>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleFunction_call);
        setState(578);

        if (!(precpred(_ctx, 1))) throw FailedPredicateException(this, "precpred(_ctx, 1)");
        setState(580);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (((((_la - 84) & ~ 0x3fULL) == 0) &&
          ((1ULL << (_la - 84)) & 15) != 0)) {
          setState(579);
          _la = _input->LA(1);
          if (!(((((_la - 84) & ~ 0x3fULL) == 0) &&
            ((1ULL << (_la - 84)) & 15) != 0))) {
          _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
        }
        setState(582);
        function_component();
        setState(583);
        match(gscParser::T__13);
        setState(584);
        expression_list();
        setState(585);
        match(gscParser::T__14); 
      }
      setState(591);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 59, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- Left_valueContext ------------------------------------------------------------------

gscParser::Left_valueContext::Left_valueContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

gscParser::IdfContext* gscParser::Left_valueContext::idf() {
  return getRuleContext<gscParser::IdfContext>(0);
}

gscParser::Function_callContext* gscParser::Left_valueContext::function_call() {
  return getRuleContext<gscParser::Function_callContext>(0);
}

gscParser::Const_exprContext* gscParser::Left_valueContext::const_expr() {
  return getRuleContext<gscParser::Const_exprContext>(0);
}

gscParser::Expression15Context* gscParser::Left_valueContext::expression15() {
  return getRuleContext<gscParser::Expression15Context>(0);
}

gscParser::ExpressionContext* gscParser::Left_valueContext::expression() {
  return getRuleContext<gscParser::ExpressionContext>(0);
}

gscParser::Left_valueContext* gscParser::Left_valueContext::left_value() {
  return getRuleContext<gscParser::Left_valueContext>(0);
}


size_t gscParser::Left_valueContext::getRuleIndex() const {
  return gscParser::RuleLeft_value;
}


std::any gscParser::Left_valueContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitLeft_value(this);
  else
    return visitor->visitChildren(this);
}


gscParser::Left_valueContext* gscParser::left_value() {
   return left_value(0);
}

gscParser::Left_valueContext* gscParser::left_value(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  gscParser::Left_valueContext *_localctx = _tracker.createInstance<Left_valueContext>(_ctx, parentState);
  gscParser::Left_valueContext *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 90;
  enterRecursionRule(_localctx, 90, gscParser::RuleLeft_value, precedence);

    size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(616);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 63, _ctx)) {
    case 1: {
      setState(593);
      idf();
      break;
    }

    case 2: {
      setState(597);
      _errHandler->sync(this);
      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 60, _ctx)) {
      case 1: {
        setState(594);
        function_call(0);
        break;
      }

      case 2: {
        setState(595);
        const_expr();
        break;
      }

      case 3: {
        setState(596);
        expression15();
        break;
      }

      default:
        break;
      }
      setState(599);
      _la = _input->LA(1);
      if (!(_la == gscParser::T__87

      || _la == gscParser::T__88)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(605);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case gscParser::IDENTIFIER: {
          setState(600);
          idf();
          break;
        }

        case gscParser::T__13: {
          setState(601);
          match(gscParser::T__13);
          setState(602);
          expression();
          setState(603);
          match(gscParser::T__14);
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      break;
    }

    case 3: {
      setState(610);
      _errHandler->sync(this);
      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 62, _ctx)) {
      case 1: {
        setState(607);
        function_call(0);
        break;
      }

      case 2: {
        setState(608);
        const_expr();
        break;
      }

      case 3: {
        setState(609);
        expression15();
        break;
      }

      default:
        break;
      }
      setState(612);
      match(gscParser::T__16);
      setState(613);
      expression();
      setState(614);
      match(gscParser::T__17);
      break;
    }

    default:
      break;
    }
    _ctx->stop = _input->LT(-1);
    setState(634);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 66, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        setState(632);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 65, _ctx)) {
        case 1: {
          _localctx = _tracker.createInstance<Left_valueContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleLeft_value);
          setState(618);

          if (!(precpred(_ctx, 4))) throw FailedPredicateException(this, "precpred(_ctx, 4)");
          setState(619);
          _la = _input->LA(1);
          if (!(_la == gscParser::T__87

          || _la == gscParser::T__88)) {
          _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(625);
          _errHandler->sync(this);
          switch (_input->LA(1)) {
            case gscParser::IDENTIFIER: {
              setState(620);
              idf();
              break;
            }

            case gscParser::T__13: {
              setState(621);
              match(gscParser::T__13);
              setState(622);
              expression();
              setState(623);
              match(gscParser::T__14);
              break;
            }

          default:
            throw NoViableAltException(this);
          }
          break;
        }

        case 2: {
          _localctx = _tracker.createInstance<Left_valueContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleLeft_value);
          setState(627);

          if (!(precpred(_ctx, 3))) throw FailedPredicateException(this, "precpred(_ctx, 3)");
          setState(628);
          match(gscParser::T__16);
          setState(629);
          expression();
          setState(630);
          match(gscParser::T__17);
          break;
        }

        default:
          break;
        } 
      }
      setState(636);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 66, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- Const_exprContext ------------------------------------------------------------------

gscParser::Const_exprContext::Const_exprContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

gscParser::Vector_valueContext* gscParser::Const_exprContext::vector_value() {
  return getRuleContext<gscParser::Vector_valueContext>(0);
}

gscParser::Array_defContext* gscParser::Const_exprContext::array_def() {
  return getRuleContext<gscParser::Array_defContext>(0);
}

gscParser::Struct_defContext* gscParser::Const_exprContext::struct_def() {
  return getRuleContext<gscParser::Struct_defContext>(0);
}

gscParser::NumberContext* gscParser::Const_exprContext::number() {
  return getRuleContext<gscParser::NumberContext>(0);
}

gscParser::FunctionContext* gscParser::Const_exprContext::function() {
  return getRuleContext<gscParser::FunctionContext>(0);
}

tree::TerminalNode* gscParser::Const_exprContext::BOOL_VALUE() {
  return getToken(gscParser::BOOL_VALUE, 0);
}

tree::TerminalNode* gscParser::Const_exprContext::FLOATVAL() {
  return getToken(gscParser::FLOATVAL, 0);
}

tree::TerminalNode* gscParser::Const_exprContext::STRING() {
  return getToken(gscParser::STRING, 0);
}

tree::TerminalNode* gscParser::Const_exprContext::HASHSTRING() {
  return getToken(gscParser::HASHSTRING, 0);
}

tree::TerminalNode* gscParser::Const_exprContext::UNDEFINED_VALUE() {
  return getToken(gscParser::UNDEFINED_VALUE, 0);
}

gscParser::Function_refContext* gscParser::Const_exprContext::function_ref() {
  return getRuleContext<gscParser::Function_refContext>(0);
}


size_t gscParser::Const_exprContext::getRuleIndex() const {
  return gscParser::RuleConst_expr;
}


std::any gscParser::Const_exprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitConst_expr(this);
  else
    return visitor->visitChildren(this);
}

gscParser::Const_exprContext* gscParser::const_expr() {
  Const_exprContext *_localctx = _tracker.createInstance<Const_exprContext>(_ctx, getState());
  enterRule(_localctx, 92, gscParser::RuleConst_expr);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(648);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 67, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(637);
      vector_value();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(638);
      array_def();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(639);
      struct_def();
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(640);
      number();
      break;
    }

    case 5: {
      enterOuterAlt(_localctx, 5);
      setState(641);
      function();
      break;
    }

    case 6: {
      enterOuterAlt(_localctx, 6);
      setState(642);
      match(gscParser::BOOL_VALUE);
      break;
    }

    case 7: {
      enterOuterAlt(_localctx, 7);
      setState(643);
      match(gscParser::FLOATVAL);
      break;
    }

    case 8: {
      enterOuterAlt(_localctx, 8);
      setState(644);
      match(gscParser::STRING);
      break;
    }

    case 9: {
      enterOuterAlt(_localctx, 9);
      setState(645);
      match(gscParser::HASHSTRING);
      break;
    }

    case 10: {
      enterOuterAlt(_localctx, 10);
      setState(646);
      match(gscParser::UNDEFINED_VALUE);
      break;
    }

    case 11: {
      enterOuterAlt(_localctx, 11);
      setState(647);
      function_ref();
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Function_refContext ------------------------------------------------------------------

gscParser::Function_refContext::Function_refContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> gscParser::Function_refContext::IDENTIFIER() {
  return getTokens(gscParser::IDENTIFIER);
}

tree::TerminalNode* gscParser::Function_refContext::IDENTIFIER(size_t i) {
  return getToken(gscParser::IDENTIFIER, i);
}

tree::TerminalNode* gscParser::Function_refContext::PATH() {
  return getToken(gscParser::PATH, 0);
}

gscParser::Left_valueContext* gscParser::Function_refContext::left_value() {
  return getRuleContext<gscParser::Left_valueContext>(0);
}


size_t gscParser::Function_refContext::getRuleIndex() const {
  return gscParser::RuleFunction_ref;
}


std::any gscParser::Function_refContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitFunction_ref(this);
  else
    return visitor->visitChildren(this);
}

gscParser::Function_refContext* gscParser::function_ref() {
  Function_refContext *_localctx = _tracker.createInstance<Function_refContext>(_ctx, getState());
  enterRule(_localctx, 94, gscParser::RuleFunction_ref);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(665);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 69, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(650);
      match(gscParser::T__25);
      setState(653);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 68, _ctx)) {
      case 1: {
        setState(651);
        match(gscParser::IDENTIFIER);
        setState(652);
        match(gscParser::T__22);
        break;
      }

      default:
        break;
      }
      setState(655);
      match(gscParser::IDENTIFIER);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(656);
      match(gscParser::T__89);
      setState(657);
      match(gscParser::IDENTIFIER);
      setState(658);
      match(gscParser::T__20);
      setState(659);
      match(gscParser::PATH);
      setState(660);
      match(gscParser::T__21);
      setState(661);
      match(gscParser::T__22);
      setState(662);
      match(gscParser::IDENTIFIER);
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(663);
      match(gscParser::T__25);
      setState(664);
      left_value(0);
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- NumberContext ------------------------------------------------------------------

gscParser::NumberContext::NumberContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* gscParser::NumberContext::INTEGER10() {
  return getToken(gscParser::INTEGER10, 0);
}

tree::TerminalNode* gscParser::NumberContext::INTEGER16() {
  return getToken(gscParser::INTEGER16, 0);
}

tree::TerminalNode* gscParser::NumberContext::INTEGER8() {
  return getToken(gscParser::INTEGER8, 0);
}

tree::TerminalNode* gscParser::NumberContext::INTEGER2() {
  return getToken(gscParser::INTEGER2, 0);
}

tree::TerminalNode* gscParser::NumberContext::IDENTIFIER() {
  return getToken(gscParser::IDENTIFIER, 0);
}


size_t gscParser::NumberContext::getRuleIndex() const {
  return gscParser::RuleNumber;
}


std::any gscParser::NumberContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitNumber(this);
  else
    return visitor->visitChildren(this);
}

gscParser::NumberContext* gscParser::number() {
  NumberContext *_localctx = _tracker.createInstance<NumberContext>(_ctx, getState());
  enterRule(_localctx, 96, gscParser::RuleNumber);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(667);
    _la = _input->LA(1);
    if (!(((((_la - 93) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 93)) & 271) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Vector_valueContext ------------------------------------------------------------------

gscParser::Vector_valueContext::Vector_valueContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<gscParser::ExpressionContext *> gscParser::Vector_valueContext::expression() {
  return getRuleContexts<gscParser::ExpressionContext>();
}

gscParser::ExpressionContext* gscParser::Vector_valueContext::expression(size_t i) {
  return getRuleContext<gscParser::ExpressionContext>(i);
}


size_t gscParser::Vector_valueContext::getRuleIndex() const {
  return gscParser::RuleVector_value;
}


std::any gscParser::Vector_valueContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitVector_value(this);
  else
    return visitor->visitChildren(this);
}

gscParser::Vector_valueContext* gscParser::vector_value() {
  Vector_valueContext *_localctx = _tracker.createInstance<Vector_valueContext>(_ctx, getState());
  enterRule(_localctx, 98, gscParser::RuleVector_value);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(669);
    match(gscParser::T__13);
    setState(670);
    expression();
    setState(671);
    match(gscParser::T__23);
    setState(672);
    expression();
    setState(673);
    match(gscParser::T__23);
    setState(674);
    expression();
    setState(675);
    match(gscParser::T__14);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Array_defContext ------------------------------------------------------------------

gscParser::Array_defContext::Array_defContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<gscParser::ExpressionContext *> gscParser::Array_defContext::expression() {
  return getRuleContexts<gscParser::ExpressionContext>();
}

gscParser::ExpressionContext* gscParser::Array_defContext::expression(size_t i) {
  return getRuleContext<gscParser::ExpressionContext>(i);
}


size_t gscParser::Array_defContext::getRuleIndex() const {
  return gscParser::RuleArray_def;
}


std::any gscParser::Array_defContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitArray_def(this);
  else
    return visitor->visitChildren(this);
}

gscParser::Array_defContext* gscParser::array_def() {
  Array_defContext *_localctx = _tracker.createInstance<Array_defContext>(_ctx, getState());
  enterRule(_localctx, 100, gscParser::RuleArray_def);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(677);
    match(gscParser::T__16);
    setState(699);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 336820224) != 0) || ((((_la - 78) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 78)) & 217027535) != 0)) {
      setState(681);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 70, _ctx)) {
      case 1: {
        setState(678);
        expression();
        setState(679);
        match(gscParser::T__29);
        break;
      }

      default:
        break;
      }
      setState(683);
      expression();
      setState(693);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 72, _ctx);
      while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
        if (alt == 1) {
          setState(684);
          match(gscParser::T__23);
          setState(688);
          _errHandler->sync(this);

          switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 71, _ctx)) {
          case 1: {
            setState(685);
            expression();
            setState(686);
            match(gscParser::T__29);
            break;
          }

          default:
            break;
          }
          setState(690);
          expression(); 
        }
        setState(695);
        _errHandler->sync(this);
        alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 72, _ctx);
      }
      setState(697);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == gscParser::T__23) {
        setState(696);
        match(gscParser::T__23);
      }
    }
    setState(701);
    match(gscParser::T__17);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Struct_defContext ------------------------------------------------------------------

gscParser::Struct_defContext::Struct_defContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<gscParser::ExpressionContext *> gscParser::Struct_defContext::expression() {
  return getRuleContexts<gscParser::ExpressionContext>();
}

gscParser::ExpressionContext* gscParser::Struct_defContext::expression(size_t i) {
  return getRuleContext<gscParser::ExpressionContext>(i);
}

std::vector<tree::TerminalNode *> gscParser::Struct_defContext::STRUCT_IDENTIFIER() {
  return getTokens(gscParser::STRUCT_IDENTIFIER);
}

tree::TerminalNode* gscParser::Struct_defContext::STRUCT_IDENTIFIER(size_t i) {
  return getToken(gscParser::STRUCT_IDENTIFIER, i);
}


size_t gscParser::Struct_defContext::getRuleIndex() const {
  return gscParser::RuleStruct_def;
}


std::any gscParser::Struct_defContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitStruct_def(this);
  else
    return visitor->visitChildren(this);
}

gscParser::Struct_defContext* gscParser::struct_def() {
  Struct_defContext *_localctx = _tracker.createInstance<Struct_defContext>(_ctx, getState());
  enterRule(_localctx, 102, gscParser::RuleStruct_def);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(703);
    match(gscParser::T__27);
    setState(725);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 336820224) != 0) || ((((_la - 78) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 78)) & 233804751) != 0)) {
      setState(706);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case gscParser::STRUCT_IDENTIFIER: {
          setState(704);
          match(gscParser::STRUCT_IDENTIFIER);
          break;
        }

        case gscParser::T__10:
        case gscParser::T__11:
        case gscParser::T__12:
        case gscParser::T__13:
        case gscParser::T__15:
        case gscParser::T__16:
        case gscParser::T__19:
        case gscParser::T__25:
        case gscParser::T__27:
        case gscParser::T__77:
        case gscParser::T__78:
        case gscParser::T__79:
        case gscParser::T__80:
        case gscParser::T__83:
        case gscParser::T__84:
        case gscParser::T__85:
        case gscParser::T__86:
        case gscParser::T__89:
        case gscParser::INTEGER10:
        case gscParser::INTEGER16:
        case gscParser::INTEGER8:
        case gscParser::INTEGER2:
        case gscParser::FLOATVAL:
        case gscParser::BOOL_VALUE:
        case gscParser::UNDEFINED_VALUE:
        case gscParser::IDENTIFIER:
        case gscParser::STRING:
        case gscParser::HASHSTRING: {
          setState(705);
          expression();
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(708);
      match(gscParser::T__29);
      setState(709);
      expression();
      setState(719);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 77, _ctx);
      while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
        if (alt == 1) {
          setState(710);
          match(gscParser::T__23);
          setState(713);
          _errHandler->sync(this);
          switch (_input->LA(1)) {
            case gscParser::STRUCT_IDENTIFIER: {
              setState(711);
              match(gscParser::STRUCT_IDENTIFIER);
              break;
            }

            case gscParser::T__10:
            case gscParser::T__11:
            case gscParser::T__12:
            case gscParser::T__13:
            case gscParser::T__15:
            case gscParser::T__16:
            case gscParser::T__19:
            case gscParser::T__25:
            case gscParser::T__27:
            case gscParser::T__77:
            case gscParser::T__78:
            case gscParser::T__79:
            case gscParser::T__80:
            case gscParser::T__83:
            case gscParser::T__84:
            case gscParser::T__85:
            case gscParser::T__86:
            case gscParser::T__89:
            case gscParser::INTEGER10:
            case gscParser::INTEGER16:
            case gscParser::INTEGER8:
            case gscParser::INTEGER2:
            case gscParser::FLOATVAL:
            case gscParser::BOOL_VALUE:
            case gscParser::UNDEFINED_VALUE:
            case gscParser::IDENTIFIER:
            case gscParser::STRING:
            case gscParser::HASHSTRING: {
              setState(712);
              expression();
              break;
            }

          default:
            throw NoViableAltException(this);
          }
          setState(715);
          match(gscParser::T__29);
          setState(716);
          expression(); 
        }
        setState(721);
        _errHandler->sync(this);
        alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 77, _ctx);
      }
      setState(723);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == gscParser::T__23) {
        setState(722);
        match(gscParser::T__23);
      }
    }
    setState(727);
    match(gscParser::T__28);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- IdfContext ------------------------------------------------------------------

gscParser::IdfContext::IdfContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* gscParser::IdfContext::IDENTIFIER() {
  return getToken(gscParser::IDENTIFIER, 0);
}


size_t gscParser::IdfContext::getRuleIndex() const {
  return gscParser::RuleIdf;
}


std::any gscParser::IdfContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitIdf(this);
  else
    return visitor->visitChildren(this);
}

gscParser::IdfContext* gscParser::idf() {
  IdfContext *_localctx = _tracker.createInstance<IdfContext>(_ctx, getState());
  enterRule(_localctx, 104, gscParser::RuleIdf);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(729);
    match(gscParser::IDENTIFIER);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

bool gscParser::sempred(RuleContext *context, size_t ruleIndex, size_t predicateIndex) {
  switch (ruleIndex) {
    case 26: return expression0Sempred(antlrcpp::downCast<Expression0Context *>(context), predicateIndex);
    case 27: return expression1Sempred(antlrcpp::downCast<Expression1Context *>(context), predicateIndex);
    case 28: return expression2Sempred(antlrcpp::downCast<Expression2Context *>(context), predicateIndex);
    case 29: return expression3Sempred(antlrcpp::downCast<Expression3Context *>(context), predicateIndex);
    case 30: return expression4Sempred(antlrcpp::downCast<Expression4Context *>(context), predicateIndex);
    case 31: return expression5Sempred(antlrcpp::downCast<Expression5Context *>(context), predicateIndex);
    case 32: return expression6Sempred(antlrcpp::downCast<Expression6Context *>(context), predicateIndex);
    case 33: return expression7Sempred(antlrcpp::downCast<Expression7Context *>(context), predicateIndex);
    case 34: return expression8Sempred(antlrcpp::downCast<Expression8Context *>(context), predicateIndex);
    case 35: return expression9Sempred(antlrcpp::downCast<Expression9Context *>(context), predicateIndex);
    case 36: return expression10Sempred(antlrcpp::downCast<Expression10Context *>(context), predicateIndex);
    case 37: return expression11Sempred(antlrcpp::downCast<Expression11Context *>(context), predicateIndex);
    case 44: return function_callSempred(antlrcpp::downCast<Function_callContext *>(context), predicateIndex);
    case 45: return left_valueSempred(antlrcpp::downCast<Left_valueContext *>(context), predicateIndex);

  default:
    break;
  }
  return true;
}

bool gscParser::expression0Sempred(Expression0Context *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 0: return precpred(_ctx, 2);

  default:
    break;
  }
  return true;
}

bool gscParser::expression1Sempred(Expression1Context *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 1: return precpred(_ctx, 2);

  default:
    break;
  }
  return true;
}

bool gscParser::expression2Sempred(Expression2Context *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 2: return precpred(_ctx, 2);

  default:
    break;
  }
  return true;
}

bool gscParser::expression3Sempred(Expression3Context *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 3: return precpred(_ctx, 2);

  default:
    break;
  }
  return true;
}

bool gscParser::expression4Sempred(Expression4Context *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 4: return precpred(_ctx, 2);

  default:
    break;
  }
  return true;
}

bool gscParser::expression5Sempred(Expression5Context *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 5: return precpred(_ctx, 2);

  default:
    break;
  }
  return true;
}

bool gscParser::expression6Sempred(Expression6Context *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 6: return precpred(_ctx, 2);

  default:
    break;
  }
  return true;
}

bool gscParser::expression7Sempred(Expression7Context *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 7: return precpred(_ctx, 2);

  default:
    break;
  }
  return true;
}

bool gscParser::expression8Sempred(Expression8Context *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 8: return precpred(_ctx, 2);

  default:
    break;
  }
  return true;
}

bool gscParser::expression9Sempred(Expression9Context *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 9: return precpred(_ctx, 2);

  default:
    break;
  }
  return true;
}

bool gscParser::expression10Sempred(Expression10Context *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 10: return precpred(_ctx, 2);

  default:
    break;
  }
  return true;
}

bool gscParser::expression11Sempred(Expression11Context *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 11: return precpred(_ctx, 2);

  default:
    break;
  }
  return true;
}

bool gscParser::function_callSempred(Function_callContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 12: return precpred(_ctx, 1);

  default:
    break;
  }
  return true;
}

bool gscParser::left_valueSempred(Left_valueContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 13: return precpred(_ctx, 4);
    case 14: return precpred(_ctx, 3);

  default:
    break;
  }
  return true;
}

void gscParser::initialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  gscParserInitialize();
#else
  ::antlr4::internal::call_once(gscParserOnceFlag, gscParserInitialize);
#endif
}
