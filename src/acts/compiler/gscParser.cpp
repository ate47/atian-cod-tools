
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
      "statement_switch", "statement_inst", "function_call", "nop_def", 
      "function_component", "operator_inst", "expression", "set_expression", 
      "expression1", "expression2", "expression3", "expression4", "expression5", 
      "expression6", "expression7", "expression8", "expression9", "expression10", 
      "expression11", "expression12", "expression13", "expression14", "left_value", 
      "array_left_value", "object_left_value", "const_expr", "function_ref", 
      "number", "vector_value", "array_def", "struct_def"
    },
    std::vector<std::string>{
      "", "'#include'", "'#using'", "';'", "'#namespace'", "'function'", 
      "'private'", "'autoexec'", "'('", "')'", "'event_handler'", "'['", 
      "']'", "','", "'*'", "'&'", "'='", "'...'", "'{'", "'}'", "':'", "'for'", 
      "'while'", "'do'", "'foreach'", "'in'", "'if'", "'else'", "'switch'", 
      "'case'", "'default'", "'thread'", "'childthread'", "'nop'", "'::'", 
      "'->'", "'\\u003F'", "'+='", "'-='", "'/='", "'*='", "'%='", "'&='", 
      "'|='", "'^='", "'<<='", "'>>='", "'~='", "'||'", "'&&'", "'|'", "'^'", 
      "'!='", "'=='", "'!=='", "'==='", "'<'", "'<='", "'>'", "'>='", "'<<'", 
      "'>>'", "'+'", "'-'", "'/'", "'%'", "'!'", "'~'", "'++'", "'--'", 
      "'.'", "'@'", "", "", "", "", "", "", "", "", "", "'undefined'"
    },
    std::vector<std::string>{
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
      "", "", "", "", "NEWLINE", "WHITESPACE", "INTEGER10", "INTEGER16", 
      "INTEGER8", "INTEGER2", "FLOATVAL", "BUILTIN", "BOOL_VALUE", "UNDEFINED_VALUE", 
      "IDENTIFIER", "STRUCT_IDENTIFIER", "PATH", "STRING", "HASHSTRING"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,86,631,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,7,
  	14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,7,
  	21,2,22,7,22,2,23,7,23,2,24,7,24,2,25,7,25,2,26,7,26,2,27,7,27,2,28,7,
  	28,2,29,7,29,2,30,7,30,2,31,7,31,2,32,7,32,2,33,7,33,2,34,7,34,2,35,7,
  	35,2,36,7,36,2,37,7,37,2,38,7,38,2,39,7,39,2,40,7,40,2,41,7,41,2,42,7,
  	42,2,43,7,43,2,44,7,44,1,0,1,0,1,0,5,0,94,8,0,10,0,12,0,97,9,0,1,0,1,
  	0,1,1,1,1,1,1,1,1,1,2,1,2,1,2,1,2,1,3,3,3,110,8,3,1,3,3,3,113,8,3,1,3,
  	1,3,1,3,1,3,1,3,3,3,120,8,3,3,3,122,8,3,1,3,1,3,1,3,1,3,3,3,128,8,3,1,
  	3,1,3,1,3,1,3,1,3,1,3,1,4,1,4,1,4,5,4,139,8,4,10,4,12,4,142,9,4,3,4,144,
  	8,4,1,5,3,5,147,8,5,1,5,1,5,1,5,3,5,152,8,5,1,5,3,5,155,8,5,1,6,1,6,1,
  	6,5,6,160,8,6,10,6,12,6,163,9,6,3,6,165,8,6,1,7,1,7,5,7,169,8,7,10,7,
  	12,7,172,9,7,1,7,1,7,1,8,1,8,3,8,178,8,8,1,8,1,8,1,8,1,8,1,8,1,8,1,8,
  	3,8,187,8,8,1,9,1,9,1,9,3,9,192,8,9,1,9,1,9,3,9,196,8,9,1,9,1,9,3,9,200,
  	8,9,1,9,1,9,1,9,1,10,1,10,1,10,1,10,1,10,1,10,1,11,1,11,1,11,1,11,1,11,
  	1,11,1,11,1,12,1,12,1,12,1,12,1,12,3,12,223,8,12,1,12,1,12,1,12,1,12,
  	1,12,1,13,1,13,1,13,1,13,1,13,1,13,1,13,3,13,237,8,13,1,14,1,14,1,14,
  	1,14,1,14,1,14,1,14,1,14,3,14,247,8,14,1,14,1,14,5,14,251,8,14,10,14,
  	12,14,254,9,14,4,14,256,8,14,11,14,12,14,257,1,14,1,14,1,15,1,15,1,15,
  	1,15,1,15,3,15,267,8,15,1,15,1,15,1,16,1,16,3,16,273,8,16,1,16,1,16,1,
  	16,1,16,1,16,1,16,1,16,3,16,282,8,16,1,16,1,16,1,16,1,16,1,16,3,16,289,
  	8,16,1,16,1,16,3,16,293,8,16,1,16,1,16,1,16,1,16,1,16,5,16,300,8,16,10,
  	16,12,16,303,9,16,1,17,1,17,1,17,1,17,1,17,3,17,310,8,17,1,18,1,18,3,
  	18,314,8,18,1,18,1,18,1,18,1,18,1,18,1,18,1,18,1,18,1,18,1,18,1,18,1,
  	18,1,18,1,18,1,18,3,18,331,8,18,1,19,1,19,1,19,3,19,336,8,19,1,20,1,20,
  	1,20,1,20,1,20,1,20,1,20,1,20,3,20,346,8,20,1,21,1,21,1,21,1,21,1,22,
  	1,22,1,22,1,22,1,22,1,22,5,22,358,8,22,10,22,12,22,361,9,22,1,23,1,23,
  	1,23,1,23,1,23,1,23,5,23,369,8,23,10,23,12,23,372,9,23,1,24,1,24,1,24,
  	1,24,1,24,1,24,5,24,380,8,24,10,24,12,24,383,9,24,1,25,1,25,1,25,1,25,
  	1,25,1,25,5,25,391,8,25,10,25,12,25,394,9,25,1,26,1,26,1,26,1,26,1,26,
  	1,26,5,26,402,8,26,10,26,12,26,405,9,26,1,27,1,27,1,27,1,27,1,27,1,27,
  	5,27,413,8,27,10,27,12,27,416,9,27,1,28,1,28,1,28,1,28,1,28,1,28,5,28,
  	424,8,28,10,28,12,28,427,9,28,1,29,1,29,1,29,1,29,1,29,1,29,5,29,435,
  	8,29,10,29,12,29,438,9,29,1,30,1,30,1,30,1,30,1,30,1,30,5,30,446,8,30,
  	10,30,12,30,449,9,30,1,31,1,31,1,31,1,31,1,31,1,31,5,31,457,8,31,10,31,
  	12,31,460,9,31,1,32,1,32,1,32,1,32,1,32,1,32,5,32,468,8,32,10,32,12,32,
  	471,9,32,1,33,1,33,1,33,1,33,1,33,1,33,1,33,1,33,3,33,481,8,33,1,34,1,
  	34,3,34,485,8,34,1,35,1,35,1,35,1,35,1,35,1,35,3,35,493,8,35,1,36,1,36,
  	1,36,1,36,3,36,499,8,36,1,36,1,36,1,36,1,36,1,36,1,36,1,36,3,36,508,8,
  	36,1,36,1,36,1,36,1,36,1,36,5,36,515,8,36,10,36,12,36,518,9,36,1,37,1,
  	37,1,37,1,37,1,37,3,37,525,8,37,1,37,1,37,1,37,1,37,1,38,1,38,1,38,1,
  	38,1,38,3,38,536,8,38,1,38,1,38,1,38,1,38,1,38,1,38,3,38,544,8,38,1,39,
  	1,39,1,39,1,39,1,39,1,39,1,39,1,39,1,39,1,39,3,39,556,8,39,1,40,1,40,
  	1,40,3,40,561,8,40,1,40,1,40,1,40,1,40,1,40,1,40,1,40,1,40,1,40,1,40,
  	3,40,573,8,40,1,41,1,41,1,42,1,42,1,42,1,42,1,42,1,42,1,42,1,42,1,43,
  	1,43,1,43,1,43,3,43,589,8,43,1,43,1,43,1,43,1,43,1,43,3,43,596,8,43,1,
  	43,5,43,599,8,43,10,43,12,43,602,9,43,1,43,3,43,605,8,43,3,43,607,8,43,
  	1,43,1,43,1,44,1,44,1,44,1,44,1,44,1,44,1,44,1,44,5,44,619,8,44,10,44,
  	12,44,622,9,44,1,44,3,44,625,8,44,3,44,627,8,44,1,44,1,44,1,44,0,13,32,
  	44,46,48,50,52,54,56,58,60,62,64,72,45,0,2,4,6,8,10,12,14,16,18,20,22,
  	24,26,28,30,32,34,36,38,40,42,44,46,48,50,52,54,56,58,60,62,64,66,68,
  	70,72,74,76,78,80,82,84,86,88,0,14,1,0,1,2,2,0,82,82,84,84,1,0,14,15,
  	1,0,31,32,2,0,16,16,37,47,1,0,52,55,1,0,56,59,1,0,56,57,1,0,60,61,1,0,
  	62,63,2,0,14,14,64,65,1,0,66,67,1,0,68,69,1,0,74,77,678,0,95,1,0,0,0,
  	2,100,1,0,0,0,4,104,1,0,0,0,6,109,1,0,0,0,8,143,1,0,0,0,10,154,1,0,0,
  	0,12,164,1,0,0,0,14,166,1,0,0,0,16,177,1,0,0,0,18,188,1,0,0,0,20,204,
  	1,0,0,0,22,210,1,0,0,0,24,217,1,0,0,0,26,229,1,0,0,0,28,238,1,0,0,0,30,
  	266,1,0,0,0,32,288,1,0,0,0,34,304,1,0,0,0,36,330,1,0,0,0,38,332,1,0,0,
  	0,40,345,1,0,0,0,42,347,1,0,0,0,44,351,1,0,0,0,46,362,1,0,0,0,48,373,
  	1,0,0,0,50,384,1,0,0,0,52,395,1,0,0,0,54,406,1,0,0,0,56,417,1,0,0,0,58,
  	428,1,0,0,0,60,439,1,0,0,0,62,450,1,0,0,0,64,461,1,0,0,0,66,480,1,0,0,
  	0,68,484,1,0,0,0,70,492,1,0,0,0,72,498,1,0,0,0,74,524,1,0,0,0,76,535,
  	1,0,0,0,78,555,1,0,0,0,80,572,1,0,0,0,82,574,1,0,0,0,84,576,1,0,0,0,86,
  	584,1,0,0,0,88,610,1,0,0,0,90,94,3,6,3,0,91,94,3,2,1,0,92,94,3,4,2,0,
  	93,90,1,0,0,0,93,91,1,0,0,0,93,92,1,0,0,0,94,97,1,0,0,0,95,93,1,0,0,0,
  	95,96,1,0,0,0,96,98,1,0,0,0,97,95,1,0,0,0,98,99,5,0,0,1,99,1,1,0,0,0,
  	100,101,7,0,0,0,101,102,7,1,0,0,102,103,5,3,0,0,103,3,1,0,0,0,104,105,
  	5,4,0,0,105,106,5,82,0,0,106,107,5,3,0,0,107,5,1,0,0,0,108,110,5,5,0,
  	0,109,108,1,0,0,0,109,110,1,0,0,0,110,112,1,0,0,0,111,113,5,6,0,0,112,
  	111,1,0,0,0,112,113,1,0,0,0,113,121,1,0,0,0,114,119,5,7,0,0,115,116,5,
  	8,0,0,116,117,3,82,41,0,117,118,5,9,0,0,118,120,1,0,0,0,119,115,1,0,0,
  	0,119,120,1,0,0,0,120,122,1,0,0,0,121,114,1,0,0,0,121,122,1,0,0,0,122,
  	127,1,0,0,0,123,124,5,10,0,0,124,125,5,11,0,0,125,126,5,82,0,0,126,128,
  	5,12,0,0,127,123,1,0,0,0,127,128,1,0,0,0,128,129,1,0,0,0,129,130,5,82,
  	0,0,130,131,5,8,0,0,131,132,3,8,4,0,132,133,5,9,0,0,133,134,3,14,7,0,
  	134,7,1,0,0,0,135,140,3,10,5,0,136,137,5,13,0,0,137,139,3,10,5,0,138,
  	136,1,0,0,0,139,142,1,0,0,0,140,138,1,0,0,0,140,141,1,0,0,0,141,144,1,
  	0,0,0,142,140,1,0,0,0,143,135,1,0,0,0,143,144,1,0,0,0,144,9,1,0,0,0,145,
  	147,7,2,0,0,146,145,1,0,0,0,146,147,1,0,0,0,147,148,1,0,0,0,148,151,5,
  	82,0,0,149,150,5,16,0,0,150,152,3,40,20,0,151,149,1,0,0,0,151,152,1,0,
  	0,0,152,155,1,0,0,0,153,155,5,17,0,0,154,146,1,0,0,0,154,153,1,0,0,0,
  	155,11,1,0,0,0,156,161,3,40,20,0,157,158,5,13,0,0,158,160,3,40,20,0,159,
  	157,1,0,0,0,160,163,1,0,0,0,161,159,1,0,0,0,161,162,1,0,0,0,162,165,1,
  	0,0,0,163,161,1,0,0,0,164,156,1,0,0,0,164,165,1,0,0,0,165,13,1,0,0,0,
  	166,170,5,18,0,0,167,169,3,16,8,0,168,167,1,0,0,0,169,172,1,0,0,0,170,
  	168,1,0,0,0,170,171,1,0,0,0,171,173,1,0,0,0,172,170,1,0,0,0,173,174,5,
  	19,0,0,174,15,1,0,0,0,175,176,5,82,0,0,176,178,5,20,0,0,177,175,1,0,0,
  	0,177,178,1,0,0,0,178,186,1,0,0,0,179,187,3,14,7,0,180,187,3,18,9,0,181,
  	187,3,26,13,0,182,187,3,20,10,0,183,187,3,24,12,0,184,187,3,30,15,0,185,
  	187,3,28,14,0,186,179,1,0,0,0,186,180,1,0,0,0,186,181,1,0,0,0,186,182,
  	1,0,0,0,186,183,1,0,0,0,186,184,1,0,0,0,186,185,1,0,0,0,187,17,1,0,0,
  	0,188,189,5,21,0,0,189,191,5,8,0,0,190,192,3,40,20,0,191,190,1,0,0,0,
  	191,192,1,0,0,0,192,193,1,0,0,0,193,195,5,3,0,0,194,196,3,40,20,0,195,
  	194,1,0,0,0,195,196,1,0,0,0,196,197,1,0,0,0,197,199,5,3,0,0,198,200,3,
  	40,20,0,199,198,1,0,0,0,199,200,1,0,0,0,200,201,1,0,0,0,201,202,5,9,0,
  	0,202,203,3,16,8,0,203,19,1,0,0,0,204,205,5,22,0,0,205,206,5,8,0,0,206,
  	207,3,40,20,0,207,208,5,9,0,0,208,209,3,16,8,0,209,21,1,0,0,0,210,211,
  	5,23,0,0,211,212,3,16,8,0,212,213,5,22,0,0,213,214,5,8,0,0,214,215,3,
  	40,20,0,215,216,5,9,0,0,216,23,1,0,0,0,217,218,5,24,0,0,218,219,5,8,0,
  	0,219,222,5,82,0,0,220,221,5,13,0,0,221,223,5,82,0,0,222,220,1,0,0,0,
  	222,223,1,0,0,0,223,224,1,0,0,0,224,225,5,25,0,0,225,226,3,40,20,0,226,
  	227,5,9,0,0,227,228,3,16,8,0,228,25,1,0,0,0,229,230,5,26,0,0,230,231,
  	5,8,0,0,231,232,3,40,20,0,232,233,5,9,0,0,233,236,3,16,8,0,234,235,5,
  	27,0,0,235,237,3,16,8,0,236,234,1,0,0,0,236,237,1,0,0,0,237,27,1,0,0,
  	0,238,239,5,28,0,0,239,240,5,8,0,0,240,241,3,40,20,0,241,242,5,9,0,0,
  	242,255,5,18,0,0,243,244,5,29,0,0,244,247,3,78,39,0,245,247,5,30,0,0,
  	246,243,1,0,0,0,246,245,1,0,0,0,247,248,1,0,0,0,248,252,5,20,0,0,249,
  	251,3,16,8,0,250,249,1,0,0,0,251,254,1,0,0,0,252,250,1,0,0,0,252,253,
  	1,0,0,0,253,256,1,0,0,0,254,252,1,0,0,0,255,246,1,0,0,0,256,257,1,0,0,
  	0,257,255,1,0,0,0,257,258,1,0,0,0,258,259,1,0,0,0,259,260,5,19,0,0,260,
  	29,1,0,0,0,261,267,3,40,20,0,262,267,3,38,19,0,263,267,3,22,11,0,264,
  	267,3,32,16,0,265,267,3,34,17,0,266,261,1,0,0,0,266,262,1,0,0,0,266,263,
  	1,0,0,0,266,264,1,0,0,0,266,265,1,0,0,0,267,268,1,0,0,0,268,269,5,3,0,
  	0,269,31,1,0,0,0,270,272,6,16,-1,0,271,273,7,3,0,0,272,271,1,0,0,0,272,
  	273,1,0,0,0,273,274,1,0,0,0,274,275,3,36,18,0,275,276,5,8,0,0,276,277,
  	3,12,6,0,277,278,5,9,0,0,278,289,1,0,0,0,279,281,3,70,35,0,280,282,7,
  	3,0,0,281,280,1,0,0,0,281,282,1,0,0,0,282,283,1,0,0,0,283,284,3,36,18,
  	0,284,285,5,8,0,0,285,286,3,12,6,0,286,287,5,9,0,0,287,289,1,0,0,0,288,
  	270,1,0,0,0,288,279,1,0,0,0,289,301,1,0,0,0,290,292,10,1,0,0,291,293,
  	7,3,0,0,292,291,1,0,0,0,292,293,1,0,0,0,293,294,1,0,0,0,294,295,3,36,
  	18,0,295,296,5,8,0,0,296,297,3,12,6,0,297,298,5,9,0,0,298,300,1,0,0,0,
  	299,290,1,0,0,0,300,303,1,0,0,0,301,299,1,0,0,0,301,302,1,0,0,0,302,33,
  	1,0,0,0,303,301,1,0,0,0,304,309,5,33,0,0,305,306,5,8,0,0,306,307,3,82,
  	41,0,307,308,5,9,0,0,308,310,1,0,0,0,309,305,1,0,0,0,309,310,1,0,0,0,
  	310,35,1,0,0,0,311,312,5,82,0,0,312,314,5,34,0,0,313,311,1,0,0,0,313,
  	314,1,0,0,0,314,315,1,0,0,0,315,331,5,82,0,0,316,317,5,11,0,0,317,318,
  	5,11,0,0,318,319,3,40,20,0,319,320,5,12,0,0,320,321,5,12,0,0,321,331,
  	1,0,0,0,322,323,5,11,0,0,323,324,5,11,0,0,324,325,3,40,20,0,325,326,5,
  	12,0,0,326,327,5,12,0,0,327,328,5,35,0,0,328,329,5,82,0,0,329,331,1,0,
  	0,0,330,313,1,0,0,0,330,316,1,0,0,0,330,322,1,0,0,0,331,37,1,0,0,0,332,
  	335,5,79,0,0,333,336,5,82,0,0,334,336,3,40,20,0,335,333,1,0,0,0,335,334,
  	1,0,0,0,335,336,1,0,0,0,336,39,1,0,0,0,337,346,3,42,21,0,338,339,3,44,
  	22,0,339,340,5,36,0,0,340,341,3,40,20,0,341,342,5,20,0,0,342,343,3,40,
  	20,0,343,346,1,0,0,0,344,346,3,44,22,0,345,337,1,0,0,0,345,338,1,0,0,
  	0,345,344,1,0,0,0,346,41,1,0,0,0,347,348,3,72,36,0,348,349,7,4,0,0,349,
  	350,3,40,20,0,350,43,1,0,0,0,351,352,6,22,-1,0,352,353,3,46,23,0,353,
  	359,1,0,0,0,354,355,10,2,0,0,355,356,5,48,0,0,356,358,3,46,23,0,357,354,
  	1,0,0,0,358,361,1,0,0,0,359,357,1,0,0,0,359,360,1,0,0,0,360,45,1,0,0,
  	0,361,359,1,0,0,0,362,363,6,23,-1,0,363,364,3,48,24,0,364,370,1,0,0,0,
  	365,366,10,2,0,0,366,367,5,49,0,0,367,369,3,48,24,0,368,365,1,0,0,0,369,
  	372,1,0,0,0,370,368,1,0,0,0,370,371,1,0,0,0,371,47,1,0,0,0,372,370,1,
  	0,0,0,373,374,6,24,-1,0,374,375,3,50,25,0,375,381,1,0,0,0,376,377,10,
  	2,0,0,377,378,5,50,0,0,378,380,3,50,25,0,379,376,1,0,0,0,380,383,1,0,
  	0,0,381,379,1,0,0,0,381,382,1,0,0,0,382,49,1,0,0,0,383,381,1,0,0,0,384,
  	385,6,25,-1,0,385,386,3,52,26,0,386,392,1,0,0,0,387,388,10,2,0,0,388,
  	389,5,51,0,0,389,391,3,52,26,0,390,387,1,0,0,0,391,394,1,0,0,0,392,390,
  	1,0,0,0,392,393,1,0,0,0,393,51,1,0,0,0,394,392,1,0,0,0,395,396,6,26,-1,
  	0,396,397,3,54,27,0,397,403,1,0,0,0,398,399,10,2,0,0,399,400,5,15,0,0,
  	400,402,3,54,27,0,401,398,1,0,0,0,402,405,1,0,0,0,403,401,1,0,0,0,403,
  	404,1,0,0,0,404,53,1,0,0,0,405,403,1,0,0,0,406,407,6,27,-1,0,407,408,
  	3,56,28,0,408,414,1,0,0,0,409,410,10,2,0,0,410,411,7,5,0,0,411,413,3,
  	56,28,0,412,409,1,0,0,0,413,416,1,0,0,0,414,412,1,0,0,0,414,415,1,0,0,
  	0,415,55,1,0,0,0,416,414,1,0,0,0,417,418,6,28,-1,0,418,419,3,58,29,0,
  	419,425,1,0,0,0,420,421,10,2,0,0,421,422,7,6,0,0,422,424,3,58,29,0,423,
  	420,1,0,0,0,424,427,1,0,0,0,425,423,1,0,0,0,425,426,1,0,0,0,426,57,1,
  	0,0,0,427,425,1,0,0,0,428,429,6,29,-1,0,429,430,3,60,30,0,430,436,1,0,
  	0,0,431,432,10,2,0,0,432,433,7,7,0,0,433,435,3,60,30,0,434,431,1,0,0,
  	0,435,438,1,0,0,0,436,434,1,0,0,0,436,437,1,0,0,0,437,59,1,0,0,0,438,
  	436,1,0,0,0,439,440,6,30,-1,0,440,441,3,62,31,0,441,447,1,0,0,0,442,443,
  	10,2,0,0,443,444,7,8,0,0,444,446,3,62,31,0,445,442,1,0,0,0,446,449,1,
  	0,0,0,447,445,1,0,0,0,447,448,1,0,0,0,448,61,1,0,0,0,449,447,1,0,0,0,
  	450,451,6,31,-1,0,451,452,3,64,32,0,452,458,1,0,0,0,453,454,10,2,0,0,
  	454,455,7,9,0,0,455,457,3,64,32,0,456,453,1,0,0,0,457,460,1,0,0,0,458,
  	456,1,0,0,0,458,459,1,0,0,0,459,63,1,0,0,0,460,458,1,0,0,0,461,462,6,
  	32,-1,0,462,463,3,66,33,0,463,469,1,0,0,0,464,465,10,2,0,0,465,466,7,
  	10,0,0,466,468,3,66,33,0,467,464,1,0,0,0,468,471,1,0,0,0,469,467,1,0,
  	0,0,469,470,1,0,0,0,470,65,1,0,0,0,471,469,1,0,0,0,472,473,7,11,0,0,473,
  	481,3,68,34,0,474,475,7,12,0,0,475,481,3,72,36,0,476,477,3,72,36,0,477,
  	478,7,12,0,0,478,481,1,0,0,0,479,481,3,68,34,0,480,472,1,0,0,0,480,474,
  	1,0,0,0,480,476,1,0,0,0,480,479,1,0,0,0,481,67,1,0,0,0,482,485,3,32,16,
  	0,483,485,3,70,35,0,484,482,1,0,0,0,484,483,1,0,0,0,485,69,1,0,0,0,486,
  	493,3,78,39,0,487,488,5,8,0,0,488,489,3,40,20,0,489,490,5,9,0,0,490,493,
  	1,0,0,0,491,493,3,72,36,0,492,486,1,0,0,0,492,487,1,0,0,0,492,491,1,0,
  	0,0,493,71,1,0,0,0,494,495,6,36,-1,0,495,499,5,82,0,0,496,499,3,76,38,
  	0,497,499,3,74,37,0,498,494,1,0,0,0,498,496,1,0,0,0,498,497,1,0,0,0,499,
  	516,1,0,0,0,500,501,10,2,0,0,501,507,5,70,0,0,502,508,5,82,0,0,503,504,
  	5,8,0,0,504,505,3,40,20,0,505,506,5,9,0,0,506,508,1,0,0,0,507,502,1,0,
  	0,0,507,503,1,0,0,0,508,515,1,0,0,0,509,510,10,1,0,0,510,511,5,11,0,0,
  	511,512,3,40,20,0,512,513,5,12,0,0,513,515,1,0,0,0,514,500,1,0,0,0,514,
  	509,1,0,0,0,515,518,1,0,0,0,516,514,1,0,0,0,516,517,1,0,0,0,517,73,1,
  	0,0,0,518,516,1,0,0,0,519,525,3,78,39,0,520,521,5,8,0,0,521,522,3,40,
  	20,0,522,523,5,9,0,0,523,525,1,0,0,0,524,519,1,0,0,0,524,520,1,0,0,0,
  	525,526,1,0,0,0,526,527,5,11,0,0,527,528,3,40,20,0,528,529,5,12,0,0,529,
  	75,1,0,0,0,530,536,3,78,39,0,531,532,5,8,0,0,532,533,3,40,20,0,533,534,
  	5,9,0,0,534,536,1,0,0,0,535,530,1,0,0,0,535,531,1,0,0,0,536,537,1,0,0,
  	0,537,543,5,70,0,0,538,544,5,82,0,0,539,540,5,8,0,0,540,541,3,40,20,0,
  	541,542,5,9,0,0,542,544,1,0,0,0,543,538,1,0,0,0,543,539,1,0,0,0,544,77,
  	1,0,0,0,545,556,3,84,42,0,546,556,3,86,43,0,547,556,3,88,44,0,548,556,
  	3,82,41,0,549,556,5,80,0,0,550,556,5,78,0,0,551,556,5,85,0,0,552,556,
  	5,86,0,0,553,556,5,81,0,0,554,556,3,80,40,0,555,545,1,0,0,0,555,546,1,
  	0,0,0,555,547,1,0,0,0,555,548,1,0,0,0,555,549,1,0,0,0,555,550,1,0,0,0,
  	555,551,1,0,0,0,555,552,1,0,0,0,555,553,1,0,0,0,555,554,1,0,0,0,556,79,
  	1,0,0,0,557,560,5,15,0,0,558,559,5,82,0,0,559,561,5,34,0,0,560,558,1,
  	0,0,0,560,561,1,0,0,0,561,562,1,0,0,0,562,573,5,82,0,0,563,564,5,71,0,
  	0,564,565,5,82,0,0,565,566,5,56,0,0,566,567,5,84,0,0,567,568,5,58,0,0,
  	568,569,5,34,0,0,569,573,5,82,0,0,570,571,5,15,0,0,571,573,3,72,36,0,
  	572,557,1,0,0,0,572,563,1,0,0,0,572,570,1,0,0,0,573,81,1,0,0,0,574,575,
  	7,13,0,0,575,83,1,0,0,0,576,577,5,8,0,0,577,578,3,40,20,0,578,579,5,13,
  	0,0,579,580,3,40,20,0,580,581,5,13,0,0,581,582,3,40,20,0,582,583,5,9,
  	0,0,583,85,1,0,0,0,584,606,5,11,0,0,585,586,3,40,20,0,586,587,5,20,0,
  	0,587,589,1,0,0,0,588,585,1,0,0,0,588,589,1,0,0,0,589,590,1,0,0,0,590,
  	600,3,40,20,0,591,595,5,13,0,0,592,593,3,40,20,0,593,594,5,20,0,0,594,
  	596,1,0,0,0,595,592,1,0,0,0,595,596,1,0,0,0,596,597,1,0,0,0,597,599,3,
  	40,20,0,598,591,1,0,0,0,599,602,1,0,0,0,600,598,1,0,0,0,600,601,1,0,0,
  	0,601,604,1,0,0,0,602,600,1,0,0,0,603,605,5,13,0,0,604,603,1,0,0,0,604,
  	605,1,0,0,0,605,607,1,0,0,0,606,588,1,0,0,0,606,607,1,0,0,0,607,608,1,
  	0,0,0,608,609,5,12,0,0,609,87,1,0,0,0,610,626,5,18,0,0,611,612,5,83,0,
  	0,612,613,5,20,0,0,613,620,3,40,20,0,614,615,5,13,0,0,615,616,5,83,0,
  	0,616,617,5,20,0,0,617,619,3,40,20,0,618,614,1,0,0,0,619,622,1,0,0,0,
  	620,618,1,0,0,0,620,621,1,0,0,0,621,624,1,0,0,0,622,620,1,0,0,0,623,625,
  	5,13,0,0,624,623,1,0,0,0,624,625,1,0,0,0,625,627,1,0,0,0,626,611,1,0,
  	0,0,626,627,1,0,0,0,627,628,1,0,0,0,628,629,5,19,0,0,629,89,1,0,0,0,68,
  	93,95,109,112,119,121,127,140,143,146,151,154,161,164,170,177,186,191,
  	195,199,222,236,246,252,257,266,272,281,288,292,301,309,313,330,335,345,
  	359,370,381,392,403,414,425,436,447,458,469,480,484,492,498,507,514,516,
  	524,535,543,555,560,572,588,595,600,604,606,620,624,626
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
    setState(95);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 1270) != 0) || _la == gscParser::IDENTIFIER) {
      setState(93);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case gscParser::T__4:
        case gscParser::T__5:
        case gscParser::T__6:
        case gscParser::T__9:
        case gscParser::IDENTIFIER: {
          setState(90);
          function();
          break;
        }

        case gscParser::T__0:
        case gscParser::T__1: {
          setState(91);
          include();
          break;
        }

        case gscParser::T__3: {
          setState(92);
          namespace_();
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(97);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(98);
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
    setState(100);
    _la = _input->LA(1);
    if (!(_la == gscParser::T__0

    || _la == gscParser::T__1)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(101);
    _la = _input->LA(1);
    if (!(_la == gscParser::IDENTIFIER

    || _la == gscParser::PATH)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(102);
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
    setState(104);
    match(gscParser::T__3);
    setState(105);
    match(gscParser::IDENTIFIER);
    setState(106);
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
    setState(109);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__4) {
      setState(108);
      match(gscParser::T__4);
    }
    setState(112);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__5) {
      setState(111);
      match(gscParser::T__5);
    }
    setState(121);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__6) {
      setState(114);
      match(gscParser::T__6);
      setState(119);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == gscParser::T__7) {
        setState(115);
        match(gscParser::T__7);
        setState(116);
        number();
        setState(117);
        match(gscParser::T__8);
      }
    }
    setState(127);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__9) {
      setState(123);
      match(gscParser::T__9);
      setState(124);
      match(gscParser::T__10);
      setState(125);
      match(gscParser::IDENTIFIER);
      setState(126);
      match(gscParser::T__11);
    }
    setState(129);
    match(gscParser::IDENTIFIER);
    setState(130);
    match(gscParser::T__7);
    setState(131);
    param_list();
    setState(132);
    match(gscParser::T__8);
    setState(133);
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
    setState(143);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 180224) != 0) || _la == gscParser::IDENTIFIER) {
      setState(135);
      param_val();
      setState(140);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == gscParser::T__12) {
        setState(136);
        match(gscParser::T__12);
        setState(137);
        param_val();
        setState(142);
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
    setState(154);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case gscParser::T__13:
      case gscParser::T__14:
      case gscParser::IDENTIFIER: {
        enterOuterAlt(_localctx, 1);
        setState(146);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == gscParser::T__13

        || _la == gscParser::T__14) {
          setState(145);
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
        setState(148);
        match(gscParser::IDENTIFIER);
        setState(151);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == gscParser::T__15) {
          setState(149);
          match(gscParser::T__15);
          setState(150);
          expression();
        }
        break;
      }

      case gscParser::T__16: {
        enterOuterAlt(_localctx, 2);
        setState(153);
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
    setState(164);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 6442748160) != 0) || ((((_la - 66) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 66)) & 1695535) != 0)) {
      setState(156);
      expression();
      setState(161);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == gscParser::T__12) {
        setState(157);
        match(gscParser::T__12);
        setState(158);
        expression();
        setState(163);
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
    setState(166);
    match(gscParser::T__17);
    setState(170);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 15399684352) != 0) || ((((_la - 66) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 66)) & 1703727) != 0)) {
      setState(167);
      statement();
      setState(172);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(173);
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
    setState(177);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 15, _ctx)) {
    case 1: {
      setState(175);
      match(gscParser::IDENTIFIER);
      setState(176);
      match(gscParser::T__19);
      break;
    }

    default:
      break;
    }
    setState(186);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 16, _ctx)) {
    case 1: {
      setState(179);
      statement_block();
      break;
    }

    case 2: {
      setState(180);
      statement_for();
      break;
    }

    case 3: {
      setState(181);
      statement_if();
      break;
    }

    case 4: {
      setState(182);
      statement_while();
      break;
    }

    case 5: {
      setState(183);
      statement_foreach();
      break;
    }

    case 6: {
      setState(184);
      statement_inst();
      break;
    }

    case 7: {
      setState(185);
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
    setState(188);
    match(gscParser::T__20);
    setState(189);
    match(gscParser::T__7);
    setState(191);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 6442748160) != 0) || ((((_la - 66) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 66)) & 1695535) != 0)) {
      setState(190);
      expression();
    }
    setState(193);
    match(gscParser::T__2);
    setState(195);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 6442748160) != 0) || ((((_la - 66) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 66)) & 1695535) != 0)) {
      setState(194);
      expression();
    }
    setState(197);
    match(gscParser::T__2);
    setState(199);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 6442748160) != 0) || ((((_la - 66) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 66)) & 1695535) != 0)) {
      setState(198);
      expression();
    }
    setState(201);
    match(gscParser::T__8);
    setState(202);
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
    setState(204);
    match(gscParser::T__21);
    setState(205);
    match(gscParser::T__7);
    setState(206);
    expression();
    setState(207);
    match(gscParser::T__8);
    setState(208);
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
    setState(210);
    match(gscParser::T__22);
    setState(211);
    statement();
    setState(212);
    match(gscParser::T__21);
    setState(213);
    match(gscParser::T__7);
    setState(214);
    expression();
    setState(215);
    match(gscParser::T__8);
   
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
    setState(217);
    match(gscParser::T__23);
    setState(218);
    match(gscParser::T__7);
    setState(219);
    match(gscParser::IDENTIFIER);
    setState(222);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__12) {
      setState(220);
      match(gscParser::T__12);
      setState(221);
      match(gscParser::IDENTIFIER);
    }
    setState(224);
    match(gscParser::T__24);
    setState(225);
    expression();
    setState(226);
    match(gscParser::T__8);
    setState(227);
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
    setState(229);
    match(gscParser::T__25);
    setState(230);
    match(gscParser::T__7);
    setState(231);
    expression();
    setState(232);
    match(gscParser::T__8);
    setState(233);
    statement();
    setState(236);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 21, _ctx)) {
    case 1: {
      setState(234);
      match(gscParser::T__26);
      setState(235);
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
    setState(238);
    match(gscParser::T__27);
    setState(239);
    match(gscParser::T__7);
    setState(240);
    expression();
    setState(241);
    match(gscParser::T__8);
    setState(242);
    match(gscParser::T__17);
    setState(255); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(246);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case gscParser::T__28: {
          setState(243);
          match(gscParser::T__28);
          setState(244);
          const_expr();
          break;
        }

        case gscParser::T__29: {
          setState(245);
          match(gscParser::T__29);
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(248);
      match(gscParser::T__19);
      setState(252);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while ((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & 15399684352) != 0) || ((((_la - 66) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 66)) & 1703727) != 0)) {
        setState(249);
        statement();
        setState(254);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
      setState(257); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while (_la == gscParser::T__28

    || _la == gscParser::T__29);
    setState(259);
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

gscParser::ExpressionContext* gscParser::Statement_instContext::expression() {
  return getRuleContext<gscParser::ExpressionContext>(0);
}

gscParser::Operator_instContext* gscParser::Statement_instContext::operator_inst() {
  return getRuleContext<gscParser::Operator_instContext>(0);
}

gscParser::Statement_dowhileContext* gscParser::Statement_instContext::statement_dowhile() {
  return getRuleContext<gscParser::Statement_dowhileContext>(0);
}

gscParser::Function_callContext* gscParser::Statement_instContext::function_call() {
  return getRuleContext<gscParser::Function_callContext>(0);
}

gscParser::Nop_defContext* gscParser::Statement_instContext::nop_def() {
  return getRuleContext<gscParser::Nop_defContext>(0);
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
    setState(266);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 25, _ctx)) {
    case 1: {
      setState(261);
      expression();
      break;
    }

    case 2: {
      setState(262);
      operator_inst();
      break;
    }

    case 3: {
      setState(263);
      statement_dowhile();
      break;
    }

    case 4: {
      setState(264);
      function_call(0);
      break;
    }

    case 5: {
      setState(265);
      nop_def();
      break;
    }

    default:
      break;
    }
    setState(268);
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
    setState(288);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 28, _ctx)) {
    case 1: {
      setState(272);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == gscParser::T__30

      || _la == gscParser::T__31) {
        setState(271);
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
      setState(274);
      function_component();
      setState(275);
      match(gscParser::T__7);
      setState(276);
      expression_list();
      setState(277);
      match(gscParser::T__8);
      break;
    }

    case 2: {
      setState(279);
      expression14();
      setState(281);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == gscParser::T__30

      || _la == gscParser::T__31) {
        setState(280);
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
      setState(283);
      function_component();
      setState(284);
      match(gscParser::T__7);
      setState(285);
      expression_list();
      setState(286);
      match(gscParser::T__8);
      break;
    }

    default:
      break;
    }
    _ctx->stop = _input->LT(-1);
    setState(301);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 30, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Function_callContext>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleFunction_call);
        setState(290);

        if (!(precpred(_ctx, 1))) throw FailedPredicateException(this, "precpred(_ctx, 1)");
        setState(292);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == gscParser::T__30

        || _la == gscParser::T__31) {
          setState(291);
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
        setState(294);
        function_component();
        setState(295);
        match(gscParser::T__7);
        setState(296);
        expression_list();
        setState(297);
        match(gscParser::T__8); 
      }
      setState(303);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 30, _ctx);
    }
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
  enterRule(_localctx, 34, gscParser::RuleNop_def);
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
    setState(304);
    match(gscParser::T__32);
    setState(309);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__7) {
      setState(305);
      match(gscParser::T__7);
      setState(306);
      number();
      setState(307);
      match(gscParser::T__8);
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
  enterRule(_localctx, 36, gscParser::RuleFunction_component);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(330);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 33, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(313);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 32, _ctx)) {
      case 1: {
        setState(311);
        match(gscParser::IDENTIFIER);
        setState(312);
        match(gscParser::T__33);
        break;
      }

      default:
        break;
      }
      setState(315);
      match(gscParser::IDENTIFIER);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(316);
      match(gscParser::T__10);
      setState(317);
      match(gscParser::T__10);
      setState(318);
      expression();
      setState(319);
      match(gscParser::T__11);
      setState(320);
      match(gscParser::T__11);
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(322);
      match(gscParser::T__10);
      setState(323);
      match(gscParser::T__10);
      setState(324);
      expression();
      setState(325);
      match(gscParser::T__11);
      setState(326);
      match(gscParser::T__11);
      setState(327);
      match(gscParser::T__34);
      setState(328);
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
  enterRule(_localctx, 38, gscParser::RuleOperator_inst);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(332);
    match(gscParser::BUILTIN);
    setState(335);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 34, _ctx)) {
    case 1: {
      setState(333);
      match(gscParser::IDENTIFIER);
      break;
    }

    case 2: {
      setState(334);
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
  enterRule(_localctx, 40, gscParser::RuleExpression);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(345);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 35, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(337);
      set_expression();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(338);
      expression1(0);
      setState(339);
      match(gscParser::T__35);
      setState(340);
      expression();
      setState(341);
      match(gscParser::T__19);
      setState(342);
      expression();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(344);
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
  enterRule(_localctx, 42, gscParser::RuleSet_expression);
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
    setState(347);
    left_value(0);
    setState(348);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 281337537822720) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(349);
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
  size_t startState = 44;
  enterRecursionRule(_localctx, 44, gscParser::RuleExpression1, precedence);

    

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
    setState(352);
    expression2(0);
    _ctx->stop = _input->LT(-1);
    setState(359);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 36, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression1Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression1);
        setState(354);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(355);
        match(gscParser::T__47);
        setState(356);
        expression2(0); 
      }
      setState(361);
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
  size_t startState = 46;
  enterRecursionRule(_localctx, 46, gscParser::RuleExpression2, precedence);

    

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
    setState(363);
    expression3(0);
    _ctx->stop = _input->LT(-1);
    setState(370);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 37, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression2Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression2);
        setState(365);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(366);
        match(gscParser::T__48);
        setState(367);
        expression3(0); 
      }
      setState(372);
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
  size_t startState = 48;
  enterRecursionRule(_localctx, 48, gscParser::RuleExpression3, precedence);

    

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
    setState(374);
    expression4(0);
    _ctx->stop = _input->LT(-1);
    setState(381);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 38, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression3Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression3);
        setState(376);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(377);
        match(gscParser::T__49);
        setState(378);
        expression4(0); 
      }
      setState(383);
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
  size_t startState = 50;
  enterRecursionRule(_localctx, 50, gscParser::RuleExpression4, precedence);

    

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
    setState(385);
    expression5(0);
    _ctx->stop = _input->LT(-1);
    setState(392);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 39, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression4Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression4);
        setState(387);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(388);
        match(gscParser::T__50);
        setState(389);
        expression5(0); 
      }
      setState(394);
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
  size_t startState = 52;
  enterRecursionRule(_localctx, 52, gscParser::RuleExpression5, precedence);

    

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
    setState(396);
    expression6(0);
    _ctx->stop = _input->LT(-1);
    setState(403);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 40, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression5Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression5);
        setState(398);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(399);
        match(gscParser::T__14);
        setState(400);
        expression6(0); 
      }
      setState(405);
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
  size_t startState = 54;
  enterRecursionRule(_localctx, 54, gscParser::RuleExpression6, precedence);

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
    setState(407);
    expression7(0);
    _ctx->stop = _input->LT(-1);
    setState(414);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 41, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression6Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression6);
        setState(409);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(410);
        _la = _input->LA(1);
        if (!((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & 67553994410557440) != 0))) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(411);
        expression7(0); 
      }
      setState(416);
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
  size_t startState = 56;
  enterRecursionRule(_localctx, 56, gscParser::RuleExpression7, precedence);

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
    setState(418);
    expression8(0);
    _ctx->stop = _input->LT(-1);
    setState(425);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 42, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression7Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression7);
        setState(420);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(421);
        _la = _input->LA(1);
        if (!((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & 1080863910568919040) != 0))) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(422);
        expression8(0); 
      }
      setState(427);
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
  size_t startState = 58;
  enterRecursionRule(_localctx, 58, gscParser::RuleExpression8, precedence);

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
    setState(429);
    expression9(0);
    _ctx->stop = _input->LT(-1);
    setState(436);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 43, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression8Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression8);
        setState(431);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(432);
        _la = _input->LA(1);
        if (!(_la == gscParser::T__55

        || _la == gscParser::T__56)) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(433);
        expression9(0); 
      }
      setState(438);
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
  size_t startState = 60;
  enterRecursionRule(_localctx, 60, gscParser::RuleExpression9, precedence);

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
    setState(440);
    expression10(0);
    _ctx->stop = _input->LT(-1);
    setState(447);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 44, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression9Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression9);
        setState(442);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(443);
        _la = _input->LA(1);
        if (!(_la == gscParser::T__59

        || _la == gscParser::T__60)) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(444);
        expression10(0); 
      }
      setState(449);
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
  size_t startState = 62;
  enterRecursionRule(_localctx, 62, gscParser::RuleExpression10, precedence);

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
    setState(451);
    expression11(0);
    _ctx->stop = _input->LT(-1);
    setState(458);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 45, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression10Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression10);
        setState(453);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(454);
        _la = _input->LA(1);
        if (!(_la == gscParser::T__61

        || _la == gscParser::T__62)) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(455);
        expression11(0); 
      }
      setState(460);
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
  size_t startState = 64;
  enterRecursionRule(_localctx, 64, gscParser::RuleExpression11, precedence);

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
    setState(462);
    expression12();
    _ctx->stop = _input->LT(-1);
    setState(469);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 46, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression11Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression11);
        setState(464);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(465);
        _la = _input->LA(1);
        if (!(((((_la - 14) & ~ 0x3fULL) == 0) &&
          ((1ULL << (_la - 14)) & 3377699720527873) != 0))) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(466);
        expression12(); 
      }
      setState(471);
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
  enterRule(_localctx, 66, gscParser::RuleExpression12);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(480);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 47, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(472);
      _la = _input->LA(1);
      if (!(_la == gscParser::T__65

      || _la == gscParser::T__66)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(473);
      expression13();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(474);
      _la = _input->LA(1);
      if (!(_la == gscParser::T__67

      || _la == gscParser::T__68)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(475);
      left_value(0);
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(476);
      left_value(0);
      setState(477);
      _la = _input->LA(1);
      if (!(_la == gscParser::T__67

      || _la == gscParser::T__68)) {
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
      setState(479);
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
  enterRule(_localctx, 68, gscParser::RuleExpression13);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(484);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 48, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(482);
      function_call(0);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(483);
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
  enterRule(_localctx, 70, gscParser::RuleExpression14);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(492);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 49, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(486);
      const_expr();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(487);
      match(gscParser::T__7);
      setState(488);
      expression();
      setState(489);
      match(gscParser::T__8);
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(491);
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

gscParser::Object_left_valueContext* gscParser::Left_valueContext::object_left_value() {
  return getRuleContext<gscParser::Object_left_valueContext>(0);
}

gscParser::Array_left_valueContext* gscParser::Left_valueContext::array_left_value() {
  return getRuleContext<gscParser::Array_left_valueContext>(0);
}

gscParser::Left_valueContext* gscParser::Left_valueContext::left_value() {
  return getRuleContext<gscParser::Left_valueContext>(0);
}

gscParser::ExpressionContext* gscParser::Left_valueContext::expression() {
  return getRuleContext<gscParser::ExpressionContext>(0);
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
  size_t startState = 72;
  enterRecursionRule(_localctx, 72, gscParser::RuleLeft_value, precedence);

    

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
    setState(498);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 50, _ctx)) {
    case 1: {
      setState(495);
      match(gscParser::IDENTIFIER);
      break;
    }

    case 2: {
      setState(496);
      object_left_value();
      break;
    }

    case 3: {
      setState(497);
      array_left_value();
      break;
    }

    default:
      break;
    }
    _ctx->stop = _input->LT(-1);
    setState(516);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 53, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        setState(514);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 52, _ctx)) {
        case 1: {
          _localctx = _tracker.createInstance<Left_valueContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleLeft_value);
          setState(500);

          if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
          setState(501);
          match(gscParser::T__69);
          setState(507);
          _errHandler->sync(this);
          switch (_input->LA(1)) {
            case gscParser::IDENTIFIER: {
              setState(502);
              match(gscParser::IDENTIFIER);
              break;
            }

            case gscParser::T__7: {
              setState(503);
              match(gscParser::T__7);
              setState(504);
              expression();
              setState(505);
              match(gscParser::T__8);
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
          setState(509);

          if (!(precpred(_ctx, 1))) throw FailedPredicateException(this, "precpred(_ctx, 1)");
          setState(510);
          match(gscParser::T__10);
          setState(511);
          expression();
          setState(512);
          match(gscParser::T__11);
          break;
        }

        default:
          break;
        } 
      }
      setState(518);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 53, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- Array_left_valueContext ------------------------------------------------------------------

