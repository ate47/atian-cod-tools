
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
      "statement_foreach", "statement_if", "statement_switch", "statement_inst", 
      "function_call", "function_component", "operator_inst", "expression", 
      "set_expression", "expression1", "expression2", "expression3", "expression4", 
      "expression5", "expression6", "expression7", "expression8", "expression9", 
      "expression10", "expression11", "expression12", "expression13", "expression14", 
      "left_value", "const_expr", "function_ref", "number", "vector_value", 
      "array_def", "struct_def"
    },
    std::vector<std::string>{
      "", "'#include'", "'#using'", "';'", "'#namespace'", "'function'", 
      "'private'", "'autoexec'", "'event_handler'", "'['", "']'", "'('", 
      "')'", "','", "'*'", "'&'", "'='", "'...'", "'{'", "'}'", "'for'", 
      "'foreach'", "'in'", "'if'", "'else'", "'switch'", "'case'", "'default'", 
      "':'", "'thread'", "'childthread'", "'::'", "'[['", "']]'", "'->'", 
      "'+='", "'-='", "'/='", "'*='", "'%='", "'&='", "'|='", "'^='", "'<<='", 
      "'>>='", "'~='", "'||'", "'&&'", "'|'", "'^'", "'!='", "'=='", "'!=='", 
      "'==='", "'<'", "'<='", "'>'", "'>='", "'<<'", "'>>'", "'+'", "'-'", 
      "'/'", "'%'", "'!'", "'~'", "'++'", "'--'", "'.'", "'@'", "'#'", "", 
      "", "", "", "", "", "", "", "", "", "", "", "", "'undefined'"
    },
    std::vector<std::string>{
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
      "", "", "", "NEWLINE", "WHITESPACE", "INTEGER10", "INTEGER16", "INTEGER8", 
      "INTEGER2", "FLOATVAL", "IDENTIFIER", "STRUCT_IDENTIFIER", "PATH", 
      "STRING", "HASHSTRING", "BOOL_VALUE", "UNDEFINED_VALUE"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,84,566,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,7,
  	14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,7,
  	21,2,22,7,22,2,23,7,23,2,24,7,24,2,25,7,25,2,26,7,26,2,27,7,27,2,28,7,
  	28,2,29,7,29,2,30,7,30,2,31,7,31,2,32,7,32,2,33,7,33,2,34,7,34,2,35,7,
  	35,2,36,7,36,2,37,7,37,2,38,7,38,2,39,7,39,1,0,1,0,1,0,5,0,84,8,0,10,
  	0,12,0,87,9,0,1,0,1,0,1,1,1,1,1,1,1,1,1,2,1,2,1,2,1,2,1,3,3,3,100,8,3,
  	1,3,3,3,103,8,3,1,3,3,3,106,8,3,1,3,1,3,1,3,1,3,3,3,112,8,3,1,3,1,3,1,
  	3,1,3,1,3,1,3,1,4,1,4,1,4,5,4,123,8,4,10,4,12,4,126,9,4,3,4,128,8,4,1,
  	5,3,5,131,8,5,1,5,1,5,1,5,3,5,136,8,5,1,5,3,5,139,8,5,1,6,1,6,1,6,5,6,
  	144,8,6,10,6,12,6,147,9,6,3,6,149,8,6,1,7,1,7,5,7,153,8,7,10,7,12,7,156,
  	9,7,1,7,1,7,1,8,1,8,1,8,1,8,1,8,1,8,3,8,166,8,8,1,9,1,9,1,9,3,9,171,8,
  	9,1,9,1,9,3,9,175,8,9,1,9,1,9,3,9,179,8,9,1,9,1,9,1,9,1,10,1,10,1,10,
  	1,10,1,10,3,10,189,8,10,1,10,1,10,1,10,1,10,1,10,1,11,1,11,1,11,1,11,
  	1,11,1,11,1,11,3,11,203,8,11,1,12,1,12,1,12,1,12,1,12,1,12,1,12,1,12,
  	3,12,213,8,12,1,12,1,12,5,12,217,8,12,10,12,12,12,220,9,12,4,12,222,8,
  	12,11,12,12,12,223,1,12,1,12,1,13,1,13,3,13,230,8,13,1,13,1,13,1,14,1,
  	14,3,14,236,8,14,1,14,1,14,1,14,1,14,1,14,1,14,1,14,3,14,245,8,14,1,14,
  	1,14,1,14,1,14,1,14,3,14,252,8,14,1,14,1,14,3,14,256,8,14,1,14,1,14,1,
  	14,1,14,1,14,5,14,263,8,14,10,14,12,14,266,9,14,1,15,1,15,3,15,270,8,
  	15,1,15,1,15,1,15,1,15,1,15,1,15,1,15,1,15,1,15,1,15,1,15,3,15,283,8,
  	15,1,16,1,16,3,16,287,8,16,1,17,1,17,3,17,291,8,17,1,18,1,18,1,18,1,18,
  	1,19,1,19,1,19,1,19,1,19,1,19,5,19,303,8,19,10,19,12,19,306,9,19,1,20,
  	1,20,1,20,1,20,1,20,1,20,5,20,314,8,20,10,20,12,20,317,9,20,1,21,1,21,
  	1,21,1,21,1,21,1,21,5,21,325,8,21,10,21,12,21,328,9,21,1,22,1,22,1,22,
  	1,22,1,22,1,22,5,22,336,8,22,10,22,12,22,339,9,22,1,23,1,23,1,23,1,23,
  	1,23,1,23,5,23,347,8,23,10,23,12,23,350,9,23,1,24,1,24,1,24,1,24,1,24,
  	1,24,5,24,358,8,24,10,24,12,24,361,9,24,1,25,1,25,1,25,1,25,1,25,1,25,
  	5,25,369,8,25,10,25,12,25,372,9,25,1,26,1,26,1,26,1,26,1,26,1,26,5,26,
  	380,8,26,10,26,12,26,383,9,26,1,27,1,27,1,27,1,27,1,27,1,27,5,27,391,
  	8,27,10,27,12,27,394,9,27,1,28,1,28,1,28,1,28,1,28,1,28,5,28,402,8,28,
  	10,28,12,28,405,9,28,1,29,1,29,1,29,1,29,1,29,1,29,5,29,413,8,29,10,29,
  	12,29,416,9,29,1,30,1,30,1,30,1,30,1,30,1,30,1,30,1,30,3,30,426,8,30,
  	1,31,1,31,3,31,430,8,31,1,32,1,32,1,32,1,32,1,32,1,32,3,32,438,8,32,1,
  	33,1,33,1,33,1,33,1,33,1,33,1,33,1,33,1,33,1,33,1,33,1,33,1,33,3,33,453,
  	8,33,1,33,1,33,1,33,1,33,1,33,1,33,3,33,461,8,33,1,33,1,33,1,33,1,33,
  	1,33,3,33,468,8,33,1,33,1,33,1,33,1,33,3,33,474,8,33,1,33,1,33,1,33,1,
  	33,1,33,1,33,1,33,3,33,483,8,33,1,33,1,33,1,33,1,33,1,33,5,33,490,8,33,
  	10,33,12,33,493,9,33,1,34,1,34,1,34,1,34,1,34,1,34,1,34,1,34,1,34,1,34,
  	3,34,505,8,34,1,35,1,35,1,35,3,35,510,8,35,1,35,1,35,1,35,1,35,1,35,1,
  	35,1,35,1,35,3,35,520,8,35,1,36,1,36,1,37,1,37,1,37,1,37,1,37,1,37,1,
  	37,1,37,1,38,1,38,1,38,1,38,3,38,536,8,38,1,38,3,38,539,8,38,1,38,1,38,
  	3,38,543,8,38,5,38,545,8,38,10,38,12,38,548,9,38,1,38,1,38,1,39,1,39,
  	1,39,1,39,1,39,3,39,557,8,39,5,39,559,8,39,10,39,12,39,562,9,39,1,39,
  	1,39,1,39,0,13,28,38,40,42,44,46,48,50,52,54,56,58,66,40,0,2,4,6,8,10,
  	12,14,16,18,20,22,24,26,28,30,32,34,36,38,40,42,44,46,48,50,52,54,56,
  	58,60,62,64,66,68,70,72,74,76,78,0,14,1,0,1,2,2,0,78,78,80,80,1,0,14,
  	15,1,0,29,30,2,0,16,16,35,45,1,0,50,53,1,0,54,57,1,0,54,55,1,0,58,59,
  	1,0,60,61,2,0,14,14,62,63,1,0,64,65,1,0,66,67,1,0,73,76,608,0,85,1,0,
  	0,0,2,90,1,0,0,0,4,94,1,0,0,0,6,99,1,0,0,0,8,127,1,0,0,0,10,138,1,0,0,
  	0,12,148,1,0,0,0,14,150,1,0,0,0,16,165,1,0,0,0,18,167,1,0,0,0,20,183,
  	1,0,0,0,22,195,1,0,0,0,24,204,1,0,0,0,26,229,1,0,0,0,28,251,1,0,0,0,30,
  	282,1,0,0,0,32,284,1,0,0,0,34,290,1,0,0,0,36,292,1,0,0,0,38,296,1,0,0,
  	0,40,307,1,0,0,0,42,318,1,0,0,0,44,329,1,0,0,0,46,340,1,0,0,0,48,351,
  	1,0,0,0,50,362,1,0,0,0,52,373,1,0,0,0,54,384,1,0,0,0,56,395,1,0,0,0,58,
  	406,1,0,0,0,60,425,1,0,0,0,62,429,1,0,0,0,64,437,1,0,0,0,66,473,1,0,0,
  	0,68,504,1,0,0,0,70,519,1,0,0,0,72,521,1,0,0,0,74,523,1,0,0,0,76,531,
  	1,0,0,0,78,551,1,0,0,0,80,84,3,6,3,0,81,84,3,2,1,0,82,84,3,4,2,0,83,80,
  	1,0,0,0,83,81,1,0,0,0,83,82,1,0,0,0,84,87,1,0,0,0,85,83,1,0,0,0,85,86,
  	1,0,0,0,86,88,1,0,0,0,87,85,1,0,0,0,88,89,5,0,0,1,89,1,1,0,0,0,90,91,
  	7,0,0,0,91,92,7,1,0,0,92,93,5,3,0,0,93,3,1,0,0,0,94,95,5,4,0,0,95,96,
  	5,78,0,0,96,97,5,3,0,0,97,5,1,0,0,0,98,100,5,5,0,0,99,98,1,0,0,0,99,100,
  	1,0,0,0,100,102,1,0,0,0,101,103,5,6,0,0,102,101,1,0,0,0,102,103,1,0,0,
  	0,103,105,1,0,0,0,104,106,5,7,0,0,105,104,1,0,0,0,105,106,1,0,0,0,106,
  	111,1,0,0,0,107,108,5,8,0,0,108,109,5,9,0,0,109,110,5,78,0,0,110,112,
  	5,10,0,0,111,107,1,0,0,0,111,112,1,0,0,0,112,113,1,0,0,0,113,114,5,78,
  	0,0,114,115,5,11,0,0,115,116,3,8,4,0,116,117,5,12,0,0,117,118,3,14,7,
  	0,118,7,1,0,0,0,119,124,3,10,5,0,120,121,5,13,0,0,121,123,3,10,5,0,122,
  	120,1,0,0,0,123,126,1,0,0,0,124,122,1,0,0,0,124,125,1,0,0,0,125,128,1,
  	0,0,0,126,124,1,0,0,0,127,119,1,0,0,0,127,128,1,0,0,0,128,9,1,0,0,0,129,
  	131,7,2,0,0,130,129,1,0,0,0,130,131,1,0,0,0,131,132,1,0,0,0,132,135,5,
  	78,0,0,133,134,5,16,0,0,134,136,3,34,17,0,135,133,1,0,0,0,135,136,1,0,
  	0,0,136,139,1,0,0,0,137,139,5,17,0,0,138,130,1,0,0,0,138,137,1,0,0,0,
  	139,11,1,0,0,0,140,145,3,34,17,0,141,142,5,13,0,0,142,144,3,34,17,0,143,
  	141,1,0,0,0,144,147,1,0,0,0,145,143,1,0,0,0,145,146,1,0,0,0,146,149,1,
  	0,0,0,147,145,1,0,0,0,148,140,1,0,0,0,148,149,1,0,0,0,149,13,1,0,0,0,
  	150,154,5,18,0,0,151,153,3,16,8,0,152,151,1,0,0,0,153,156,1,0,0,0,154,
  	152,1,0,0,0,154,155,1,0,0,0,155,157,1,0,0,0,156,154,1,0,0,0,157,158,5,
  	19,0,0,158,15,1,0,0,0,159,166,3,14,7,0,160,166,3,18,9,0,161,166,3,22,
  	11,0,162,166,3,20,10,0,163,166,3,26,13,0,164,166,3,24,12,0,165,159,1,
  	0,0,0,165,160,1,0,0,0,165,161,1,0,0,0,165,162,1,0,0,0,165,163,1,0,0,0,
  	165,164,1,0,0,0,166,17,1,0,0,0,167,168,5,20,0,0,168,170,5,11,0,0,169,
  	171,3,34,17,0,170,169,1,0,0,0,170,171,1,0,0,0,171,172,1,0,0,0,172,174,
  	5,3,0,0,173,175,3,34,17,0,174,173,1,0,0,0,174,175,1,0,0,0,175,176,1,0,
  	0,0,176,178,5,3,0,0,177,179,3,34,17,0,178,177,1,0,0,0,178,179,1,0,0,0,
  	179,180,1,0,0,0,180,181,5,12,0,0,181,182,3,16,8,0,182,19,1,0,0,0,183,
  	184,5,21,0,0,184,185,5,11,0,0,185,188,5,78,0,0,186,187,5,13,0,0,187,189,
  	5,78,0,0,188,186,1,0,0,0,188,189,1,0,0,0,189,190,1,0,0,0,190,191,5,22,
  	0,0,191,192,3,34,17,0,192,193,5,12,0,0,193,194,3,16,8,0,194,21,1,0,0,
  	0,195,196,5,23,0,0,196,197,5,11,0,0,197,198,3,34,17,0,198,199,5,12,0,
  	0,199,202,3,16,8,0,200,201,5,24,0,0,201,203,3,16,8,0,202,200,1,0,0,0,
  	202,203,1,0,0,0,203,23,1,0,0,0,204,205,5,25,0,0,205,206,5,11,0,0,206,
  	207,3,34,17,0,207,208,5,12,0,0,208,221,5,18,0,0,209,210,5,26,0,0,210,
  	213,3,68,34,0,211,213,5,27,0,0,212,209,1,0,0,0,212,211,1,0,0,0,213,214,
  	1,0,0,0,214,218,5,28,0,0,215,217,3,16,8,0,216,215,1,0,0,0,217,220,1,0,
  	0,0,218,216,1,0,0,0,218,219,1,0,0,0,219,222,1,0,0,0,220,218,1,0,0,0,221,
  	212,1,0,0,0,222,223,1,0,0,0,223,221,1,0,0,0,223,224,1,0,0,0,224,225,1,
  	0,0,0,225,226,5,19,0,0,226,25,1,0,0,0,227,230,3,28,14,0,228,230,3,32,
  	16,0,229,227,1,0,0,0,229,228,1,0,0,0,229,230,1,0,0,0,230,231,1,0,0,0,
  	231,232,5,3,0,0,232,27,1,0,0,0,233,235,6,14,-1,0,234,236,7,3,0,0,235,
  	234,1,0,0,0,235,236,1,0,0,0,236,237,1,0,0,0,237,238,3,30,15,0,238,239,
  	5,11,0,0,239,240,3,12,6,0,240,241,5,12,0,0,241,252,1,0,0,0,242,244,3,
  	64,32,0,243,245,7,3,0,0,244,243,1,0,0,0,244,245,1,0,0,0,245,246,1,0,0,
  	0,246,247,3,30,15,0,247,248,5,11,0,0,248,249,3,12,6,0,249,250,5,12,0,
  	0,250,252,1,0,0,0,251,233,1,0,0,0,251,242,1,0,0,0,252,264,1,0,0,0,253,
  	255,10,1,0,0,254,256,7,3,0,0,255,254,1,0,0,0,255,256,1,0,0,0,256,257,
  	1,0,0,0,257,258,3,30,15,0,258,259,5,11,0,0,259,260,3,12,6,0,260,261,5,
  	12,0,0,261,263,1,0,0,0,262,253,1,0,0,0,263,266,1,0,0,0,264,262,1,0,0,
  	0,264,265,1,0,0,0,265,29,1,0,0,0,266,264,1,0,0,0,267,268,5,78,0,0,268,
  	270,5,31,0,0,269,267,1,0,0,0,269,270,1,0,0,0,270,271,1,0,0,0,271,283,
  	5,78,0,0,272,273,5,32,0,0,273,274,3,34,17,0,274,275,5,33,0,0,275,283,
  	1,0,0,0,276,277,5,32,0,0,277,278,3,34,17,0,278,279,5,33,0,0,279,280,5,
  	34,0,0,280,281,5,78,0,0,281,283,1,0,0,0,282,269,1,0,0,0,282,272,1,0,0,
  	0,282,276,1,0,0,0,283,31,1,0,0,0,284,286,5,78,0,0,285,287,3,34,17,0,286,
  	285,1,0,0,0,286,287,1,0,0,0,287,33,1,0,0,0,288,291,3,36,18,0,289,291,
  	3,38,19,0,290,288,1,0,0,0,290,289,1,0,0,0,291,35,1,0,0,0,292,293,3,66,
  	33,0,293,294,7,4,0,0,294,295,3,34,17,0,295,37,1,0,0,0,296,297,6,19,-1,
  	0,297,298,3,40,20,0,298,304,1,0,0,0,299,300,10,2,0,0,300,301,5,46,0,0,
  	301,303,3,40,20,0,302,299,1,0,0,0,303,306,1,0,0,0,304,302,1,0,0,0,304,
  	305,1,0,0,0,305,39,1,0,0,0,306,304,1,0,0,0,307,308,6,20,-1,0,308,309,
  	3,42,21,0,309,315,1,0,0,0,310,311,10,2,0,0,311,312,5,47,0,0,312,314,3,
  	42,21,0,313,310,1,0,0,0,314,317,1,0,0,0,315,313,1,0,0,0,315,316,1,0,0,
  	0,316,41,1,0,0,0,317,315,1,0,0,0,318,319,6,21,-1,0,319,320,3,44,22,0,
  	320,326,1,0,0,0,321,322,10,2,0,0,322,323,5,48,0,0,323,325,3,44,22,0,324,
  	321,1,0,0,0,325,328,1,0,0,0,326,324,1,0,0,0,326,327,1,0,0,0,327,43,1,
  	0,0,0,328,326,1,0,0,0,329,330,6,22,-1,0,330,331,3,46,23,0,331,337,1,0,
  	0,0,332,333,10,2,0,0,333,334,5,49,0,0,334,336,3,46,23,0,335,332,1,0,0,
  	0,336,339,1,0,0,0,337,335,1,0,0,0,337,338,1,0,0,0,338,45,1,0,0,0,339,
  	337,1,0,0,0,340,341,6,23,-1,0,341,342,3,48,24,0,342,348,1,0,0,0,343,344,
  	10,2,0,0,344,345,5,15,0,0,345,347,3,48,24,0,346,343,1,0,0,0,347,350,1,
  	0,0,0,348,346,1,0,0,0,348,349,1,0,0,0,349,47,1,0,0,0,350,348,1,0,0,0,
  	351,352,6,24,-1,0,352,353,3,50,25,0,353,359,1,0,0,0,354,355,10,2,0,0,
  	355,356,7,5,0,0,356,358,3,50,25,0,357,354,1,0,0,0,358,361,1,0,0,0,359,
  	357,1,0,0,0,359,360,1,0,0,0,360,49,1,0,0,0,361,359,1,0,0,0,362,363,6,
  	25,-1,0,363,364,3,52,26,0,364,370,1,0,0,0,365,366,10,2,0,0,366,367,7,
  	6,0,0,367,369,3,52,26,0,368,365,1,0,0,0,369,372,1,0,0,0,370,368,1,0,0,
  	0,370,371,1,0,0,0,371,51,1,0,0,0,372,370,1,0,0,0,373,374,6,26,-1,0,374,
  	375,3,54,27,0,375,381,1,0,0,0,376,377,10,2,0,0,377,378,7,7,0,0,378,380,
  	3,54,27,0,379,376,1,0,0,0,380,383,1,0,0,0,381,379,1,0,0,0,381,382,1,0,
  	0,0,382,53,1,0,0,0,383,381,1,0,0,0,384,385,6,27,-1,0,385,386,3,56,28,
  	0,386,392,1,0,0,0,387,388,10,2,0,0,388,389,7,8,0,0,389,391,3,56,28,0,
  	390,387,1,0,0,0,391,394,1,0,0,0,392,390,1,0,0,0,392,393,1,0,0,0,393,55,
  	1,0,0,0,394,392,1,0,0,0,395,396,6,28,-1,0,396,397,3,58,29,0,397,403,1,
  	0,0,0,398,399,10,2,0,0,399,400,7,9,0,0,400,402,3,58,29,0,401,398,1,0,
  	0,0,402,405,1,0,0,0,403,401,1,0,0,0,403,404,1,0,0,0,404,57,1,0,0,0,405,
  	403,1,0,0,0,406,407,6,29,-1,0,407,408,3,60,30,0,408,414,1,0,0,0,409,410,
  	10,2,0,0,410,411,7,10,0,0,411,413,3,60,30,0,412,409,1,0,0,0,413,416,1,
  	0,0,0,414,412,1,0,0,0,414,415,1,0,0,0,415,59,1,0,0,0,416,414,1,0,0,0,
  	417,418,7,11,0,0,418,426,3,62,31,0,419,420,7,12,0,0,420,426,3,66,33,0,
  	421,422,3,66,33,0,422,423,7,12,0,0,423,426,1,0,0,0,424,426,3,62,31,0,
  	425,417,1,0,0,0,425,419,1,0,0,0,425,421,1,0,0,0,425,424,1,0,0,0,426,61,
  	1,0,0,0,427,430,3,28,14,0,428,430,3,64,32,0,429,427,1,0,0,0,429,428,1,
  	0,0,0,430,63,1,0,0,0,431,438,3,68,34,0,432,433,5,11,0,0,433,434,3,34,
  	17,0,434,435,5,12,0,0,435,438,1,0,0,0,436,438,3,66,33,0,437,431,1,0,0,
  	0,437,432,1,0,0,0,437,436,1,0,0,0,438,65,1,0,0,0,439,440,6,33,-1,0,440,
  	474,5,78,0,0,441,442,5,32,0,0,442,443,3,34,17,0,443,444,5,33,0,0,444,
  	445,5,34,0,0,445,446,5,78,0,0,446,474,1,0,0,0,447,453,3,68,34,0,448,449,
  	5,11,0,0,449,450,3,34,17,0,450,451,5,12,0,0,451,453,1,0,0,0,452,447,1,
  	0,0,0,452,448,1,0,0,0,453,454,1,0,0,0,454,460,5,68,0,0,455,461,5,78,0,
  	0,456,457,5,11,0,0,457,458,3,34,17,0,458,459,5,12,0,0,459,461,1,0,0,0,
  	460,455,1,0,0,0,460,456,1,0,0,0,461,474,1,0,0,0,462,468,3,68,34,0,463,
  	464,5,11,0,0,464,465,3,34,17,0,465,466,5,12,0,0,466,468,1,0,0,0,467,462,
  	1,0,0,0,467,463,1,0,0,0,468,469,1,0,0,0,469,470,5,9,0,0,470,471,3,34,
  	17,0,471,472,5,10,0,0,472,474,1,0,0,0,473,439,1,0,0,0,473,441,1,0,0,0,
  	473,452,1,0,0,0,473,467,1,0,0,0,474,491,1,0,0,0,475,476,10,2,0,0,476,
  	482,5,68,0,0,477,483,5,78,0,0,478,479,5,11,0,0,479,480,3,34,17,0,480,
  	481,5,12,0,0,481,483,1,0,0,0,482,477,1,0,0,0,482,478,1,0,0,0,483,490,
  	1,0,0,0,484,485,10,1,0,0,485,486,5,9,0,0,486,487,3,34,17,0,487,488,5,
  	10,0,0,488,490,1,0,0,0,489,475,1,0,0,0,489,484,1,0,0,0,490,493,1,0,0,
  	0,491,489,1,0,0,0,491,492,1,0,0,0,492,67,1,0,0,0,493,491,1,0,0,0,494,
  	505,3,74,37,0,495,505,3,76,38,0,496,505,3,78,39,0,497,505,3,72,36,0,498,
  	505,5,83,0,0,499,505,5,77,0,0,500,505,5,81,0,0,501,505,5,82,0,0,502,505,
  	5,84,0,0,503,505,3,70,35,0,504,494,1,0,0,0,504,495,1,0,0,0,504,496,1,
  	0,0,0,504,497,1,0,0,0,504,498,1,0,0,0,504,499,1,0,0,0,504,500,1,0,0,0,
  	504,501,1,0,0,0,504,502,1,0,0,0,504,503,1,0,0,0,505,69,1,0,0,0,506,509,
  	5,15,0,0,507,508,5,78,0,0,508,510,5,31,0,0,509,507,1,0,0,0,509,510,1,
  	0,0,0,510,511,1,0,0,0,511,520,5,78,0,0,512,513,5,69,0,0,513,514,5,78,
  	0,0,514,515,5,54,0,0,515,516,5,80,0,0,516,517,5,56,0,0,517,518,5,31,0,
  	0,518,520,5,78,0,0,519,506,1,0,0,0,519,512,1,0,0,0,520,71,1,0,0,0,521,
  	522,7,13,0,0,522,73,1,0,0,0,523,524,5,11,0,0,524,525,3,34,17,0,525,526,
  	5,13,0,0,526,527,3,34,17,0,527,528,5,13,0,0,528,529,3,34,17,0,529,530,
  	5,12,0,0,530,75,1,0,0,0,531,546,5,9,0,0,532,536,5,82,0,0,533,534,5,70,
  	0,0,534,536,3,72,36,0,535,532,1,0,0,0,535,533,1,0,0,0,536,537,1,0,0,0,
  	537,539,5,28,0,0,538,535,1,0,0,0,538,539,1,0,0,0,539,540,1,0,0,0,540,
  	542,3,34,17,0,541,543,5,13,0,0,542,541,1,0,0,0,542,543,1,0,0,0,543,545,
  	1,0,0,0,544,538,1,0,0,0,545,548,1,0,0,0,546,544,1,0,0,0,546,547,1,0,0,
  	0,547,549,1,0,0,0,548,546,1,0,0,0,549,550,5,10,0,0,550,77,1,0,0,0,551,
  	560,5,18,0,0,552,553,5,79,0,0,553,554,5,28,0,0,554,556,3,34,17,0,555,
  	557,5,13,0,0,556,555,1,0,0,0,556,557,1,0,0,0,557,559,1,0,0,0,558,552,
  	1,0,0,0,559,562,1,0,0,0,560,558,1,0,0,0,560,561,1,0,0,0,561,563,1,0,0,
  	0,562,560,1,0,0,0,563,564,5,19,0,0,564,79,1,0,0,0,63,83,85,99,102,105,
  	111,124,127,130,135,138,145,148,154,165,170,174,178,188,202,212,218,223,
  	229,235,244,251,255,264,269,282,286,290,304,315,326,337,348,359,370,381,
  	392,403,414,425,429,437,452,460,467,473,482,489,491,504,509,519,535,538,
  	542,546,556,560
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
    setState(85);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 502) != 0) || _la == gscParser::IDENTIFIER) {
      setState(83);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case gscParser::T__4:
        case gscParser::T__5:
        case gscParser::T__6:
        case gscParser::T__7:
        case gscParser::IDENTIFIER: {
          setState(80);
          function();
          break;
        }

        case gscParser::T__0:
        case gscParser::T__1: {
          setState(81);
          include();
          break;
        }

        case gscParser::T__3: {
          setState(82);
          namespace_();
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(87);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(88);
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
    setState(90);
    _la = _input->LA(1);
    if (!(_la == gscParser::T__0

    || _la == gscParser::T__1)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(91);
    _la = _input->LA(1);
    if (!(_la == gscParser::IDENTIFIER

    || _la == gscParser::PATH)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(92);
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
    setState(94);
    match(gscParser::T__3);
    setState(95);
    match(gscParser::IDENTIFIER);
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
    setState(99);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__4) {
      setState(98);
      match(gscParser::T__4);
    }
    setState(102);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__5) {
      setState(101);
      match(gscParser::T__5);
    }
    setState(105);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__6) {
      setState(104);
      match(gscParser::T__6);
    }
    setState(111);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__7) {
      setState(107);
      match(gscParser::T__7);
      setState(108);
      match(gscParser::T__8);
      setState(109);
      match(gscParser::IDENTIFIER);
      setState(110);
      match(gscParser::T__9);
    }
    setState(113);
    match(gscParser::IDENTIFIER);
    setState(114);
    match(gscParser::T__10);
    setState(115);
    param_list();
    setState(116);
    match(gscParser::T__11);
    setState(117);
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
    setState(127);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 180224) != 0) || _la == gscParser::IDENTIFIER) {
      setState(119);
      param_val();
      setState(124);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == gscParser::T__12) {
        setState(120);
        match(gscParser::T__12);
        setState(121);
        param_val();
        setState(126);
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
    setState(138);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case gscParser::T__13:
      case gscParser::T__14:
      case gscParser::IDENTIFIER: {
        enterOuterAlt(_localctx, 1);
        setState(130);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == gscParser::T__13

        || _la == gscParser::T__14) {
          setState(129);
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
        setState(132);
        match(gscParser::IDENTIFIER);
        setState(135);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == gscParser::T__15) {
          setState(133);
          match(gscParser::T__15);
          setState(134);
          expression();
        }
        break;
      }

      case gscParser::T__16: {
        enterOuterAlt(_localctx, 2);
        setState(137);
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
    setState(148);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 5905877504) != 0) || ((((_la - 64) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 64)) & 1998383) != 0)) {
      setState(140);
      expression();
      setState(145);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == gscParser::T__12) {
        setState(141);
        match(gscParser::T__12);
        setState(142);
        expression();
        setState(147);
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
    setState(150);
    match(gscParser::T__17);
    setState(154);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 5950966280) != 0) || ((((_la - 69) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 69)) & 62449) != 0)) {
      setState(151);
      statement();
      setState(156);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(157);
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

gscParser::Statement_foreachContext* gscParser::StatementContext::statement_foreach() {
  return getRuleContext<gscParser::Statement_foreachContext>(0);
}

gscParser::Statement_instContext* gscParser::StatementContext::statement_inst() {
  return getRuleContext<gscParser::Statement_instContext>(0);
}

gscParser::Statement_switchContext* gscParser::StatementContext::statement_switch() {
  return getRuleContext<gscParser::Statement_switchContext>(0);
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
    setState(165);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 14, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(159);
      statement_block();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(160);
      statement_for();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(161);
      statement_if();
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(162);
      statement_foreach();
      break;
    }

    case 5: {
      enterOuterAlt(_localctx, 5);
      setState(163);
      statement_inst();
      break;
    }

    case 6: {
      enterOuterAlt(_localctx, 6);
      setState(164);
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
    setState(167);
    match(gscParser::T__19);
    setState(168);
    match(gscParser::T__10);
    setState(170);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 5905877504) != 0) || ((((_la - 64) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 64)) & 1998383) != 0)) {
      setState(169);
      expression();
    }
    setState(172);
    match(gscParser::T__2);
    setState(174);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 5905877504) != 0) || ((((_la - 64) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 64)) & 1998383) != 0)) {
      setState(173);
      expression();
    }
    setState(176);
    match(gscParser::T__2);
    setState(178);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 5905877504) != 0) || ((((_la - 64) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 64)) & 1998383) != 0)) {
      setState(177);
      expression();
    }
    setState(180);
    match(gscParser::T__11);
    setState(181);
    statement();
   
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
  enterRule(_localctx, 20, gscParser::RuleStatement_foreach);
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
    setState(183);
    match(gscParser::T__20);
    setState(184);
    match(gscParser::T__10);
    setState(185);
    match(gscParser::IDENTIFIER);
    setState(188);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__12) {
      setState(186);
      match(gscParser::T__12);
      setState(187);
      match(gscParser::IDENTIFIER);
    }
    setState(190);
    match(gscParser::T__21);
    setState(191);
    expression();
    setState(192);
    match(gscParser::T__11);
    setState(193);
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
  enterRule(_localctx, 22, gscParser::RuleStatement_if);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(195);
    match(gscParser::T__22);
    setState(196);
    match(gscParser::T__10);
    setState(197);
    expression();
    setState(198);
    match(gscParser::T__11);
    setState(199);
    statement();
    setState(202);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 19, _ctx)) {
    case 1: {
      setState(200);
      match(gscParser::T__23);
      setState(201);
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
  enterRule(_localctx, 24, gscParser::RuleStatement_switch);
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
    setState(204);
    match(gscParser::T__24);
    setState(205);
    match(gscParser::T__10);
    setState(206);
    expression();
    setState(207);
    match(gscParser::T__11);
    setState(208);
    match(gscParser::T__17);
    setState(221); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(212);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case gscParser::T__25: {
          setState(209);
          match(gscParser::T__25);
          setState(210);
          const_expr();
          break;
        }

        case gscParser::T__26: {
          setState(211);
          match(gscParser::T__26);
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(214);
      match(gscParser::T__27);
      setState(218);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while ((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & 5950966280) != 0) || ((((_la - 69) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 69)) & 62449) != 0)) {
        setState(215);
        statement();
        setState(220);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
      setState(223); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while (_la == gscParser::T__25

    || _la == gscParser::T__26);
    setState(225);
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
  enterRule(_localctx, 26, gscParser::RuleStatement_inst);

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
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 23, _ctx)) {
    case 1: {
      setState(227);
      function_call(0);
      break;
    }

    case 2: {
      setState(228);
      operator_inst();
      break;
    }

    default:
      break;
    }
    setState(231);
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
  size_t startState = 28;
  enterRecursionRule(_localctx, 28, gscParser::RuleFunction_call, precedence);

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
    setState(251);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 26, _ctx)) {
    case 1: {
      setState(235);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == gscParser::T__28

      || _la == gscParser::T__29) {
        setState(234);
        _la = _input->LA(1);
        if (!(_la == gscParser::T__28

        || _la == gscParser::T__29)) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
      }
      setState(237);
      function_component();
      setState(238);
      match(gscParser::T__10);
      setState(239);
      expression_list();
      setState(240);
      match(gscParser::T__11);
      break;
    }

    case 2: {
      setState(242);
      expression14();
      setState(244);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == gscParser::T__28

      || _la == gscParser::T__29) {
        setState(243);
        _la = _input->LA(1);
        if (!(_la == gscParser::T__28

        || _la == gscParser::T__29)) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
      }
      setState(246);
      function_component();
      setState(247);
      match(gscParser::T__10);
      setState(248);
      expression_list();
      setState(249);
      match(gscParser::T__11);
      break;
    }

    default:
      break;
    }
    _ctx->stop = _input->LT(-1);
    setState(264);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 28, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Function_callContext>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleFunction_call);
        setState(253);

        if (!(precpred(_ctx, 1))) throw FailedPredicateException(this, "precpred(_ctx, 1)");
        setState(255);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == gscParser::T__28

        || _la == gscParser::T__29) {
          setState(254);
          _la = _input->LA(1);
          if (!(_la == gscParser::T__28

          || _la == gscParser::T__29)) {
          _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
        }
        setState(257);
        function_component();
        setState(258);
        match(gscParser::T__10);
        setState(259);
        expression_list();
        setState(260);
        match(gscParser::T__11); 
      }
      setState(266);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 28, _ctx);
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
  enterRule(_localctx, 30, gscParser::RuleFunction_component);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(282);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 30, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(269);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 29, _ctx)) {
      case 1: {
        setState(267);
        match(gscParser::IDENTIFIER);
        setState(268);
        match(gscParser::T__30);
        break;
      }

      default:
        break;
      }
      setState(271);
      match(gscParser::IDENTIFIER);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(272);
      match(gscParser::T__31);
      setState(273);
      expression();
      setState(274);
      match(gscParser::T__32);
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(276);
      match(gscParser::T__31);
      setState(277);
      expression();
      setState(278);
      match(gscParser::T__32);
      setState(279);
      match(gscParser::T__33);
      setState(280);
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
  enterRule(_localctx, 32, gscParser::RuleOperator_inst);
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
    setState(284);
    match(gscParser::IDENTIFIER);
    setState(286);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 5905877504) != 0) || ((((_la - 64) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 64)) & 1998383) != 0)) {
      setState(285);
      expression();
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
  enterRule(_localctx, 34, gscParser::RuleExpression);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(290);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 32, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(288);
      set_expression();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(289);
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
  enterRule(_localctx, 36, gscParser::RuleSet_expression);
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
    setState(292);
    left_value(0);
    setState(293);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 70334384504832) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(294);
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
  size_t startState = 38;
  enterRecursionRule(_localctx, 38, gscParser::RuleExpression1, precedence);

    

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
    setState(297);
    expression2(0);
    _ctx->stop = _input->LT(-1);
    setState(304);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 33, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression1Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression1);
        setState(299);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(300);
        match(gscParser::T__45);
        setState(301);
        expression2(0); 
      }
      setState(306);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 33, _ctx);
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
  size_t startState = 40;
  enterRecursionRule(_localctx, 40, gscParser::RuleExpression2, precedence);

    

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
    setState(308);
    expression3(0);
    _ctx->stop = _input->LT(-1);
    setState(315);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 34, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression2Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression2);
        setState(310);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(311);
        match(gscParser::T__46);
        setState(312);
        expression3(0); 
      }
      setState(317);
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
  size_t startState = 42;
  enterRecursionRule(_localctx, 42, gscParser::RuleExpression3, precedence);

    

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
    setState(319);
    expression4(0);
    _ctx->stop = _input->LT(-1);
    setState(326);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 35, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression3Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression3);
        setState(321);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(322);
        match(gscParser::T__47);
        setState(323);
        expression4(0); 
      }
      setState(328);
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
  size_t startState = 44;
  enterRecursionRule(_localctx, 44, gscParser::RuleExpression4, precedence);

    

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
    setState(330);
    expression5(0);
    _ctx->stop = _input->LT(-1);
    setState(337);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 36, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression4Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression4);
        setState(332);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(333);
        match(gscParser::T__48);
        setState(334);
        expression5(0); 
      }
      setState(339);
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
  size_t startState = 46;
  enterRecursionRule(_localctx, 46, gscParser::RuleExpression5, precedence);

    

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
    setState(341);
    expression6(0);
    _ctx->stop = _input->LT(-1);
    setState(348);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 37, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression5Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression5);
        setState(343);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(344);
        match(gscParser::T__14);
        setState(345);
        expression6(0); 
      }
      setState(350);
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
  size_t startState = 48;
  enterRecursionRule(_localctx, 48, gscParser::RuleExpression6, precedence);

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
    setState(352);
    expression7(0);
    _ctx->stop = _input->LT(-1);
    setState(359);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 38, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression6Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression6);
        setState(354);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(355);
        _la = _input->LA(1);
        if (!((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & 16888498602639360) != 0))) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(356);
        expression7(0); 
      }
      setState(361);
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
  size_t startState = 50;
  enterRecursionRule(_localctx, 50, gscParser::RuleExpression7, precedence);

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
    setState(363);
    expression8(0);
    _ctx->stop = _input->LT(-1);
    setState(370);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 39, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression7Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression7);
        setState(365);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(366);
        _la = _input->LA(1);
        if (!((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & 270215977642229760) != 0))) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(367);
        expression8(0); 
      }
      setState(372);
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
  size_t startState = 52;
  enterRecursionRule(_localctx, 52, gscParser::RuleExpression8, precedence);

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
    setState(374);
    expression9(0);
    _ctx->stop = _input->LT(-1);
    setState(381);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 40, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression8Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression8);
        setState(376);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(377);
        _la = _input->LA(1);
        if (!(_la == gscParser::T__53

        || _la == gscParser::T__54)) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(378);
        expression9(0); 
      }
      setState(383);
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
  size_t startState = 54;
  enterRecursionRule(_localctx, 54, gscParser::RuleExpression9, precedence);

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
    setState(385);
    expression10(0);
    _ctx->stop = _input->LT(-1);
    setState(392);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 41, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression9Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression9);
        setState(387);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(388);
        _la = _input->LA(1);
        if (!(_la == gscParser::T__57

        || _la == gscParser::T__58)) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(389);
        expression10(0); 
      }
      setState(394);
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
  size_t startState = 56;
  enterRecursionRule(_localctx, 56, gscParser::RuleExpression10, precedence);

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
    setState(396);
    expression11(0);
    _ctx->stop = _input->LT(-1);
    setState(403);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 42, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression10Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression10);
        setState(398);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(399);
        _la = _input->LA(1);
        if (!(_la == gscParser::T__59

        || _la == gscParser::T__60)) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(400);
        expression11(0); 
      }
      setState(405);
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
  size_t startState = 58;
  enterRecursionRule(_localctx, 58, gscParser::RuleExpression11, precedence);

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
    expression12();
    _ctx->stop = _input->LT(-1);
    setState(414);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 43, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression11Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression11);
        setState(409);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(410);
        _la = _input->LA(1);
        if (!((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & -4611686018427371520) != 0))) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(411);
        expression12(); 
      }
      setState(416);
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
  enterRule(_localctx, 60, gscParser::RuleExpression12);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(425);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 44, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(417);
      _la = _input->LA(1);
      if (!(_la == gscParser::T__63

      || _la == gscParser::T__64)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(418);
      expression13();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(419);
      _la = _input->LA(1);
      if (!(_la == gscParser::T__65

      || _la == gscParser::T__66)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(420);
      left_value(0);
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(421);
      left_value(0);
      setState(422);
      _la = _input->LA(1);
      if (!(_la == gscParser::T__65

      || _la == gscParser::T__66)) {
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
      setState(424);
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
  enterRule(_localctx, 62, gscParser::RuleExpression13);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(429);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 45, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(427);
      function_call(0);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(428);
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
  enterRule(_localctx, 64, gscParser::RuleExpression14);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(437);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 46, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(431);
      const_expr();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(432);
      match(gscParser::T__10);
      setState(433);
      expression();
      setState(434);
      match(gscParser::T__11);
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(436);
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
  size_t startState = 66;
  enterRecursionRule(_localctx, 66, gscParser::RuleLeft_value, precedence);

    

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
    setState(473);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 50, _ctx)) {
    case 1: {
      setState(440);
      match(gscParser::IDENTIFIER);
      break;
    }

    case 2: {
      setState(441);
      match(gscParser::T__31);
      setState(442);
      expression();
      setState(443);
      match(gscParser::T__32);
      setState(444);
      match(gscParser::T__33);
      setState(445);
      match(gscParser::IDENTIFIER);
      break;
    }

    case 3: {
      setState(452);
      _errHandler->sync(this);
      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 47, _ctx)) {
      case 1: {
        setState(447);
        const_expr();
        break;
      }

      case 2: {
        setState(448);
        match(gscParser::T__10);
        setState(449);
        expression();
        setState(450);
        match(gscParser::T__11);
        break;
      }

      default:
        break;
      }
      setState(454);
      match(gscParser::T__67);
      setState(460);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case gscParser::IDENTIFIER: {
          setState(455);
          match(gscParser::IDENTIFIER);
          break;
        }

        case gscParser::T__10: {
          setState(456);
          match(gscParser::T__10);
          setState(457);
          expression();
          setState(458);
          match(gscParser::T__11);
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      break;
    }

    case 4: {
      setState(467);
      _errHandler->sync(this);
      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 49, _ctx)) {
      case 1: {
        setState(462);
        const_expr();
        break;
      }

      case 2: {
        setState(463);
        match(gscParser::T__10);
        setState(464);
        expression();
        setState(465);
        match(gscParser::T__11);
        break;
      }

      default:
        break;
      }
      setState(469);
      match(gscParser::T__8);
      setState(470);
      expression();
      setState(471);
      match(gscParser::T__9);
      break;
    }

    default:
      break;
    }
    _ctx->stop = _input->LT(-1);
    setState(491);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 53, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        setState(489);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 52, _ctx)) {
        case 1: {
          _localctx = _tracker.createInstance<Left_valueContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleLeft_value);
          setState(475);

          if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
          setState(476);
          match(gscParser::T__67);
          setState(482);
          _errHandler->sync(this);
          switch (_input->LA(1)) {
            case gscParser::IDENTIFIER: {
              setState(477);
              match(gscParser::IDENTIFIER);
              break;
            }

            case gscParser::T__10: {
              setState(478);
              match(gscParser::T__10);
              setState(479);
              expression();
              setState(480);
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
          setState(484);

          if (!(precpred(_ctx, 1))) throw FailedPredicateException(this, "precpred(_ctx, 1)");
          setState(485);
          match(gscParser::T__8);
          setState(486);
          expression();
          setState(487);
          match(gscParser::T__9);
          break;
        }

        default:
          break;
        } 
      }
      setState(493);
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
  enterRule(_localctx, 68, gscParser::RuleConst_expr);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(504);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case gscParser::T__10: {
        enterOuterAlt(_localctx, 1);
        setState(494);
        vector_value();
        break;
      }

      case gscParser::T__8: {
        enterOuterAlt(_localctx, 2);
        setState(495);
        array_def();
        break;
      }

      case gscParser::T__17: {
        enterOuterAlt(_localctx, 3);
        setState(496);
        struct_def();
        break;
      }

      case gscParser::INTEGER10:
      case gscParser::INTEGER16:
      case gscParser::INTEGER8:
      case gscParser::INTEGER2: {
        enterOuterAlt(_localctx, 4);
        setState(497);
        number();
        break;
      }

      case gscParser::BOOL_VALUE: {
        enterOuterAlt(_localctx, 5);
        setState(498);
        match(gscParser::BOOL_VALUE);
        break;
      }

      case gscParser::FLOATVAL: {
        enterOuterAlt(_localctx, 6);
        setState(499);
        match(gscParser::FLOATVAL);
        break;
      }

      case gscParser::STRING: {
        enterOuterAlt(_localctx, 7);
        setState(500);
        match(gscParser::STRING);
        break;
      }

      case gscParser::HASHSTRING: {
        enterOuterAlt(_localctx, 8);
        setState(501);
        match(gscParser::HASHSTRING);
        break;
      }

      case gscParser::UNDEFINED_VALUE: {
        enterOuterAlt(_localctx, 9);
        setState(502);
        match(gscParser::UNDEFINED_VALUE);
        break;
      }

      case gscParser::T__14:
      case gscParser::T__68: {
        enterOuterAlt(_localctx, 10);
        setState(503);
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
  enterRule(_localctx, 70, gscParser::RuleFunction_ref);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(519);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case gscParser::T__14: {
        enterOuterAlt(_localctx, 1);
        setState(506);
        match(gscParser::T__14);
        setState(509);
        _errHandler->sync(this);

        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 55, _ctx)) {
        case 1: {
          setState(507);
          match(gscParser::IDENTIFIER);
          setState(508);
          match(gscParser::T__30);
          break;
        }

        default:
          break;
        }
        setState(511);
        match(gscParser::IDENTIFIER);
        break;
      }

      case gscParser::T__68: {
        enterOuterAlt(_localctx, 2);
        setState(512);
        match(gscParser::T__68);
        setState(513);
        match(gscParser::IDENTIFIER);
        setState(514);
        match(gscParser::T__53);
        setState(515);
        match(gscParser::PATH);
        setState(516);
        match(gscParser::T__55);
        setState(517);
        match(gscParser::T__30);
        setState(518);
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
  enterRule(_localctx, 72, gscParser::RuleNumber);
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
    setState(521);
    _la = _input->LA(1);
    if (!(((((_la - 73) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 73)) & 15) != 0))) {
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
  enterRule(_localctx, 74, gscParser::RuleVector_value);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(523);
    match(gscParser::T__10);
    setState(524);
    expression();
    setState(525);
    match(gscParser::T__12);
    setState(526);
    expression();
    setState(527);
    match(gscParser::T__12);
    setState(528);
    expression();
    setState(529);
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
  enterRule(_localctx, 76, gscParser::RuleArray_def);
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
    setState(531);
    match(gscParser::T__8);
    setState(546);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 5905877504) != 0) || ((((_la - 64) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 64)) & 1998447) != 0)) {
      setState(538);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 58, _ctx)) {
      case 1: {
        setState(535);
        _errHandler->sync(this);
        switch (_input->LA(1)) {
          case gscParser::HASHSTRING: {
            setState(532);
            match(gscParser::HASHSTRING);
            break;
          }

          case gscParser::T__69: {
            setState(533);
            match(gscParser::T__69);
            setState(534);
            number();
            break;
          }

        default:
          throw NoViableAltException(this);
        }
        setState(537);
        match(gscParser::T__27);
        break;
      }

      default:
        break;
      }
      setState(540);
      expression();
      setState(542);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == gscParser::T__12) {
        setState(541);
        match(gscParser::T__12);
      }
      setState(548);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(549);
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
  enterRule(_localctx, 78, gscParser::RuleStruct_def);
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
    setState(551);
    match(gscParser::T__17);
    setState(560);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == gscParser::STRUCT_IDENTIFIER) {
      setState(552);
      match(gscParser::STRUCT_IDENTIFIER);
      setState(553);
      match(gscParser::T__27);
      setState(554);
      expression();
      setState(556);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == gscParser::T__12) {
        setState(555);
        match(gscParser::T__12);
      }
      setState(562);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(563);
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
    case 14: return function_callSempred(antlrcpp::downCast<Function_callContext *>(context), predicateIndex);
    case 19: return expression1Sempred(antlrcpp::downCast<Expression1Context *>(context), predicateIndex);
    case 20: return expression2Sempred(antlrcpp::downCast<Expression2Context *>(context), predicateIndex);
    case 21: return expression3Sempred(antlrcpp::downCast<Expression3Context *>(context), predicateIndex);
    case 22: return expression4Sempred(antlrcpp::downCast<Expression4Context *>(context), predicateIndex);
    case 23: return expression5Sempred(antlrcpp::downCast<Expression5Context *>(context), predicateIndex);
    case 24: return expression6Sempred(antlrcpp::downCast<Expression6Context *>(context), predicateIndex);
    case 25: return expression7Sempred(antlrcpp::downCast<Expression7Context *>(context), predicateIndex);
    case 26: return expression8Sempred(antlrcpp::downCast<Expression8Context *>(context), predicateIndex);
    case 27: return expression9Sempred(antlrcpp::downCast<Expression9Context *>(context), predicateIndex);
    case 28: return expression10Sempred(antlrcpp::downCast<Expression10Context *>(context), predicateIndex);
    case 29: return expression11Sempred(antlrcpp::downCast<Expression11Context *>(context), predicateIndex);
    case 33: return left_valueSempred(antlrcpp::downCast<Left_valueContext *>(context), predicateIndex);

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
