
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
      "prog", "include", "namespace", "function", "param_list", "param_val", 
      "expression_list", "statement_block", "statement", "statement_for", 
      "statement_while", "statement_dowhile", "statement_foreach", "statement_if", 
      "statement_switch", "statement_inst", "function_call", "function_component", 
      "operator_inst", "expression", "set_expression", "expression1", "expression2", 
      "expression3", "expression4", "expression5", "expression6", "expression7", 
      "expression8", "expression9", "expression10", "expression11", "expression12", 
      "expression13", "expression14", "left_value", "const_expr", "function_ref", 
      "number", "vector_value", "array_def", "struct_def"
    },
    std::vector<std::string>{
      "", "'#include'", "'#using'", "';'", "'#namespace'", "'function'", 
      "'private'", "'autoexec'", "'event_handler'", "'['", "']'", "'('", 
      "')'", "','", "'*'", "'&'", "'='", "'...'", "'{'", "'}'", "':'", "'for'", 
      "'while'", "'do'", "'foreach'", "'in'", "'if'", "'else'", "'switch'", 
      "'case'", "'default'", "'thread'", "'childthread'", "'::'", "'[['", 
      "']]'", "'->'", "'\\u003F'", "'+='", "'-='", "'/='", "'*='", "'%='", 
      "'&='", "'|='", "'^='", "'<<='", "'>>='", "'~='", "'||'", "'&&'", 
      "'|'", "'^'", "'!='", "'=='", "'!=='", "'==='", "'<'", "'<='", "'>'", 
      "'>='", "'<<'", "'>>'", "'+'", "'-'", "'/'", "'%'", "'!'", "'~'", 
      "'++'", "'--'", "'.'", "'@'", "'#'", "", "", "", "", "", "", "", "", 
      "", "", "", "", "", "'undefined'"
    },
    std::vector<std::string>{
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
      "", "", "", "", "", "", "NEWLINE", "WHITESPACE", "INTEGER10", "INTEGER16", 
      "INTEGER8", "INTEGER2", "FLOATVAL", "IDENTIFIER", "STRUCT_IDENTIFIER", 
      "PATH", "STRING", "HASHSTRING", "BOOL_VALUE", "UNDEFINED_VALUE"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,87,595,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,7,
  	14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,7,
  	21,2,22,7,22,2,23,7,23,2,24,7,24,2,25,7,25,2,26,7,26,2,27,7,27,2,28,7,
  	28,2,29,7,29,2,30,7,30,2,31,7,31,2,32,7,32,2,33,7,33,2,34,7,34,2,35,7,
  	35,2,36,7,36,2,37,7,37,2,38,7,38,2,39,7,39,2,40,7,40,2,41,7,41,1,0,1,
  	0,1,0,5,0,88,8,0,10,0,12,0,91,9,0,1,0,1,0,1,1,1,1,1,1,1,1,1,2,1,2,1,2,
  	1,2,1,3,3,3,104,8,3,1,3,3,3,107,8,3,1,3,3,3,110,8,3,1,3,1,3,1,3,1,3,3,
  	3,116,8,3,1,3,1,3,1,3,1,3,1,3,1,3,1,4,1,4,1,4,5,4,127,8,4,10,4,12,4,130,
  	9,4,3,4,132,8,4,1,5,3,5,135,8,5,1,5,1,5,1,5,3,5,140,8,5,1,5,3,5,143,8,
  	5,1,6,1,6,1,6,5,6,148,8,6,10,6,12,6,151,9,6,3,6,153,8,6,1,7,1,7,5,7,157,
  	8,7,10,7,12,7,160,9,7,1,7,1,7,1,8,1,8,3,8,166,8,8,1,8,1,8,1,8,1,8,1,8,
  	1,8,1,8,3,8,175,8,8,1,9,1,9,1,9,3,9,180,8,9,1,9,1,9,3,9,184,8,9,1,9,1,
  	9,3,9,188,8,9,1,9,1,9,1,9,1,10,1,10,1,10,1,10,1,10,1,10,1,11,1,11,1,11,
  	1,11,1,11,1,11,1,11,1,12,1,12,1,12,1,12,1,12,3,12,211,8,12,1,12,1,12,
  	1,12,1,12,1,12,1,13,1,13,1,13,1,13,1,13,1,13,1,13,3,13,225,8,13,1,14,
  	1,14,1,14,1,14,1,14,1,14,1,14,1,14,3,14,235,8,14,1,14,1,14,5,14,239,8,
  	14,10,14,12,14,242,9,14,4,14,244,8,14,11,14,12,14,245,1,14,1,14,1,15,
  	1,15,1,15,3,15,253,8,15,1,15,1,15,1,16,1,16,3,16,259,8,16,1,16,1,16,1,
  	16,1,16,1,16,1,16,1,16,3,16,268,8,16,1,16,1,16,1,16,1,16,1,16,3,16,275,
  	8,16,1,16,1,16,3,16,279,8,16,1,16,1,16,1,16,1,16,1,16,5,16,286,8,16,10,
  	16,12,16,289,9,16,1,17,1,17,3,17,293,8,17,1,17,1,17,1,17,1,17,1,17,1,
  	17,1,17,1,17,1,17,1,17,1,17,3,17,306,8,17,1,18,1,18,3,18,310,8,18,1,19,
  	1,19,1,19,1,19,1,19,1,19,1,19,1,19,3,19,320,8,19,1,20,1,20,1,20,1,20,
  	1,21,1,21,1,21,1,21,1,21,1,21,5,21,332,8,21,10,21,12,21,335,9,21,1,22,
  	1,22,1,22,1,22,1,22,1,22,5,22,343,8,22,10,22,12,22,346,9,22,1,23,1,23,
  	1,23,1,23,1,23,1,23,5,23,354,8,23,10,23,12,23,357,9,23,1,24,1,24,1,24,
  	1,24,1,24,1,24,5,24,365,8,24,10,24,12,24,368,9,24,1,25,1,25,1,25,1,25,
  	1,25,1,25,5,25,376,8,25,10,25,12,25,379,9,25,1,26,1,26,1,26,1,26,1,26,
  	1,26,5,26,387,8,26,10,26,12,26,390,9,26,1,27,1,27,1,27,1,27,1,27,1,27,
  	5,27,398,8,27,10,27,12,27,401,9,27,1,28,1,28,1,28,1,28,1,28,1,28,5,28,
  	409,8,28,10,28,12,28,412,9,28,1,29,1,29,1,29,1,29,1,29,1,29,5,29,420,
  	8,29,10,29,12,29,423,9,29,1,30,1,30,1,30,1,30,1,30,1,30,5,30,431,8,30,
  	10,30,12,30,434,9,30,1,31,1,31,1,31,1,31,1,31,1,31,5,31,442,8,31,10,31,
  	12,31,445,9,31,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,3,32,455,8,32,
  	1,33,1,33,3,33,459,8,33,1,34,1,34,1,34,1,34,1,34,1,34,3,34,467,8,34,1,
  	35,1,35,1,35,1,35,1,35,1,35,1,35,1,35,1,35,1,35,1,35,1,35,1,35,3,35,482,
  	8,35,1,35,1,35,1,35,1,35,1,35,1,35,3,35,490,8,35,1,35,1,35,1,35,1,35,
  	1,35,3,35,497,8,35,1,35,1,35,1,35,1,35,3,35,503,8,35,1,35,1,35,1,35,1,
  	35,1,35,1,35,1,35,3,35,512,8,35,1,35,1,35,1,35,1,35,1,35,5,35,519,8,35,
  	10,35,12,35,522,9,35,1,36,1,36,1,36,1,36,1,36,1,36,1,36,1,36,1,36,1,36,
  	3,36,534,8,36,1,37,1,37,1,37,3,37,539,8,37,1,37,1,37,1,37,1,37,1,37,1,
  	37,1,37,1,37,3,37,549,8,37,1,38,1,38,1,39,1,39,1,39,1,39,1,39,1,39,1,
  	39,1,39,1,40,1,40,1,40,1,40,3,40,565,8,40,1,40,3,40,568,8,40,1,40,1,40,
  	3,40,572,8,40,5,40,574,8,40,10,40,12,40,577,9,40,1,40,1,40,1,41,1,41,
  	1,41,1,41,1,41,3,41,586,8,41,5,41,588,8,41,10,41,12,41,591,9,41,1,41,
  	1,41,1,41,0,13,32,42,44,46,48,50,52,54,56,58,60,62,70,42,0,2,4,6,8,10,
  	12,14,16,18,20,22,24,26,28,30,32,34,36,38,40,42,44,46,48,50,52,54,56,
  	58,60,62,64,66,68,70,72,74,76,78,80,82,0,14,1,0,1,2,2,0,81,81,83,83,1,
  	0,14,15,1,0,31,32,2,0,16,16,38,48,1,0,53,56,1,0,57,60,1,0,57,58,1,0,61,
  	62,1,0,63,64,2,0,14,14,65,66,1,0,67,68,1,0,69,70,1,0,76,79,639,0,89,1,
  	0,0,0,2,94,1,0,0,0,4,98,1,0,0,0,6,103,1,0,0,0,8,131,1,0,0,0,10,142,1,
  	0,0,0,12,152,1,0,0,0,14,154,1,0,0,0,16,165,1,0,0,0,18,176,1,0,0,0,20,
  	192,1,0,0,0,22,198,1,0,0,0,24,205,1,0,0,0,26,217,1,0,0,0,28,226,1,0,0,
  	0,30,252,1,0,0,0,32,274,1,0,0,0,34,305,1,0,0,0,36,307,1,0,0,0,38,319,
  	1,0,0,0,40,321,1,0,0,0,42,325,1,0,0,0,44,336,1,0,0,0,46,347,1,0,0,0,48,
  	358,1,0,0,0,50,369,1,0,0,0,52,380,1,0,0,0,54,391,1,0,0,0,56,402,1,0,0,
  	0,58,413,1,0,0,0,60,424,1,0,0,0,62,435,1,0,0,0,64,454,1,0,0,0,66,458,
  	1,0,0,0,68,466,1,0,0,0,70,502,1,0,0,0,72,533,1,0,0,0,74,548,1,0,0,0,76,
  	550,1,0,0,0,78,552,1,0,0,0,80,560,1,0,0,0,82,580,1,0,0,0,84,88,3,6,3,
  	0,85,88,3,2,1,0,86,88,3,4,2,0,87,84,1,0,0,0,87,85,1,0,0,0,87,86,1,0,0,
  	0,88,91,1,0,0,0,89,87,1,0,0,0,89,90,1,0,0,0,90,92,1,0,0,0,91,89,1,0,0,
  	0,92,93,5,0,0,1,93,1,1,0,0,0,94,95,7,0,0,0,95,96,7,1,0,0,96,97,5,3,0,
  	0,97,3,1,0,0,0,98,99,5,4,0,0,99,100,5,81,0,0,100,101,5,3,0,0,101,5,1,
  	0,0,0,102,104,5,5,0,0,103,102,1,0,0,0,103,104,1,0,0,0,104,106,1,0,0,0,
  	105,107,5,6,0,0,106,105,1,0,0,0,106,107,1,0,0,0,107,109,1,0,0,0,108,110,
  	5,7,0,0,109,108,1,0,0,0,109,110,1,0,0,0,110,115,1,0,0,0,111,112,5,8,0,
  	0,112,113,5,9,0,0,113,114,5,81,0,0,114,116,5,10,0,0,115,111,1,0,0,0,115,
  	116,1,0,0,0,116,117,1,0,0,0,117,118,5,81,0,0,118,119,5,11,0,0,119,120,
  	3,8,4,0,120,121,5,12,0,0,121,122,3,14,7,0,122,7,1,0,0,0,123,128,3,10,
  	5,0,124,125,5,13,0,0,125,127,3,10,5,0,126,124,1,0,0,0,127,130,1,0,0,0,
  	128,126,1,0,0,0,128,129,1,0,0,0,129,132,1,0,0,0,130,128,1,0,0,0,131,123,
  	1,0,0,0,131,132,1,0,0,0,132,9,1,0,0,0,133,135,7,2,0,0,134,133,1,0,0,0,
  	134,135,1,0,0,0,135,136,1,0,0,0,136,139,5,81,0,0,137,138,5,16,0,0,138,
  	140,3,38,19,0,139,137,1,0,0,0,139,140,1,0,0,0,140,143,1,0,0,0,141,143,
  	5,17,0,0,142,134,1,0,0,0,142,141,1,0,0,0,143,11,1,0,0,0,144,149,3,38,
  	19,0,145,146,5,13,0,0,146,148,3,38,19,0,147,145,1,0,0,0,148,151,1,0,0,
  	0,149,147,1,0,0,0,149,150,1,0,0,0,150,153,1,0,0,0,151,149,1,0,0,0,152,
  	144,1,0,0,0,152,153,1,0,0,0,153,13,1,0,0,0,154,158,5,18,0,0,155,157,3,
  	16,8,0,156,155,1,0,0,0,157,160,1,0,0,0,158,156,1,0,0,0,158,159,1,0,0,
  	0,159,161,1,0,0,0,160,158,1,0,0,0,161,162,5,19,0,0,162,15,1,0,0,0,163,
  	164,5,81,0,0,164,166,5,20,0,0,165,163,1,0,0,0,165,166,1,0,0,0,166,174,
  	1,0,0,0,167,175,3,14,7,0,168,175,3,18,9,0,169,175,3,26,13,0,170,175,3,
  	20,10,0,171,175,3,24,12,0,172,175,3,30,15,0,173,175,3,28,14,0,174,167,
  	1,0,0,0,174,168,1,0,0,0,174,169,1,0,0,0,174,170,1,0,0,0,174,171,1,0,0,
  	0,174,172,1,0,0,0,174,173,1,0,0,0,175,17,1,0,0,0,176,177,5,21,0,0,177,
  	179,5,11,0,0,178,180,3,38,19,0,179,178,1,0,0,0,179,180,1,0,0,0,180,181,
  	1,0,0,0,181,183,5,3,0,0,182,184,3,38,19,0,183,182,1,0,0,0,183,184,1,0,
  	0,0,184,185,1,0,0,0,185,187,5,3,0,0,186,188,3,38,19,0,187,186,1,0,0,0,
  	187,188,1,0,0,0,188,189,1,0,0,0,189,190,5,12,0,0,190,191,3,16,8,0,191,
  	19,1,0,0,0,192,193,5,22,0,0,193,194,5,11,0,0,194,195,3,38,19,0,195,196,
  	5,12,0,0,196,197,3,16,8,0,197,21,1,0,0,0,198,199,5,23,0,0,199,200,3,16,
  	8,0,200,201,5,22,0,0,201,202,5,11,0,0,202,203,3,38,19,0,203,204,5,12,
  	0,0,204,23,1,0,0,0,205,206,5,24,0,0,206,207,5,11,0,0,207,210,5,81,0,0,
  	208,209,5,13,0,0,209,211,5,81,0,0,210,208,1,0,0,0,210,211,1,0,0,0,211,
  	212,1,0,0,0,212,213,5,25,0,0,213,214,3,38,19,0,214,215,5,12,0,0,215,216,
  	3,16,8,0,216,25,1,0,0,0,217,218,5,26,0,0,218,219,5,11,0,0,219,220,3,38,
  	19,0,220,221,5,12,0,0,221,224,3,16,8,0,222,223,5,27,0,0,223,225,3,16,
  	8,0,224,222,1,0,0,0,224,225,1,0,0,0,225,27,1,0,0,0,226,227,5,28,0,0,227,
  	228,5,11,0,0,228,229,3,38,19,0,229,230,5,12,0,0,230,243,5,18,0,0,231,
  	232,5,29,0,0,232,235,3,72,36,0,233,235,5,30,0,0,234,231,1,0,0,0,234,233,
  	1,0,0,0,235,236,1,0,0,0,236,240,5,20,0,0,237,239,3,16,8,0,238,237,1,0,
  	0,0,239,242,1,0,0,0,240,238,1,0,0,0,240,241,1,0,0,0,241,244,1,0,0,0,242,
  	240,1,0,0,0,243,234,1,0,0,0,244,245,1,0,0,0,245,243,1,0,0,0,245,246,1,
  	0,0,0,246,247,1,0,0,0,247,248,5,19,0,0,248,29,1,0,0,0,249,253,3,32,16,
  	0,250,253,3,36,18,0,251,253,3,22,11,0,252,249,1,0,0,0,252,250,1,0,0,0,
  	252,251,1,0,0,0,252,253,1,0,0,0,253,254,1,0,0,0,254,255,5,3,0,0,255,31,
  	1,0,0,0,256,258,6,16,-1,0,257,259,7,3,0,0,258,257,1,0,0,0,258,259,1,0,
  	0,0,259,260,1,0,0,0,260,261,3,34,17,0,261,262,5,11,0,0,262,263,3,12,6,
  	0,263,264,5,12,0,0,264,275,1,0,0,0,265,267,3,68,34,0,266,268,7,3,0,0,
  	267,266,1,0,0,0,267,268,1,0,0,0,268,269,1,0,0,0,269,270,3,34,17,0,270,
  	271,5,11,0,0,271,272,3,12,6,0,272,273,5,12,0,0,273,275,1,0,0,0,274,256,
  	1,0,0,0,274,265,1,0,0,0,275,287,1,0,0,0,276,278,10,1,0,0,277,279,7,3,
  	0,0,278,277,1,0,0,0,278,279,1,0,0,0,279,280,1,0,0,0,280,281,3,34,17,0,
  	281,282,5,11,0,0,282,283,3,12,6,0,283,284,5,12,0,0,284,286,1,0,0,0,285,
  	276,1,0,0,0,286,289,1,0,0,0,287,285,1,0,0,0,287,288,1,0,0,0,288,33,1,
  	0,0,0,289,287,1,0,0,0,290,291,5,81,0,0,291,293,5,33,0,0,292,290,1,0,0,
  	0,292,293,1,0,0,0,293,294,1,0,0,0,294,306,5,81,0,0,295,296,5,34,0,0,296,
  	297,3,38,19,0,297,298,5,35,0,0,298,306,1,0,0,0,299,300,5,34,0,0,300,301,
  	3,38,19,0,301,302,5,35,0,0,302,303,5,36,0,0,303,304,5,81,0,0,304,306,
  	1,0,0,0,305,292,1,0,0,0,305,295,1,0,0,0,305,299,1,0,0,0,306,35,1,0,0,
  	0,307,309,5,81,0,0,308,310,5,81,0,0,309,308,1,0,0,0,309,310,1,0,0,0,310,
  	37,1,0,0,0,311,320,3,40,20,0,312,313,3,42,21,0,313,314,5,37,0,0,314,315,
  	3,38,19,0,315,316,5,20,0,0,316,317,3,38,19,0,317,320,1,0,0,0,318,320,
  	3,42,21,0,319,311,1,0,0,0,319,312,1,0,0,0,319,318,1,0,0,0,320,39,1,0,
  	0,0,321,322,3,70,35,0,322,323,7,4,0,0,323,324,3,38,19,0,324,41,1,0,0,
  	0,325,326,6,21,-1,0,326,327,3,44,22,0,327,333,1,0,0,0,328,329,10,2,0,
  	0,329,330,5,49,0,0,330,332,3,44,22,0,331,328,1,0,0,0,332,335,1,0,0,0,
  	333,331,1,0,0,0,333,334,1,0,0,0,334,43,1,0,0,0,335,333,1,0,0,0,336,337,
  	6,22,-1,0,337,338,3,46,23,0,338,344,1,0,0,0,339,340,10,2,0,0,340,341,
  	5,50,0,0,341,343,3,46,23,0,342,339,1,0,0,0,343,346,1,0,0,0,344,342,1,
  	0,0,0,344,345,1,0,0,0,345,45,1,0,0,0,346,344,1,0,0,0,347,348,6,23,-1,
  	0,348,349,3,48,24,0,349,355,1,0,0,0,350,351,10,2,0,0,351,352,5,51,0,0,
  	352,354,3,48,24,0,353,350,1,0,0,0,354,357,1,0,0,0,355,353,1,0,0,0,355,
  	356,1,0,0,0,356,47,1,0,0,0,357,355,1,0,0,0,358,359,6,24,-1,0,359,360,
  	3,50,25,0,360,366,1,0,0,0,361,362,10,2,0,0,362,363,5,52,0,0,363,365,3,
  	50,25,0,364,361,1,0,0,0,365,368,1,0,0,0,366,364,1,0,0,0,366,367,1,0,0,
  	0,367,49,1,0,0,0,368,366,1,0,0,0,369,370,6,25,-1,0,370,371,3,52,26,0,
  	371,377,1,0,0,0,372,373,10,2,0,0,373,374,5,15,0,0,374,376,3,52,26,0,375,
  	372,1,0,0,0,376,379,1,0,0,0,377,375,1,0,0,0,377,378,1,0,0,0,378,51,1,
  	0,0,0,379,377,1,0,0,0,380,381,6,26,-1,0,381,382,3,54,27,0,382,388,1,0,
  	0,0,383,384,10,2,0,0,384,385,7,5,0,0,385,387,3,54,27,0,386,383,1,0,0,
  	0,387,390,1,0,0,0,388,386,1,0,0,0,388,389,1,0,0,0,389,53,1,0,0,0,390,
  	388,1,0,0,0,391,392,6,27,-1,0,392,393,3,56,28,0,393,399,1,0,0,0,394,395,
  	10,2,0,0,395,396,7,6,0,0,396,398,3,56,28,0,397,394,1,0,0,0,398,401,1,
  	0,0,0,399,397,1,0,0,0,399,400,1,0,0,0,400,55,1,0,0,0,401,399,1,0,0,0,
  	402,403,6,28,-1,0,403,404,3,58,29,0,404,410,1,0,0,0,405,406,10,2,0,0,
  	406,407,7,7,0,0,407,409,3,58,29,0,408,405,1,0,0,0,409,412,1,0,0,0,410,
  	408,1,0,0,0,410,411,1,0,0,0,411,57,1,0,0,0,412,410,1,0,0,0,413,414,6,
  	29,-1,0,414,415,3,60,30,0,415,421,1,0,0,0,416,417,10,2,0,0,417,418,7,
  	8,0,0,418,420,3,60,30,0,419,416,1,0,0,0,420,423,1,0,0,0,421,419,1,0,0,
  	0,421,422,1,0,0,0,422,59,1,0,0,0,423,421,1,0,0,0,424,425,6,30,-1,0,425,
  	426,3,62,31,0,426,432,1,0,0,0,427,428,10,2,0,0,428,429,7,9,0,0,429,431,
  	3,62,31,0,430,427,1,0,0,0,431,434,1,0,0,0,432,430,1,0,0,0,432,433,1,0,
  	0,0,433,61,1,0,0,0,434,432,1,0,0,0,435,436,6,31,-1,0,436,437,3,64,32,
  	0,437,443,1,0,0,0,438,439,10,2,0,0,439,440,7,10,0,0,440,442,3,64,32,0,
  	441,438,1,0,0,0,442,445,1,0,0,0,443,441,1,0,0,0,443,444,1,0,0,0,444,63,
  	1,0,0,0,445,443,1,0,0,0,446,447,7,11,0,0,447,455,3,66,33,0,448,449,7,
  	12,0,0,449,455,3,70,35,0,450,451,3,70,35,0,451,452,7,12,0,0,452,455,1,
  	0,0,0,453,455,3,66,33,0,454,446,1,0,0,0,454,448,1,0,0,0,454,450,1,0,0,
  	0,454,453,1,0,0,0,455,65,1,0,0,0,456,459,3,32,16,0,457,459,3,68,34,0,
  	458,456,1,0,0,0,458,457,1,0,0,0,459,67,1,0,0,0,460,467,3,72,36,0,461,
  	462,5,11,0,0,462,463,3,38,19,0,463,464,5,12,0,0,464,467,1,0,0,0,465,467,
  	3,70,35,0,466,460,1,0,0,0,466,461,1,0,0,0,466,465,1,0,0,0,467,69,1,0,
  	0,0,468,469,6,35,-1,0,469,503,5,81,0,0,470,471,5,34,0,0,471,472,3,38,
  	19,0,472,473,5,35,0,0,473,474,5,36,0,0,474,475,5,81,0,0,475,503,1,0,0,
  	0,476,482,3,72,36,0,477,478,5,11,0,0,478,479,3,38,19,0,479,480,5,12,0,
  	0,480,482,1,0,0,0,481,476,1,0,0,0,481,477,1,0,0,0,482,483,1,0,0,0,483,
  	489,5,71,0,0,484,490,5,81,0,0,485,486,5,11,0,0,486,487,3,38,19,0,487,
  	488,5,12,0,0,488,490,1,0,0,0,489,484,1,0,0,0,489,485,1,0,0,0,490,503,
  	1,0,0,0,491,497,3,72,36,0,492,493,5,11,0,0,493,494,3,38,19,0,494,495,
  	5,12,0,0,495,497,1,0,0,0,496,491,1,0,0,0,496,492,1,0,0,0,497,498,1,0,
  	0,0,498,499,5,9,0,0,499,500,3,38,19,0,500,501,5,10,0,0,501,503,1,0,0,
  	0,502,468,1,0,0,0,502,470,1,0,0,0,502,481,1,0,0,0,502,496,1,0,0,0,503,
  	520,1,0,0,0,504,505,10,2,0,0,505,511,5,71,0,0,506,512,5,81,0,0,507,508,
  	5,11,0,0,508,509,3,38,19,0,509,510,5,12,0,0,510,512,1,0,0,0,511,506,1,
  	0,0,0,511,507,1,0,0,0,512,519,1,0,0,0,513,514,10,1,0,0,514,515,5,9,0,
  	0,515,516,3,38,19,0,516,517,5,10,0,0,517,519,1,0,0,0,518,504,1,0,0,0,
  	518,513,1,0,0,0,519,522,1,0,0,0,520,518,1,0,0,0,520,521,1,0,0,0,521,71,
  	1,0,0,0,522,520,1,0,0,0,523,534,3,78,39,0,524,534,3,80,40,0,525,534,3,
  	82,41,0,526,534,3,76,38,0,527,534,5,86,0,0,528,534,5,80,0,0,529,534,5,
  	84,0,0,530,534,5,85,0,0,531,534,5,87,0,0,532,534,3,74,37,0,533,523,1,
  	0,0,0,533,524,1,0,0,0,533,525,1,0,0,0,533,526,1,0,0,0,533,527,1,0,0,0,
  	533,528,1,0,0,0,533,529,1,0,0,0,533,530,1,0,0,0,533,531,1,0,0,0,533,532,
  	1,0,0,0,534,73,1,0,0,0,535,538,5,15,0,0,536,537,5,81,0,0,537,539,5,33,
  	0,0,538,536,1,0,0,0,538,539,1,0,0,0,539,540,1,0,0,0,540,549,5,81,0,0,
  	541,542,5,72,0,0,542,543,5,81,0,0,543,544,5,57,0,0,544,545,5,83,0,0,545,
  	546,5,59,0,0,546,547,5,33,0,0,547,549,5,81,0,0,548,535,1,0,0,0,548,541,
  	1,0,0,0,549,75,1,0,0,0,550,551,7,13,0,0,551,77,1,0,0,0,552,553,5,11,0,
  	0,553,554,3,38,19,0,554,555,5,13,0,0,555,556,3,38,19,0,556,557,5,13,0,
  	0,557,558,3,38,19,0,558,559,5,12,0,0,559,79,1,0,0,0,560,575,5,9,0,0,561,
  	565,5,85,0,0,562,563,5,73,0,0,563,565,3,76,38,0,564,561,1,0,0,0,564,562,
  	1,0,0,0,565,566,1,0,0,0,566,568,5,20,0,0,567,564,1,0,0,0,567,568,1,0,
  	0,0,568,569,1,0,0,0,569,571,3,38,19,0,570,572,5,13,0,0,571,570,1,0,0,
  	0,571,572,1,0,0,0,572,574,1,0,0,0,573,567,1,0,0,0,574,577,1,0,0,0,575,
  	573,1,0,0,0,575,576,1,0,0,0,576,578,1,0,0,0,577,575,1,0,0,0,578,579,5,
  	10,0,0,579,81,1,0,0,0,580,589,5,18,0,0,581,582,5,82,0,0,582,583,5,20,
  	0,0,583,585,3,38,19,0,584,586,5,13,0,0,585,584,1,0,0,0,585,586,1,0,0,
  	0,586,588,1,0,0,0,587,581,1,0,0,0,588,591,1,0,0,0,589,587,1,0,0,0,589,
  	590,1,0,0,0,590,592,1,0,0,0,591,589,1,0,0,0,592,593,5,19,0,0,593,83,1,
  	0,0,0,64,87,89,103,106,109,115,128,131,134,139,142,149,152,158,165,174,
  	179,183,187,210,224,234,240,245,252,258,267,274,278,287,292,305,309,319,
  	333,344,355,366,377,388,399,410,421,432,443,454,458,466,481,489,496,502,
  	511,518,520,533,538,548,564,567,571,575,585,589
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
    setState(89);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 502) != 0) || _la == gscParser::IDENTIFIER) {
      setState(87);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case gscParser::T__4:
        case gscParser::T__5:
        case gscParser::T__6:
        case gscParser::T__7:
        case gscParser::IDENTIFIER: {
          setState(84);
          function();
          break;
        }

        case gscParser::T__0:
        case gscParser::T__1: {
          setState(85);
          include();
          break;
        }

        case gscParser::T__3: {
          setState(86);
          namespace_();
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(91);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(92);
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
    setState(94);
    _la = _input->LA(1);
    if (!(_la == gscParser::T__0

    || _la == gscParser::T__1)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(95);
    _la = _input->LA(1);
    if (!(_la == gscParser::IDENTIFIER

    || _la == gscParser::PATH)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(96);
    match(gscParser::T__2);
   
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
    setState(98);
    match(gscParser::T__3);
    setState(99);
    match(gscParser::IDENTIFIER);
    setState(100);
    match(gscParser::T__2);
   
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

std::vector<tree::TerminalNode *> gscParser::FunctionContext::IDENTIFIER() {
  return getTokens(gscParser::IDENTIFIER);
}

tree::TerminalNode* gscParser::FunctionContext::IDENTIFIER(size_t i) {
  return getToken(gscParser::IDENTIFIER, i);
}

gscParser::Param_listContext* gscParser::FunctionContext::param_list() {
  return getRuleContext<gscParser::Param_listContext>(0);
}

gscParser::Statement_blockContext* gscParser::FunctionContext::statement_block() {
  return getRuleContext<gscParser::Statement_blockContext>(0);
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
  enterRule(_localctx, 6, gscParser::RuleFunction);
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
    setState(103);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__4) {
      setState(102);
      match(gscParser::T__4);
    }
    setState(106);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__5) {
      setState(105);
      match(gscParser::T__5);
    }
    setState(109);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__6) {
      setState(108);
      match(gscParser::T__6);
    }
    setState(115);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__7) {
      setState(111);
      match(gscParser::T__7);
      setState(112);
      match(gscParser::T__8);
      setState(113);
      match(gscParser::IDENTIFIER);
      setState(114);
      match(gscParser::T__9);
    }
    setState(117);
    match(gscParser::IDENTIFIER);
    setState(118);
    match(gscParser::T__10);
    setState(119);
    param_list();
    setState(120);
    match(gscParser::T__11);
    setState(121);
    statement_block();
   
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
  enterRule(_localctx, 8, gscParser::RuleParam_list);
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
    setState(131);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 180224) != 0) || _la == gscParser::IDENTIFIER) {
      setState(123);
      param_val();
      setState(128);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == gscParser::T__12) {
        setState(124);
        match(gscParser::T__12);
        setState(125);
        param_val();
        setState(130);
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
  enterRule(_localctx, 10, gscParser::RuleParam_val);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(142);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case gscParser::T__13:
      case gscParser::T__14:
      case gscParser::IDENTIFIER: {
        enterOuterAlt(_localctx, 1);
        setState(134);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == gscParser::T__13

        || _la == gscParser::T__14) {
          setState(133);
          _la = _input->LA(1);
          if (!(_la == gscParser::T__13

          || _la == gscParser::T__14)) {
          _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
        }
        setState(136);
        match(gscParser::IDENTIFIER);
        setState(139);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == gscParser::T__15) {
          setState(137);
          match(gscParser::T__15);
          setState(138);
          expression();
        }
        break;
      }

      case gscParser::T__16: {
        enterOuterAlt(_localctx, 2);
        setState(141);
        match(gscParser::T__16);
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
  enterRule(_localctx, 12, gscParser::RuleExpression_list);
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
    setState(152);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 23622617600) != 0) || ((((_la - 67) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 67)) & 1998383) != 0)) {
      setState(144);
      expression();
      setState(149);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == gscParser::T__12) {
        setState(145);
        match(gscParser::T__12);
        setState(146);
        expression();
        setState(151);
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
  enterRule(_localctx, 14, gscParser::RuleStatement_block);
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
    setState(154);
    match(gscParser::T__17);
    setState(158);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 23989619208) != 0) || ((((_la - 72) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 72)) & 62449) != 0)) {
      setState(155);
      statement();
      setState(160);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(161);
    match(gscParser::T__18);
   
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
  enterRule(_localctx, 16, gscParser::RuleStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(165);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 14, _ctx)) {
    case 1: {
      setState(163);
      match(gscParser::IDENTIFIER);
      setState(164);
      match(gscParser::T__19);
      break;
    }

    default:
      break;
    }
    setState(174);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 15, _ctx)) {
    case 1: {
      setState(167);
      statement_block();
      break;
    }

    case 2: {
      setState(168);
      statement_for();
      break;
    }

    case 3: {
      setState(169);
      statement_if();
      break;
    }

    case 4: {
      setState(170);
      statement_while();
      break;
    }

    case 5: {
      setState(171);
      statement_foreach();
      break;
    }

    case 6: {
      setState(172);
      statement_inst();
      break;
    }

    case 7: {
      setState(173);
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
  enterRule(_localctx, 18, gscParser::RuleStatement_for);
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
    setState(176);
    match(gscParser::T__20);
    setState(177);
    match(gscParser::T__10);
    setState(179);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 23622617600) != 0) || ((((_la - 67) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 67)) & 1998383) != 0)) {
      setState(178);
      expression();
    }
    setState(181);
    match(gscParser::T__2);
    setState(183);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 23622617600) != 0) || ((((_la - 67) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 67)) & 1998383) != 0)) {
      setState(182);
      expression();
    }
    setState(185);
    match(gscParser::T__2);
    setState(187);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 23622617600) != 0) || ((((_la - 67) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 67)) & 1998383) != 0)) {
      setState(186);
      expression();
    }
    setState(189);
    match(gscParser::T__11);
    setState(190);
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
  enterRule(_localctx, 20, gscParser::RuleStatement_while);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(192);
    match(gscParser::T__21);
    setState(193);
    match(gscParser::T__10);
    setState(194);
    expression();
    setState(195);
    match(gscParser::T__11);
    setState(196);
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
  enterRule(_localctx, 22, gscParser::RuleStatement_dowhile);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(198);
    match(gscParser::T__22);
    setState(199);
    statement();
    setState(200);
    match(gscParser::T__21);
    setState(201);
    match(gscParser::T__10);
    setState(202);
    expression();
    setState(203);
    match(gscParser::T__11);
   
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
  enterRule(_localctx, 24, gscParser::RuleStatement_foreach);
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
    setState(205);
    match(gscParser::T__23);
    setState(206);
    match(gscParser::T__10);
    setState(207);
    match(gscParser::IDENTIFIER);
    setState(210);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__12) {
      setState(208);
      match(gscParser::T__12);
      setState(209);
      match(gscParser::IDENTIFIER);
    }
    setState(212);
    match(gscParser::T__24);
    setState(213);
    expression();
    setState(214);
    match(gscParser::T__11);
    setState(215);
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
  enterRule(_localctx, 26, gscParser::RuleStatement_if);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(217);
    match(gscParser::T__25);
    setState(218);
    match(gscParser::T__10);
    setState(219);
    expression();
    setState(220);
    match(gscParser::T__11);
    setState(221);
    statement();
    setState(224);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 20, _ctx)) {
    case 1: {
      setState(222);
      match(gscParser::T__26);
      setState(223);
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
  enterRule(_localctx, 28, gscParser::RuleStatement_switch);
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
    setState(226);
    match(gscParser::T__27);
    setState(227);
    match(gscParser::T__10);
    setState(228);
    expression();
    setState(229);
    match(gscParser::T__11);
    setState(230);
    match(gscParser::T__17);
    setState(243); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(234);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case gscParser::T__28: {
          setState(231);
          match(gscParser::T__28);
          setState(232);
          const_expr();
          break;
        }

        case gscParser::T__29: {
          setState(233);
          match(gscParser::T__29);
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(236);
      match(gscParser::T__19);
      setState(240);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while ((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & 23989619208) != 0) || ((((_la - 72) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 72)) & 62449) != 0)) {
        setState(237);
        statement();
        setState(242);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
      setState(245); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while (_la == gscParser::T__28

    || _la == gscParser::T__29);
    setState(247);
    match(gscParser::T__18);
   
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

gscParser::Function_callContext* gscParser::Statement_instContext::function_call() {
  return getRuleContext<gscParser::Function_callContext>(0);
}

gscParser::Operator_instContext* gscParser::Statement_instContext::operator_inst() {
  return getRuleContext<gscParser::Operator_instContext>(0);
}

gscParser::Statement_dowhileContext* gscParser::Statement_instContext::statement_dowhile() {
  return getRuleContext<gscParser::Statement_dowhileContext>(0);
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
  enterRule(_localctx, 30, gscParser::RuleStatement_inst);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(252);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 24, _ctx)) {
    case 1: {
      setState(249);
      function_call(0);
      break;
    }

    case 2: {
      setState(250);
      operator_inst();
      break;
    }

    case 3: {
      setState(251);
      statement_dowhile();
      break;
    }

    default:
      break;
    }
    setState(254);
    match(gscParser::T__2);
   
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

gscParser::Expression14Context* gscParser::Function_callContext::expression14() {
  return getRuleContext<gscParser::Expression14Context>(0);
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
  size_t startState = 32;
  enterRecursionRule(_localctx, 32, gscParser::RuleFunction_call, precedence);

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
    setState(274);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 27, _ctx)) {
    case 1: {
      setState(258);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == gscParser::T__30

      || _la == gscParser::T__31) {
        setState(257);
        _la = _input->LA(1);
        if (!(_la == gscParser::T__30

        || _la == gscParser::T__31)) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
      }
      setState(260);
      function_component();
      setState(261);
      match(gscParser::T__10);
      setState(262);
      expression_list();
      setState(263);
      match(gscParser::T__11);
      break;
    }

    case 2: {
      setState(265);
      expression14();
      setState(267);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == gscParser::T__30

      || _la == gscParser::T__31) {
        setState(266);
        _la = _input->LA(1);
        if (!(_la == gscParser::T__30

        || _la == gscParser::T__31)) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
      }
      setState(269);
      function_component();
      setState(270);
      match(gscParser::T__10);
      setState(271);
      expression_list();
      setState(272);
      match(gscParser::T__11);
      break;
    }

    default:
      break;
    }
    _ctx->stop = _input->LT(-1);
    setState(287);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 29, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Function_callContext>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleFunction_call);
        setState(276);

        if (!(precpred(_ctx, 1))) throw FailedPredicateException(this, "precpred(_ctx, 1)");
        setState(278);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == gscParser::T__30

        || _la == gscParser::T__31) {
          setState(277);
          _la = _input->LA(1);
          if (!(_la == gscParser::T__30

          || _la == gscParser::T__31)) {
          _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
        }
        setState(280);
        function_component();
        setState(281);
        match(gscParser::T__10);
        setState(282);
        expression_list();
        setState(283);
        match(gscParser::T__11); 
      }
      setState(289);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 29, _ctx);
    }
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
  enterRule(_localctx, 34, gscParser::RuleFunction_component);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(305);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 31, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(292);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 30, _ctx)) {
      case 1: {
        setState(290);
        match(gscParser::IDENTIFIER);
        setState(291);
        match(gscParser::T__32);
        break;
      }

      default:
        break;
      }
      setState(294);
      match(gscParser::IDENTIFIER);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(295);
      match(gscParser::T__33);
      setState(296);
      expression();
      setState(297);
      match(gscParser::T__34);
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(299);
      match(gscParser::T__33);
      setState(300);
      expression();
      setState(301);
      match(gscParser::T__34);
      setState(302);
      match(gscParser::T__35);
      setState(303);
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

