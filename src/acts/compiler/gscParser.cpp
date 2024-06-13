
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
      "prog", "include", "namespace", "filenamespace", "function", "detour_info", 
      "param_list", "param_val", "expression_list", "statement_block", "statement", 
      "statement_dev_block", "statement_for", "statement_while", "statement_dowhile", 
      "statement_foreach", "statement_if", "statement_switch", "statement_inst", 
      "nop_def", "devop_def", "function_component", "operator_inst", "expression", 
      "set_expression", "expression1", "expression2", "expression3", "expression4", 
      "expression5", "expression6", "expression7", "expression8", "expression9", 
      "expression10", "expression11", "expression12", "expression13", "expression14", 
      "expression15", "function_call_exp", "function_call", "left_value", 
      "const_expr", "function_ref", "number", "vector_value", "array_def", 
      "struct_def", "idf"
    },
    std::vector<std::string>{
      "", "'/#'", "'#/'", "'#include'", "'#using'", "';'", "'#namespace'", 
      "'#file'", "'function'", "'private'", "'autoexec'", "'('", "')'", 
      "'event_handler'", "'['", "']'", "'detour'", "'<'", "'>'", "'::'", 
      "','", "'*'", "'&'", "'='", "'...'", "'{'", "'}'", "':'", "'for'", 
      "'while'", "'do'", "'foreach'", "'in'", "'if'", "'else'", "'switch'", 
      "'case'", "'default'", "'nop'", "'Nop'", "'DevOp'", "'devop'", "'Devop'", 
      "'->'", "'\\u003F'", "'+='", "'-='", "'/='", "'*='", "'%='", "'&='", 
      "'|='", "'^='", "'<<='", "'>>='", "'~='", "'||'", "'&&'", "'|'", "'^'", 
      "'!='", "'=='", "'!=='", "'==='", "'<='", "'>='", "'<<'", "'>>'", 
      "'+'", "'-'", "'/'", "'%'", "'!'", "'~'", "'++'", "'--'", "'thread'", 
      "'childthread'", "'threadendon'", "'builtin'", "'.'", "'@'", "", "", 
      "", "", "", "", "", "", "", "'undefined'"
    },
    std::vector<std::string>{
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "NEWLINE", 
      "WHITESPACE", "INTEGER10", "INTEGER16", "INTEGER8", "INTEGER2", "FLOATVAL", 
      "BUILTIN", "BOOL_VALUE", "UNDEFINED_VALUE", "IDENTIFIER", "STRUCT_IDENTIFIER", 
      "PATH", "STRING", "HASHSTRING"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,96,696,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,7,
  	14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,7,
  	21,2,22,7,22,2,23,7,23,2,24,7,24,2,25,7,25,2,26,7,26,2,27,7,27,2,28,7,
  	28,2,29,7,29,2,30,7,30,2,31,7,31,2,32,7,32,2,33,7,33,2,34,7,34,2,35,7,
  	35,2,36,7,36,2,37,7,37,2,38,7,38,2,39,7,39,2,40,7,40,2,41,7,41,2,42,7,
  	42,2,43,7,43,2,44,7,44,2,45,7,45,2,46,7,46,2,47,7,47,2,48,7,48,2,49,7,
  	49,1,0,1,0,1,0,1,0,1,0,1,0,5,0,107,8,0,10,0,12,0,110,9,0,1,0,1,0,1,1,
  	1,1,1,1,1,1,1,2,1,2,1,2,1,2,1,3,1,3,1,3,1,3,1,4,3,4,127,8,4,1,4,3,4,130,
  	8,4,1,4,1,4,1,4,1,4,1,4,3,4,137,8,4,3,4,139,8,4,1,4,1,4,1,4,1,4,3,4,145,
  	8,4,1,4,3,4,148,8,4,1,4,3,4,151,8,4,1,4,1,4,1,4,1,4,1,4,1,5,1,5,1,5,1,
  	5,1,5,1,5,1,5,3,5,165,8,5,1,6,1,6,1,6,5,6,170,8,6,10,6,12,6,173,9,6,3,
  	6,175,8,6,1,7,3,7,178,8,7,1,7,1,7,1,7,3,7,183,8,7,1,7,3,7,186,8,7,1,8,
  	1,8,1,8,5,8,191,8,8,10,8,12,8,194,9,8,3,8,196,8,8,1,9,1,9,5,9,200,8,9,
  	10,9,12,9,203,9,9,1,9,1,9,1,10,1,10,3,10,209,8,10,1,10,1,10,1,10,1,10,
  	1,10,1,10,1,10,1,10,3,10,219,8,10,1,11,1,11,5,11,223,8,11,10,11,12,11,
  	226,9,11,1,11,1,11,1,12,1,12,1,12,3,12,233,8,12,1,12,1,12,3,12,237,8,
  	12,1,12,1,12,3,12,241,8,12,1,12,1,12,1,12,1,13,1,13,1,13,1,13,1,13,1,
  	13,1,14,1,14,1,14,1,14,1,14,1,14,1,14,1,15,1,15,1,15,1,15,1,15,3,15,264,
  	8,15,1,15,1,15,1,15,1,15,1,15,1,16,1,16,1,16,1,16,1,16,1,16,1,16,3,16,
  	278,8,16,1,17,1,17,1,17,1,17,1,17,1,17,1,17,1,17,3,17,288,8,17,1,17,1,
  	17,5,17,292,8,17,10,17,12,17,295,9,17,4,17,297,8,17,11,17,12,17,298,1,
  	17,1,17,1,18,1,18,1,18,1,18,1,18,1,18,3,18,309,8,18,1,18,1,18,1,19,1,
  	19,1,19,1,19,1,19,3,19,318,8,19,1,20,1,20,1,20,1,20,1,20,1,21,1,21,3,
  	21,327,8,21,1,21,1,21,1,21,1,21,1,21,1,21,1,21,1,21,1,21,1,21,1,21,1,
  	21,1,21,1,21,1,21,3,21,344,8,21,1,22,1,22,1,22,3,22,349,8,22,1,23,1,23,
  	1,23,1,23,1,23,1,23,1,23,1,23,3,23,359,8,23,1,24,1,24,1,24,1,24,1,25,
  	1,25,1,25,1,25,1,25,1,25,5,25,371,8,25,10,25,12,25,374,9,25,1,26,1,26,
  	1,26,1,26,1,26,1,26,5,26,382,8,26,10,26,12,26,385,9,26,1,27,1,27,1,27,
  	1,27,1,27,1,27,5,27,393,8,27,10,27,12,27,396,9,27,1,28,1,28,1,28,1,28,
  	1,28,1,28,5,28,404,8,28,10,28,12,28,407,9,28,1,29,1,29,1,29,1,29,1,29,
  	1,29,5,29,415,8,29,10,29,12,29,418,9,29,1,30,1,30,1,30,1,30,1,30,1,30,
  	5,30,426,8,30,10,30,12,30,429,9,30,1,31,1,31,1,31,1,31,1,31,1,31,5,31,
  	437,8,31,10,31,12,31,440,9,31,1,32,1,32,1,32,1,32,1,32,1,32,5,32,448,
  	8,32,10,32,12,32,451,9,32,1,33,1,33,1,33,1,33,1,33,1,33,5,33,459,8,33,
  	10,33,12,33,462,9,33,1,34,1,34,1,34,1,34,1,34,1,34,5,34,470,8,34,10,34,
  	12,34,473,9,34,1,35,1,35,1,35,1,35,1,35,1,35,5,35,481,8,35,10,35,12,35,
  	484,9,35,1,36,1,36,1,36,1,36,1,36,1,36,1,36,1,36,3,36,494,8,36,1,37,1,
  	37,3,37,498,8,37,1,38,1,38,1,38,3,38,503,8,38,1,39,1,39,1,39,1,39,1,40,
  	1,40,1,40,3,40,512,8,40,1,40,1,40,1,40,1,40,1,40,3,40,519,8,40,1,41,1,
  	41,3,41,523,8,41,1,41,1,41,1,41,1,41,1,41,1,41,1,41,3,41,532,8,41,1,41,
  	3,41,535,8,41,1,41,1,41,1,41,1,41,1,41,3,41,542,8,41,1,41,1,41,3,41,546,
  	8,41,1,41,1,41,1,41,1,41,1,41,5,41,553,8,41,10,41,12,41,556,9,41,1,42,
  	1,42,1,42,1,42,1,42,3,42,563,8,42,1,42,1,42,1,42,1,42,1,42,1,42,3,42,
  	571,8,42,1,42,1,42,1,42,3,42,576,8,42,1,42,1,42,1,42,1,42,3,42,582,8,
  	42,1,42,1,42,1,42,1,42,1,42,1,42,1,42,3,42,591,8,42,1,42,1,42,1,42,1,
  	42,1,42,5,42,598,8,42,10,42,12,42,601,9,42,1,43,1,43,1,43,1,43,1,43,1,
  	43,1,43,1,43,1,43,1,43,3,43,613,8,43,1,44,1,44,1,44,3,44,618,8,44,1,44,
  	1,44,1,44,1,44,1,44,1,44,1,44,1,44,1,44,1,44,3,44,630,8,44,1,45,1,45,
  	1,46,1,46,1,46,1,46,1,46,1,46,1,46,1,46,1,47,1,47,1,47,1,47,3,47,646,
  	8,47,1,47,1,47,1,47,1,47,1,47,3,47,653,8,47,1,47,5,47,656,8,47,10,47,
  	12,47,659,9,47,1,47,3,47,662,8,47,3,47,664,8,47,1,47,1,47,1,48,1,48,1,
  	48,3,48,671,8,48,1,48,1,48,1,48,1,48,1,48,3,48,678,8,48,1,48,1,48,5,48,
  	682,8,48,10,48,12,48,685,9,48,1,48,3,48,688,8,48,3,48,690,8,48,1,48,1,
  	48,1,49,1,49,1,49,0,13,50,52,54,56,58,60,62,64,66,68,70,82,84,50,0,2,
  	4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,38,40,42,44,46,48,50,
  	52,54,56,58,60,62,64,66,68,70,72,74,76,78,80,82,84,86,88,90,92,94,96,
  	98,0,16,1,0,3,4,2,0,92,92,94,94,1,0,21,22,1,0,38,39,1,0,40,42,2,0,23,
  	23,45,55,1,0,60,63,2,0,17,18,64,65,2,0,17,17,64,64,1,0,66,67,1,0,68,69,
  	2,0,21,21,70,71,1,0,72,73,1,0,74,75,1,0,76,79,1,0,84,87,754,0,108,1,0,
  	0,0,2,113,1,0,0,0,4,117,1,0,0,0,6,121,1,0,0,0,8,126,1,0,0,0,10,157,1,
  	0,0,0,12,174,1,0,0,0,14,185,1,0,0,0,16,195,1,0,0,0,18,197,1,0,0,0,20,
  	208,1,0,0,0,22,220,1,0,0,0,24,229,1,0,0,0,26,245,1,0,0,0,28,251,1,0,0,
  	0,30,258,1,0,0,0,32,270,1,0,0,0,34,279,1,0,0,0,36,308,1,0,0,0,38,312,
  	1,0,0,0,40,319,1,0,0,0,42,343,1,0,0,0,44,345,1,0,0,0,46,358,1,0,0,0,48,
  	360,1,0,0,0,50,364,1,0,0,0,52,375,1,0,0,0,54,386,1,0,0,0,56,397,1,0,0,
  	0,58,408,1,0,0,0,60,419,1,0,0,0,62,430,1,0,0,0,64,441,1,0,0,0,66,452,
  	1,0,0,0,68,463,1,0,0,0,70,474,1,0,0,0,72,493,1,0,0,0,74,497,1,0,0,0,76,
  	502,1,0,0,0,78,504,1,0,0,0,80,518,1,0,0,0,82,541,1,0,0,0,84,581,1,0,0,
  	0,86,612,1,0,0,0,88,629,1,0,0,0,90,631,1,0,0,0,92,633,1,0,0,0,94,641,
  	1,0,0,0,96,667,1,0,0,0,98,693,1,0,0,0,100,107,5,1,0,0,101,107,5,2,0,0,
  	102,107,3,8,4,0,103,107,3,2,1,0,104,107,3,4,2,0,105,107,3,6,3,0,106,100,
  	1,0,0,0,106,101,1,0,0,0,106,102,1,0,0,0,106,103,1,0,0,0,106,104,1,0,0,
  	0,106,105,1,0,0,0,107,110,1,0,0,0,108,106,1,0,0,0,108,109,1,0,0,0,109,
  	111,1,0,0,0,110,108,1,0,0,0,111,112,5,0,0,1,112,1,1,0,0,0,113,114,7,0,
  	0,0,114,115,7,1,0,0,115,116,5,5,0,0,116,3,1,0,0,0,117,118,5,6,0,0,118,
  	119,5,92,0,0,119,120,5,5,0,0,120,5,1,0,0,0,121,122,5,7,0,0,122,123,7,
  	1,0,0,123,124,5,5,0,0,124,7,1,0,0,0,125,127,5,8,0,0,126,125,1,0,0,0,126,
  	127,1,0,0,0,127,129,1,0,0,0,128,130,5,9,0,0,129,128,1,0,0,0,129,130,1,
  	0,0,0,130,138,1,0,0,0,131,136,5,10,0,0,132,133,5,11,0,0,133,134,3,90,
  	45,0,134,135,5,12,0,0,135,137,1,0,0,0,136,132,1,0,0,0,136,137,1,0,0,0,
  	137,139,1,0,0,0,138,131,1,0,0,0,138,139,1,0,0,0,139,144,1,0,0,0,140,141,
  	5,13,0,0,141,142,5,14,0,0,142,143,5,92,0,0,143,145,5,15,0,0,144,140,1,
  	0,0,0,144,145,1,0,0,0,145,147,1,0,0,0,146,148,3,10,5,0,147,146,1,0,0,
  	0,147,148,1,0,0,0,148,150,1,0,0,0,149,151,5,92,0,0,150,149,1,0,0,0,150,
  	151,1,0,0,0,151,152,1,0,0,0,152,153,5,11,0,0,153,154,3,12,6,0,154,155,
  	5,12,0,0,155,156,3,18,9,0,156,9,1,0,0,0,157,158,5,16,0,0,158,164,5,92,
  	0,0,159,160,5,17,0,0,160,161,5,94,0,0,161,162,5,18,0,0,162,163,5,19,0,
  	0,163,165,5,92,0,0,164,159,1,0,0,0,164,165,1,0,0,0,165,11,1,0,0,0,166,
  	171,3,14,7,0,167,168,5,20,0,0,168,170,3,14,7,0,169,167,1,0,0,0,170,173,
  	1,0,0,0,171,169,1,0,0,0,171,172,1,0,0,0,172,175,1,0,0,0,173,171,1,0,0,
  	0,174,166,1,0,0,0,174,175,1,0,0,0,175,13,1,0,0,0,176,178,7,2,0,0,177,
  	176,1,0,0,0,177,178,1,0,0,0,178,179,1,0,0,0,179,182,5,92,0,0,180,181,
  	5,23,0,0,181,183,3,46,23,0,182,180,1,0,0,0,182,183,1,0,0,0,183,186,1,
  	0,0,0,184,186,5,24,0,0,185,177,1,0,0,0,185,184,1,0,0,0,186,15,1,0,0,0,
  	187,192,3,46,23,0,188,189,5,20,0,0,189,191,3,46,23,0,190,188,1,0,0,0,
  	191,194,1,0,0,0,192,190,1,0,0,0,192,193,1,0,0,0,193,196,1,0,0,0,194,192,
  	1,0,0,0,195,187,1,0,0,0,195,196,1,0,0,0,196,17,1,0,0,0,197,201,5,25,0,
  	0,198,200,3,20,10,0,199,198,1,0,0,0,200,203,1,0,0,0,201,199,1,0,0,0,201,
  	202,1,0,0,0,202,204,1,0,0,0,203,201,1,0,0,0,204,205,5,26,0,0,205,19,1,
  	0,0,0,206,207,5,92,0,0,207,209,5,27,0,0,208,206,1,0,0,0,208,209,1,0,0,
  	0,209,218,1,0,0,0,210,219,3,18,9,0,211,219,3,22,11,0,212,219,3,24,12,
  	0,213,219,3,32,16,0,214,219,3,26,13,0,215,219,3,30,15,0,216,219,3,36,
  	18,0,217,219,3,34,17,0,218,210,1,0,0,0,218,211,1,0,0,0,218,212,1,0,0,
  	0,218,213,1,0,0,0,218,214,1,0,0,0,218,215,1,0,0,0,218,216,1,0,0,0,218,
  	217,1,0,0,0,219,21,1,0,0,0,220,224,5,1,0,0,221,223,3,20,10,0,222,221,
  	1,0,0,0,223,226,1,0,0,0,224,222,1,0,0,0,224,225,1,0,0,0,225,227,1,0,0,
  	0,226,224,1,0,0,0,227,228,5,2,0,0,228,23,1,0,0,0,229,230,5,28,0,0,230,
  	232,5,11,0,0,231,233,3,46,23,0,232,231,1,0,0,0,232,233,1,0,0,0,233,234,
  	1,0,0,0,234,236,5,5,0,0,235,237,3,46,23,0,236,235,1,0,0,0,236,237,1,0,
  	0,0,237,238,1,0,0,0,238,240,5,5,0,0,239,241,3,46,23,0,240,239,1,0,0,0,
  	240,241,1,0,0,0,241,242,1,0,0,0,242,243,5,12,0,0,243,244,3,20,10,0,244,
  	25,1,0,0,0,245,246,5,29,0,0,246,247,5,11,0,0,247,248,3,46,23,0,248,249,
  	5,12,0,0,249,250,3,20,10,0,250,27,1,0,0,0,251,252,5,30,0,0,252,253,3,
  	20,10,0,253,254,5,29,0,0,254,255,5,11,0,0,255,256,3,46,23,0,256,257,5,
  	12,0,0,257,29,1,0,0,0,258,259,5,31,0,0,259,260,5,11,0,0,260,263,5,92,
  	0,0,261,262,5,20,0,0,262,264,5,92,0,0,263,261,1,0,0,0,263,264,1,0,0,0,
  	264,265,1,0,0,0,265,266,5,32,0,0,266,267,3,46,23,0,267,268,5,12,0,0,268,
  	269,3,20,10,0,269,31,1,0,0,0,270,271,5,33,0,0,271,272,5,11,0,0,272,273,
  	3,46,23,0,273,274,5,12,0,0,274,277,3,20,10,0,275,276,5,34,0,0,276,278,
  	3,20,10,0,277,275,1,0,0,0,277,278,1,0,0,0,278,33,1,0,0,0,279,280,5,35,
  	0,0,280,281,5,11,0,0,281,282,3,46,23,0,282,283,5,12,0,0,283,296,5,25,
  	0,0,284,285,5,36,0,0,285,288,3,86,43,0,286,288,5,37,0,0,287,284,1,0,0,
  	0,287,286,1,0,0,0,288,289,1,0,0,0,289,293,5,27,0,0,290,292,3,20,10,0,
  	291,290,1,0,0,0,292,295,1,0,0,0,293,291,1,0,0,0,293,294,1,0,0,0,294,297,
  	1,0,0,0,295,293,1,0,0,0,296,287,1,0,0,0,297,298,1,0,0,0,298,296,1,0,0,
  	0,298,299,1,0,0,0,299,300,1,0,0,0,300,301,5,26,0,0,301,35,1,0,0,0,302,
  	309,3,46,23,0,303,309,3,44,22,0,304,309,3,28,14,0,305,309,3,80,40,0,306,
  	309,3,38,19,0,307,309,3,40,20,0,308,302,1,0,0,0,308,303,1,0,0,0,308,304,
  	1,0,0,0,308,305,1,0,0,0,308,306,1,0,0,0,308,307,1,0,0,0,309,310,1,0,0,
  	0,310,311,5,5,0,0,311,37,1,0,0,0,312,317,7,3,0,0,313,314,5,11,0,0,314,
  	315,3,90,45,0,315,316,5,12,0,0,316,318,1,0,0,0,317,313,1,0,0,0,317,318,
  	1,0,0,0,318,39,1,0,0,0,319,320,7,4,0,0,320,321,5,11,0,0,321,322,3,90,
  	45,0,322,323,5,12,0,0,323,41,1,0,0,0,324,325,5,92,0,0,325,327,5,19,0,
  	0,326,324,1,0,0,0,326,327,1,0,0,0,327,328,1,0,0,0,328,344,5,92,0,0,329,
  	330,5,14,0,0,330,331,5,14,0,0,331,332,3,46,23,0,332,333,5,15,0,0,333,
  	334,5,15,0,0,334,344,1,0,0,0,335,336,5,14,0,0,336,337,5,14,0,0,337,338,
  	3,46,23,0,338,339,5,15,0,0,339,340,5,15,0,0,340,341,5,43,0,0,341,342,
  	5,92,0,0,342,344,1,0,0,0,343,326,1,0,0,0,343,329,1,0,0,0,343,335,1,0,
  	0,0,344,43,1,0,0,0,345,348,5,89,0,0,346,349,5,92,0,0,347,349,3,46,23,
  	0,348,346,1,0,0,0,348,347,1,0,0,0,348,349,1,0,0,0,349,45,1,0,0,0,350,
  	359,3,48,24,0,351,352,3,50,25,0,352,353,5,44,0,0,353,354,3,46,23,0,354,
  	355,5,27,0,0,355,356,3,46,23,0,356,359,1,0,0,0,357,359,3,50,25,0,358,
  	350,1,0,0,0,358,351,1,0,0,0,358,357,1,0,0,0,359,47,1,0,0,0,360,361,3,
  	84,42,0,361,362,7,5,0,0,362,363,3,46,23,0,363,49,1,0,0,0,364,365,6,25,
  	-1,0,365,366,3,52,26,0,366,372,1,0,0,0,367,368,10,2,0,0,368,369,5,56,
  	0,0,369,371,3,52,26,0,370,367,1,0,0,0,371,374,1,0,0,0,372,370,1,0,0,0,
  	372,373,1,0,0,0,373,51,1,0,0,0,374,372,1,0,0,0,375,376,6,26,-1,0,376,
  	377,3,54,27,0,377,383,1,0,0,0,378,379,10,2,0,0,379,380,5,57,0,0,380,382,
  	3,54,27,0,381,378,1,0,0,0,382,385,1,0,0,0,383,381,1,0,0,0,383,384,1,0,
  	0,0,384,53,1,0,0,0,385,383,1,0,0,0,386,387,6,27,-1,0,387,388,3,56,28,
  	0,388,394,1,0,0,0,389,390,10,2,0,0,390,391,5,58,0,0,391,393,3,56,28,0,
  	392,389,1,0,0,0,393,396,1,0,0,0,394,392,1,0,0,0,394,395,1,0,0,0,395,55,
  	1,0,0,0,396,394,1,0,0,0,397,398,6,28,-1,0,398,399,3,58,29,0,399,405,1,
  	0,0,0,400,401,10,2,0,0,401,402,5,59,0,0,402,404,3,58,29,0,403,400,1,0,
  	0,0,404,407,1,0,0,0,405,403,1,0,0,0,405,406,1,0,0,0,406,57,1,0,0,0,407,
  	405,1,0,0,0,408,409,6,29,-1,0,409,410,3,60,30,0,410,416,1,0,0,0,411,412,
  	10,2,0,0,412,413,5,22,0,0,413,415,3,60,30,0,414,411,1,0,0,0,415,418,1,
  	0,0,0,416,414,1,0,0,0,416,417,1,0,0,0,417,59,1,0,0,0,418,416,1,0,0,0,
  	419,420,6,30,-1,0,420,421,3,62,31,0,421,427,1,0,0,0,422,423,10,2,0,0,
  	423,424,7,6,0,0,424,426,3,62,31,0,425,422,1,0,0,0,426,429,1,0,0,0,427,
  	425,1,0,0,0,427,428,1,0,0,0,428,61,1,0,0,0,429,427,1,0,0,0,430,431,6,
  	31,-1,0,431,432,3,64,32,0,432,438,1,0,0,0,433,434,10,2,0,0,434,435,7,
  	7,0,0,435,437,3,64,32,0,436,433,1,0,0,0,437,440,1,0,0,0,438,436,1,0,0,
  	0,438,439,1,0,0,0,439,63,1,0,0,0,440,438,1,0,0,0,441,442,6,32,-1,0,442,
  	443,3,66,33,0,443,449,1,0,0,0,444,445,10,2,0,0,445,446,7,8,0,0,446,448,
  	3,66,33,0,447,444,1,0,0,0,448,451,1,0,0,0,449,447,1,0,0,0,449,450,1,0,
  	0,0,450,65,1,0,0,0,451,449,1,0,0,0,452,453,6,33,-1,0,453,454,3,68,34,
  	0,454,460,1,0,0,0,455,456,10,2,0,0,456,457,7,9,0,0,457,459,3,68,34,0,
  	458,455,1,0,0,0,459,462,1,0,0,0,460,458,1,0,0,0,460,461,1,0,0,0,461,67,
  	1,0,0,0,462,460,1,0,0,0,463,464,6,34,-1,0,464,465,3,70,35,0,465,471,1,
  	0,0,0,466,467,10,2,0,0,467,468,7,10,0,0,468,470,3,70,35,0,469,466,1,0,
  	0,0,470,473,1,0,0,0,471,469,1,0,0,0,471,472,1,0,0,0,472,69,1,0,0,0,473,
  	471,1,0,0,0,474,475,6,35,-1,0,475,476,3,72,36,0,476,482,1,0,0,0,477,478,
  	10,2,0,0,478,479,7,11,0,0,479,481,3,72,36,0,480,477,1,0,0,0,481,484,1,
  	0,0,0,482,480,1,0,0,0,482,483,1,0,0,0,483,71,1,0,0,0,484,482,1,0,0,0,
  	485,486,7,12,0,0,486,494,3,74,37,0,487,488,7,13,0,0,488,494,3,84,42,0,
  	489,490,3,84,42,0,490,491,7,13,0,0,491,494,1,0,0,0,492,494,3,74,37,0,
  	493,485,1,0,0,0,493,487,1,0,0,0,493,489,1,0,0,0,493,492,1,0,0,0,494,73,
  	1,0,0,0,495,498,3,80,40,0,496,498,3,76,38,0,497,495,1,0,0,0,497,496,1,
  	0,0,0,498,75,1,0,0,0,499,503,3,86,43,0,500,503,3,78,39,0,501,503,3,84,
  	42,0,502,499,1,0,0,0,502,500,1,0,0,0,502,501,1,0,0,0,503,77,1,0,0,0,504,
  	505,5,11,0,0,505,506,3,46,23,0,506,507,5,12,0,0,507,79,1,0,0,0,508,519,
  	3,82,41,0,509,511,3,76,38,0,510,512,7,14,0,0,511,510,1,0,0,0,511,512,
  	1,0,0,0,512,513,1,0,0,0,513,514,3,42,21,0,514,515,5,11,0,0,515,516,3,
  	16,8,0,516,517,5,12,0,0,517,519,1,0,0,0,518,508,1,0,0,0,518,509,1,0,0,
  	0,519,81,1,0,0,0,520,522,6,41,-1,0,521,523,7,14,0,0,522,521,1,0,0,0,522,
  	523,1,0,0,0,523,524,1,0,0,0,524,525,3,42,21,0,525,526,5,11,0,0,526,527,
  	3,16,8,0,527,528,5,12,0,0,528,542,1,0,0,0,529,532,3,86,43,0,530,532,3,
  	78,39,0,531,529,1,0,0,0,531,530,1,0,0,0,532,534,1,0,0,0,533,535,7,14,
  	0,0,534,533,1,0,0,0,534,535,1,0,0,0,535,536,1,0,0,0,536,537,3,42,21,0,
  	537,538,5,11,0,0,538,539,3,16,8,0,539,540,5,12,0,0,540,542,1,0,0,0,541,
  	520,1,0,0,0,541,531,1,0,0,0,542,554,1,0,0,0,543,545,10,1,0,0,544,546,
  	7,14,0,0,545,544,1,0,0,0,545,546,1,0,0,0,546,547,1,0,0,0,547,548,3,42,
  	21,0,548,549,5,11,0,0,549,550,3,16,8,0,550,551,5,12,0,0,551,553,1,0,0,
  	0,552,543,1,0,0,0,553,556,1,0,0,0,554,552,1,0,0,0,554,555,1,0,0,0,555,
  	83,1,0,0,0,556,554,1,0,0,0,557,558,6,42,-1,0,558,582,3,98,49,0,559,563,
  	3,82,41,0,560,563,3,86,43,0,561,563,3,78,39,0,562,559,1,0,0,0,562,560,
  	1,0,0,0,562,561,1,0,0,0,563,564,1,0,0,0,564,570,5,80,0,0,565,571,3,98,
  	49,0,566,567,5,11,0,0,567,568,3,46,23,0,568,569,5,12,0,0,569,571,1,0,
  	0,0,570,565,1,0,0,0,570,566,1,0,0,0,571,582,1,0,0,0,572,576,3,82,41,0,
  	573,576,3,86,43,0,574,576,3,78,39,0,575,572,1,0,0,0,575,573,1,0,0,0,575,
  	574,1,0,0,0,576,577,1,0,0,0,577,578,5,14,0,0,578,579,3,46,23,0,579,580,
  	5,15,0,0,580,582,1,0,0,0,581,557,1,0,0,0,581,562,1,0,0,0,581,575,1,0,
  	0,0,582,599,1,0,0,0,583,584,10,4,0,0,584,590,5,80,0,0,585,591,3,98,49,
  	0,586,587,5,11,0,0,587,588,3,46,23,0,588,589,5,12,0,0,589,591,1,0,0,0,
  	590,585,1,0,0,0,590,586,1,0,0,0,591,598,1,0,0,0,592,593,10,3,0,0,593,
  	594,5,14,0,0,594,595,3,46,23,0,595,596,5,15,0,0,596,598,1,0,0,0,597,583,
  	1,0,0,0,597,592,1,0,0,0,598,601,1,0,0,0,599,597,1,0,0,0,599,600,1,0,0,
  	0,600,85,1,0,0,0,601,599,1,0,0,0,602,613,3,92,46,0,603,613,3,94,47,0,
  	604,613,3,96,48,0,605,613,3,90,45,0,606,613,5,90,0,0,607,613,5,88,0,0,
  	608,613,5,95,0,0,609,613,5,96,0,0,610,613,5,91,0,0,611,613,3,88,44,0,
  	612,602,1,0,0,0,612,603,1,0,0,0,612,604,1,0,0,0,612,605,1,0,0,0,612,606,
  	1,0,0,0,612,607,1,0,0,0,612,608,1,0,0,0,612,609,1,0,0,0,612,610,1,0,0,
  	0,612,611,1,0,0,0,613,87,1,0,0,0,614,617,5,22,0,0,615,616,5,92,0,0,616,
  	618,5,19,0,0,617,615,1,0,0,0,617,618,1,0,0,0,618,619,1,0,0,0,619,630,
  	5,92,0,0,620,621,5,81,0,0,621,622,5,92,0,0,622,623,5,17,0,0,623,624,5,
  	94,0,0,624,625,5,18,0,0,625,626,5,19,0,0,626,630,5,92,0,0,627,628,5,22,
  	0,0,628,630,3,84,42,0,629,614,1,0,0,0,629,620,1,0,0,0,629,627,1,0,0,0,
  	630,89,1,0,0,0,631,632,7,15,0,0,632,91,1,0,0,0,633,634,5,11,0,0,634,635,
  	3,46,23,0,635,636,5,20,0,0,636,637,3,46,23,0,637,638,5,20,0,0,638,639,
  	3,46,23,0,639,640,5,12,0,0,640,93,1,0,0,0,641,663,5,14,0,0,642,643,3,
  	46,23,0,643,644,5,27,0,0,644,646,1,0,0,0,645,642,1,0,0,0,645,646,1,0,
  	0,0,646,647,1,0,0,0,647,657,3,46,23,0,648,652,5,20,0,0,649,650,3,46,23,
  	0,650,651,5,27,0,0,651,653,1,0,0,0,652,649,1,0,0,0,652,653,1,0,0,0,653,
  	654,1,0,0,0,654,656,3,46,23,0,655,648,1,0,0,0,656,659,1,0,0,0,657,655,
  	1,0,0,0,657,658,1,0,0,0,658,661,1,0,0,0,659,657,1,0,0,0,660,662,5,20,
  	0,0,661,660,1,0,0,0,661,662,1,0,0,0,662,664,1,0,0,0,663,645,1,0,0,0,663,
  	664,1,0,0,0,664,665,1,0,0,0,665,666,5,15,0,0,666,95,1,0,0,0,667,689,5,
  	25,0,0,668,671,5,93,0,0,669,671,3,46,23,0,670,668,1,0,0,0,670,669,1,0,
  	0,0,671,672,1,0,0,0,672,673,5,27,0,0,673,683,3,46,23,0,674,677,5,20,0,
  	0,675,678,5,93,0,0,676,678,3,46,23,0,677,675,1,0,0,0,677,676,1,0,0,0,
  	678,679,1,0,0,0,679,680,5,27,0,0,680,682,3,46,23,0,681,674,1,0,0,0,682,
  	685,1,0,0,0,683,681,1,0,0,0,683,684,1,0,0,0,684,687,1,0,0,0,685,683,1,
  	0,0,0,686,688,5,20,0,0,687,686,1,0,0,0,687,688,1,0,0,0,688,690,1,0,0,
  	0,689,670,1,0,0,0,689,690,1,0,0,0,690,691,1,0,0,0,691,692,5,26,0,0,692,
  	97,1,0,0,0,693,694,5,92,0,0,694,99,1,0,0,0,77,106,108,126,129,136,138,
  	144,147,150,164,171,174,177,182,185,192,195,201,208,218,224,232,236,240,
  	263,277,287,293,298,308,317,326,343,348,358,372,383,394,405,416,427,438,
  	449,460,471,482,493,497,502,511,518,522,531,534,541,545,554,562,570,575,
  	581,590,597,599,612,617,629,645,652,657,661,663,670,677,683,687,689
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
    setState(108);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 77790) != 0) || _la == gscParser::IDENTIFIER) {
      setState(106);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case gscParser::T__0: {
          setState(100);
          match(gscParser::T__0);
          break;
        }

        case gscParser::T__1: {
          setState(101);
          match(gscParser::T__1);
          break;
        }

        case gscParser::T__7:
        case gscParser::T__8:
        case gscParser::T__9:
        case gscParser::T__10:
        case gscParser::T__12:
        case gscParser::T__15:
        case gscParser::IDENTIFIER: {
          setState(102);
          function();
          break;
        }

        case gscParser::T__2:
        case gscParser::T__3: {
          setState(103);
          include();
          break;
        }

        case gscParser::T__5: {
          setState(104);
          namespace_();
          break;
        }

        case gscParser::T__6: {
          setState(105);
          filenamespace();
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(110);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(111);
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
    setState(113);
    _la = _input->LA(1);
    if (!(_la == gscParser::T__2

    || _la == gscParser::T__3)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(114);
    _la = _input->LA(1);
    if (!(_la == gscParser::IDENTIFIER

    || _la == gscParser::PATH)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(115);
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
    setState(117);
    match(gscParser::T__5);
    setState(118);
    match(gscParser::IDENTIFIER);
    setState(119);
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
    setState(121);
    match(gscParser::T__6);
    setState(122);
    _la = _input->LA(1);
    if (!(_la == gscParser::IDENTIFIER

    || _la == gscParser::PATH)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(123);
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
  enterRule(_localctx, 8, gscParser::RuleFunction);
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
    setState(126);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__7) {
      setState(125);
      match(gscParser::T__7);
    }
    setState(129);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__8) {
      setState(128);
      match(gscParser::T__8);
    }
    setState(138);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__9) {
      setState(131);
      match(gscParser::T__9);
      setState(136);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 4, _ctx)) {
      case 1: {
        setState(132);
        match(gscParser::T__10);
        setState(133);
        number();
        setState(134);
        match(gscParser::T__11);
        break;
      }

      default:
        break;
      }
    }
    setState(144);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__12) {
      setState(140);
      match(gscParser::T__12);
      setState(141);
      match(gscParser::T__13);
      setState(142);
      match(gscParser::IDENTIFIER);
      setState(143);
      match(gscParser::T__14);
    }
    setState(147);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__15) {
      setState(146);
      detour_info();
    }
    setState(150);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::IDENTIFIER) {
      setState(149);
      match(gscParser::IDENTIFIER);
    }
    setState(152);
    match(gscParser::T__10);
    setState(153);
    param_list();
    setState(154);
    match(gscParser::T__11);
    setState(155);
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
  enterRule(_localctx, 10, gscParser::RuleDetour_info);
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
    setState(157);
    match(gscParser::T__15);
    setState(158);
    match(gscParser::IDENTIFIER);
    setState(164);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__16) {
      setState(159);
      match(gscParser::T__16);
      setState(160);
      match(gscParser::PATH);
      setState(161);
      match(gscParser::T__17);
      setState(162);
      match(gscParser::T__18);
      setState(163);
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
  enterRule(_localctx, 12, gscParser::RuleParam_list);
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
    setState(174);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 23068672) != 0) || _la == gscParser::IDENTIFIER) {
      setState(166);
      param_val();
      setState(171);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == gscParser::T__19) {
        setState(167);
        match(gscParser::T__19);
        setState(168);
        param_val();
        setState(173);
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
  enterRule(_localctx, 14, gscParser::RuleParam_val);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(185);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case gscParser::T__20:
      case gscParser::T__21:
      case gscParser::IDENTIFIER: {
        enterOuterAlt(_localctx, 1);
        setState(177);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == gscParser::T__20

        || _la == gscParser::T__21) {
          setState(176);
          _la = _input->LA(1);
          if (!(_la == gscParser::T__20

          || _la == gscParser::T__21)) {
          _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
        }
        setState(179);
        match(gscParser::IDENTIFIER);
        setState(182);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == gscParser::T__22) {
          setState(180);
          match(gscParser::T__22);
          setState(181);
          expression();
        }
        break;
      }

      case gscParser::T__23: {
        enterOuterAlt(_localctx, 2);
        setState(184);
        match(gscParser::T__23);
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
  enterRule(_localctx, 16, gscParser::RuleExpression_list);
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
    setState(195);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 37767168) != 0) || ((((_la - 72) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 72)) & 27128575) != 0)) {
      setState(187);
      expression();
      setState(192);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == gscParser::T__19) {
        setState(188);
        match(gscParser::T__19);
        setState(189);
        expression();
        setState(194);
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
  enterRule(_localctx, 18, gscParser::RuleStatement_block);
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
    setState(197);
    match(gscParser::T__24);
    setState(201);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 8568229087234) != 0) || ((((_la - 72) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 72)) & 27259647) != 0)) {
      setState(198);
      statement();
      setState(203);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(204);
    match(gscParser::T__25);
   
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
  enterRule(_localctx, 20, gscParser::RuleStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(208);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 18, _ctx)) {
    case 1: {
      setState(206);
      match(gscParser::IDENTIFIER);
      setState(207);
      match(gscParser::T__26);
      break;
    }

    default:
      break;
    }
    setState(218);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 19, _ctx)) {
    case 1: {
      setState(210);
      statement_block();
      break;
    }

    case 2: {
      setState(211);
      statement_dev_block();
      break;
    }

    case 3: {
      setState(212);
      statement_for();
      break;
    }

    case 4: {
      setState(213);
      statement_if();
      break;
    }

    case 5: {
      setState(214);
      statement_while();
      break;
    }

    case 6: {
      setState(215);
      statement_foreach();
      break;
    }

    case 7: {
      setState(216);
      statement_inst();
      break;
    }

    case 8: {
      setState(217);
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
  enterRule(_localctx, 22, gscParser::RuleStatement_dev_block);
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
    setState(220);
    match(gscParser::T__0);
    setState(224);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 8568229087234) != 0) || ((((_la - 72) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 72)) & 27259647) != 0)) {
      setState(221);
      statement();
      setState(226);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(227);
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
  enterRule(_localctx, 24, gscParser::RuleStatement_for);
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
    setState(229);
    match(gscParser::T__27);
    setState(230);
    match(gscParser::T__10);
    setState(232);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 37767168) != 0) || ((((_la - 72) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 72)) & 27128575) != 0)) {
      setState(231);
      expression();
    }
    setState(234);
    match(gscParser::T__4);
    setState(236);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 37767168) != 0) || ((((_la - 72) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 72)) & 27128575) != 0)) {
      setState(235);
      expression();
    }
    setState(238);
    match(gscParser::T__4);
    setState(240);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 37767168) != 0) || ((((_la - 72) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 72)) & 27128575) != 0)) {
      setState(239);
      expression();
    }
    setState(242);
    match(gscParser::T__11);
    setState(243);
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
  enterRule(_localctx, 26, gscParser::RuleStatement_while);

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
    match(gscParser::T__28);
    setState(246);
    match(gscParser::T__10);
    setState(247);
    expression();
    setState(248);
    match(gscParser::T__11);
    setState(249);
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
  enterRule(_localctx, 28, gscParser::RuleStatement_dowhile);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(251);
    match(gscParser::T__29);
    setState(252);
    statement();
    setState(253);
    match(gscParser::T__28);
    setState(254);
    match(gscParser::T__10);
    setState(255);
    expression();
    setState(256);
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
  enterRule(_localctx, 30, gscParser::RuleStatement_foreach);
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
    setState(258);
    match(gscParser::T__30);
    setState(259);
    match(gscParser::T__10);
    setState(260);
    match(gscParser::IDENTIFIER);
    setState(263);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__19) {
      setState(261);
      match(gscParser::T__19);
      setState(262);
      match(gscParser::IDENTIFIER);
    }
    setState(265);
    match(gscParser::T__31);
    setState(266);
    expression();
    setState(267);
    match(gscParser::T__11);
    setState(268);
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
  enterRule(_localctx, 32, gscParser::RuleStatement_if);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(270);
    match(gscParser::T__32);
    setState(271);
    match(gscParser::T__10);
    setState(272);
    expression();
    setState(273);
    match(gscParser::T__11);
    setState(274);
    statement();
    setState(277);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 25, _ctx)) {
    case 1: {
      setState(275);
      match(gscParser::T__33);
      setState(276);
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
  enterRule(_localctx, 34, gscParser::RuleStatement_switch);
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
    setState(279);
    match(gscParser::T__34);
    setState(280);
    match(gscParser::T__10);
    setState(281);
    expression();
    setState(282);
    match(gscParser::T__11);
    setState(283);
    match(gscParser::T__24);
    setState(296); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(287);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case gscParser::T__35: {
          setState(284);
          match(gscParser::T__35);
          setState(285);
          const_expr();
          break;
        }

        case gscParser::T__36: {
          setState(286);
          match(gscParser::T__36);
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(289);
      match(gscParser::T__26);
      setState(293);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while ((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & 8568229087234) != 0) || ((((_la - 72) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 72)) & 27259647) != 0)) {
        setState(290);
        statement();
        setState(295);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
      setState(298); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while (_la == gscParser::T__35

    || _la == gscParser::T__36);
    setState(300);
    match(gscParser::T__25);
   
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
  enterRule(_localctx, 36, gscParser::RuleStatement_inst);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(308);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 29, _ctx)) {
    case 1: {
      setState(302);
      expression();
      break;
    }

    case 2: {
      setState(303);
      operator_inst();
      break;
    }

    case 3: {
      setState(304);
      statement_dowhile();
      break;
    }

    case 4: {
      setState(305);
      function_call_exp();
      break;
    }

    case 5: {
      setState(306);
      nop_def();
      break;
    }

    case 6: {
      setState(307);
      devop_def();
      break;
    }

    default:
      break;
    }
    setState(310);
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
  enterRule(_localctx, 38, gscParser::RuleNop_def);
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
    setState(312);
    _la = _input->LA(1);
    if (!(_la == gscParser::T__37

    || _la == gscParser::T__38)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(317);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__10) {
      setState(313);
      match(gscParser::T__10);
      setState(314);
      number();
      setState(315);
      match(gscParser::T__11);
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
  enterRule(_localctx, 40, gscParser::RuleDevop_def);
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
    setState(319);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 7696581394432) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(320);
    match(gscParser::T__10);
    setState(321);
    number();
    setState(322);
    match(gscParser::T__11);
   
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
  enterRule(_localctx, 42, gscParser::RuleFunction_component);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(343);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 32, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(326);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 31, _ctx)) {
      case 1: {
        setState(324);
        match(gscParser::IDENTIFIER);
        setState(325);
        match(gscParser::T__18);
        break;
      }

      default:
        break;
      }
      setState(328);
      match(gscParser::IDENTIFIER);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(329);
      match(gscParser::T__13);
      setState(330);
      match(gscParser::T__13);
      setState(331);
      expression();
      setState(332);
      match(gscParser::T__14);
      setState(333);
      match(gscParser::T__14);
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(335);
      match(gscParser::T__13);
      setState(336);
      match(gscParser::T__13);
      setState(337);
      expression();
      setState(338);
      match(gscParser::T__14);
      setState(339);
      match(gscParser::T__14);
      setState(340);
      match(gscParser::T__42);
      setState(341);
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
  enterRule(_localctx, 44, gscParser::RuleOperator_inst);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(345);
    match(gscParser::BUILTIN);
    setState(348);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 33, _ctx)) {
    case 1: {
      setState(346);
      match(gscParser::IDENTIFIER);
      break;
    }

    case 2: {
      setState(347);
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
  enterRule(_localctx, 46, gscParser::RuleExpression);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(358);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 34, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(350);
      set_expression();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(351);
      expression1(0);
      setState(352);
      match(gscParser::T__43);
      setState(353);
      expression();
      setState(354);
      match(gscParser::T__26);
      setState(355);
      expression();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(357);
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
  enterRule(_localctx, 48, gscParser::RuleSet_expression);
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
    setState(360);
    left_value(0);
    setState(361);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 72022409674227712) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(362);
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
  size_t startState = 50;
  enterRecursionRule(_localctx, 50, gscParser::RuleExpression1, precedence);

    

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
    setState(365);
    expression2(0);
    _ctx->stop = _input->LT(-1);
    setState(372);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 35, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression1Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression1);
        setState(367);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(368);
        match(gscParser::T__55);
        setState(369);
        expression2(0); 
      }
      setState(374);
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
  size_t startState = 52;
  enterRecursionRule(_localctx, 52, gscParser::RuleExpression2, precedence);

    

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
    setState(376);
    expression3(0);
    _ctx->stop = _input->LT(-1);
    setState(383);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 36, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression2Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression2);
        setState(378);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(379);
        match(gscParser::T__56);
        setState(380);
        expression3(0); 
      }
      setState(385);
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
  size_t startState = 54;
  enterRecursionRule(_localctx, 54, gscParser::RuleExpression3, precedence);

    

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
    setState(387);
    expression4(0);
    _ctx->stop = _input->LT(-1);
    setState(394);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 37, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression3Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression3);
        setState(389);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(390);
        match(gscParser::T__57);
        setState(391);
        expression4(0); 
      }
      setState(396);
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
  size_t startState = 56;
  enterRecursionRule(_localctx, 56, gscParser::RuleExpression4, precedence);

    

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
    setState(398);
    expression5(0);
    _ctx->stop = _input->LT(-1);
    setState(405);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 38, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression4Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression4);
        setState(400);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(401);
        match(gscParser::T__58);
        setState(402);
        expression5(0); 
      }
      setState(407);
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
  size_t startState = 58;
  enterRecursionRule(_localctx, 58, gscParser::RuleExpression5, precedence);

    

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
    setState(409);
    expression6(0);
    _ctx->stop = _input->LT(-1);
    setState(416);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 39, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression5Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression5);
        setState(411);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(412);
        match(gscParser::T__21);
        setState(413);
        expression6(0); 
      }
      setState(418);
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
  size_t startState = 60;
  enterRecursionRule(_localctx, 60, gscParser::RuleExpression6, precedence);

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
    setState(420);
    expression7(0);
    _ctx->stop = _input->LT(-1);
    setState(427);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 40, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression6Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression6);
        setState(422);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(423);
        _la = _input->LA(1);
        if (!((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & -1152921504606846976) != 0))) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(424);
        expression7(0); 
      }
      setState(429);
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
  size_t startState = 62;
  enterRecursionRule(_localctx, 62, gscParser::RuleExpression7, precedence);

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
    setState(431);
    expression8(0);
    _ctx->stop = _input->LT(-1);
    setState(438);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 41, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression7Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression7);
        setState(433);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(434);
        _la = _input->LA(1);
        if (!(((((_la - 17) & ~ 0x3fULL) == 0) &&
          ((1ULL << (_la - 17)) & 422212465065987) != 0))) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(435);
        expression8(0); 
      }
      setState(440);
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
  size_t startState = 64;
  enterRecursionRule(_localctx, 64, gscParser::RuleExpression8, precedence);

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
    setState(442);
    expression9(0);
    _ctx->stop = _input->LT(-1);
    setState(449);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 42, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression8Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression8);
        setState(444);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(445);
        _la = _input->LA(1);
        if (!(_la == gscParser::T__16

        || _la == gscParser::T__63)) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(446);
        expression9(0); 
      }
      setState(451);
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
  size_t startState = 66;
  enterRecursionRule(_localctx, 66, gscParser::RuleExpression9, precedence);

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
    setState(453);
    expression10(0);
    _ctx->stop = _input->LT(-1);
    setState(460);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 43, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression9Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression9);
        setState(455);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(456);
        _la = _input->LA(1);
        if (!(_la == gscParser::T__65

        || _la == gscParser::T__66)) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(457);
        expression10(0); 
      }
      setState(462);
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
  size_t startState = 68;
  enterRecursionRule(_localctx, 68, gscParser::RuleExpression10, precedence);

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
    setState(464);
    expression11(0);
    _ctx->stop = _input->LT(-1);
    setState(471);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 44, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression10Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression10);
        setState(466);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(467);
        _la = _input->LA(1);
        if (!(_la == gscParser::T__67

        || _la == gscParser::T__68)) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(468);
        expression11(0); 
      }
      setState(473);
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
  size_t startState = 70;
  enterRecursionRule(_localctx, 70, gscParser::RuleExpression11, precedence);

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
    setState(475);
    expression12();
    _ctx->stop = _input->LT(-1);
    setState(482);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 45, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression11Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression11);
        setState(477);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(478);
        _la = _input->LA(1);
        if (!(((((_la - 21) & ~ 0x3fULL) == 0) &&
          ((1ULL << (_la - 21)) & 1688849860263937) != 0))) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(479);
        expression12(); 
      }
      setState(484);
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
  enterRule(_localctx, 72, gscParser::RuleExpression12);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(493);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 46, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(485);
      _la = _input->LA(1);
      if (!(_la == gscParser::T__71

      || _la == gscParser::T__72)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(486);
      expression13();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(487);
      _la = _input->LA(1);
      if (!(_la == gscParser::T__73

      || _la == gscParser::T__74)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(488);
      left_value(0);
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(489);
      left_value(0);
      setState(490);
      _la = _input->LA(1);
      if (!(_la == gscParser::T__73

      || _la == gscParser::T__74)) {
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
      setState(492);
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
  enterRule(_localctx, 74, gscParser::RuleExpression13);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(497);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 47, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(495);
      function_call_exp();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(496);
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
  enterRule(_localctx, 76, gscParser::RuleExpression14);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(502);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 48, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(499);
      const_expr();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(500);
      expression15();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(501);
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
  enterRule(_localctx, 78, gscParser::RuleExpression15);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(504);
    match(gscParser::T__10);
    setState(505);
    expression();
    setState(506);
    match(gscParser::T__11);
   
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
  enterRule(_localctx, 80, gscParser::RuleFunction_call_exp);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(518);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 50, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(508);
      function_call(0);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(509);
      expression14();
      setState(511);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (((((_la - 76) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 76)) & 15) != 0)) {
        setState(510);
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
      setState(513);
      function_component();
      setState(514);
      match(gscParser::T__10);
      setState(515);
      expression_list();
      setState(516);
      match(gscParser::T__11);
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
  size_t startState = 82;
  enterRecursionRule(_localctx, 82, gscParser::RuleFunction_call, precedence);

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
    setState(541);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 54, _ctx)) {
    case 1: {
      setState(522);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (((((_la - 76) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 76)) & 15) != 0)) {
        setState(521);
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
      setState(524);
      function_component();
      setState(525);
      match(gscParser::T__10);
      setState(526);
      expression_list();
      setState(527);
      match(gscParser::T__11);
      break;
    }

    case 2: {
      setState(531);
      _errHandler->sync(this);
      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 52, _ctx)) {
      case 1: {
        setState(529);
        const_expr();
        break;
      }

      case 2: {
        setState(530);
        expression15();
        break;
      }

      default:
        break;
      }
      setState(534);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (((((_la - 76) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 76)) & 15) != 0)) {
        setState(533);
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
      setState(536);
      function_component();
      setState(537);
      match(gscParser::T__10);
      setState(538);
      expression_list();
      setState(539);
      match(gscParser::T__11);
      break;
    }

    default:
      break;
    }
    _ctx->stop = _input->LT(-1);
    setState(554);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 56, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Function_callContext>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleFunction_call);
        setState(543);

        if (!(precpred(_ctx, 1))) throw FailedPredicateException(this, "precpred(_ctx, 1)");
        setState(545);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (((((_la - 76) & ~ 0x3fULL) == 0) &&
          ((1ULL << (_la - 76)) & 15) != 0)) {
          setState(544);
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
        setState(547);
        function_component();
        setState(548);
        match(gscParser::T__10);
        setState(549);
        expression_list();
        setState(550);
        match(gscParser::T__11); 
      }
      setState(556);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 56, _ctx);
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
  size_t startState = 84;
  enterRecursionRule(_localctx, 84, gscParser::RuleLeft_value, precedence);

    

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
    setState(581);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 60, _ctx)) {
    case 1: {
      setState(558);
      idf();
      break;
    }

    case 2: {
      setState(562);
      _errHandler->sync(this);
      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 57, _ctx)) {
      case 1: {
        setState(559);
        function_call(0);
        break;
      }

      case 2: {
        setState(560);
        const_expr();
        break;
      }

      case 3: {
        setState(561);
        expression15();
        break;
      }

      default:
        break;
      }
      setState(564);
      match(gscParser::T__79);
      setState(570);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case gscParser::IDENTIFIER: {
          setState(565);
          idf();
          break;
        }

        case gscParser::T__10: {
          setState(566);
          match(gscParser::T__10);
          setState(567);
          expression();
          setState(568);
          match(gscParser::T__11);
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      break;
    }

    case 3: {
      setState(575);
      _errHandler->sync(this);
      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 59, _ctx)) {
      case 1: {
        setState(572);
        function_call(0);
        break;
      }

      case 2: {
        setState(573);
        const_expr();
        break;
      }

      case 3: {
        setState(574);
        expression15();
        break;
      }

      default:
        break;
      }
      setState(577);
      match(gscParser::T__13);
      setState(578);
      expression();
      setState(579);
      match(gscParser::T__14);
      break;
    }

    default:
      break;
    }
    _ctx->stop = _input->LT(-1);
    setState(599);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 63, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        setState(597);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 62, _ctx)) {
        case 1: {
          _localctx = _tracker.createInstance<Left_valueContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleLeft_value);
          setState(583);

          if (!(precpred(_ctx, 4))) throw FailedPredicateException(this, "precpred(_ctx, 4)");
          setState(584);
          match(gscParser::T__79);
          setState(590);
          _errHandler->sync(this);
          switch (_input->LA(1)) {
            case gscParser::IDENTIFIER: {
              setState(585);
              idf();
              break;
            }

            case gscParser::T__10: {
              setState(586);
              match(gscParser::T__10);
              setState(587);
              expression();
              setState(588);
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
          setState(592);

          if (!(precpred(_ctx, 3))) throw FailedPredicateException(this, "precpred(_ctx, 3)");
          setState(593);
          match(gscParser::T__13);
          setState(594);
          expression();
          setState(595);
          match(gscParser::T__14);
          break;
        }

        default:
          break;
        } 
      }
      setState(601);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 63, _ctx);
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
  enterRule(_localctx, 86, gscParser::RuleConst_expr);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(612);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case gscParser::T__10: {
        enterOuterAlt(_localctx, 1);
        setState(602);
        vector_value();
        break;
      }

      case gscParser::T__13: {
        enterOuterAlt(_localctx, 2);
        setState(603);
        array_def();
        break;
      }

      case gscParser::T__24: {
        enterOuterAlt(_localctx, 3);
        setState(604);
        struct_def();
        break;
      }

      case gscParser::INTEGER10:
      case gscParser::INTEGER16:
      case gscParser::INTEGER8:
      case gscParser::INTEGER2: {
        enterOuterAlt(_localctx, 4);
        setState(605);
        number();
        break;
      }

      case gscParser::BOOL_VALUE: {
        enterOuterAlt(_localctx, 5);
        setState(606);
        match(gscParser::BOOL_VALUE);
        break;
      }

      case gscParser::FLOATVAL: {
        enterOuterAlt(_localctx, 6);
        setState(607);
        match(gscParser::FLOATVAL);
        break;
      }

      case gscParser::STRING: {
        enterOuterAlt(_localctx, 7);
        setState(608);
        match(gscParser::STRING);
        break;
      }

      case gscParser::HASHSTRING: {
        enterOuterAlt(_localctx, 8);
        setState(609);
        match(gscParser::HASHSTRING);
        break;
      }

      case gscParser::UNDEFINED_VALUE: {
        enterOuterAlt(_localctx, 9);
        setState(610);
        match(gscParser::UNDEFINED_VALUE);
        break;
      }

      case gscParser::T__21:
      case gscParser::T__80: {
        enterOuterAlt(_localctx, 10);
        setState(611);
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
  enterRule(_localctx, 88, gscParser::RuleFunction_ref);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(629);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 66, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(614);
      match(gscParser::T__21);
      setState(617);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 65, _ctx)) {
      case 1: {
        setState(615);
        match(gscParser::IDENTIFIER);
        setState(616);
        match(gscParser::T__18);
        break;
      }

      default:
        break;
      }
      setState(619);
      match(gscParser::IDENTIFIER);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(620);
      match(gscParser::T__80);
      setState(621);
      match(gscParser::IDENTIFIER);
      setState(622);
      match(gscParser::T__16);
      setState(623);
      match(gscParser::PATH);
      setState(624);
      match(gscParser::T__17);
      setState(625);
      match(gscParser::T__18);
      setState(626);
      match(gscParser::IDENTIFIER);
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(627);
      match(gscParser::T__21);
      setState(628);
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
  enterRule(_localctx, 90, gscParser::RuleNumber);
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
    setState(631);
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
  enterRule(_localctx, 92, gscParser::RuleVector_value);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(633);
    match(gscParser::T__10);
    setState(634);
    expression();
    setState(635);
    match(gscParser::T__19);
    setState(636);
    expression();
    setState(637);
    match(gscParser::T__19);
    setState(638);
    expression();
    setState(639);
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
  enterRule(_localctx, 94, gscParser::RuleArray_def);
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
    setState(641);
    match(gscParser::T__13);
    setState(663);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 37767168) != 0) || ((((_la - 72) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 72)) & 27128575) != 0)) {
      setState(645);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 67, _ctx)) {
      case 1: {
        setState(642);
        expression();
        setState(643);
        match(gscParser::T__26);
        break;
      }

      default:
        break;
      }
      setState(647);
      expression();
      setState(657);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 69, _ctx);
      while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
        if (alt == 1) {
          setState(648);
          match(gscParser::T__19);
          setState(652);
          _errHandler->sync(this);

          switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 68, _ctx)) {
          case 1: {
            setState(649);
            expression();
            setState(650);
            match(gscParser::T__26);
            break;
          }

          default:
            break;
          }
          setState(654);
          expression(); 
        }
        setState(659);
        _errHandler->sync(this);
        alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 69, _ctx);
      }
      setState(661);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == gscParser::T__19) {
        setState(660);
        match(gscParser::T__19);
      }
    }
    setState(665);
    match(gscParser::T__14);
   
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
  enterRule(_localctx, 96, gscParser::RuleStruct_def);
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
    setState(667);
    match(gscParser::T__24);
    setState(689);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 37767168) != 0) || ((((_la - 72) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 72)) & 29225727) != 0)) {
      setState(670);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case gscParser::STRUCT_IDENTIFIER: {
          setState(668);
          match(gscParser::STRUCT_IDENTIFIER);
          break;
        }

        case gscParser::T__10:
        case gscParser::T__13:
        case gscParser::T__21:
        case gscParser::T__24:
        case gscParser::T__71:
        case gscParser::T__72:
        case gscParser::T__73:
        case gscParser::T__74:
        case gscParser::T__75:
        case gscParser::T__76:
        case gscParser::T__77:
        case gscParser::T__78:
        case gscParser::T__80:
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
          setState(669);
          expression();
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(672);
      match(gscParser::T__26);
      setState(673);
      expression();
      setState(683);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 74, _ctx);
      while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
        if (alt == 1) {
          setState(674);
          match(gscParser::T__19);
          setState(677);
          _errHandler->sync(this);
          switch (_input->LA(1)) {
            case gscParser::STRUCT_IDENTIFIER: {
              setState(675);
              match(gscParser::STRUCT_IDENTIFIER);
              break;
            }

            case gscParser::T__10:
            case gscParser::T__13:
            case gscParser::T__21:
            case gscParser::T__24:
            case gscParser::T__71:
            case gscParser::T__72:
            case gscParser::T__73:
            case gscParser::T__74:
            case gscParser::T__75:
            case gscParser::T__76:
            case gscParser::T__77:
            case gscParser::T__78:
            case gscParser::T__80:
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
              setState(676);
              expression();
              break;
            }

          default:
            throw NoViableAltException(this);
          }
          setState(679);
          match(gscParser::T__26);
          setState(680);
          expression(); 
        }
        setState(685);
        _errHandler->sync(this);
        alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 74, _ctx);
      }
      setState(687);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == gscParser::T__19) {
        setState(686);
        match(gscParser::T__19);
      }
    }
    setState(691);
    match(gscParser::T__25);
   
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
  enterRule(_localctx, 98, gscParser::RuleIdf);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(693);
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
    case 25: return expression1Sempred(antlrcpp::downCast<Expression1Context *>(context), predicateIndex);
    case 26: return expression2Sempred(antlrcpp::downCast<Expression2Context *>(context), predicateIndex);
    case 27: return expression3Sempred(antlrcpp::downCast<Expression3Context *>(context), predicateIndex);
    case 28: return expression4Sempred(antlrcpp::downCast<Expression4Context *>(context), predicateIndex);
    case 29: return expression5Sempred(antlrcpp::downCast<Expression5Context *>(context), predicateIndex);
    case 30: return expression6Sempred(antlrcpp::downCast<Expression6Context *>(context), predicateIndex);
    case 31: return expression7Sempred(antlrcpp::downCast<Expression7Context *>(context), predicateIndex);
    case 32: return expression8Sempred(antlrcpp::downCast<Expression8Context *>(context), predicateIndex);
    case 33: return expression9Sempred(antlrcpp::downCast<Expression9Context *>(context), predicateIndex);
    case 34: return expression10Sempred(antlrcpp::downCast<Expression10Context *>(context), predicateIndex);
    case 35: return expression11Sempred(antlrcpp::downCast<Expression11Context *>(context), predicateIndex);
    case 41: return function_callSempred(antlrcpp::downCast<Function_callContext *>(context), predicateIndex);
    case 42: return left_valueSempred(antlrcpp::downCast<Left_valueContext *>(context), predicateIndex);

  default:
    break;
  }
  return true;
}

