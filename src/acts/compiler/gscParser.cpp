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
      "prog", "include", "namespace", "filenamespace", "constexpr", "class_def", 
      "class_var", "function", "detour_info", "param_list", "param_val", 
      "expression_list", "statement_block", "statement", "statement_dev_block", 
      "statement_for", "statement_while", "statement_dowhile", "statement_foreach", 
      "statement_if", "statement_switch", "statement_inst", "nop_def", "devop_def", 
      "function_component", "operator_inst", "expression", "set_expression", 
      "expression0", "expression1", "expression2", "expression3", "expression4", 
      "expression5", "expression6", "expression7", "expression8", "expression9", 
      "expression10", "expression11", "expression12", "expression13", "expression14", 
      "expression15", "is_expression", "function_call_exp", "function_call", 
      "left_value", "const_expr", "function_ref", "number", "vector_value", 
      "array_def", "struct_def", "class_init", "idf"
    },
    std::vector<std::string>{
      "", "'/#'", "'#/'", "'#include'", "'#using'", "';'", "'#namespace'", 
      "'#file'", "'#constexpr'", "'#define'", "'='", "'class'", "':'", "','", 
      "'{'", "'}'", "'var'", "'function'", "'private'", "'autoexec'", "'('", 
      "')'", "'event_handler'", "'['", "']'", "'=>'", "'detour'", "'<'", 
      "'>'", "'::'", "'*'", "'&'", "'...'", "'for'", "'while'", "'do'", 
      "'foreach'", "'in'", "'if'", "'else'", "'switch'", "'case'", "'default'", 
      "'nop'", "'Nop'", "'DevOp'", "'devop'", "'Devop'", "'->'", "'\\u003F'", 
      "'+='", "'-='", "'/='", "'*='", "'%='", "'&='", "'|='", "'^='", "'<<='", 
      "'>>='", "'~='", "'\\u003F\\u003F'", "'||'", "'&&'", "'|'", "'^'", 
      "'!='", "'=='", "'!=='", "'==='", "'<='", "'>='", "'<=='", "'>=='", 
      "'<<'", "'>>'", "'+'", "'-'", "'/'", "'%'", "'!'", "'~'", "'++'", 
      "'--'", "'is'", "'not'", "'thread'", "'childthread'", "'threadendon'", 
      "'builtin'", "'.'", "'\\u003F.'", "'@'", "'new'", "", "", "", "", 
      "", "", "", "", "", "'undefined'"
    },
    std::vector<std::string>{
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
      "", "", "", "", "", "", "", "", "", "NEWLINE", "WHITESPACE", "INTEGER10", 
      "INTEGER16", "INTEGER8", "INTEGER2", "FLOATVAL", "BUILTIN", "BOOL_VALUE", 
      "UNDEFINED_VALUE", "IDENTIFIER", "STRUCT_IDENTIFIER", "PATH", "STRING", 
      "HASHSTRING"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,108,791,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,7,
  	14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,7,
  	21,2,22,7,22,2,23,7,23,2,24,7,24,2,25,7,25,2,26,7,26,2,27,7,27,2,28,7,
  	28,2,29,7,29,2,30,7,30,2,31,7,31,2,32,7,32,2,33,7,33,2,34,7,34,2,35,7,
  	35,2,36,7,36,2,37,7,37,2,38,7,38,2,39,7,39,2,40,7,40,2,41,7,41,2,42,7,
  	42,2,43,7,43,2,44,7,44,2,45,7,45,2,46,7,46,2,47,7,47,2,48,7,48,2,49,7,
  	49,2,50,7,50,2,51,7,51,2,52,7,52,2,53,7,53,2,54,7,54,2,55,7,55,1,0,1,
  	0,1,0,1,0,1,0,1,0,1,0,1,0,5,0,121,8,0,10,0,12,0,124,9,0,1,0,1,0,1,1,1,
  	1,1,1,1,1,1,2,1,2,1,2,1,2,1,3,1,3,1,3,1,3,1,4,1,4,1,4,1,4,1,4,1,4,1,5,
  	1,5,1,5,1,5,1,5,1,5,5,5,152,8,5,10,5,12,5,155,9,5,3,5,157,8,5,1,5,1,5,
  	5,5,161,8,5,10,5,12,5,164,9,5,1,5,5,5,167,8,5,10,5,12,5,170,9,5,1,5,1,
  	5,1,6,1,6,1,6,1,6,3,6,178,8,6,1,6,1,6,1,7,3,7,183,8,7,1,7,3,7,186,8,7,
  	1,7,1,7,1,7,1,7,1,7,3,7,193,8,7,3,7,195,8,7,1,7,1,7,1,7,1,7,3,7,201,8,
  	7,1,7,3,7,204,8,7,1,7,3,7,207,8,7,1,7,1,7,1,7,1,7,3,7,213,8,7,1,7,1,7,
  	1,8,1,8,1,8,1,8,1,8,1,8,1,8,3,8,224,8,8,1,9,1,9,1,9,5,9,229,8,9,10,9,
  	12,9,232,9,9,3,9,234,8,9,1,10,3,10,237,8,10,1,10,1,10,1,10,3,10,242,8,
  	10,1,10,3,10,245,8,10,1,11,1,11,1,11,5,11,250,8,11,10,11,12,11,253,9,
  	11,3,11,255,8,11,1,12,1,12,5,12,259,8,12,10,12,12,12,262,9,12,1,12,1,
  	12,1,13,1,13,3,13,268,8,13,1,13,1,13,1,13,1,13,1,13,1,13,1,13,1,13,3,
  	13,278,8,13,1,14,1,14,5,14,282,8,14,10,14,12,14,285,9,14,1,14,1,14,1,
  	15,1,15,1,15,3,15,292,8,15,1,15,1,15,3,15,296,8,15,1,15,1,15,3,15,300,
  	8,15,1,15,1,15,1,15,1,16,1,16,1,16,1,16,1,16,1,16,1,17,1,17,1,17,1,17,
  	1,17,1,17,1,17,1,18,1,18,1,18,1,18,1,18,3,18,323,8,18,1,18,1,18,1,18,
  	1,18,1,18,1,19,1,19,1,19,1,19,1,19,1,19,1,19,3,19,337,8,19,1,20,1,20,
  	1,20,1,20,1,20,1,20,1,20,1,20,3,20,347,8,20,1,20,1,20,5,20,351,8,20,10,
  	20,12,20,354,9,20,4,20,356,8,20,11,20,12,20,357,1,20,1,20,1,21,1,21,1,
  	21,1,21,1,21,1,21,3,21,368,8,21,1,21,1,21,1,22,1,22,1,22,1,22,1,22,3,
  	22,377,8,22,1,23,1,23,1,23,1,23,1,23,1,24,1,24,3,24,386,8,24,1,24,1,24,
  	1,24,1,24,1,24,1,24,1,24,1,24,1,24,1,24,1,24,1,24,1,24,1,24,1,24,3,24,
  	403,8,24,1,25,1,25,1,25,3,25,408,8,25,1,26,1,26,1,26,1,26,1,26,1,26,1,
  	26,1,26,3,26,418,8,26,1,27,1,27,1,27,1,27,1,28,1,28,1,28,1,28,1,28,1,
  	28,5,28,430,8,28,10,28,12,28,433,9,28,1,29,1,29,1,29,1,29,1,29,1,29,5,
  	29,441,8,29,10,29,12,29,444,9,29,1,30,1,30,1,30,1,30,1,30,1,30,5,30,452,
  	8,30,10,30,12,30,455,9,30,1,31,1,31,1,31,1,31,1,31,1,31,5,31,463,8,31,
  	10,31,12,31,466,9,31,1,32,1,32,1,32,1,32,1,32,1,32,5,32,474,8,32,10,32,
  	12,32,477,9,32,1,33,1,33,1,33,1,33,1,33,1,33,5,33,485,8,33,10,33,12,33,
  	488,9,33,1,34,1,34,1,34,1,34,1,34,1,34,5,34,496,8,34,10,34,12,34,499,
  	9,34,1,35,1,35,1,35,1,35,1,35,1,35,5,35,507,8,35,10,35,12,35,510,9,35,
  	1,36,1,36,1,36,1,36,1,36,1,36,5,36,518,8,36,10,36,12,36,521,9,36,1,37,
  	1,37,1,37,1,37,1,37,1,37,5,37,529,8,37,10,37,12,37,532,9,37,1,38,1,38,
  	1,38,1,38,1,38,1,38,5,38,540,8,38,10,38,12,38,543,9,38,1,39,1,39,1,39,
  	1,39,1,39,1,39,5,39,551,8,39,10,39,12,39,554,9,39,1,40,1,40,1,40,1,40,
  	1,40,1,40,1,40,1,40,1,40,3,40,565,8,40,1,41,1,41,3,41,569,8,41,1,42,1,
  	42,1,42,3,42,574,8,42,1,43,1,43,1,43,1,43,1,44,1,44,1,44,3,44,583,8,44,
  	1,44,1,44,1,45,1,45,1,45,3,45,590,8,45,1,45,1,45,1,45,1,45,1,45,3,45,
  	597,8,45,1,46,1,46,3,46,601,8,46,1,46,1,46,1,46,1,46,1,46,1,46,1,46,3,
  	46,610,8,46,1,46,3,46,613,8,46,1,46,1,46,1,46,1,46,1,46,3,46,620,8,46,
  	1,46,1,46,3,46,624,8,46,1,46,1,46,1,46,1,46,1,46,5,46,631,8,46,10,46,
  	12,46,634,9,46,1,47,1,47,1,47,1,47,1,47,3,47,641,8,47,1,47,1,47,1,47,
  	1,47,1,47,1,47,3,47,649,8,47,1,47,1,47,1,47,3,47,654,8,47,1,47,1,47,1,
  	47,1,47,3,47,660,8,47,1,47,1,47,1,47,1,47,1,47,1,47,1,47,3,47,669,8,47,
  	1,47,1,47,1,47,1,47,1,47,5,47,676,8,47,10,47,12,47,679,9,47,1,48,1,48,
  	1,48,1,48,1,48,1,48,1,48,1,48,1,48,1,48,1,48,1,48,3,48,693,8,48,1,49,
  	1,49,1,49,3,49,698,8,49,1,49,1,49,1,49,1,49,1,49,1,49,1,49,1,49,1,49,
  	1,49,3,49,710,8,49,1,50,1,50,1,51,1,51,1,51,1,51,1,51,1,51,1,51,1,51,
  	1,52,1,52,1,52,1,52,3,52,726,8,52,1,52,1,52,1,52,1,52,1,52,3,52,733,8,
  	52,1,52,5,52,736,8,52,10,52,12,52,739,9,52,1,52,3,52,742,8,52,3,52,744,
  	8,52,1,52,1,52,1,53,1,53,1,53,3,53,751,8,53,1,53,1,53,1,53,1,53,1,53,
  	3,53,758,8,53,1,53,1,53,5,53,762,8,53,10,53,12,53,765,9,53,1,53,3,53,
  	768,8,53,3,53,770,8,53,1,53,1,53,1,54,1,54,1,54,1,54,1,54,1,54,5,54,780,
  	8,54,10,54,12,54,783,9,54,3,54,785,8,54,1,54,1,54,1,55,1,55,1,55,0,14,
  	56,58,60,62,64,66,68,70,72,74,76,78,92,94,56,0,2,4,6,8,10,12,14,16,18,
  	20,22,24,26,28,30,32,34,36,38,40,42,44,46,48,50,52,54,56,58,60,62,64,
  	66,68,70,72,74,76,78,80,82,84,86,88,90,92,94,96,98,100,102,104,106,108,
  	110,0,19,1,0,3,4,2,0,104,104,106,106,1,0,8,9,1,0,30,31,1,0,43,44,1,0,
  	45,47,2,0,10,10,50,60,1,0,66,69,2,0,27,28,70,73,2,0,27,27,70,70,1,0,74,
  	75,1,0,76,77,2,0,30,30,78,79,1,0,80,81,1,0,82,83,2,0,17,17,102,104,1,
  	0,86,89,1,0,90,91,2,0,96,99,104,104,858,0,122,1,0,0,0,2,127,1,0,0,0,4,
  	131,1,0,0,0,6,135,1,0,0,0,8,139,1,0,0,0,10,145,1,0,0,0,12,173,1,0,0,0,
  	14,182,1,0,0,0,16,216,1,0,0,0,18,233,1,0,0,0,20,244,1,0,0,0,22,254,1,
  	0,0,0,24,256,1,0,0,0,26,267,1,0,0,0,28,279,1,0,0,0,30,288,1,0,0,0,32,
  	304,1,0,0,0,34,310,1,0,0,0,36,317,1,0,0,0,38,329,1,0,0,0,40,338,1,0,0,
  	0,42,367,1,0,0,0,44,371,1,0,0,0,46,378,1,0,0,0,48,402,1,0,0,0,50,404,
  	1,0,0,0,52,417,1,0,0,0,54,419,1,0,0,0,56,423,1,0,0,0,58,434,1,0,0,0,60,
  	445,1,0,0,0,62,456,1,0,0,0,64,467,1,0,0,0,66,478,1,0,0,0,68,489,1,0,0,
  	0,70,500,1,0,0,0,72,511,1,0,0,0,74,522,1,0,0,0,76,533,1,0,0,0,78,544,
  	1,0,0,0,80,564,1,0,0,0,82,568,1,0,0,0,84,573,1,0,0,0,86,575,1,0,0,0,88,
  	579,1,0,0,0,90,596,1,0,0,0,92,619,1,0,0,0,94,659,1,0,0,0,96,692,1,0,0,
  	0,98,709,1,0,0,0,100,711,1,0,0,0,102,713,1,0,0,0,104,721,1,0,0,0,106,
  	747,1,0,0,0,108,773,1,0,0,0,110,788,1,0,0,0,112,121,5,1,0,0,113,121,5,
  	2,0,0,114,121,3,14,7,0,115,121,3,2,1,0,116,121,3,4,2,0,117,121,3,6,3,
  	0,118,121,3,8,4,0,119,121,3,10,5,0,120,112,1,0,0,0,120,113,1,0,0,0,120,
  	114,1,0,0,0,120,115,1,0,0,0,120,116,1,0,0,0,120,117,1,0,0,0,120,118,1,
  	0,0,0,120,119,1,0,0,0,121,124,1,0,0,0,122,120,1,0,0,0,122,123,1,0,0,0,
  	123,125,1,0,0,0,124,122,1,0,0,0,125,126,5,0,0,1,126,1,1,0,0,0,127,128,
  	7,0,0,0,128,129,7,1,0,0,129,130,5,5,0,0,130,3,1,0,0,0,131,132,5,6,0,0,
  	132,133,5,104,0,0,133,134,5,5,0,0,134,5,1,0,0,0,135,136,5,7,0,0,136,137,
  	7,1,0,0,137,138,5,5,0,0,138,7,1,0,0,0,139,140,7,2,0,0,140,141,5,104,0,
  	0,141,142,5,10,0,0,142,143,3,52,26,0,143,144,5,5,0,0,144,9,1,0,0,0,145,
  	146,5,11,0,0,146,156,5,104,0,0,147,148,5,12,0,0,148,153,5,104,0,0,149,
  	150,5,13,0,0,150,152,5,104,0,0,151,149,1,0,0,0,152,155,1,0,0,0,153,151,
  	1,0,0,0,153,154,1,0,0,0,154,157,1,0,0,0,155,153,1,0,0,0,156,147,1,0,0,
  	0,156,157,1,0,0,0,157,158,1,0,0,0,158,162,5,14,0,0,159,161,3,12,6,0,160,
  	159,1,0,0,0,161,164,1,0,0,0,162,160,1,0,0,0,162,163,1,0,0,0,163,168,1,
  	0,0,0,164,162,1,0,0,0,165,167,3,14,7,0,166,165,1,0,0,0,167,170,1,0,0,
  	0,168,166,1,0,0,0,168,169,1,0,0,0,169,171,1,0,0,0,170,168,1,0,0,0,171,
  	172,5,15,0,0,172,11,1,0,0,0,173,174,5,16,0,0,174,177,5,104,0,0,175,176,
  	5,10,0,0,176,178,3,52,26,0,177,175,1,0,0,0,177,178,1,0,0,0,178,179,1,
  	0,0,0,179,180,5,5,0,0,180,13,1,0,0,0,181,183,5,17,0,0,182,181,1,0,0,0,
  	182,183,1,0,0,0,183,185,1,0,0,0,184,186,5,18,0,0,185,184,1,0,0,0,185,
  	186,1,0,0,0,186,194,1,0,0,0,187,192,5,19,0,0,188,189,5,20,0,0,189,190,
  	3,100,50,0,190,191,5,21,0,0,191,193,1,0,0,0,192,188,1,0,0,0,192,193,1,
  	0,0,0,193,195,1,0,0,0,194,187,1,0,0,0,194,195,1,0,0,0,195,200,1,0,0,0,
  	196,197,5,22,0,0,197,198,5,23,0,0,198,199,5,104,0,0,199,201,5,24,0,0,
  	200,196,1,0,0,0,200,201,1,0,0,0,201,203,1,0,0,0,202,204,3,16,8,0,203,
  	202,1,0,0,0,203,204,1,0,0,0,204,206,1,0,0,0,205,207,5,104,0,0,206,205,
  	1,0,0,0,206,207,1,0,0,0,207,208,1,0,0,0,208,209,5,20,0,0,209,210,3,18,
  	9,0,210,212,5,21,0,0,211,213,5,25,0,0,212,211,1,0,0,0,212,213,1,0,0,0,
  	213,214,1,0,0,0,214,215,3,24,12,0,215,15,1,0,0,0,216,217,5,26,0,0,217,
  	223,5,104,0,0,218,219,5,27,0,0,219,220,5,106,0,0,220,221,5,28,0,0,221,
  	222,5,29,0,0,222,224,5,104,0,0,223,218,1,0,0,0,223,224,1,0,0,0,224,17,
  	1,0,0,0,225,230,3,20,10,0,226,227,5,13,0,0,227,229,3,20,10,0,228,226,
  	1,0,0,0,229,232,1,0,0,0,230,228,1,0,0,0,230,231,1,0,0,0,231,234,1,0,0,
  	0,232,230,1,0,0,0,233,225,1,0,0,0,233,234,1,0,0,0,234,19,1,0,0,0,235,
  	237,7,3,0,0,236,235,1,0,0,0,236,237,1,0,0,0,237,238,1,0,0,0,238,241,5,
  	104,0,0,239,240,5,10,0,0,240,242,3,52,26,0,241,239,1,0,0,0,241,242,1,
  	0,0,0,242,245,1,0,0,0,243,245,5,32,0,0,244,236,1,0,0,0,244,243,1,0,0,
  	0,245,21,1,0,0,0,246,251,3,52,26,0,247,248,5,13,0,0,248,250,3,52,26,0,
  	249,247,1,0,0,0,250,253,1,0,0,0,251,249,1,0,0,0,251,252,1,0,0,0,252,255,
  	1,0,0,0,253,251,1,0,0,0,254,246,1,0,0,0,254,255,1,0,0,0,255,23,1,0,0,
  	0,256,260,5,14,0,0,257,259,3,26,13,0,258,257,1,0,0,0,259,262,1,0,0,0,
  	260,258,1,0,0,0,260,261,1,0,0,0,261,263,1,0,0,0,262,260,1,0,0,0,263,264,
  	5,15,0,0,264,25,1,0,0,0,265,266,5,104,0,0,266,268,5,12,0,0,267,265,1,
  	0,0,0,267,268,1,0,0,0,268,277,1,0,0,0,269,278,3,24,12,0,270,278,3,28,
  	14,0,271,278,3,30,15,0,272,278,3,38,19,0,273,278,3,32,16,0,274,278,3,
  	36,18,0,275,278,3,42,21,0,276,278,3,40,20,0,277,269,1,0,0,0,277,270,1,
  	0,0,0,277,271,1,0,0,0,277,272,1,0,0,0,277,273,1,0,0,0,277,274,1,0,0,0,
  	277,275,1,0,0,0,277,276,1,0,0,0,278,27,1,0,0,0,279,283,5,1,0,0,280,282,
  	3,26,13,0,281,280,1,0,0,0,282,285,1,0,0,0,283,281,1,0,0,0,283,284,1,0,
  	0,0,284,286,1,0,0,0,285,283,1,0,0,0,286,287,5,2,0,0,287,29,1,0,0,0,288,
  	289,5,33,0,0,289,291,5,20,0,0,290,292,3,52,26,0,291,290,1,0,0,0,291,292,
  	1,0,0,0,292,293,1,0,0,0,293,295,5,5,0,0,294,296,3,52,26,0,295,294,1,0,
  	0,0,295,296,1,0,0,0,296,297,1,0,0,0,297,299,5,5,0,0,298,300,3,52,26,0,
  	299,298,1,0,0,0,299,300,1,0,0,0,300,301,1,0,0,0,301,302,5,21,0,0,302,
  	303,3,26,13,0,303,31,1,0,0,0,304,305,5,34,0,0,305,306,5,20,0,0,306,307,
  	3,52,26,0,307,308,5,21,0,0,308,309,3,26,13,0,309,33,1,0,0,0,310,311,5,
  	35,0,0,311,312,3,26,13,0,312,313,5,34,0,0,313,314,5,20,0,0,314,315,3,
  	52,26,0,315,316,5,21,0,0,316,35,1,0,0,0,317,318,5,36,0,0,318,319,5,20,
  	0,0,319,322,5,104,0,0,320,321,5,13,0,0,321,323,5,104,0,0,322,320,1,0,
  	0,0,322,323,1,0,0,0,323,324,1,0,0,0,324,325,5,37,0,0,325,326,3,52,26,
  	0,326,327,5,21,0,0,327,328,3,26,13,0,328,37,1,0,0,0,329,330,5,38,0,0,
  	330,331,5,20,0,0,331,332,3,52,26,0,332,333,5,21,0,0,333,336,3,26,13,0,
  	334,335,5,39,0,0,335,337,3,26,13,0,336,334,1,0,0,0,336,337,1,0,0,0,337,
  	39,1,0,0,0,338,339,5,40,0,0,339,340,5,20,0,0,340,341,3,52,26,0,341,342,
  	5,21,0,0,342,355,5,14,0,0,343,344,5,41,0,0,344,347,3,96,48,0,345,347,
  	5,42,0,0,346,343,1,0,0,0,346,345,1,0,0,0,347,348,1,0,0,0,348,352,5,12,
  	0,0,349,351,3,26,13,0,350,349,1,0,0,0,351,354,1,0,0,0,352,350,1,0,0,0,
  	352,353,1,0,0,0,353,356,1,0,0,0,354,352,1,0,0,0,355,346,1,0,0,0,356,357,
  	1,0,0,0,357,355,1,0,0,0,357,358,1,0,0,0,358,359,1,0,0,0,359,360,5,15,
  	0,0,360,41,1,0,0,0,361,368,3,52,26,0,362,368,3,50,25,0,363,368,3,34,17,
  	0,364,368,3,90,45,0,365,368,3,44,22,0,366,368,3,46,23,0,367,361,1,0,0,
  	0,367,362,1,0,0,0,367,363,1,0,0,0,367,364,1,0,0,0,367,365,1,0,0,0,367,
  	366,1,0,0,0,368,369,1,0,0,0,369,370,5,5,0,0,370,43,1,0,0,0,371,376,7,
  	4,0,0,372,373,5,20,0,0,373,374,3,100,50,0,374,375,5,21,0,0,375,377,1,
  	0,0,0,376,372,1,0,0,0,376,377,1,0,0,0,377,45,1,0,0,0,378,379,7,5,0,0,
  	379,380,5,20,0,0,380,381,3,100,50,0,381,382,5,21,0,0,382,47,1,0,0,0,383,
  	384,5,104,0,0,384,386,5,29,0,0,385,383,1,0,0,0,385,386,1,0,0,0,386,387,
  	1,0,0,0,387,403,5,104,0,0,388,389,5,23,0,0,389,390,5,23,0,0,390,391,3,
  	52,26,0,391,392,5,24,0,0,392,393,5,24,0,0,393,403,1,0,0,0,394,395,5,23,
  	0,0,395,396,5,23,0,0,396,397,3,52,26,0,397,398,5,24,0,0,398,399,5,24,
  	0,0,399,400,5,48,0,0,400,401,5,104,0,0,401,403,1,0,0,0,402,385,1,0,0,
  	0,402,388,1,0,0,0,402,394,1,0,0,0,403,49,1,0,0,0,404,407,5,101,0,0,405,
  	408,5,104,0,0,406,408,3,52,26,0,407,405,1,0,0,0,407,406,1,0,0,0,407,408,
  	1,0,0,0,408,51,1,0,0,0,409,418,3,54,27,0,410,411,3,56,28,0,411,412,5,
  	49,0,0,412,413,3,52,26,0,413,414,5,12,0,0,414,415,3,52,26,0,415,418,1,
  	0,0,0,416,418,3,56,28,0,417,409,1,0,0,0,417,410,1,0,0,0,417,416,1,0,0,
  	0,418,53,1,0,0,0,419,420,3,94,47,0,420,421,7,6,0,0,421,422,3,52,26,0,
  	422,55,1,0,0,0,423,424,6,28,-1,0,424,425,3,58,29,0,425,431,1,0,0,0,426,
  	427,10,2,0,0,427,428,5,61,0,0,428,430,3,58,29,0,429,426,1,0,0,0,430,433,
  	1,0,0,0,431,429,1,0,0,0,431,432,1,0,0,0,432,57,1,0,0,0,433,431,1,0,0,
  	0,434,435,6,29,-1,0,435,436,3,60,30,0,436,442,1,0,0,0,437,438,10,2,0,
  	0,438,439,5,62,0,0,439,441,3,60,30,0,440,437,1,0,0,0,441,444,1,0,0,0,
  	442,440,1,0,0,0,442,443,1,0,0,0,443,59,1,0,0,0,444,442,1,0,0,0,445,446,
  	6,30,-1,0,446,447,3,62,31,0,447,453,1,0,0,0,448,449,10,2,0,0,449,450,
  	5,63,0,0,450,452,3,62,31,0,451,448,1,0,0,0,452,455,1,0,0,0,453,451,1,
  	0,0,0,453,454,1,0,0,0,454,61,1,0,0,0,455,453,1,0,0,0,456,457,6,31,-1,
  	0,457,458,3,64,32,0,458,464,1,0,0,0,459,460,10,2,0,0,460,461,5,64,0,0,
  	461,463,3,64,32,0,462,459,1,0,0,0,463,466,1,0,0,0,464,462,1,0,0,0,464,
  	465,1,0,0,0,465,63,1,0,0,0,466,464,1,0,0,0,467,468,6,32,-1,0,468,469,
  	3,66,33,0,469,475,1,0,0,0,470,471,10,2,0,0,471,472,5,65,0,0,472,474,3,
  	66,33,0,473,470,1,0,0,0,474,477,1,0,0,0,475,473,1,0,0,0,475,476,1,0,0,
  	0,476,65,1,0,0,0,477,475,1,0,0,0,478,479,6,33,-1,0,479,480,3,68,34,0,
  	480,486,1,0,0,0,481,482,10,2,0,0,482,483,5,31,0,0,483,485,3,68,34,0,484,
  	481,1,0,0,0,485,488,1,0,0,0,486,484,1,0,0,0,486,487,1,0,0,0,487,67,1,
  	0,0,0,488,486,1,0,0,0,489,490,6,34,-1,0,490,491,3,70,35,0,491,497,1,0,
  	0,0,492,493,10,2,0,0,493,494,7,7,0,0,494,496,3,70,35,0,495,492,1,0,0,
  	0,496,499,1,0,0,0,497,495,1,0,0,0,497,498,1,0,0,0,498,69,1,0,0,0,499,
  	497,1,0,0,0,500,501,6,35,-1,0,501,502,3,72,36,0,502,508,1,0,0,0,503,504,
  	10,2,0,0,504,505,7,8,0,0,505,507,3,72,36,0,506,503,1,0,0,0,507,510,1,
  	0,0,0,508,506,1,0,0,0,508,509,1,0,0,0,509,71,1,0,0,0,510,508,1,0,0,0,
  	511,512,6,36,-1,0,512,513,3,74,37,0,513,519,1,0,0,0,514,515,10,2,0,0,
  	515,516,7,9,0,0,516,518,3,74,37,0,517,514,1,0,0,0,518,521,1,0,0,0,519,
  	517,1,0,0,0,519,520,1,0,0,0,520,73,1,0,0,0,521,519,1,0,0,0,522,523,6,
  	37,-1,0,523,524,3,76,38,0,524,530,1,0,0,0,525,526,10,2,0,0,526,527,7,
  	10,0,0,527,529,3,76,38,0,528,525,1,0,0,0,529,532,1,0,0,0,530,528,1,0,
  	0,0,530,531,1,0,0,0,531,75,1,0,0,0,532,530,1,0,0,0,533,534,6,38,-1,0,
  	534,535,3,78,39,0,535,541,1,0,0,0,536,537,10,2,0,0,537,538,7,11,0,0,538,
  	540,3,78,39,0,539,536,1,0,0,0,540,543,1,0,0,0,541,539,1,0,0,0,541,542,
  	1,0,0,0,542,77,1,0,0,0,543,541,1,0,0,0,544,545,6,39,-1,0,545,546,3,80,
  	40,0,546,552,1,0,0,0,547,548,10,2,0,0,548,549,7,12,0,0,549,551,3,80,40,
  	0,550,547,1,0,0,0,551,554,1,0,0,0,552,550,1,0,0,0,552,553,1,0,0,0,553,
  	79,1,0,0,0,554,552,1,0,0,0,555,556,7,13,0,0,556,565,3,82,41,0,557,558,
  	7,14,0,0,558,565,3,94,47,0,559,560,3,94,47,0,560,561,7,14,0,0,561,565,
  	1,0,0,0,562,565,3,88,44,0,563,565,3,82,41,0,564,555,1,0,0,0,564,557,1,
  	0,0,0,564,559,1,0,0,0,564,562,1,0,0,0,564,563,1,0,0,0,565,81,1,0,0,0,
  	566,569,3,90,45,0,567,569,3,84,42,0,568,566,1,0,0,0,568,567,1,0,0,0,569,
  	83,1,0,0,0,570,574,3,96,48,0,571,574,3,86,43,0,572,574,3,94,47,0,573,
  	570,1,0,0,0,573,571,1,0,0,0,573,572,1,0,0,0,574,85,1,0,0,0,575,576,5,
  	20,0,0,576,577,3,52,26,0,577,578,5,21,0,0,578,87,1,0,0,0,579,580,3,82,
  	41,0,580,582,5,84,0,0,581,583,5,85,0,0,582,581,1,0,0,0,582,583,1,0,0,
  	0,583,584,1,0,0,0,584,585,7,15,0,0,585,89,1,0,0,0,586,597,3,92,46,0,587,
  	589,3,84,42,0,588,590,7,16,0,0,589,588,1,0,0,0,589,590,1,0,0,0,590,591,
  	1,0,0,0,591,592,3,48,24,0,592,593,5,20,0,0,593,594,3,22,11,0,594,595,
  	5,21,0,0,595,597,1,0,0,0,596,586,1,0,0,0,596,587,1,0,0,0,597,91,1,0,0,
  	0,598,600,6,46,-1,0,599,601,7,16,0,0,600,599,1,0,0,0,600,601,1,0,0,0,
  	601,602,1,0,0,0,602,603,3,48,24,0,603,604,5,20,0,0,604,605,3,22,11,0,
  	605,606,5,21,0,0,606,620,1,0,0,0,607,610,3,96,48,0,608,610,3,86,43,0,
  	609,607,1,0,0,0,609,608,1,0,0,0,610,612,1,0,0,0,611,613,7,16,0,0,612,
  	611,1,0,0,0,612,613,1,0,0,0,613,614,1,0,0,0,614,615,3,48,24,0,615,616,
  	5,20,0,0,616,617,3,22,11,0,617,618,5,21,0,0,618,620,1,0,0,0,619,598,1,
  	0,0,0,619,609,1,0,0,0,620,632,1,0,0,0,621,623,10,1,0,0,622,624,7,16,0,
  	0,623,622,1,0,0,0,623,624,1,0,0,0,624,625,1,0,0,0,625,626,3,48,24,0,626,
  	627,5,20,0,0,627,628,3,22,11,0,628,629,5,21,0,0,629,631,1,0,0,0,630,621,
  	1,0,0,0,631,634,1,0,0,0,632,630,1,0,0,0,632,633,1,0,0,0,633,93,1,0,0,
  	0,634,632,1,0,0,0,635,636,6,47,-1,0,636,660,3,110,55,0,637,641,3,92,46,
  	0,638,641,3,96,48,0,639,641,3,86,43,0,640,637,1,0,0,0,640,638,1,0,0,0,
  	640,639,1,0,0,0,641,642,1,0,0,0,642,648,7,17,0,0,643,649,3,110,55,0,644,
  	645,5,20,0,0,645,646,3,52,26,0,646,647,5,21,0,0,647,649,1,0,0,0,648,643,
  	1,0,0,0,648,644,1,0,0,0,649,660,1,0,0,0,650,654,3,92,46,0,651,654,3,96,
  	48,0,652,654,3,86,43,0,653,650,1,0,0,0,653,651,1,0,0,0,653,652,1,0,0,
  	0,654,655,1,0,0,0,655,656,5,23,0,0,656,657,3,52,26,0,657,658,5,24,0,0,
  	658,660,1,0,0,0,659,635,1,0,0,0,659,640,1,0,0,0,659,653,1,0,0,0,660,677,
  	1,0,0,0,661,662,10,4,0,0,662,668,7,17,0,0,663,669,3,110,55,0,664,665,
  	5,20,0,0,665,666,3,52,26,0,666,667,5,21,0,0,667,669,1,0,0,0,668,663,1,
  	0,0,0,668,664,1,0,0,0,669,676,1,0,0,0,670,671,10,3,0,0,671,672,5,23,0,
  	0,672,673,3,52,26,0,673,674,5,24,0,0,674,676,1,0,0,0,675,661,1,0,0,0,
  	675,670,1,0,0,0,676,679,1,0,0,0,677,675,1,0,0,0,677,678,1,0,0,0,678,95,
  	1,0,0,0,679,677,1,0,0,0,680,693,3,102,51,0,681,693,3,108,54,0,682,693,
  	3,104,52,0,683,693,3,106,53,0,684,693,3,100,50,0,685,693,3,14,7,0,686,
  	693,5,102,0,0,687,693,5,100,0,0,688,693,5,107,0,0,689,693,5,108,0,0,690,
  	693,5,103,0,0,691,693,3,98,49,0,692,680,1,0,0,0,692,681,1,0,0,0,692,682,
  	1,0,0,0,692,683,1,0,0,0,692,684,1,0,0,0,692,685,1,0,0,0,692,686,1,0,0,
  	0,692,687,1,0,0,0,692,688,1,0,0,0,692,689,1,0,0,0,692,690,1,0,0,0,692,
  	691,1,0,0,0,693,97,1,0,0,0,694,697,5,31,0,0,695,696,5,104,0,0,696,698,
  	5,29,0,0,697,695,1,0,0,0,697,698,1,0,0,0,698,699,1,0,0,0,699,710,5,104,
  	0,0,700,701,5,92,0,0,701,702,5,104,0,0,702,703,5,27,0,0,703,704,5,106,
  	0,0,704,705,5,28,0,0,705,706,5,29,0,0,706,710,5,104,0,0,707,708,5,31,
  	0,0,708,710,3,94,47,0,709,694,1,0,0,0,709,700,1,0,0,0,709,707,1,0,0,0,
  	710,99,1,0,0,0,711,712,7,18,0,0,712,101,1,0,0,0,713,714,5,20,0,0,714,
  	715,3,52,26,0,715,716,5,13,0,0,716,717,3,52,26,0,717,718,5,13,0,0,718,
  	719,3,52,26,0,719,720,5,21,0,0,720,103,1,0,0,0,721,743,5,23,0,0,722,723,
  	3,52,26,0,723,724,5,12,0,0,724,726,1,0,0,0,725,722,1,0,0,0,725,726,1,
  	0,0,0,726,727,1,0,0,0,727,737,3,52,26,0,728,732,5,13,0,0,729,730,3,52,
  	26,0,730,731,5,12,0,0,731,733,1,0,0,0,732,729,1,0,0,0,732,733,1,0,0,0,
  	733,734,1,0,0,0,734,736,3,52,26,0,735,728,1,0,0,0,736,739,1,0,0,0,737,
  	735,1,0,0,0,737,738,1,0,0,0,738,741,1,0,0,0,739,737,1,0,0,0,740,742,5,
  	13,0,0,741,740,1,0,0,0,741,742,1,0,0,0,742,744,1,0,0,0,743,725,1,0,0,
  	0,743,744,1,0,0,0,744,745,1,0,0,0,745,746,5,24,0,0,746,105,1,0,0,0,747,
  	769,5,14,0,0,748,751,5,105,0,0,749,751,3,52,26,0,750,748,1,0,0,0,750,
  	749,1,0,0,0,751,752,1,0,0,0,752,753,5,12,0,0,753,763,3,52,26,0,754,757,
  	5,13,0,0,755,758,5,105,0,0,756,758,3,52,26,0,757,755,1,0,0,0,757,756,
  	1,0,0,0,758,759,1,0,0,0,759,760,5,12,0,0,760,762,3,52,26,0,761,754,1,
  	0,0,0,762,765,1,0,0,0,763,761,1,0,0,0,763,764,1,0,0,0,764,767,1,0,0,0,
  	765,763,1,0,0,0,766,768,5,13,0,0,767,766,1,0,0,0,767,768,1,0,0,0,768,
  	770,1,0,0,0,769,750,1,0,0,0,769,770,1,0,0,0,770,771,1,0,0,0,771,772,5,
  	15,0,0,772,107,1,0,0,0,773,774,5,93,0,0,774,775,5,104,0,0,775,784,5,20,
  	0,0,776,781,3,52,26,0,777,778,5,13,0,0,778,780,3,52,26,0,779,777,1,0,
  	0,0,780,783,1,0,0,0,781,779,1,0,0,0,781,782,1,0,0,0,782,785,1,0,0,0,783,
  	781,1,0,0,0,784,776,1,0,0,0,784,785,1,0,0,0,785,786,1,0,0,0,786,787,5,
  	21,0,0,787,109,1,0,0,0,788,789,5,104,0,0,789,111,1,0,0,0,87,120,122,153,
  	156,162,168,177,182,185,192,194,200,203,206,212,223,230,233,236,241,244,
  	251,254,260,267,277,283,291,295,299,322,336,346,352,357,367,376,385,402,
  	407,417,431,442,453,464,475,486,497,508,519,530,541,552,564,568,573,582,
  	589,596,600,609,612,619,623,632,640,648,653,659,668,675,677,692,697,709,
  	725,732,737,741,743,750,757,763,767,769,781,784
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

std::vector<gscParser::Class_defContext *> gscParser::ProgContext::class_def() {
  return getRuleContexts<gscParser::Class_defContext>();
}

gscParser::Class_defContext* gscParser::ProgContext::class_def(size_t i) {
  return getRuleContext<gscParser::Class_defContext>(i);
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
    setState(122);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 73272286) != 0) || _la == gscParser::IDENTIFIER) {
      setState(120);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case gscParser::T__0: {
          setState(112);
          match(gscParser::T__0);
          break;
        }

        case gscParser::T__1: {
          setState(113);
          match(gscParser::T__1);
          break;
        }

        case gscParser::T__16:
        case gscParser::T__17:
        case gscParser::T__18:
        case gscParser::T__19:
        case gscParser::T__21:
        case gscParser::T__25:
        case gscParser::IDENTIFIER: {
          setState(114);
          function();
          break;
        }

        case gscParser::T__2:
        case gscParser::T__3: {
          setState(115);
          include();
          break;
        }

        case gscParser::T__5: {
          setState(116);
          namespace_();
          break;
        }

        case gscParser::T__6: {
          setState(117);
          filenamespace();
          break;
        }

        case gscParser::T__7:
        case gscParser::T__8: {
          setState(118);
          constexpr_();
          break;
        }

        case gscParser::T__10: {
          setState(119);
          class_def();
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(124);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(125);
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
    setState(127);
    _la = _input->LA(1);
    if (!(_la == gscParser::T__2

    || _la == gscParser::T__3)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(128);
    _la = _input->LA(1);
    if (!(_la == gscParser::IDENTIFIER

    || _la == gscParser::PATH)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(129);
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
    setState(131);
    match(gscParser::T__5);
    setState(132);
    match(gscParser::IDENTIFIER);
    setState(133);
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
    setState(135);
    match(gscParser::T__6);
    setState(136);
    _la = _input->LA(1);
    if (!(_la == gscParser::IDENTIFIER

    || _la == gscParser::PATH)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(137);
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
    setState(139);
    _la = _input->LA(1);
    if (!(_la == gscParser::T__7

    || _la == gscParser::T__8)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(140);
    match(gscParser::IDENTIFIER);
    setState(141);
    match(gscParser::T__9);
    setState(142);
    expression();
    setState(143);
    match(gscParser::T__4);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Class_defContext ------------------------------------------------------------------

gscParser::Class_defContext::Class_defContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> gscParser::Class_defContext::IDENTIFIER() {
  return getTokens(gscParser::IDENTIFIER);
}

tree::TerminalNode* gscParser::Class_defContext::IDENTIFIER(size_t i) {
  return getToken(gscParser::IDENTIFIER, i);
}

std::vector<gscParser::Class_varContext *> gscParser::Class_defContext::class_var() {
  return getRuleContexts<gscParser::Class_varContext>();
}

gscParser::Class_varContext* gscParser::Class_defContext::class_var(size_t i) {
  return getRuleContext<gscParser::Class_varContext>(i);
}

std::vector<gscParser::FunctionContext *> gscParser::Class_defContext::function() {
  return getRuleContexts<gscParser::FunctionContext>();
}

gscParser::FunctionContext* gscParser::Class_defContext::function(size_t i) {
  return getRuleContext<gscParser::FunctionContext>(i);
}


size_t gscParser::Class_defContext::getRuleIndex() const {
  return gscParser::RuleClass_def;
}


std::any gscParser::Class_defContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitClass_def(this);
  else
    return visitor->visitChildren(this);
}

gscParser::Class_defContext* gscParser::class_def() {
  Class_defContext *_localctx = _tracker.createInstance<Class_defContext>(_ctx, getState());
  enterRule(_localctx, 10, gscParser::RuleClass_def);
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
    setState(145);
    match(gscParser::T__10);
    setState(146);
    match(gscParser::IDENTIFIER);
    setState(156);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__11) {
      setState(147);
      match(gscParser::T__11);
      setState(148);
      match(gscParser::IDENTIFIER);
      setState(153);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == gscParser::T__12) {
        setState(149);
        match(gscParser::T__12);
        setState(150);
        match(gscParser::IDENTIFIER);
        setState(155);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
    }
    setState(158);
    match(gscParser::T__13);
    setState(162);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == gscParser::T__15) {
      setState(159);
      class_var();
      setState(164);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(168);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 73269248) != 0) || _la == gscParser::IDENTIFIER) {
      setState(165);
      function();
      setState(170);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(171);
    match(gscParser::T__14);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Class_varContext ------------------------------------------------------------------

gscParser::Class_varContext::Class_varContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* gscParser::Class_varContext::IDENTIFIER() {
  return getToken(gscParser::IDENTIFIER, 0);
}

gscParser::ExpressionContext* gscParser::Class_varContext::expression() {
  return getRuleContext<gscParser::ExpressionContext>(0);
}


size_t gscParser::Class_varContext::getRuleIndex() const {
  return gscParser::RuleClass_var;
}


std::any gscParser::Class_varContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitClass_var(this);
  else
    return visitor->visitChildren(this);
}

gscParser::Class_varContext* gscParser::class_var() {
  Class_varContext *_localctx = _tracker.createInstance<Class_varContext>(_ctx, getState());
  enterRule(_localctx, 12, gscParser::RuleClass_var);
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
    match(gscParser::T__15);
    setState(174);
    match(gscParser::IDENTIFIER);
    setState(177);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__9) {
      setState(175);
      match(gscParser::T__9);
      setState(176);
      expression();
    }
    setState(179);
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
  enterRule(_localctx, 14, gscParser::RuleFunction);
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
    setState(182);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__16) {
      setState(181);
      match(gscParser::T__16);
    }
    setState(185);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__17) {
      setState(184);
      match(gscParser::T__17);
    }
    setState(194);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__18) {
      setState(187);
      match(gscParser::T__18);
      setState(192);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 9, _ctx)) {
      case 1: {
        setState(188);
        match(gscParser::T__19);
        setState(189);
        number();
        setState(190);
        match(gscParser::T__20);
        break;
      }

      default:
        break;
      }
    }
    setState(200);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__21) {
      setState(196);
      match(gscParser::T__21);
      setState(197);
      match(gscParser::T__22);
      setState(198);
      match(gscParser::IDENTIFIER);
      setState(199);
      match(gscParser::T__23);
    }
    setState(203);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__25) {
      setState(202);
      detour_info();
    }
    setState(206);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::IDENTIFIER) {
      setState(205);
      match(gscParser::IDENTIFIER);
    }
    setState(208);
    match(gscParser::T__19);
    setState(209);
    param_list();
    setState(210);
    match(gscParser::T__20);
    setState(212);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__24) {
      setState(211);
      match(gscParser::T__24);
    }
    setState(214);
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
  enterRule(_localctx, 16, gscParser::RuleDetour_info);
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
    setState(216);
    match(gscParser::T__25);
    setState(217);
    match(gscParser::IDENTIFIER);
    setState(223);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__26) {
      setState(218);
      match(gscParser::T__26);
      setState(219);
      match(gscParser::PATH);
      setState(220);
      match(gscParser::T__27);
      setState(221);
      match(gscParser::T__28);
      setState(222);
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
  enterRule(_localctx, 18, gscParser::RuleParam_list);
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
    setState(233);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 7516192768) != 0) || _la == gscParser::IDENTIFIER) {
      setState(225);
      param_val();
      setState(230);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == gscParser::T__12) {
        setState(226);
        match(gscParser::T__12);
        setState(227);
        param_val();
        setState(232);
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
  enterRule(_localctx, 20, gscParser::RuleParam_val);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(244);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case gscParser::T__29:
      case gscParser::T__30:
      case gscParser::IDENTIFIER: {
        enterOuterAlt(_localctx, 1);
        setState(236);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == gscParser::T__29

        || _la == gscParser::T__30) {
          setState(235);
          _la = _input->LA(1);
          if (!(_la == gscParser::T__29

          || _la == gscParser::T__30)) {
          _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
        }
        setState(238);
        match(gscParser::IDENTIFIER);
        setState(241);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == gscParser::T__9) {
          setState(239);
          match(gscParser::T__9);
          setState(240);
          expression();
        }
        break;
      }

      case gscParser::T__31: {
        enterOuterAlt(_localctx, 2);
        setState(243);
        match(gscParser::T__31);
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
  enterRule(_localctx, 22, gscParser::RuleExpression_list);
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
    setState(254);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 2229157888) != 0) || ((((_la - 80) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 80)) & 434058191) != 0)) {
      setState(246);
      expression();
      setState(251);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == gscParser::T__12) {
        setState(247);
        match(gscParser::T__12);
        setState(248);
        expression();
        setState(253);
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
  enterRule(_localctx, 24, gscParser::RuleStatement_block);
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
    setState(256);
    match(gscParser::T__13);
    setState(260);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 274184351399938) != 0) || ((((_la - 80) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 80)) & 436155343) != 0)) {
      setState(257);
      statement();
      setState(262);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(263);
    match(gscParser::T__14);
   
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
  enterRule(_localctx, 26, gscParser::RuleStatement);

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
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 24, _ctx)) {
    case 1: {
      setState(265);
      match(gscParser::IDENTIFIER);
      setState(266);
      match(gscParser::T__11);
      break;
    }

    default:
      break;
    }
    setState(277);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 25, _ctx)) {
    case 1: {
      setState(269);
      statement_block();
      break;
    }

    case 2: {
      setState(270);
      statement_dev_block();
      break;
    }

    case 3: {
      setState(271);
      statement_for();
      break;
    }

    case 4: {
      setState(272);
      statement_if();
      break;
    }

    case 5: {
      setState(273);
      statement_while();
      break;
    }

    case 6: {
      setState(274);
      statement_foreach();
      break;
    }

    case 7: {
      setState(275);
      statement_inst();
      break;
    }

    case 8: {
      setState(276);
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
  enterRule(_localctx, 28, gscParser::RuleStatement_dev_block);
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
    match(gscParser::T__0);
    setState(283);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 274184351399938) != 0) || ((((_la - 80) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 80)) & 436155343) != 0)) {
      setState(280);
      statement();
      setState(285);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(286);
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
  enterRule(_localctx, 30, gscParser::RuleStatement_for);
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
    setState(288);
    match(gscParser::T__32);
    setState(289);
    match(gscParser::T__19);
    setState(291);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 2229157888) != 0) || ((((_la - 80) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 80)) & 434058191) != 0)) {
      setState(290);
      expression();
    }
    setState(293);
    match(gscParser::T__4);
    setState(295);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 2229157888) != 0) || ((((_la - 80) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 80)) & 434058191) != 0)) {
      setState(294);
      expression();
    }
    setState(297);
    match(gscParser::T__4);
    setState(299);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 2229157888) != 0) || ((((_la - 80) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 80)) & 434058191) != 0)) {
      setState(298);
      expression();
    }
    setState(301);
    match(gscParser::T__20);
    setState(302);
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
  enterRule(_localctx, 32, gscParser::RuleStatement_while);

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
    match(gscParser::T__33);
    setState(305);
    match(gscParser::T__19);
    setState(306);
    expression();
    setState(307);
    match(gscParser::T__20);
    setState(308);
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
  enterRule(_localctx, 34, gscParser::RuleStatement_dowhile);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(310);
    match(gscParser::T__34);
    setState(311);
    statement();
    setState(312);
    match(gscParser::T__33);
    setState(313);
    match(gscParser::T__19);
    setState(314);
    expression();
    setState(315);
    match(gscParser::T__20);
   
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
  enterRule(_localctx, 36, gscParser::RuleStatement_foreach);
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
    setState(317);
    match(gscParser::T__35);
    setState(318);
    match(gscParser::T__19);
    setState(319);
    match(gscParser::IDENTIFIER);
    setState(322);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__12) {
      setState(320);
      match(gscParser::T__12);
      setState(321);
      match(gscParser::IDENTIFIER);
    }
    setState(324);
    match(gscParser::T__36);
    setState(325);
    expression();
    setState(326);
    match(gscParser::T__20);
    setState(327);
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
  enterRule(_localctx, 38, gscParser::RuleStatement_if);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(329);
    match(gscParser::T__37);
    setState(330);
    match(gscParser::T__19);
    setState(331);
    expression();
    setState(332);
    match(gscParser::T__20);
    setState(333);
    statement();
    setState(336);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 31, _ctx)) {
    case 1: {
      setState(334);
      match(gscParser::T__38);
      setState(335);
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
  enterRule(_localctx, 40, gscParser::RuleStatement_switch);
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
    setState(338);
    match(gscParser::T__39);
    setState(339);
    match(gscParser::T__19);
    setState(340);
    expression();
    setState(341);
    match(gscParser::T__20);
    setState(342);
    match(gscParser::T__13);
    setState(355); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(346);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case gscParser::T__40: {
          setState(343);
          match(gscParser::T__40);
          setState(344);
          const_expr();
          break;
        }

        case gscParser::T__41: {
          setState(345);
          match(gscParser::T__41);
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(348);
      match(gscParser::T__11);
      setState(352);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while ((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & 274184351399938) != 0) || ((((_la - 80) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 80)) & 436155343) != 0)) {
        setState(349);
        statement();
        setState(354);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
      setState(357); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while (_la == gscParser::T__40

    || _la == gscParser::T__41);
    setState(359);
    match(gscParser::T__14);
   
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
  enterRule(_localctx, 42, gscParser::RuleStatement_inst);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(367);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 35, _ctx)) {
    case 1: {
      setState(361);
      expression();
      break;
    }

    case 2: {
      setState(362);
      operator_inst();
      break;
    }

    case 3: {
      setState(363);
      statement_dowhile();
      break;
    }

    case 4: {
      setState(364);
      function_call_exp();
      break;
    }

    case 5: {
      setState(365);
      nop_def();
      break;
    }

    case 6: {
      setState(366);
      devop_def();
      break;
    }

    default:
      break;
    }
    setState(369);
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
  enterRule(_localctx, 44, gscParser::RuleNop_def);
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
    setState(371);
    _la = _input->LA(1);
    if (!(_la == gscParser::T__42

    || _la == gscParser::T__43)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(376);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__19) {
      setState(372);
      match(gscParser::T__19);
      setState(373);
      number();
      setState(374);
      match(gscParser::T__20);
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
  enterRule(_localctx, 46, gscParser::RuleDevop_def);
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
    setState(378);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 246290604621824) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(379);
    match(gscParser::T__19);
    setState(380);
    number();
    setState(381);
    match(gscParser::T__20);
   
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
  enterRule(_localctx, 48, gscParser::RuleFunction_component);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(402);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 38, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(385);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 37, _ctx)) {
      case 1: {
        setState(383);
        match(gscParser::IDENTIFIER);
        setState(384);
        match(gscParser::T__28);
        break;
      }

      default:
        break;
      }
      setState(387);
      match(gscParser::IDENTIFIER);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(388);
      match(gscParser::T__22);
      setState(389);
      match(gscParser::T__22);
      setState(390);
      expression();
      setState(391);
      match(gscParser::T__23);
      setState(392);
      match(gscParser::T__23);
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(394);
      match(gscParser::T__22);
      setState(395);
      match(gscParser::T__22);
      setState(396);
      expression();
      setState(397);
      match(gscParser::T__23);
      setState(398);
      match(gscParser::T__23);
      setState(399);
      match(gscParser::T__47);
      setState(400);
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
  enterRule(_localctx, 50, gscParser::RuleOperator_inst);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(404);
    match(gscParser::BUILTIN);
    setState(407);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 39, _ctx)) {
    case 1: {
      setState(405);
      match(gscParser::IDENTIFIER);
      break;
    }

    case 2: {
      setState(406);
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
  enterRule(_localctx, 52, gscParser::RuleExpression);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(417);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 40, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(409);
      set_expression();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(410);
      expression0(0);
      setState(411);
      match(gscParser::T__48);
      setState(412);
      expression();
      setState(413);
      match(gscParser::T__11);
      setState(414);
      expression();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(416);
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
  enterRule(_localctx, 54, gscParser::RuleSet_expression);
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
    setState(419);
    left_value(0);
    setState(420);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 2304717109306852352) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(421);
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
  size_t startState = 56;
  enterRecursionRule(_localctx, 56, gscParser::RuleExpression0, precedence);

    

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
    setState(424);
    expression1(0);
    _ctx->stop = _input->LT(-1);
    setState(431);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 41, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression0Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression0);
        setState(426);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(427);
        match(gscParser::T__60);
        setState(428);
        expression1(0); 
      }
      setState(433);
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
  size_t startState = 58;
  enterRecursionRule(_localctx, 58, gscParser::RuleExpression1, precedence);

    

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
    setState(435);
    expression2(0);
    _ctx->stop = _input->LT(-1);
    setState(442);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 42, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression1Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression1);
        setState(437);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(438);
        match(gscParser::T__61);
        setState(439);
        expression2(0); 
      }
      setState(444);
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
  size_t startState = 60;
  enterRecursionRule(_localctx, 60, gscParser::RuleExpression2, precedence);

    

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
    setState(446);
    expression3(0);
    _ctx->stop = _input->LT(-1);
    setState(453);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 43, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression2Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression2);
        setState(448);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(449);
        match(gscParser::T__62);
        setState(450);
        expression3(0); 
      }
      setState(455);
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
  size_t startState = 62;
  enterRecursionRule(_localctx, 62, gscParser::RuleExpression3, precedence);

    

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
    setState(457);
    expression4(0);
    _ctx->stop = _input->LT(-1);
    setState(464);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 44, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression3Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression3);
        setState(459);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(460);
        match(gscParser::T__63);
        setState(461);
        expression4(0); 
      }
      setState(466);
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
  size_t startState = 64;
  enterRecursionRule(_localctx, 64, gscParser::RuleExpression4, precedence);

    

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
    setState(468);
    expression5(0);
    _ctx->stop = _input->LT(-1);
    setState(475);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 45, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression4Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression4);
        setState(470);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(471);
        match(gscParser::T__64);
        setState(472);
        expression5(0); 
      }
      setState(477);
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
  size_t startState = 66;
  enterRecursionRule(_localctx, 66, gscParser::RuleExpression5, precedence);

    

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
    setState(479);
    expression6(0);
    _ctx->stop = _input->LT(-1);
    setState(486);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 46, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression5Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression5);
        setState(481);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(482);
        match(gscParser::T__30);
        setState(483);
        expression6(0); 
      }
      setState(488);
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
  size_t startState = 68;
  enterRecursionRule(_localctx, 68, gscParser::RuleExpression6, precedence);

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
    setState(490);
    expression7(0);
    _ctx->stop = _input->LT(-1);
    setState(497);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 47, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression6Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression6);
        setState(492);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(493);
        _la = _input->LA(1);
        if (!(((((_la - 66) & ~ 0x3fULL) == 0) &&
          ((1ULL << (_la - 66)) & 15) != 0))) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(494);
        expression7(0); 
      }
      setState(499);
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
  size_t startState = 70;
  enterRecursionRule(_localctx, 70, gscParser::RuleExpression7, precedence);

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
    setState(501);
    expression8(0);
    _ctx->stop = _input->LT(-1);
    setState(508);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 48, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression7Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression7);
        setState(503);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(504);
        _la = _input->LA(1);
        if (!(((((_la - 27) & ~ 0x3fULL) == 0) &&
          ((1ULL << (_la - 27)) & 131941395333123) != 0))) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(505);
        expression8(0); 
      }
      setState(510);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 48, _ctx);
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
  size_t startState = 72;
  enterRecursionRule(_localctx, 72, gscParser::RuleExpression8, precedence);

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
    setState(512);
    expression9(0);
    _ctx->stop = _input->LT(-1);
    setState(519);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 49, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression8Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression8);
        setState(514);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(515);
        _la = _input->LA(1);
        if (!(_la == gscParser::T__26

        || _la == gscParser::T__69)) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(516);
        expression9(0); 
      }
      setState(521);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 49, _ctx);
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
  size_t startState = 74;
  enterRecursionRule(_localctx, 74, gscParser::RuleExpression9, precedence);

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
    setState(523);
    expression10(0);
    _ctx->stop = _input->LT(-1);
    setState(530);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 50, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression9Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression9);
        setState(525);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(526);
        _la = _input->LA(1);
        if (!(_la == gscParser::T__73

        || _la == gscParser::T__74)) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(527);
        expression10(0); 
      }
      setState(532);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 50, _ctx);
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
  size_t startState = 76;
  enterRecursionRule(_localctx, 76, gscParser::RuleExpression10, precedence);

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
    setState(534);
    expression11(0);
    _ctx->stop = _input->LT(-1);
    setState(541);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 51, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression10Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression10);
        setState(536);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(537);
        _la = _input->LA(1);
        if (!(_la == gscParser::T__75

        || _la == gscParser::T__76)) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(538);
        expression11(0); 
      }
      setState(543);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 51, _ctx);
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
  size_t startState = 78;
  enterRecursionRule(_localctx, 78, gscParser::RuleExpression11, precedence);

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
    setState(545);
    expression12();
    _ctx->stop = _input->LT(-1);
    setState(552);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 52, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression11Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression11);
        setState(547);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(548);
        _la = _input->LA(1);
        if (!(((((_la - 30) & ~ 0x3fULL) == 0) &&
          ((1ULL << (_la - 30)) & 844424930131969) != 0))) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(549);
        expression12(); 
      }
      setState(554);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 52, _ctx);
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
  enterRule(_localctx, 80, gscParser::RuleExpression12);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(564);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 53, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(555);
      _la = _input->LA(1);
      if (!(_la == gscParser::T__79

      || _la == gscParser::T__80)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(556);
      expression13();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(557);
      _la = _input->LA(1);
      if (!(_la == gscParser::T__81

      || _la == gscParser::T__82)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(558);
      left_value(0);
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(559);
      left_value(0);
      setState(560);
      _la = _input->LA(1);
      if (!(_la == gscParser::T__81

      || _la == gscParser::T__82)) {
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
      setState(562);
      is_expression();
      break;
    }

    case 5: {
      enterOuterAlt(_localctx, 5);
      setState(563);
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
  enterRule(_localctx, 82, gscParser::RuleExpression13);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(568);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 54, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(566);
      function_call_exp();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(567);
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
  enterRule(_localctx, 84, gscParser::RuleExpression14);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(573);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 55, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(570);
      const_expr();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(571);
      expression15();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(572);
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
  enterRule(_localctx, 86, gscParser::RuleExpression15);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(575);
    match(gscParser::T__19);
    setState(576);
    expression();
    setState(577);
    match(gscParser::T__20);
   
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
  enterRule(_localctx, 88, gscParser::RuleIs_expression);
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
    setState(579);
    expression13();
    setState(580);
    match(gscParser::T__83);
    setState(582);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__84) {
      setState(581);
      match(gscParser::T__84);
    }
    setState(584);
    _la = _input->LA(1);
    if (!(_la == gscParser::T__16 || ((((_la - 102) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 102)) & 7) != 0))) {
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
  enterRule(_localctx, 90, gscParser::RuleFunction_call_exp);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(596);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 58, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(586);
      function_call(0);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(587);
      expression14();
      setState(589);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (((((_la - 86) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 86)) & 15) != 0)) {
        setState(588);
        _la = _input->LA(1);
        if (!(((((_la - 86) & ~ 0x3fULL) == 0) &&
          ((1ULL << (_la - 86)) & 15) != 0))) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
      }
      setState(591);
      function_component();
      setState(592);
      match(gscParser::T__19);
      setState(593);
      expression_list();
      setState(594);
      match(gscParser::T__20);
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
  size_t startState = 92;
  enterRecursionRule(_localctx, 92, gscParser::RuleFunction_call, precedence);

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
    setState(619);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 62, _ctx)) {
    case 1: {
      setState(600);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (((((_la - 86) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 86)) & 15) != 0)) {
        setState(599);
        _la = _input->LA(1);
        if (!(((((_la - 86) & ~ 0x3fULL) == 0) &&
          ((1ULL << (_la - 86)) & 15) != 0))) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
      }
      setState(602);
      function_component();
      setState(603);
      match(gscParser::T__19);
      setState(604);
      expression_list();
      setState(605);
      match(gscParser::T__20);
      break;
    }

    case 2: {
      setState(609);
      _errHandler->sync(this);
      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 60, _ctx)) {
      case 1: {
        setState(607);
        const_expr();
        break;
      }

      case 2: {
        setState(608);
        expression15();
        break;
      }

      default:
        break;
      }
      setState(612);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (((((_la - 86) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 86)) & 15) != 0)) {
        setState(611);
        _la = _input->LA(1);
        if (!(((((_la - 86) & ~ 0x3fULL) == 0) &&
          ((1ULL << (_la - 86)) & 15) != 0))) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
      }
      setState(614);
      function_component();
      setState(615);
      match(gscParser::T__19);
      setState(616);
      expression_list();
      setState(617);
      match(gscParser::T__20);
      break;
    }

    default:
      break;
    }
    _ctx->stop = _input->LT(-1);
    setState(632);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 64, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Function_callContext>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleFunction_call);
        setState(621);

        if (!(precpred(_ctx, 1))) throw FailedPredicateException(this, "precpred(_ctx, 1)");
        setState(623);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (((((_la - 86) & ~ 0x3fULL) == 0) &&
          ((1ULL << (_la - 86)) & 15) != 0)) {
          setState(622);
          _la = _input->LA(1);
          if (!(((((_la - 86) & ~ 0x3fULL) == 0) &&
            ((1ULL << (_la - 86)) & 15) != 0))) {
          _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
        }
        setState(625);
        function_component();
        setState(626);
        match(gscParser::T__19);
        setState(627);
        expression_list();
        setState(628);
        match(gscParser::T__20); 
      }
      setState(634);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 64, _ctx);
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
  size_t startState = 94;
  enterRecursionRule(_localctx, 94, gscParser::RuleLeft_value, precedence);

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
    setState(659);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 68, _ctx)) {
    case 1: {
      setState(636);
      idf();
      break;
    }

    case 2: {
      setState(640);
      _errHandler->sync(this);
      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 65, _ctx)) {
      case 1: {
        setState(637);
        function_call(0);
        break;
      }

      case 2: {
        setState(638);
        const_expr();
        break;
      }

      case 3: {
        setState(639);
        expression15();
        break;
      }

      default:
        break;
      }
      setState(642);
      _la = _input->LA(1);
      if (!(_la == gscParser::T__89

      || _la == gscParser::T__90)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(648);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case gscParser::IDENTIFIER: {
          setState(643);
          idf();
          break;
        }

        case gscParser::T__19: {
          setState(644);
          match(gscParser::T__19);
          setState(645);
          expression();
          setState(646);
          match(gscParser::T__20);
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      break;
    }

    case 3: {
      setState(653);
      _errHandler->sync(this);
      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 67, _ctx)) {
      case 1: {
        setState(650);
        function_call(0);
        break;
      }

      case 2: {
        setState(651);
        const_expr();
        break;
      }

      case 3: {
        setState(652);
        expression15();
        break;
      }

      default:
        break;
      }
      setState(655);
      match(gscParser::T__22);
      setState(656);
      expression();
      setState(657);
      match(gscParser::T__23);
      break;
    }

    default:
      break;
    }
    _ctx->stop = _input->LT(-1);
    setState(677);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 71, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        setState(675);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 70, _ctx)) {
        case 1: {
          _localctx = _tracker.createInstance<Left_valueContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleLeft_value);
          setState(661);

          if (!(precpred(_ctx, 4))) throw FailedPredicateException(this, "precpred(_ctx, 4)");
          setState(662);
          _la = _input->LA(1);
          if (!(_la == gscParser::T__89

          || _la == gscParser::T__90)) {
          _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(668);
          _errHandler->sync(this);
          switch (_input->LA(1)) {
            case gscParser::IDENTIFIER: {
              setState(663);
              idf();
              break;
            }

            case gscParser::T__19: {
              setState(664);
              match(gscParser::T__19);
              setState(665);
              expression();
              setState(666);
              match(gscParser::T__20);
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
          setState(670);

          if (!(precpred(_ctx, 3))) throw FailedPredicateException(this, "precpred(_ctx, 3)");
          setState(671);
          match(gscParser::T__22);
          setState(672);
          expression();
          setState(673);
          match(gscParser::T__23);
          break;
        }

        default:
          break;
        } 
      }
      setState(679);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 71, _ctx);
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