gscParser::Array_left_valueContext::Array_left_valueContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<gscParser::ExpressionContext *> gscParser::Array_left_valueContext::expression() {
  return getRuleContexts<gscParser::ExpressionContext>();
}

gscParser::ExpressionContext* gscParser::Array_left_valueContext::expression(size_t i) {
  return getRuleContext<gscParser::ExpressionContext>(i);
}

gscParser::Const_exprContext* gscParser::Array_left_valueContext::const_expr() {
  return getRuleContext<gscParser::Const_exprContext>(0);
}


size_t gscParser::Array_left_valueContext::getRuleIndex() const {
  return gscParser::RuleArray_left_value;
}


std::any gscParser::Array_left_valueContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitArray_left_value(this);
  else
    return visitor->visitChildren(this);
}

gscParser::Array_left_valueContext* gscParser::array_left_value() {
  Array_left_valueContext *_localctx = _tracker.createInstance<Array_left_valueContext>(_ctx, getState());
  enterRule(_localctx, 74, gscParser::RuleArray_left_value);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(524);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 54, _ctx)) {
    case 1: {
      setState(519);
      const_expr();
      break;
    }

    case 2: {
      setState(520);
      match(gscParser::T__7);
      setState(521);
      expression();
      setState(522);
      match(gscParser::T__8);
      break;
    }

    default:
      break;
    }
    setState(526);
    match(gscParser::T__10);
    setState(527);
    expression();
    setState(528);
    match(gscParser::T__11);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Object_left_valueContext ------------------------------------------------------------------