std::vector<tree::TerminalNode *> gscParser::Operator_instContext::IDENTIFIER() {
  return getTokens(gscParser::IDENTIFIER);
}

tree::TerminalNode* gscParser::Operator_instContext::IDENTIFIER(size_t i) {
  return getToken(gscParser::IDENTIFIER, i);
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
  enterRule(_localctx, 36, gscParser::RuleOperator_inst);
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
    setState(307);
    match(gscParser::IDENTIFIER);
    setState(309);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::IDENTIFIER) {
      setState(308);
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

//----------------- ExpressionContext ------------------------------------------------------------------

gscParser::ExpressionContext::ExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

gscParser::Set_expressionContext* gscParser::ExpressionContext::set_expression() {
  return getRuleContext<gscParser::Set_expressionContext>(0);
}

gscParser::Expression1Context* gscParser::ExpressionContext::expression1() {
  return getRuleContext<gscParser::Expression1Context>(0);
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
  enterRule(_localctx, 38, gscParser::RuleExpression);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(319);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 33, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(311);
      set_expression();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(312);
      expression1(0);
      setState(313);
      match(gscParser::T__36);
      setState(314);
      expression();
      setState(315);
      match(gscParser::T__19);
      setState(316);
      expression();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(318);
      expression1(0);
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
  enterRule(_localctx, 40, gscParser::RuleSet_expression);
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
    setState(321);
    left_value(0);
    setState(322);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 562675075579904) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(323);
    expression();
   
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
  size_t startState = 42;
  enterRecursionRule(_localctx, 42, gscParser::RuleExpression1, precedence);

    

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
    setState(326);
    expression2(0);
    _ctx->stop = _input->LT(-1);
    setState(333);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 34, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression1Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression1);
        setState(328);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(329);
        match(gscParser::T__48);
        setState(330);
        expression2(0); 
      }
      setState(335);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 34, _ctx);
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
  size_t startState = 44;
  enterRecursionRule(_localctx, 44, gscParser::RuleExpression2, precedence);

    

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
    setState(337);
    expression3(0);
    _ctx->stop = _input->LT(-1);
    setState(344);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 35, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression2Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression2);
        setState(339);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(340);
        match(gscParser::T__49);
        setState(341);
        expression3(0); 
      }
      setState(346);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 35, _ctx);
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
  size_t startState = 46;
  enterRecursionRule(_localctx, 46, gscParser::RuleExpression3, precedence);

    

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
    setState(348);
    expression4(0);
    _ctx->stop = _input->LT(-1);
    setState(355);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 36, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression3Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression3);
        setState(350);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(351);
        match(gscParser::T__50);
        setState(352);
        expression4(0); 
      }
      setState(357);
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
  size_t startState = 48;
  enterRecursionRule(_localctx, 48, gscParser::RuleExpression4, precedence);

    

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
    setState(359);
    expression5(0);
    _ctx->stop = _input->LT(-1);
    setState(366);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 37, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression4Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression4);
        setState(361);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(362);
        match(gscParser::T__51);
        setState(363);
        expression5(0); 
      }
      setState(368);
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
  size_t startState = 50;
  enterRecursionRule(_localctx, 50, gscParser::RuleExpression5, precedence);

    

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
    setState(370);
    expression6(0);
    _ctx->stop = _input->LT(-1);
    setState(377);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 38, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression5Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression5);
        setState(372);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(373);
        match(gscParser::T__14);
        setState(374);
        expression6(0); 
      }
      setState(379);
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
  size_t startState = 52;
  enterRecursionRule(_localctx, 52, gscParser::RuleExpression6, precedence);

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
    setState(381);
    expression7(0);
    _ctx->stop = _input->LT(-1);
    setState(388);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 39, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression6Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression6);
        setState(383);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(384);
        _la = _input->LA(1);
        if (!((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & 135107988821114880) != 0))) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(385);
        expression7(0); 
      }
      setState(390);
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
  size_t startState = 54;
  enterRecursionRule(_localctx, 54, gscParser::RuleExpression7, precedence);

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
    setState(392);
    expression8(0);
    _ctx->stop = _input->LT(-1);
    setState(399);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 40, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression7Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression7);
        setState(394);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(395);
        _la = _input->LA(1);
        if (!((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & 2161727821137838080) != 0))) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(396);
        expression8(0); 
      }
      setState(401);
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
  size_t startState = 56;
  enterRecursionRule(_localctx, 56, gscParser::RuleExpression8, precedence);

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
    setState(403);
    expression9(0);
    _ctx->stop = _input->LT(-1);
    setState(410);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 41, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression8Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression8);
        setState(405);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(406);
        _la = _input->LA(1);
        if (!(_la == gscParser::T__56

        || _la == gscParser::T__57)) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(407);
        expression9(0); 
      }
      setState(412);
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
  size_t startState = 58;
  enterRecursionRule(_localctx, 58, gscParser::RuleExpression9, precedence);

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
    setState(414);
    expression10(0);
    _ctx->stop = _input->LT(-1);
    setState(421);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 42, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression9Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression9);
        setState(416);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(417);
        _la = _input->LA(1);
        if (!(_la == gscParser::T__60

        || _la == gscParser::T__61)) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(418);
        expression10(0); 
      }
      setState(423);
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
  size_t startState = 60;
  enterRecursionRule(_localctx, 60, gscParser::RuleExpression10, precedence);

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
    setState(425);
    expression11(0);
    _ctx->stop = _input->LT(-1);
    setState(432);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 43, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression10Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression10);
        setState(427);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(428);
        _la = _input->LA(1);
        if (!(_la == gscParser::T__62

        || _la == gscParser::T__63)) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(429);
        expression11(0); 
      }
      setState(434);
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
  size_t startState = 62;
  enterRecursionRule(_localctx, 62, gscParser::RuleExpression11, precedence);

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
    setState(436);
    expression12();
    _ctx->stop = _input->LT(-1);
    setState(443);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 44, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression11Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression11);
        setState(438);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(439);
        _la = _input->LA(1);
        if (!(((((_la - 14) & ~ 0x3fULL) == 0) &&
          ((1ULL << (_la - 14)) & 6755399441055745) != 0))) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(440);
        expression12(); 
      }
      setState(445);
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
  enterRule(_localctx, 64, gscParser::RuleExpression12);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(454);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 45, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(446);
      _la = _input->LA(1);
      if (!(_la == gscParser::T__66

      || _la == gscParser::T__67)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(447);
      expression13();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(448);
      _la = _input->LA(1);
      if (!(_la == gscParser::T__68

      || _la == gscParser::T__69)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(449);
      left_value(0);
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(450);
      left_value(0);
      setState(451);
      _la = _input->LA(1);
      if (!(_la == gscParser::T__68

      || _la == gscParser::T__69)) {
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
      setState(453);
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

gscParser::Function_callContext* gscParser::Expression13Context::function_call() {
  return getRuleContext<gscParser::Function_callContext>(0);
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
  enterRule(_localctx, 66, gscParser::RuleExpression13);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(458);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 46, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(456);
      function_call(0);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(457);
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

gscParser::ExpressionContext* gscParser::Expression14Context::expression() {
  return getRuleContext<gscParser::ExpressionContext>(0);
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
  enterRule(_localctx, 68, gscParser::RuleExpression14);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(466);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 47, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(460);
      const_expr();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(461);
      match(gscParser::T__10);
      setState(462);
      expression();
      setState(463);
      match(gscParser::T__11);
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(465);
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

//----------------- Left_valueContext ------------------------------------------------------------------

gscParser::Left_valueContext::Left_valueContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* gscParser::Left_valueContext::IDENTIFIER() {
  return getToken(gscParser::IDENTIFIER, 0);
}

std::vector<gscParser::ExpressionContext *> gscParser::Left_valueContext::expression() {
  return getRuleContexts<gscParser::ExpressionContext>();
}

gscParser::ExpressionContext* gscParser::Left_valueContext::expression(size_t i) {
  return getRuleContext<gscParser::ExpressionContext>(i);
}

gscParser::Const_exprContext* gscParser::Left_valueContext::const_expr() {
  return getRuleContext<gscParser::Const_exprContext>(0);
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
  size_t startState = 70;
  enterRecursionRule(_localctx, 70, gscParser::RuleLeft_value, precedence);

    

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
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 51, _ctx)) {
    case 1: {
      setState(469);
      match(gscParser::IDENTIFIER);
      break;
    }

    case 2: {
      setState(470);
      match(gscParser::T__33);
      setState(471);
      expression();
      setState(472);
      match(gscParser::T__34);
      setState(473);
      match(gscParser::T__35);
      setState(474);
      match(gscParser::IDENTIFIER);
      break;
    }

    case 3: {
      setState(481);
      _errHandler->sync(this);
      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 48, _ctx)) {
      case 1: {
        setState(476);
        const_expr();
        break;
      }

      case 2: {
        setState(477);
        match(gscParser::T__10);
        setState(478);
        expression();
        setState(479);
        match(gscParser::T__11);
        break;
      }

      default:
        break;
      }
      setState(483);
      match(gscParser::T__70);
      setState(489);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case gscParser::IDENTIFIER: {
          setState(484);
          match(gscParser::IDENTIFIER);
          break;
        }

        case gscParser::T__10: {
          setState(485);
          match(gscParser::T__10);
          setState(486);
          expression();
          setState(487);
          match(gscParser::T__11);
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      break;
    }

    case 4: {
      setState(496);
      _errHandler->sync(this);
      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 50, _ctx)) {
      case 1: {
        setState(491);
        const_expr();
        break;
      }

      case 2: {
        setState(492);
        match(gscParser::T__10);
        setState(493);
        expression();
        setState(494);
        match(gscParser::T__11);
        break;
      }

      default:
        break;
      }
      setState(498);
      match(gscParser::T__8);
      setState(499);
      expression();
      setState(500);
      match(gscParser::T__9);
      break;
    }

    default:
      break;
    }
    _ctx->stop = _input->LT(-1);
    setState(520);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 54, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        setState(518);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 53, _ctx)) {
        case 1: {
          _localctx = _tracker.createInstance<Left_valueContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleLeft_value);
          setState(504);

          if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
          setState(505);
          match(gscParser::T__70);
          setState(511);
          _errHandler->sync(this);
          switch (_input->LA(1)) {
            case gscParser::IDENTIFIER: {
              setState(506);
              match(gscParser::IDENTIFIER);
              break;
            }

            case gscParser::T__10: {
              setState(507);
              match(gscParser::T__10);
              setState(508);
              expression();
              setState(509);
              match(gscParser::T__11);
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
          setState(513);

          if (!(precpred(_ctx, 1))) throw FailedPredicateException(this, "precpred(_ctx, 1)");
          setState(514);
          match(gscParser::T__8);
          setState(515);
          expression();
          setState(516);
          match(gscParser::T__9);
          break;
        }

        default:
          break;
        } 
      }
      setState(522);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 54, _ctx);
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
  enterRule(_localctx, 72, gscParser::RuleConst_expr);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(533);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case gscParser::T__10: {
        enterOuterAlt(_localctx, 1);
        setState(523);
        vector_value();
        break;
      }

      case gscParser::T__8: {
        enterOuterAlt(_localctx, 2);
        setState(524);
        array_def();
        break;
      }

      case gscParser::T__17: {
        enterOuterAlt(_localctx, 3);
        setState(525);
        struct_def();
        break;
      }

      case gscParser::INTEGER10:
      case gscParser::INTEGER16:
      case gscParser::INTEGER8:
      case gscParser::INTEGER2: {
        enterOuterAlt(_localctx, 4);
        setState(526);
        number();
        break;
      }

      case gscParser::BOOL_VALUE: {
        enterOuterAlt(_localctx, 5);
        setState(527);
        match(gscParser::BOOL_VALUE);
        break;
      }

      case gscParser::FLOATVAL: {
        enterOuterAlt(_localctx, 6);
        setState(528);
        match(gscParser::FLOATVAL);
        break;
      }

      case gscParser::STRING: {
        enterOuterAlt(_localctx, 7);
        setState(529);
        match(gscParser::STRING);
        break;
      }

      case gscParser::HASHSTRING: {
        enterOuterAlt(_localctx, 8);
        setState(530);
        match(gscParser::HASHSTRING);
        break;
      }

      case gscParser::UNDEFINED_VALUE: {
        enterOuterAlt(_localctx, 9);
        setState(531);
        match(gscParser::UNDEFINED_VALUE);
        break;
      }

      case gscParser::T__14:
      case gscParser::T__71: {
        enterOuterAlt(_localctx, 10);
        setState(532);
        function_ref();
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
  enterRule(_localctx, 74, gscParser::RuleFunction_ref);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(548);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case gscParser::T__14: {
        enterOuterAlt(_localctx, 1);
        setState(535);
        match(gscParser::T__14);
        setState(538);
        _errHandler->sync(this);

        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 56, _ctx)) {
        case 1: {
          setState(536);
          match(gscParser::IDENTIFIER);
          setState(537);
          match(gscParser::T__32);
          break;
        }

        default:
          break;
        }
        setState(540);
        match(gscParser::IDENTIFIER);
        break;
      }

      case gscParser::T__71: {
        enterOuterAlt(_localctx, 2);
        setState(541);
        match(gscParser::T__71);
        setState(542);
        match(gscParser::IDENTIFIER);
        setState(543);
        match(gscParser::T__56);
        setState(544);
        match(gscParser::PATH);
        setState(545);
        match(gscParser::T__58);
        setState(546);
        match(gscParser::T__32);
        setState(547);
        match(gscParser::IDENTIFIER);
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
  enterRule(_localctx, 76, gscParser::RuleNumber);
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
    setState(550);
    _la = _input->LA(1);
    if (!(((((_la - 76) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 76)) & 15) != 0))) {
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
  enterRule(_localctx, 78, gscParser::RuleVector_value);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(552);
    match(gscParser::T__10);
    setState(553);
    expression();
    setState(554);
    match(gscParser::T__12);
    setState(555);
    expression();
    setState(556);
    match(gscParser::T__12);
    setState(557);
    expression();
    setState(558);
    match(gscParser::T__11);
   
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

std::vector<tree::TerminalNode *> gscParser::Array_defContext::HASHSTRING() {
  return getTokens(gscParser::HASHSTRING);
}

tree::TerminalNode* gscParser::Array_defContext::HASHSTRING(size_t i) {
  return getToken(gscParser::HASHSTRING, i);
}

std::vector<gscParser::NumberContext *> gscParser::Array_defContext::number() {
  return getRuleContexts<gscParser::NumberContext>();
}

gscParser::NumberContext* gscParser::Array_defContext::number(size_t i) {
  return getRuleContext<gscParser::NumberContext>(i);
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
  enterRule(_localctx, 80, gscParser::RuleArray_def);
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
    setState(560);
    match(gscParser::T__8);
    setState(575);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 23622617600) != 0) || ((((_la - 67) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 67)) & 1998447) != 0)) {
      setState(567);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 59, _ctx)) {
      case 1: {
        setState(564);
        _errHandler->sync(this);
        switch (_input->LA(1)) {
          case gscParser::HASHSTRING: {
            setState(561);
            match(gscParser::HASHSTRING);
            break;
          }

          case gscParser::T__72: {
            setState(562);
            match(gscParser::T__72);
            setState(563);
            number();
            break;
          }

        default:
          throw NoViableAltException(this);
        }
        setState(566);
        match(gscParser::T__19);
        break;
      }

      default:
        break;
      }
      setState(569);
      expression();
      setState(571);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == gscParser::T__12) {
        setState(570);
        match(gscParser::T__12);
      }
      setState(577);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(578);
    match(gscParser::T__9);
   
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