gscParser::Class_initContext* gscParser::Const_exprContext::class_init() {
  return getRuleContext<gscParser::Class_initContext>(0);
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
  enterRule(_localctx, 96, gscParser::RuleConst_expr);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(692);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 72, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(680);
      vector_value();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(681);
      class_init();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(682);
      array_def();
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(683);
      struct_def();
      break;
    }

    case 5: {
      enterOuterAlt(_localctx, 5);
      setState(684);
      number();
      break;
    }

    case 6: {
      enterOuterAlt(_localctx, 6);
      setState(685);
      function();
      break;
    }

    case 7: {
      enterOuterAlt(_localctx, 7);
      setState(686);
      match(gscParser::BOOL_VALUE);
      break;
    }

    case 8: {
      enterOuterAlt(_localctx, 8);
      setState(687);
      match(gscParser::FLOATVAL);
      break;
    }

    case 9: {
      enterOuterAlt(_localctx, 9);
      setState(688);
      match(gscParser::STRING);
      break;
    }

    case 10: {
      enterOuterAlt(_localctx, 10);
      setState(689);
      match(gscParser::HASHSTRING);
      break;
    }

    case 11: {
      enterOuterAlt(_localctx, 11);
      setState(690);
      match(gscParser::UNDEFINED_VALUE);
      break;
    }

    case 12: {
      enterOuterAlt(_localctx, 12);
      setState(691);
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
  enterRule(_localctx, 98, gscParser::RuleFunction_ref);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(709);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 74, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(694);
      match(gscParser::T__30);
      setState(697);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 73, _ctx)) {
      case 1: {
        setState(695);
        match(gscParser::IDENTIFIER);
        setState(696);
        match(gscParser::T__28);
        break;
      }

      default:
        break;
      }
      setState(699);
      match(gscParser::IDENTIFIER);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(700);
      match(gscParser::T__91);
      setState(701);
      match(gscParser::IDENTIFIER);
      setState(702);
      match(gscParser::T__26);
      setState(703);
      match(gscParser::PATH);
      setState(704);
      match(gscParser::T__27);
      setState(705);
      match(gscParser::T__28);
      setState(706);
      match(gscParser::IDENTIFIER);
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(707);
      match(gscParser::T__30);
      setState(708);
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
  enterRule(_localctx, 100, gscParser::RuleNumber);
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
    setState(711);
    _la = _input->LA(1);
    if (!(((((_la - 96) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 96)) & 271) != 0))) {
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
  enterRule(_localctx, 102, gscParser::RuleVector_value);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(713);
    match(gscParser::T__19);
    setState(714);
    expression();
    setState(715);
    match(gscParser::T__12);
    setState(716);
    expression();
    setState(717);
    match(gscParser::T__12);
    setState(718);
    expression();
    setState(719);
    match(gscParser::T__20);
   
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
  enterRule(_localctx, 104, gscParser::RuleArray_def);
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
    setState(721);
    match(gscParser::T__22);
    setState(743);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 2229157888) != 0) || ((((_la - 80) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 80)) & 434058191) != 0)) {
      setState(725);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 75, _ctx)) {
      case 1: {
        setState(722);
        expression();
        setState(723);
        match(gscParser::T__11);
        break;
      }

      default:
        break;
      }
      setState(727);
      expression();
      setState(737);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 77, _ctx);
      while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
        if (alt == 1) {
          setState(728);
          match(gscParser::T__12);
          setState(732);
          _errHandler->sync(this);

          switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 76, _ctx)) {
          case 1: {
            setState(729);
            expression();
            setState(730);
            match(gscParser::T__11);
            break;
          }

          default:
            break;
          }
          setState(734);
          expression(); 
        }
        setState(739);
        _errHandler->sync(this);
        alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 77, _ctx);
      }
      setState(741);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == gscParser::T__12) {
        setState(740);
        match(gscParser::T__12);
      }
    }
    setState(745);
    match(gscParser::T__23);
   
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
  enterRule(_localctx, 106, gscParser::RuleStruct_def);
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
    setState(747);
    match(gscParser::T__13);
    setState(769);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 2229157888) != 0) || ((((_la - 80) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 80)) & 467612623) != 0)) {
      setState(750);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case gscParser::STRUCT_IDENTIFIER: {
          setState(748);
          match(gscParser::STRUCT_IDENTIFIER);
          break;
        }

        case gscParser::T__13:
        case gscParser::T__16:
        case gscParser::T__17:
        case gscParser::T__18:
        case gscParser::T__19:
        case gscParser::T__21:
        case gscParser::T__22:
        case gscParser::T__25:
        case gscParser::T__30:
        case gscParser::T__79:
        case gscParser::T__80:
        case gscParser::T__81:
        case gscParser::T__82:
        case gscParser::T__85:
        case gscParser::T__86:
        case gscParser::T__87:
        case gscParser::T__88:
        case gscParser::T__91:
        case gscParser::T__92:
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
          setState(749);
          expression();
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(752);
      match(gscParser::T__11);
      setState(753);
      expression();
      setState(763);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 82, _ctx);
      while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
        if (alt == 1) {
          setState(754);
          match(gscParser::T__12);
          setState(757);
          _errHandler->sync(this);
          switch (_input->LA(1)) {
            case gscParser::STRUCT_IDENTIFIER: {
              setState(755);
              match(gscParser::STRUCT_IDENTIFIER);
              break;
            }

            case gscParser::T__13:
            case gscParser::T__16:
            case gscParser::T__17:
            case gscParser::T__18:
            case gscParser::T__19:
            case gscParser::T__21:
            case gscParser::T__22:
            case gscParser::T__25:
            case gscParser::T__30:
            case gscParser::T__79:
            case gscParser::T__80:
            case gscParser::T__81:
            case gscParser::T__82:
            case gscParser::T__85:
            case gscParser::T__86:
            case gscParser::T__87:
            case gscParser::T__88:
            case gscParser::T__91:
            case gscParser::T__92:
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
              setState(756);
              expression();
              break;
            }

          default:
            throw NoViableAltException(this);
          }
          setState(759);
          match(gscParser::T__11);
          setState(760);
          expression(); 
        }
        setState(765);
        _errHandler->sync(this);
        alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 82, _ctx);
      }
      setState(767);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == gscParser::T__12) {
        setState(766);
        match(gscParser::T__12);
      }
    }
    setState(771);
    match(gscParser::T__14);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Class_initContext ------------------------------------------------------------------