bool gscParser::expression1Sempred(Expression1Context *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 0: return precpred(_ctx, 2);

  default:
    break;
  }
  return true;
}

bool gscParser::expression2Sempred(Expression2Context *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 1: return precpred(_ctx, 2);

  default:
    break;
  }
  return true;
}

bool gscParser::expression3Sempred(Expression3Context *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 2: return precpred(_ctx, 2);

  default:
    break;
  }
  return true;
}

bool gscParser::expression4Sempred(Expression4Context *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 3: return precpred(_ctx, 2);

  default:
    break;
  }
  return true;
}

bool gscParser::expression5Sempred(Expression5Context *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 4: return precpred(_ctx, 2);

  default:
    break;
  }
  return true;
}

bool gscParser::expression6Sempred(Expression6Context *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 5: return precpred(_ctx, 2);

  default:
    break;
  }
  return true;
}

bool gscParser::expression7Sempred(Expression7Context *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 6: return precpred(_ctx, 2);

  default:
    break;
  }
  return true;
}

bool gscParser::expression8Sempred(Expression8Context *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 7: return precpred(_ctx, 2);

  default:
    break;
  }
  return true;
}

bool gscParser::expression9Sempred(Expression9Context *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 8: return precpred(_ctx, 2);

  default:
    break;
  }
  return true;
}

bool gscParser::expression10Sempred(Expression10Context *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 9: return precpred(_ctx, 2);

  default:
    break;
  }
  return true;
}

bool gscParser::expression11Sempred(Expression11Context *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 10: return precpred(_ctx, 2);

  default:
    break;
  }
  return true;
}

bool gscParser::function_callSempred(Function_callContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 11: return precpred(_ctx, 1);

  default:
    break;
  }
  return true;
}

bool gscParser::left_valueSempred(Left_valueContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 12: return precpred(_ctx, 4);
    case 13: return precpred(_ctx, 3);

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