std::vector<tree::TerminalNode *> gscParser::Struct_defContext::STRUCT_IDENTIFIER() {
  return getTokens(gscParser::STRUCT_IDENTIFIER);
}

tree::TerminalNode* gscParser::Struct_defContext::STRUCT_IDENTIFIER(size_t i) {
  return getToken(gscParser::STRUCT_IDENTIFIER, i);
}

std::vector<gscParser::ExpressionContext *> gscParser::Struct_defContext::expression() {
  return getRuleContexts<gscParser::ExpressionContext>();
}

gscParser::ExpressionContext* gscParser::Struct_defContext::expression(size_t i) {
  return getRuleContext<gscParser::ExpressionContext>(i);
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
  enterRule(_localctx, 82, gscParser::RuleStruct_def);
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
    setState(580);
    match(gscParser::T__17);
    setState(589);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == gscParser::STRUCT_IDENTIFIER) {
      setState(581);
      match(gscParser::STRUCT_IDENTIFIER);
      setState(582);
      match(gscParser::T__19);
      setState(583);
      expression();
      setState(585);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == gscParser::T__12) {
        setState(584);
        match(gscParser::T__12);
      }
      setState(591);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(592);
    match(gscParser::T__18);
   
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
    case 16: return function_callSempred(antlrcpp::downCast<Function_callContext *>(context), predicateIndex);
    case 21: return expression1Sempred(antlrcpp::downCast<Expression1Context *>(context), predicateIndex);
    case 22: return expression2Sempred(antlrcpp::downCast<Expression2Context *>(context), predicateIndex);
    case 23: return expression3Sempred(antlrcpp::downCast<Expression3Context *>(context), predicateIndex);
    case 24: return expression4Sempred(antlrcpp::downCast<Expression4Context *>(context), predicateIndex);
    case 25: return expression5Sempred(antlrcpp::downCast<Expression5Context *>(context), predicateIndex);
    case 26: return expression6Sempred(antlrcpp::downCast<Expression6Context *>(context), predicateIndex);
    case 27: return expression7Sempred(antlrcpp::downCast<Expression7Context *>(context), predicateIndex);
    case 28: return expression8Sempred(antlrcpp::downCast<Expression8Context *>(context), predicateIndex);
    case 29: return expression9Sempred(antlrcpp::downCast<Expression9Context *>(context), predicateIndex);
    case 30: return expression10Sempred(antlrcpp::downCast<Expression10Context *>(context), predicateIndex);
    case 31: return expression11Sempred(antlrcpp::downCast<Expression11Context *>(context), predicateIndex);
    case 35: return left_valueSempred(antlrcpp::downCast<Left_valueContext *>(context), predicateIndex);

  default:
    break;
  }
  return true;
}

bool gscParser::function_callSempred(Function_callContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 0: return precpred(_ctx, 1);

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

bool gscParser::left_valueSempred(Left_valueContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 12: return precpred(_ctx, 2);
    case 13: return precpred(_ctx, 1);

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