gscParser::Object_left_valueContext::Object_left_valueContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

gscParser::Const_exprContext* gscParser::Object_left_valueContext::const_expr() {
  return getRuleContext<gscParser::Const_exprContext>(0);
}

tree::TerminalNode* gscParser::Object_left_valueContext::IDENTIFIER() {
  return getToken(gscParser::IDENTIFIER, 0);
}

std::vector<gscParser::ExpressionContext *> gscParser::Object_left_valueContext::expression() {
  return getRuleContexts<gscParser::ExpressionContext>();
}

gscParser::ExpressionContext* gscParser::Object_left_valueContext::expression(size_t i) {
  return getRuleContext<gscParser::ExpressionContext>(i);
}


size_t gscParser::Object_left_valueContext::getRuleIndex() const {
  return gscParser::RuleObject_left_value;
}


std::any gscParser::Object_left_valueContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitObject_left_value(this);
  else
    return visitor->visitChildren(this);
}

gscParser::Object_left_valueContext* gscParser::object_left_value() {
  Object_left_valueContext *_localctx = _tracker.createInstance<Object_left_valueContext>(_ctx, getState());
  enterRule(_localctx, 76, gscParser::RuleObject_left_value);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(535);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 55, _ctx)) {
    case 1: {
      setState(530);
      const_expr();
      break;
    }

    case 2: {
      setState(531);
      match(gscParser::T__7);
      setState(532);
      expression();
      setState(533);
      match(gscParser::T__8);
      break;
    }

    default:
      break;
    }
    setState(537);
    match(gscParser::T__69);
    setState(543);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case gscParser::IDENTIFIER: {
        setState(538);
        match(gscParser::IDENTIFIER);
        break;
      }

      case gscParser::T__7: {
        setState(539);
        match(gscParser::T__7);
        setState(540);
        expression();
        setState(541);
        match(gscParser::T__8);
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
  enterRule(_localctx, 78, gscParser::RuleConst_expr);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(555);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case gscParser::T__7: {
        enterOuterAlt(_localctx, 1);
        setState(545);
        vector_value();
        break;
      }

      case gscParser::T__10: {
        enterOuterAlt(_localctx, 2);
        setState(546);
        array_def();
        break;
      }

      case gscParser::T__17: {
        enterOuterAlt(_localctx, 3);
        setState(547);
        struct_def();
        break;
      }

      case gscParser::INTEGER10:
      case gscParser::INTEGER16:
      case gscParser::INTEGER8:
      case gscParser::INTEGER2: {
        enterOuterAlt(_localctx, 4);
        setState(548);
        number();
        break;
      }

      case gscParser::BOOL_VALUE: {
        enterOuterAlt(_localctx, 5);
        setState(549);
        match(gscParser::BOOL_VALUE);
        break;
      }

      case gscParser::FLOATVAL: {
        enterOuterAlt(_localctx, 6);
        setState(550);
        match(gscParser::FLOATVAL);
        break;
      }

      case gscParser::STRING: {
        enterOuterAlt(_localctx, 7);
        setState(551);
        match(gscParser::STRING);
        break;
      }

      case gscParser::HASHSTRING: {
        enterOuterAlt(_localctx, 8);
        setState(552);
        match(gscParser::HASHSTRING);
        break;
      }

      case gscParser::UNDEFINED_VALUE: {
        enterOuterAlt(_localctx, 9);
        setState(553);
        match(gscParser::UNDEFINED_VALUE);
        break;
      }

      case gscParser::T__14:
      case gscParser::T__70: {
        enterOuterAlt(_localctx, 10);
        setState(554);
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
  enterRule(_localctx, 80, gscParser::RuleFunction_ref);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(572);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 59, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(557);
      match(gscParser::T__14);
      setState(560);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 58, _ctx)) {
      case 1: {
        setState(558);
        match(gscParser::IDENTIFIER);
        setState(559);
        match(gscParser::T__33);
        break;
      }

      default:
        break;
      }
      setState(562);
      match(gscParser::IDENTIFIER);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(563);
      match(gscParser::T__70);
      setState(564);
      match(gscParser::IDENTIFIER);
      setState(565);
      match(gscParser::T__55);
      setState(566);
      match(gscParser::PATH);
      setState(567);
      match(gscParser::T__57);
      setState(568);
      match(gscParser::T__33);
      setState(569);
      match(gscParser::IDENTIFIER);
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(570);
      match(gscParser::T__14);
      setState(571);
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
  enterRule(_localctx, 82, gscParser::RuleNumber);
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
    setState(574);
    _la = _input->LA(1);
    if (!(((((_la - 74) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 74)) & 15) != 0))) {
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
  enterRule(_localctx, 84, gscParser::RuleVector_value);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(576);
    match(gscParser::T__7);
    setState(577);
    expression();
    setState(578);
    match(gscParser::T__12);
    setState(579);
    expression();
    setState(580);
    match(gscParser::T__12);
    setState(581);
    expression();
    setState(582);
    match(gscParser::T__8);
   
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
  enterRule(_localctx, 86, gscParser::RuleArray_def);
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
    setState(584);
    match(gscParser::T__10);
    setState(606);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 6442748160) != 0) || ((((_la - 66) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 66)) & 1695535) != 0)) {
      setState(588);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 60, _ctx)) {
      case 1: {
        setState(585);
        expression();
        setState(586);
        match(gscParser::T__19);
        break;
      }

      default:
        break;
      }
      setState(590);
      expression();
      setState(600);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 62, _ctx);
      while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
        if (alt == 1) {
          setState(591);
          match(gscParser::T__12);
          setState(595);
          _errHandler->sync(this);

          switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 61, _ctx)) {
          case 1: {
            setState(592);
            expression();
            setState(593);
            match(gscParser::T__19);
            break;
          }

          default:
            break;
          }
          setState(597);
          expression(); 
        }
        setState(602);
        _errHandler->sync(this);
        alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 62, _ctx);
      }
      setState(604);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == gscParser::T__12) {
        setState(603);
        match(gscParser::T__12);
      }
    }
    setState(608);
    match(gscParser::T__11);
   
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
  enterRule(_localctx, 88, gscParser::RuleStruct_def);
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
    setState(610);
    match(gscParser::T__17);
    setState(626);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::STRUCT_IDENTIFIER) {
      setState(611);
      match(gscParser::STRUCT_IDENTIFIER);
      setState(612);
      match(gscParser::T__19);
      setState(613);
      expression();
      setState(620);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 65, _ctx);
      while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
        if (alt == 1) {
          setState(614);
          match(gscParser::T__12);
          setState(615);
          match(gscParser::STRUCT_IDENTIFIER);
          setState(616);
          match(gscParser::T__19);
          setState(617);
          expression(); 
        }
        setState(622);
        _errHandler->sync(this);
        alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 65, _ctx);
      }
      setState(624);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == gscParser::T__12) {
        setState(623);
        match(gscParser::T__12);
      }
    }
    setState(628);
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
    case 22: return expression1Sempred(antlrcpp::downCast<Expression1Context *>(context), predicateIndex);
    case 23: return expression2Sempred(antlrcpp::downCast<Expression2Context *>(context), predicateIndex);
    case 24: return expression3Sempred(antlrcpp::downCast<Expression3Context *>(context), predicateIndex);
    case 25: return expression4Sempred(antlrcpp::downCast<Expression4Context *>(context), predicateIndex);
    case 26: return expression5Sempred(antlrcpp::downCast<Expression5Context *>(context), predicateIndex);
    case 27: return expression6Sempred(antlrcpp::downCast<Expression6Context *>(context), predicateIndex);
    case 28: return expression7Sempred(antlrcpp::downCast<Expression7Context *>(context), predicateIndex);
    case 29: return expression8Sempred(antlrcpp::downCast<Expression8Context *>(context), predicateIndex);
    case 30: return expression9Sempred(antlrcpp::downCast<Expression9Context *>(context), predicateIndex);
    case 31: return expression10Sempred(antlrcpp::downCast<Expression10Context *>(context), predicateIndex);
    case 32: return expression11Sempred(antlrcpp::downCast<Expression11Context *>(context), predicateIndex);
    case 36: return left_valueSempred(antlrcpp::downCast<Left_valueContext *>(context), predicateIndex);

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