gscParser::Class_initContext::Class_initContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* gscParser::Class_initContext::IDENTIFIER() {
  return getToken(gscParser::IDENTIFIER, 0);
}

std::vector<gscParser::ExpressionContext *> gscParser::Class_initContext::expression() {
  return getRuleContexts<gscParser::ExpressionContext>();
}

gscParser::ExpressionContext* gscParser::Class_initContext::expression(size_t i) {
  return getRuleContext<gscParser::ExpressionContext>(i);
}


size_t gscParser::Class_initContext::getRuleIndex() const {
  return gscParser::RuleClass_init;
}


std::any gscParser::Class_initContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitClass_init(this);
  else
    return visitor->visitChildren(this);
}

gscParser::Class_initContext* gscParser::class_init() {
  Class_initContext *_localctx = _tracker.createInstance<Class_initContext>(_ctx, getState());
  enterRule(_localctx, 108, gscParser::RuleClass_init);
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
    setState(773);
    match(gscParser::T__92);
    setState(774);
    match(gscParser::IDENTIFIER);
    setState(775);
    match(gscParser::T__19);
    setState(784);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 2229157888) != 0) || ((((_la - 80) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 80)) & 434058191) != 0)) {
      setState(776);
      expression();
      setState(781);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == gscParser::T__12) {
        setState(777);
        match(gscParser::T__12);
        setState(778);
        expression();
        setState(783);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
    }
    setState(786);
    match(gscParser::T__20);
   
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
  enterRule(_localctx, 110, gscParser::RuleIdf);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(788);
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
    case 28: return expression0Sempred(antlrcpp::downCast<Expression0Context *>(context), predicateIndex);
    case 29: return expression1Sempred(antlrcpp::downCast<Expression1Context *>(context), predicateIndex);
    case 30: return expression2Sempred(antlrcpp::downCast<Expression2Context *>(context), predicateIndex);
    case 31: return expression3Sempred(antlrcpp::downCast<Expression3Context *>(context), predicateIndex);
    case 32: return expression4Sempred(antlrcpp::downCast<Expression4Context *>(context), predicateIndex);
    case 33: return expression5Sempred(antlrcpp::downCast<Expression5Context *>(context), predicateIndex);
    case 34: return expression6Sempred(antlrcpp::downCast<Expression6Context *>(context), predicateIndex);
    case 35: return expression7Sempred(antlrcpp::downCast<Expression7Context *>(context), predicateIndex);
    case 36: return expression8Sempred(antlrcpp::downCast<Expression8Context *>(context), predicateIndex);
    case 37: return expression9Sempred(antlrcpp::downCast<Expression9Context *>(context), predicateIndex);
    case 38: return expression10Sempred(antlrcpp::downCast<Expression10Context *>(context), predicateIndex);
    case 39: return expression11Sempred(antlrcpp::downCast<Expression11Context *>(context), predicateIndex);
    case 46: return function_callSempred(antlrcpp::downCast<Function_callContext *>(context), predicateIndex);
    case 47: return left_valueSempred(antlrcpp::downCast<Left_valueContext *>(context), predicateIndex);

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
