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
      "prog", "include", "precache", "namespace", "filenamespace", "constexpr", 
      "class_def", "class_var", "function", "detour_info", "param_list", 
      "param_val", "expression_list", "statement_block", "statement", "statement_dev_block", 
      "statement_for", "statement_while", "statement_dowhile", "statement_foreach", 
      "statement_if", "statement_switch", "statement_inst", "nop_def", "devop_def", 
      "function_component", "operator_inst", "array_unpack", "expression", 
      "set_expression", "expression0", "expression1", "expression2", "expression3", 
      "expression4", "expression5", "expression6", "expression7", "expression8", 
      "expression9", "expression10", "expression11", "expression12", "expression13", 
      "expression14", "expression15", "is_expression", "function_call_exp", 
      "function_call", "left_value", "const_expr", "const_expr_static", 
      "function_ref", "data_ref", "number", "vector_value", "array_def", 
      "struct_def", "class_init", "idf"
    },
    std::vector<std::string>{
      "", "'/#'", "'#/'", "'#include'", "'#using'", "';'", "'#precache'", 
      "'('", "','", "')'", "'#namespace'", "'#file'", "'#constexpr'", "'#define'", 
      "'='", "'class'", "':'", "'{'", "'}'", "'var'", "'function'", "'private'", 
      "'autoexec'", "'event_handler'", "'['", "']'", "'=>'", "'detour'", 
      "'<'", "'>'", "'::'", "'*'", "'&'", "'...'", "'for'", "'while'", "'do'", 
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
      "", "", "", "", "", "", "", "", "", "", "NEWLINE", "WHITESPACE", "INTEGER10", 
      "INTEGER16", "INTEGER8", "INTEGER2", "FLOATVAL", "BUILTIN", "BOOL_VALUE", 
      "UNDEFINED_VALUE", "IDENTIFIER", "STRUCT_IDENTIFIER", "PATH", "STRING", 
      "HASHSTRING"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,109,833,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,7,
  	14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,7,
  	21,2,22,7,22,2,23,7,23,2,24,7,24,2,25,7,25,2,26,7,26,2,27,7,27,2,28,7,
  	28,2,29,7,29,2,30,7,30,2,31,7,31,2,32,7,32,2,33,7,33,2,34,7,34,2,35,7,
  	35,2,36,7,36,2,37,7,37,2,38,7,38,2,39,7,39,2,40,7,40,2,41,7,41,2,42,7,
  	42,2,43,7,43,2,44,7,44,2,45,7,45,2,46,7,46,2,47,7,47,2,48,7,48,2,49,7,
  	49,2,50,7,50,2,51,7,51,2,52,7,52,2,53,7,53,2,54,7,54,2,55,7,55,2,56,7,
  	56,2,57,7,57,2,58,7,58,2,59,7,59,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,
  	5,0,130,8,0,10,0,12,0,133,9,0,1,0,1,0,1,1,1,1,1,1,1,1,1,2,1,2,1,2,1,2,
  	1,2,1,2,1,2,1,2,1,3,1,3,1,3,1,3,1,4,1,4,1,4,1,4,1,5,1,5,1,5,1,5,1,5,1,
  	5,1,6,1,6,1,6,1,6,1,6,1,6,5,6,169,8,6,10,6,12,6,172,9,6,3,6,174,8,6,1,
  	6,1,6,5,6,178,8,6,10,6,12,6,181,9,6,1,6,5,6,184,8,6,10,6,12,6,187,9,6,
  	1,6,1,6,1,7,1,7,1,7,1,7,3,7,195,8,7,1,7,1,7,1,8,3,8,200,8,8,1,8,3,8,203,
  	8,8,1,8,1,8,1,8,1,8,1,8,3,8,210,8,8,3,8,212,8,8,1,8,1,8,1,8,1,8,3,8,218,
  	8,8,1,8,3,8,221,8,8,1,8,3,8,224,8,8,1,8,1,8,1,8,1,8,3,8,230,8,8,1,8,1,
  	8,1,9,1,9,1,9,1,9,1,9,1,9,1,9,3,9,241,8,9,1,10,1,10,1,10,5,10,246,8,10,
  	10,10,12,10,249,9,10,3,10,251,8,10,1,11,3,11,254,8,11,1,11,1,11,1,11,
  	3,11,259,8,11,1,11,3,11,262,8,11,1,12,1,12,1,12,5,12,267,8,12,10,12,12,
  	12,270,9,12,3,12,272,8,12,1,13,1,13,5,13,276,8,13,10,13,12,13,279,9,13,
  	1,13,1,13,1,14,1,14,3,14,285,8,14,1,14,1,14,1,14,1,14,1,14,1,14,1,14,
  	1,14,3,14,295,8,14,1,15,1,15,5,15,299,8,15,10,15,12,15,302,9,15,1,15,
  	1,15,1,16,1,16,1,16,3,16,309,8,16,1,16,1,16,3,16,313,8,16,1,16,1,16,3,
  	16,317,8,16,1,16,1,16,1,16,1,17,1,17,1,17,1,17,1,17,1,17,1,18,1,18,1,
  	18,1,18,1,18,1,18,1,18,1,19,1,19,1,19,1,19,1,19,3,19,340,8,19,1,19,1,
  	19,1,19,1,19,1,19,1,20,1,20,1,20,1,20,1,20,1,20,1,20,3,20,354,8,20,1,
  	21,1,21,1,21,1,21,1,21,1,21,1,21,1,21,3,21,364,8,21,1,21,1,21,5,21,368,
  	8,21,10,21,12,21,371,9,21,4,21,373,8,21,11,21,12,21,374,1,21,1,21,1,22,
  	1,22,1,22,1,22,1,22,1,22,1,22,3,22,386,8,22,1,22,1,22,1,23,1,23,1,23,
  	1,23,1,23,3,23,395,8,23,1,24,1,24,1,24,1,24,1,24,1,25,1,25,3,25,404,8,
  	25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,
  	25,1,25,3,25,421,8,25,1,26,1,26,1,26,3,26,426,8,26,1,27,1,27,1,27,1,27,
  	5,27,432,8,27,10,27,12,27,435,9,27,1,27,1,27,1,27,1,27,1,28,1,28,1,28,
  	1,28,1,28,1,28,1,28,1,28,3,28,449,8,28,1,29,1,29,1,29,1,29,1,30,1,30,
  	1,30,1,30,1,30,1,30,5,30,461,8,30,10,30,12,30,464,9,30,1,31,1,31,1,31,
  	1,31,1,31,1,31,5,31,472,8,31,10,31,12,31,475,9,31,1,32,1,32,1,32,1,32,
  	1,32,1,32,5,32,483,8,32,10,32,12,32,486,9,32,1,33,1,33,1,33,1,33,1,33,
  	1,33,5,33,494,8,33,10,33,12,33,497,9,33,1,34,1,34,1,34,1,34,1,34,1,34,
  	5,34,505,8,34,10,34,12,34,508,9,34,1,35,1,35,1,35,1,35,1,35,1,35,5,35,
  	516,8,35,10,35,12,35,519,9,35,1,36,1,36,1,36,1,36,1,36,1,36,5,36,527,
  	8,36,10,36,12,36,530,9,36,1,37,1,37,1,37,1,37,1,37,1,37,5,37,538,8,37,
  	10,37,12,37,541,9,37,1,38,1,38,1,38,1,38,1,38,1,38,5,38,549,8,38,10,38,
  	12,38,552,9,38,1,39,1,39,1,39,1,39,1,39,1,39,5,39,560,8,39,10,39,12,39,
  	563,9,39,1,40,1,40,1,40,1,40,1,40,1,40,5,40,571,8,40,10,40,12,40,574,
  	9,40,1,41,1,41,1,41,1,41,1,41,1,41,5,41,582,8,41,10,41,12,41,585,9,41,
  	1,42,1,42,1,42,1,42,1,42,1,42,1,42,1,42,1,42,3,42,596,8,42,1,43,1,43,
  	3,43,600,8,43,1,44,1,44,1,44,3,44,605,8,44,1,45,1,45,1,45,1,45,1,46,1,
  	46,1,46,3,46,614,8,46,1,46,1,46,1,47,1,47,1,47,3,47,621,8,47,1,47,1,47,
  	1,47,1,47,1,47,3,47,628,8,47,1,48,1,48,3,48,632,8,48,1,48,1,48,1,48,1,
  	48,1,48,1,48,1,48,3,48,641,8,48,1,48,3,48,644,8,48,1,48,1,48,1,48,1,48,
  	1,48,3,48,651,8,48,1,48,1,48,3,48,655,8,48,1,48,1,48,1,48,1,48,1,48,5,
  	48,662,8,48,10,48,12,48,665,9,48,1,49,1,49,1,49,1,49,1,49,3,49,672,8,
  	49,1,49,1,49,1,49,1,49,1,49,1,49,3,49,680,8,49,1,49,1,49,1,49,3,49,685,
  	8,49,1,49,1,49,1,49,1,49,3,49,691,8,49,1,49,1,49,1,49,1,49,1,49,1,49,
  	1,49,3,49,700,8,49,1,49,1,49,1,49,1,49,1,49,5,49,707,8,49,10,49,12,49,
  	710,9,49,1,50,1,50,1,50,1,50,1,50,1,50,1,50,1,50,3,50,720,8,50,1,51,1,
  	51,1,51,1,51,1,51,1,51,3,51,728,8,51,1,52,1,52,1,52,3,52,733,8,52,1,52,
  	1,52,1,52,1,52,1,52,1,52,1,52,1,52,1,52,1,52,3,52,745,8,52,1,53,1,53,
  	1,53,1,53,1,53,1,53,1,53,1,54,1,54,1,55,1,55,1,55,1,55,1,55,1,55,1,55,
  	1,55,1,56,1,56,1,56,1,56,3,56,768,8,56,1,56,1,56,1,56,1,56,1,56,3,56,
  	775,8,56,1,56,5,56,778,8,56,10,56,12,56,781,9,56,1,56,3,56,784,8,56,3,
  	56,786,8,56,1,56,1,56,1,57,1,57,1,57,3,57,793,8,57,1,57,1,57,1,57,1,57,
  	1,57,3,57,800,8,57,1,57,1,57,5,57,804,8,57,10,57,12,57,807,9,57,1,57,
  	3,57,810,8,57,3,57,812,8,57,1,57,1,57,1,58,1,58,1,58,1,58,1,58,1,58,5,
  	58,822,8,58,10,58,12,58,825,9,58,3,58,827,8,58,1,58,1,58,1,59,1,59,1,
  	59,0,14,60,62,64,66,68,70,72,74,76,78,80,82,96,98,60,0,2,4,6,8,10,12,
  	14,16,18,20,22,24,26,28,30,32,34,36,38,40,42,44,46,48,50,52,54,56,58,
  	60,62,64,66,68,70,72,74,76,78,80,82,84,86,88,90,92,94,96,98,100,102,104,
  	106,108,110,112,114,116,118,0,19,1,0,3,4,2,0,105,105,107,107,1,0,12,13,
  	1,0,31,32,1,0,44,45,1,0,46,48,2,0,14,14,51,61,1,0,67,70,2,0,28,29,71,
  	74,2,0,28,28,71,71,1,0,75,76,1,0,77,78,2,0,31,31,79,80,1,0,81,82,1,0,
  	83,84,2,0,20,20,103,105,1,0,87,90,1,0,91,92,2,0,97,100,105,105,900,0,
  	131,1,0,0,0,2,136,1,0,0,0,4,140,1,0,0,0,6,148,1,0,0,0,8,152,1,0,0,0,10,
  	156,1,0,0,0,12,162,1,0,0,0,14,190,1,0,0,0,16,199,1,0,0,0,18,233,1,0,0,
  	0,20,250,1,0,0,0,22,261,1,0,0,0,24,271,1,0,0,0,26,273,1,0,0,0,28,284,
  	1,0,0,0,30,296,1,0,0,0,32,305,1,0,0,0,34,321,1,0,0,0,36,327,1,0,0,0,38,
  	334,1,0,0,0,40,346,1,0,0,0,42,355,1,0,0,0,44,385,1,0,0,0,46,389,1,0,0,
  	0,48,396,1,0,0,0,50,420,1,0,0,0,52,422,1,0,0,0,54,427,1,0,0,0,56,448,
  	1,0,0,0,58,450,1,0,0,0,60,454,1,0,0,0,62,465,1,0,0,0,64,476,1,0,0,0,66,
  	487,1,0,0,0,68,498,1,0,0,0,70,509,1,0,0,0,72,520,1,0,0,0,74,531,1,0,0,
  	0,76,542,1,0,0,0,78,553,1,0,0,0,80,564,1,0,0,0,82,575,1,0,0,0,84,595,
  	1,0,0,0,86,599,1,0,0,0,88,604,1,0,0,0,90,606,1,0,0,0,92,610,1,0,0,0,94,
  	627,1,0,0,0,96,650,1,0,0,0,98,690,1,0,0,0,100,719,1,0,0,0,102,727,1,0,
  	0,0,104,744,1,0,0,0,106,746,1,0,0,0,108,753,1,0,0,0,110,755,1,0,0,0,112,
  	763,1,0,0,0,114,789,1,0,0,0,116,815,1,0,0,0,118,830,1,0,0,0,120,130,5,
  	1,0,0,121,130,5,2,0,0,122,130,3,16,8,0,123,130,3,2,1,0,124,130,3,4,2,
  	0,125,130,3,6,3,0,126,130,3,8,4,0,127,130,3,10,5,0,128,130,3,12,6,0,129,
  	120,1,0,0,0,129,121,1,0,0,0,129,122,1,0,0,0,129,123,1,0,0,0,129,124,1,
  	0,0,0,129,125,1,0,0,0,129,126,1,0,0,0,129,127,1,0,0,0,129,128,1,0,0,0,
  	130,133,1,0,0,0,131,129,1,0,0,0,131,132,1,0,0,0,132,134,1,0,0,0,133,131,
  	1,0,0,0,134,135,5,0,0,1,135,1,1,0,0,0,136,137,7,0,0,0,137,138,7,1,0,0,
  	138,139,5,5,0,0,139,3,1,0,0,0,140,141,5,6,0,0,141,142,5,7,0,0,142,143,
  	5,108,0,0,143,144,5,8,0,0,144,145,5,108,0,0,145,146,5,9,0,0,146,147,5,
  	5,0,0,147,5,1,0,0,0,148,149,5,10,0,0,149,150,5,105,0,0,150,151,5,5,0,
  	0,151,7,1,0,0,0,152,153,5,11,0,0,153,154,7,1,0,0,154,155,5,5,0,0,155,
  	9,1,0,0,0,156,157,7,2,0,0,157,158,5,105,0,0,158,159,5,14,0,0,159,160,
  	3,56,28,0,160,161,5,5,0,0,161,11,1,0,0,0,162,163,5,15,0,0,163,173,5,105,
  	0,0,164,165,5,16,0,0,165,170,5,105,0,0,166,167,5,8,0,0,167,169,5,105,
  	0,0,168,166,1,0,0,0,169,172,1,0,0,0,170,168,1,0,0,0,170,171,1,0,0,0,171,
  	174,1,0,0,0,172,170,1,0,0,0,173,164,1,0,0,0,173,174,1,0,0,0,174,175,1,
  	0,0,0,175,179,5,17,0,0,176,178,3,14,7,0,177,176,1,0,0,0,178,181,1,0,0,
  	0,179,177,1,0,0,0,179,180,1,0,0,0,180,185,1,0,0,0,181,179,1,0,0,0,182,
  	184,3,16,8,0,183,182,1,0,0,0,184,187,1,0,0,0,185,183,1,0,0,0,185,186,
  	1,0,0,0,186,188,1,0,0,0,187,185,1,0,0,0,188,189,5,18,0,0,189,13,1,0,0,
  	0,190,191,5,19,0,0,191,194,5,105,0,0,192,193,5,14,0,0,193,195,3,56,28,
  	0,194,192,1,0,0,0,194,195,1,0,0,0,195,196,1,0,0,0,196,197,5,5,0,0,197,
  	15,1,0,0,0,198,200,5,20,0,0,199,198,1,0,0,0,199,200,1,0,0,0,200,202,1,
  	0,0,0,201,203,5,21,0,0,202,201,1,0,0,0,202,203,1,0,0,0,203,211,1,0,0,
  	0,204,209,5,22,0,0,205,206,5,7,0,0,206,207,3,108,54,0,207,208,5,9,0,0,
  	208,210,1,0,0,0,209,205,1,0,0,0,209,210,1,0,0,0,210,212,1,0,0,0,211,204,
  	1,0,0,0,211,212,1,0,0,0,212,217,1,0,0,0,213,214,5,23,0,0,214,215,5,24,
  	0,0,215,216,5,105,0,0,216,218,5,25,0,0,217,213,1,0,0,0,217,218,1,0,0,
  	0,218,220,1,0,0,0,219,221,3,18,9,0,220,219,1,0,0,0,220,221,1,0,0,0,221,
  	223,1,0,0,0,222,224,5,105,0,0,223,222,1,0,0,0,223,224,1,0,0,0,224,225,
  	1,0,0,0,225,226,5,7,0,0,226,227,3,20,10,0,227,229,5,9,0,0,228,230,5,26,
  	0,0,229,228,1,0,0,0,229,230,1,0,0,0,230,231,1,0,0,0,231,232,3,26,13,0,
  	232,17,1,0,0,0,233,234,5,27,0,0,234,240,5,105,0,0,235,236,5,28,0,0,236,
  	237,5,107,0,0,237,238,5,29,0,0,238,239,5,30,0,0,239,241,5,105,0,0,240,
  	235,1,0,0,0,240,241,1,0,0,0,241,19,1,0,0,0,242,247,3,22,11,0,243,244,
  	5,8,0,0,244,246,3,22,11,0,245,243,1,0,0,0,246,249,1,0,0,0,247,245,1,0,
  	0,0,247,248,1,0,0,0,248,251,1,0,0,0,249,247,1,0,0,0,250,242,1,0,0,0,250,
  	251,1,0,0,0,251,21,1,0,0,0,252,254,7,3,0,0,253,252,1,0,0,0,253,254,1,
  	0,0,0,254,255,1,0,0,0,255,258,5,105,0,0,256,257,5,14,0,0,257,259,3,56,
  	28,0,258,256,1,0,0,0,258,259,1,0,0,0,259,262,1,0,0,0,260,262,5,33,0,0,
  	261,253,1,0,0,0,261,260,1,0,0,0,262,23,1,0,0,0,263,268,3,56,28,0,264,
  	265,5,8,0,0,265,267,3,56,28,0,266,264,1,0,0,0,267,270,1,0,0,0,268,266,
  	1,0,0,0,268,269,1,0,0,0,269,272,1,0,0,0,270,268,1,0,0,0,271,263,1,0,0,
  	0,271,272,1,0,0,0,272,25,1,0,0,0,273,277,5,17,0,0,274,276,3,28,14,0,275,
  	274,1,0,0,0,276,279,1,0,0,0,277,275,1,0,0,0,277,278,1,0,0,0,278,280,1,
  	0,0,0,279,277,1,0,0,0,280,281,5,18,0,0,281,27,1,0,0,0,282,283,5,105,0,
  	0,283,285,5,16,0,0,284,282,1,0,0,0,284,285,1,0,0,0,285,294,1,0,0,0,286,
  	295,3,26,13,0,287,295,3,30,15,0,288,295,3,32,16,0,289,295,3,40,20,0,290,
  	295,3,34,17,0,291,295,3,38,19,0,292,295,3,44,22,0,293,295,3,42,21,0,294,
  	286,1,0,0,0,294,287,1,0,0,0,294,288,1,0,0,0,294,289,1,0,0,0,294,290,1,
  	0,0,0,294,291,1,0,0,0,294,292,1,0,0,0,294,293,1,0,0,0,295,29,1,0,0,0,
  	296,300,5,1,0,0,297,299,3,28,14,0,298,297,1,0,0,0,299,302,1,0,0,0,300,
  	298,1,0,0,0,300,301,1,0,0,0,301,303,1,0,0,0,302,300,1,0,0,0,303,304,5,
  	2,0,0,304,31,1,0,0,0,305,306,5,34,0,0,306,308,5,7,0,0,307,309,3,56,28,
  	0,308,307,1,0,0,0,308,309,1,0,0,0,309,310,1,0,0,0,310,312,5,5,0,0,311,
  	313,3,56,28,0,312,311,1,0,0,0,312,313,1,0,0,0,313,314,1,0,0,0,314,316,
  	5,5,0,0,315,317,3,56,28,0,316,315,1,0,0,0,316,317,1,0,0,0,317,318,1,0,
  	0,0,318,319,5,9,0,0,319,320,3,28,14,0,320,33,1,0,0,0,321,322,5,35,0,0,
  	322,323,5,7,0,0,323,324,3,56,28,0,324,325,5,9,0,0,325,326,3,28,14,0,326,
  	35,1,0,0,0,327,328,5,36,0,0,328,329,3,28,14,0,329,330,5,35,0,0,330,331,
  	5,7,0,0,331,332,3,56,28,0,332,333,5,9,0,0,333,37,1,0,0,0,334,335,5,37,
  	0,0,335,336,5,7,0,0,336,339,5,105,0,0,337,338,5,8,0,0,338,340,5,105,0,
  	0,339,337,1,0,0,0,339,340,1,0,0,0,340,341,1,0,0,0,341,342,5,38,0,0,342,
  	343,3,56,28,0,343,344,5,9,0,0,344,345,3,28,14,0,345,39,1,0,0,0,346,347,
  	5,39,0,0,347,348,5,7,0,0,348,349,3,56,28,0,349,350,5,9,0,0,350,353,3,
  	28,14,0,351,352,5,40,0,0,352,354,3,28,14,0,353,351,1,0,0,0,353,354,1,
  	0,0,0,354,41,1,0,0,0,355,356,5,41,0,0,356,357,5,7,0,0,357,358,3,56,28,
  	0,358,359,5,9,0,0,359,372,5,17,0,0,360,361,5,42,0,0,361,364,3,100,50,
  	0,362,364,5,43,0,0,363,360,1,0,0,0,363,362,1,0,0,0,364,365,1,0,0,0,365,
  	369,5,16,0,0,366,368,3,28,14,0,367,366,1,0,0,0,368,371,1,0,0,0,369,367,
  	1,0,0,0,369,370,1,0,0,0,370,373,1,0,0,0,371,369,1,0,0,0,372,363,1,0,0,
  	0,373,374,1,0,0,0,374,372,1,0,0,0,374,375,1,0,0,0,375,376,1,0,0,0,376,
  	377,5,18,0,0,377,43,1,0,0,0,378,386,3,56,28,0,379,386,3,52,26,0,380,386,
  	3,36,18,0,381,386,3,94,47,0,382,386,3,46,23,0,383,386,3,48,24,0,384,386,
  	3,54,27,0,385,378,1,0,0,0,385,379,1,0,0,0,385,380,1,0,0,0,385,381,1,0,
  	0,0,385,382,1,0,0,0,385,383,1,0,0,0,385,384,1,0,0,0,386,387,1,0,0,0,387,
  	388,5,5,0,0,388,45,1,0,0,0,389,394,7,4,0,0,390,391,5,7,0,0,391,392,3,
  	108,54,0,392,393,5,9,0,0,393,395,1,0,0,0,394,390,1,0,0,0,394,395,1,0,
  	0,0,395,47,1,0,0,0,396,397,7,5,0,0,397,398,5,7,0,0,398,399,3,108,54,0,
  	399,400,5,9,0,0,400,49,1,0,0,0,401,402,5,105,0,0,402,404,5,30,0,0,403,
  	401,1,0,0,0,403,404,1,0,0,0,404,405,1,0,0,0,405,421,5,105,0,0,406,407,
  	5,24,0,0,407,408,5,24,0,0,408,409,3,56,28,0,409,410,5,25,0,0,410,411,
  	5,25,0,0,411,421,1,0,0,0,412,413,5,24,0,0,413,414,5,24,0,0,414,415,3,
  	56,28,0,415,416,5,25,0,0,416,417,5,25,0,0,417,418,5,49,0,0,418,419,5,
  	105,0,0,419,421,1,0,0,0,420,403,1,0,0,0,420,406,1,0,0,0,420,412,1,0,0,
  	0,421,51,1,0,0,0,422,425,5,102,0,0,423,426,5,105,0,0,424,426,3,56,28,
  	0,425,423,1,0,0,0,425,424,1,0,0,0,425,426,1,0,0,0,426,53,1,0,0,0,427,
  	428,5,24,0,0,428,433,5,105,0,0,429,430,5,8,0,0,430,432,5,105,0,0,431,
  	429,1,0,0,0,432,435,1,0,0,0,433,431,1,0,0,0,433,434,1,0,0,0,434,436,1,
  	0,0,0,435,433,1,0,0,0,436,437,5,25,0,0,437,438,5,14,0,0,438,439,3,56,
  	28,0,439,55,1,0,0,0,440,449,3,58,29,0,441,442,3,60,30,0,442,443,5,50,
  	0,0,443,444,3,56,28,0,444,445,5,16,0,0,445,446,3,56,28,0,446,449,1,0,
  	0,0,447,449,3,60,30,0,448,440,1,0,0,0,448,441,1,0,0,0,448,447,1,0,0,0,
  	449,57,1,0,0,0,450,451,3,98,49,0,451,452,7,6,0,0,452,453,3,56,28,0,453,
  	59,1,0,0,0,454,455,6,30,-1,0,455,456,3,62,31,0,456,462,1,0,0,0,457,458,
  	10,2,0,0,458,459,5,62,0,0,459,461,3,62,31,0,460,457,1,0,0,0,461,464,1,
  	0,0,0,462,460,1,0,0,0,462,463,1,0,0,0,463,61,1,0,0,0,464,462,1,0,0,0,
  	465,466,6,31,-1,0,466,467,3,64,32,0,467,473,1,0,0,0,468,469,10,2,0,0,
  	469,470,5,63,0,0,470,472,3,64,32,0,471,468,1,0,0,0,472,475,1,0,0,0,473,
  	471,1,0,0,0,473,474,1,0,0,0,474,63,1,0,0,0,475,473,1,0,0,0,476,477,6,
  	32,-1,0,477,478,3,66,33,0,478,484,1,0,0,0,479,480,10,2,0,0,480,481,5,
  	64,0,0,481,483,3,66,33,0,482,479,1,0,0,0,483,486,1,0,0,0,484,482,1,0,
  	0,0,484,485,1,0,0,0,485,65,1,0,0,0,486,484,1,0,0,0,487,488,6,33,-1,0,
  	488,489,3,68,34,0,489,495,1,0,0,0,490,491,10,2,0,0,491,492,5,65,0,0,492,
  	494,3,68,34,0,493,490,1,0,0,0,494,497,1,0,0,0,495,493,1,0,0,0,495,496,
  	1,0,0,0,496,67,1,0,0,0,497,495,1,0,0,0,498,499,6,34,-1,0,499,500,3,70,
  	35,0,500,506,1,0,0,0,501,502,10,2,0,0,502,503,5,66,0,0,503,505,3,70,35,
  	0,504,501,1,0,0,0,505,508,1,0,0,0,506,504,1,0,0,0,506,507,1,0,0,0,507,
  	69,1,0,0,0,508,506,1,0,0,0,509,510,6,35,-1,0,510,511,3,72,36,0,511,517,
  	1,0,0,0,512,513,10,2,0,0,513,514,5,32,0,0,514,516,3,72,36,0,515,512,1,
  	0,0,0,516,519,1,0,0,0,517,515,1,0,0,0,517,518,1,0,0,0,518,71,1,0,0,0,
  	519,517,1,0,0,0,520,521,6,36,-1,0,521,522,3,74,37,0,522,528,1,0,0,0,523,
  	524,10,2,0,0,524,525,7,7,0,0,525,527,3,74,37,0,526,523,1,0,0,0,527,530,
  	1,0,0,0,528,526,1,0,0,0,528,529,1,0,0,0,529,73,1,0,0,0,530,528,1,0,0,
  	0,531,532,6,37,-1,0,532,533,3,76,38,0,533,539,1,0,0,0,534,535,10,2,0,
  	0,535,536,7,8,0,0,536,538,3,76,38,0,537,534,1,0,0,0,538,541,1,0,0,0,539,
  	537,1,0,0,0,539,540,1,0,0,0,540,75,1,0,0,0,541,539,1,0,0,0,542,543,6,
  	38,-1,0,543,544,3,78,39,0,544,550,1,0,0,0,545,546,10,2,0,0,546,547,7,
  	9,0,0,547,549,3,78,39,0,548,545,1,0,0,0,549,552,1,0,0,0,550,548,1,0,0,
  	0,550,551,1,0,0,0,551,77,1,0,0,0,552,550,1,0,0,0,553,554,6,39,-1,0,554,
  	555,3,80,40,0,555,561,1,0,0,0,556,557,10,2,0,0,557,558,7,10,0,0,558,560,
  	3,80,40,0,559,556,1,0,0,0,560,563,1,0,0,0,561,559,1,0,0,0,561,562,1,0,
  	0,0,562,79,1,0,0,0,563,561,1,0,0,0,564,565,6,40,-1,0,565,566,3,82,41,
  	0,566,572,1,0,0,0,567,568,10,2,0,0,568,569,7,11,0,0,569,571,3,82,41,0,
  	570,567,1,0,0,0,571,574,1,0,0,0,572,570,1,0,0,0,572,573,1,0,0,0,573,81,
  	1,0,0,0,574,572,1,0,0,0,575,576,6,41,-1,0,576,577,3,84,42,0,577,583,1,
  	0,0,0,578,579,10,2,0,0,579,580,7,12,0,0,580,582,3,84,42,0,581,578,1,0,
  	0,0,582,585,1,0,0,0,583,581,1,0,0,0,583,584,1,0,0,0,584,83,1,0,0,0,585,
  	583,1,0,0,0,586,587,7,13,0,0,587,596,3,86,43,0,588,589,7,14,0,0,589,596,
  	3,98,49,0,590,591,3,98,49,0,591,592,7,14,0,0,592,596,1,0,0,0,593,596,
  	3,92,46,0,594,596,3,86,43,0,595,586,1,0,0,0,595,588,1,0,0,0,595,590,1,
  	0,0,0,595,593,1,0,0,0,595,594,1,0,0,0,596,85,1,0,0,0,597,600,3,94,47,
  	0,598,600,3,88,44,0,599,597,1,0,0,0,599,598,1,0,0,0,600,87,1,0,0,0,601,
  	605,3,100,50,0,602,605,3,90,45,0,603,605,3,98,49,0,604,601,1,0,0,0,604,
  	602,1,0,0,0,604,603,1,0,0,0,605,89,1,0,0,0,606,607,5,7,0,0,607,608,3,
  	56,28,0,608,609,5,9,0,0,609,91,1,0,0,0,610,611,3,86,43,0,611,613,5,85,
  	0,0,612,614,5,86,0,0,613,612,1,0,0,0,613,614,1,0,0,0,614,615,1,0,0,0,
  	615,616,7,15,0,0,616,93,1,0,0,0,617,628,3,96,48,0,618,620,3,88,44,0,619,
  	621,7,16,0,0,620,619,1,0,0,0,620,621,1,0,0,0,621,622,1,0,0,0,622,623,
  	3,50,25,0,623,624,5,7,0,0,624,625,3,24,12,0,625,626,5,9,0,0,626,628,1,
  	0,0,0,627,617,1,0,0,0,627,618,1,0,0,0,628,95,1,0,0,0,629,631,6,48,-1,
  	0,630,632,7,16,0,0,631,630,1,0,0,0,631,632,1,0,0,0,632,633,1,0,0,0,633,
  	634,3,50,25,0,634,635,5,7,0,0,635,636,3,24,12,0,636,637,5,9,0,0,637,651,
  	1,0,0,0,638,641,3,100,50,0,639,641,3,90,45,0,640,638,1,0,0,0,640,639,
  	1,0,0,0,641,643,1,0,0,0,642,644,7,16,0,0,643,642,1,0,0,0,643,644,1,0,
  	0,0,644,645,1,0,0,0,645,646,3,50,25,0,646,647,5,7,0,0,647,648,3,24,12,
  	0,648,649,5,9,0,0,649,651,1,0,0,0,650,629,1,0,0,0,650,640,1,0,0,0,651,
  	663,1,0,0,0,652,654,10,1,0,0,653,655,7,16,0,0,654,653,1,0,0,0,654,655,
  	1,0,0,0,655,656,1,0,0,0,656,657,3,50,25,0,657,658,5,7,0,0,658,659,3,24,
  	12,0,659,660,5,9,0,0,660,662,1,0,0,0,661,652,1,0,0,0,662,665,1,0,0,0,
  	663,661,1,0,0,0,663,664,1,0,0,0,664,97,1,0,0,0,665,663,1,0,0,0,666,667,
  	6,49,-1,0,667,691,3,118,59,0,668,672,3,96,48,0,669,672,3,100,50,0,670,
  	672,3,90,45,0,671,668,1,0,0,0,671,669,1,0,0,0,671,670,1,0,0,0,672,673,
  	1,0,0,0,673,679,7,17,0,0,674,680,3,118,59,0,675,676,5,7,0,0,676,677,3,
  	56,28,0,677,678,5,9,0,0,678,680,1,0,0,0,679,674,1,0,0,0,679,675,1,0,0,
  	0,680,691,1,0,0,0,681,685,3,96,48,0,682,685,3,100,50,0,683,685,3,90,45,
  	0,684,681,1,0,0,0,684,682,1,0,0,0,684,683,1,0,0,0,685,686,1,0,0,0,686,
  	687,5,24,0,0,687,688,3,56,28,0,688,689,5,25,0,0,689,691,1,0,0,0,690,666,
  	1,0,0,0,690,671,1,0,0,0,690,684,1,0,0,0,691,708,1,0,0,0,692,693,10,4,
  	0,0,693,699,7,17,0,0,694,700,3,118,59,0,695,696,5,7,0,0,696,697,3,56,
  	28,0,697,698,5,9,0,0,698,700,1,0,0,0,699,694,1,0,0,0,699,695,1,0,0,0,
  	700,707,1,0,0,0,701,702,10,3,0,0,702,703,5,24,0,0,703,704,3,56,28,0,704,
  	705,5,25,0,0,705,707,1,0,0,0,706,692,1,0,0,0,706,701,1,0,0,0,707,710,
  	1,0,0,0,708,706,1,0,0,0,708,709,1,0,0,0,709,99,1,0,0,0,710,708,1,0,0,
  	0,711,720,3,116,58,0,712,720,3,112,56,0,713,720,3,114,57,0,714,720,3,
  	16,8,0,715,720,3,102,51,0,716,720,5,104,0,0,717,720,3,104,52,0,718,720,
  	3,106,53,0,719,711,1,0,0,0,719,712,1,0,0,0,719,713,1,0,0,0,719,714,1,
  	0,0,0,719,715,1,0,0,0,719,716,1,0,0,0,719,717,1,0,0,0,719,718,1,0,0,0,
  	720,101,1,0,0,0,721,728,3,110,55,0,722,728,3,108,54,0,723,728,5,103,0,
  	0,724,728,5,101,0,0,725,728,5,108,0,0,726,728,5,109,0,0,727,721,1,0,0,
  	0,727,722,1,0,0,0,727,723,1,0,0,0,727,724,1,0,0,0,727,725,1,0,0,0,727,
  	726,1,0,0,0,728,103,1,0,0,0,729,732,5,32,0,0,730,731,5,105,0,0,731,733,
  	5,30,0,0,732,730,1,0,0,0,732,733,1,0,0,0,733,734,1,0,0,0,734,745,5,105,
  	0,0,735,736,5,93,0,0,736,737,5,105,0,0,737,738,5,28,0,0,738,739,5,107,
  	0,0,739,740,5,29,0,0,740,741,5,30,0,0,741,745,5,105,0,0,742,743,5,32,
  	0,0,743,745,3,98,49,0,744,729,1,0,0,0,744,735,1,0,0,0,744,742,1,0,0,0,
  	745,105,1,0,0,0,746,747,5,93,0,0,747,748,5,24,0,0,748,749,5,105,0,0,749,
  	750,5,8,0,0,750,751,5,105,0,0,751,752,5,25,0,0,752,107,1,0,0,0,753,754,
  	7,18,0,0,754,109,1,0,0,0,755,756,5,7,0,0,756,757,3,56,28,0,757,758,5,
  	8,0,0,758,759,3,56,28,0,759,760,5,8,0,0,760,761,3,56,28,0,761,762,5,9,
  	0,0,762,111,1,0,0,0,763,785,5,24,0,0,764,765,3,56,28,0,765,766,5,16,0,
  	0,766,768,1,0,0,0,767,764,1,0,0,0,767,768,1,0,0,0,768,769,1,0,0,0,769,
  	779,3,56,28,0,770,774,5,8,0,0,771,772,3,56,28,0,772,773,5,16,0,0,773,
  	775,1,0,0,0,774,771,1,0,0,0,774,775,1,0,0,0,775,776,1,0,0,0,776,778,3,
  	56,28,0,777,770,1,0,0,0,778,781,1,0,0,0,779,777,1,0,0,0,779,780,1,0,0,
  	0,780,783,1,0,0,0,781,779,1,0,0,0,782,784,5,8,0,0,783,782,1,0,0,0,783,
  	784,1,0,0,0,784,786,1,0,0,0,785,767,1,0,0,0,785,786,1,0,0,0,786,787,1,
  	0,0,0,787,788,5,25,0,0,788,113,1,0,0,0,789,811,5,17,0,0,790,793,5,106,
  	0,0,791,793,3,56,28,0,792,790,1,0,0,0,792,791,1,0,0,0,793,794,1,0,0,0,
  	794,795,5,16,0,0,795,805,3,56,28,0,796,799,5,8,0,0,797,800,5,106,0,0,
  	798,800,3,56,28,0,799,797,1,0,0,0,799,798,1,0,0,0,800,801,1,0,0,0,801,
  	802,5,16,0,0,802,804,3,56,28,0,803,796,1,0,0,0,804,807,1,0,0,0,805,803,
  	1,0,0,0,805,806,1,0,0,0,806,809,1,0,0,0,807,805,1,0,0,0,808,810,5,8,0,
  	0,809,808,1,0,0,0,809,810,1,0,0,0,810,812,1,0,0,0,811,792,1,0,0,0,811,
  	812,1,0,0,0,812,813,1,0,0,0,813,814,5,18,0,0,814,115,1,0,0,0,815,816,
  	5,94,0,0,816,817,5,105,0,0,817,826,5,7,0,0,818,823,3,56,28,0,819,820,
  	5,8,0,0,820,822,3,56,28,0,821,819,1,0,0,0,822,825,1,0,0,0,823,821,1,0,
  	0,0,823,824,1,0,0,0,824,827,1,0,0,0,825,823,1,0,0,0,826,818,1,0,0,0,826,
  	827,1,0,0,0,827,828,1,0,0,0,828,829,5,9,0,0,829,117,1,0,0,0,830,831,5,
  	105,0,0,831,119,1,0,0,0,89,129,131,170,173,179,185,194,199,202,209,211,
  	217,220,223,229,240,247,250,253,258,261,268,271,277,284,294,300,308,312,
  	316,339,353,363,369,374,385,394,403,420,425,433,448,462,473,484,495,506,
  	517,528,539,550,561,572,583,595,599,604,613,620,627,631,640,643,650,654,
  	663,671,679,684,690,699,706,708,719,727,732,744,767,774,779,783,785,792,
  	799,805,809,811,823,826
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

std::vector<gscParser::PrecacheContext *> gscParser::ProgContext::precache() {
  return getRuleContexts<gscParser::PrecacheContext>();
}

gscParser::PrecacheContext* gscParser::ProgContext::precache(size_t i) {
  return getRuleContext<gscParser::PrecacheContext>(i);
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
    setState(131);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 149994718) != 0) || _la == gscParser::IDENTIFIER) {
      setState(129);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case gscParser::T__0: {
          setState(120);
          match(gscParser::T__0);
          break;
        }

        case gscParser::T__1: {
          setState(121);
          match(gscParser::T__1);
          break;
        }

        case gscParser::T__6:
        case gscParser::T__19:
        case gscParser::T__20:
        case gscParser::T__21:
        case gscParser::T__22:
        case gscParser::T__26:
        case gscParser::IDENTIFIER: {
          setState(122);
          function();
          break;
        }

        case gscParser::T__2:
        case gscParser::T__3: {
          setState(123);
          include();
          break;
        }

        case gscParser::T__5: {
          setState(124);
          precache();
          break;
        }

        case gscParser::T__9: {
          setState(125);
          namespace_();
          break;
        }

        case gscParser::T__10: {
          setState(126);
          filenamespace();
          break;
        }

        case gscParser::T__11:
        case gscParser::T__12: {
          setState(127);
          constexpr_();
          break;
        }

        case gscParser::T__14: {
          setState(128);
          class_def();
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(133);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(134);
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
    setState(136);
    _la = _input->LA(1);
    if (!(_la == gscParser::T__2

    || _la == gscParser::T__3)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(137);
    _la = _input->LA(1);
    if (!(_la == gscParser::IDENTIFIER

    || _la == gscParser::PATH)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(138);
    match(gscParser::T__4);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- PrecacheContext ------------------------------------------------------------------

gscParser::PrecacheContext::PrecacheContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> gscParser::PrecacheContext::STRING() {
  return getTokens(gscParser::STRING);
}

tree::TerminalNode* gscParser::PrecacheContext::STRING(size_t i) {
  return getToken(gscParser::STRING, i);
}


size_t gscParser::PrecacheContext::getRuleIndex() const {
  return gscParser::RulePrecache;
}


std::any gscParser::PrecacheContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitPrecache(this);
  else
    return visitor->visitChildren(this);
}

gscParser::PrecacheContext* gscParser::precache() {
  PrecacheContext *_localctx = _tracker.createInstance<PrecacheContext>(_ctx, getState());
  enterRule(_localctx, 4, gscParser::RulePrecache);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(140);
    match(gscParser::T__5);
    setState(141);
    match(gscParser::T__6);
    setState(142);
    match(gscParser::STRING);
    setState(143);
    match(gscParser::T__7);
    setState(144);
    match(gscParser::STRING);
    setState(145);
    match(gscParser::T__8);
    setState(146);
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
  enterRule(_localctx, 6, gscParser::RuleNamespace);

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
    match(gscParser::T__9);
    setState(149);
    match(gscParser::IDENTIFIER);
    setState(150);
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
  enterRule(_localctx, 8, gscParser::RuleFilenamespace);
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
    match(gscParser::T__10);
    setState(153);
    _la = _input->LA(1);
    if (!(_la == gscParser::IDENTIFIER

    || _la == gscParser::PATH)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(154);
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
  enterRule(_localctx, 10, gscParser::RuleConstexpr);
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
    setState(156);
    _la = _input->LA(1);
    if (!(_la == gscParser::T__11

    || _la == gscParser::T__12)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(157);
    match(gscParser::IDENTIFIER);
    setState(158);
    match(gscParser::T__13);
    setState(159);
    expression();
    setState(160);
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
  enterRule(_localctx, 12, gscParser::RuleClass_def);
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
    setState(162);
    match(gscParser::T__14);
    setState(163);
    match(gscParser::IDENTIFIER);
    setState(173);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__15) {
      setState(164);
      match(gscParser::T__15);
      setState(165);
      match(gscParser::IDENTIFIER);
      setState(170);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == gscParser::T__7) {
        setState(166);
        match(gscParser::T__7);
        setState(167);
        match(gscParser::IDENTIFIER);
        setState(172);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
    }
    setState(175);
    match(gscParser::T__16);
    setState(179);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == gscParser::T__18) {
      setState(176);
      class_var();
      setState(181);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(185);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 149946496) != 0) || _la == gscParser::IDENTIFIER) {
      setState(182);
      function();
      setState(187);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(188);
    match(gscParser::T__17);
   
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
  enterRule(_localctx, 14, gscParser::RuleClass_var);
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
    match(gscParser::T__18);
    setState(191);
    match(gscParser::IDENTIFIER);
    setState(194);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__13) {
      setState(192);
      match(gscParser::T__13);
      setState(193);
      expression();
    }
    setState(196);
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
  enterRule(_localctx, 16, gscParser::RuleFunction);
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
    setState(199);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__19) {
      setState(198);
      match(gscParser::T__19);
    }
    setState(202);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__20) {
      setState(201);
      match(gscParser::T__20);
    }
    setState(211);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__21) {
      setState(204);
      match(gscParser::T__21);
      setState(209);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 9, _ctx)) {
      case 1: {
        setState(205);
        match(gscParser::T__6);
        setState(206);
        number();
        setState(207);
        match(gscParser::T__8);
        break;
      }

      default:
        break;
      }
    }
    setState(217);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__22) {
      setState(213);
      match(gscParser::T__22);
      setState(214);
      match(gscParser::T__23);
      setState(215);
      match(gscParser::IDENTIFIER);
      setState(216);
      match(gscParser::T__24);
    }
    setState(220);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__26) {
      setState(219);
      detour_info();
    }
    setState(223);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::IDENTIFIER) {
      setState(222);
      match(gscParser::IDENTIFIER);
    }
    setState(225);
    match(gscParser::T__6);
    setState(226);
    param_list();
    setState(227);
    match(gscParser::T__8);
    setState(229);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__25) {
      setState(228);
      match(gscParser::T__25);
    }
    setState(231);
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
  enterRule(_localctx, 18, gscParser::RuleDetour_info);
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
    match(gscParser::T__26);
    setState(234);
    match(gscParser::IDENTIFIER);
    setState(240);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__27) {
      setState(235);
      match(gscParser::T__27);
      setState(236);
      match(gscParser::PATH);
      setState(237);
      match(gscParser::T__28);
      setState(238);
      match(gscParser::T__29);
      setState(239);
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
  enterRule(_localctx, 20, gscParser::RuleParam_list);
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
    setState(250);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 15032385536) != 0) || _la == gscParser::IDENTIFIER) {
      setState(242);
      param_val();
      setState(247);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == gscParser::T__7) {
        setState(243);
        match(gscParser::T__7);
        setState(244);
        param_val();
        setState(249);
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
  enterRule(_localctx, 22, gscParser::RuleParam_val);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(261);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case gscParser::T__30:
      case gscParser::T__31:
      case gscParser::IDENTIFIER: {
        enterOuterAlt(_localctx, 1);
        setState(253);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == gscParser::T__30

        || _la == gscParser::T__31) {
          setState(252);
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
        setState(255);
        match(gscParser::IDENTIFIER);
        setState(258);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == gscParser::T__13) {
          setState(256);
          match(gscParser::T__13);
          setState(257);
          expression();
        }
        break;
      }

      case gscParser::T__32: {
        enterOuterAlt(_localctx, 2);
        setState(260);
        match(gscParser::T__32);
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
  enterRule(_localctx, 24, gscParser::RuleExpression_list);
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
    setState(271);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 4461822080) != 0) || ((((_la - 81) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 81)) & 434058191) != 0)) {
      setState(263);
      expression();
      setState(268);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == gscParser::T__7) {
        setState(264);
        match(gscParser::T__7);
        setState(265);
        expression();
        setState(270);
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
  enterRule(_localctx, 26, gscParser::RuleStatement_block);
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
    setState(273);
    match(gscParser::T__16);
    setState(277);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 548368706306178) != 0) || ((((_la - 81) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 81)) & 436155343) != 0)) {
      setState(274);
      statement();
      setState(279);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(280);
    match(gscParser::T__17);
   
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
  enterRule(_localctx, 28, gscParser::RuleStatement);

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
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 24, _ctx)) {
    case 1: {
      setState(282);
      match(gscParser::IDENTIFIER);
      setState(283);
      match(gscParser::T__15);
      break;
    }

    default:
      break;
    }
    setState(294);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 25, _ctx)) {
    case 1: {
      setState(286);
      statement_block();
      break;
    }

    case 2: {
      setState(287);
      statement_dev_block();
      break;
    }

    case 3: {
      setState(288);
      statement_for();
      break;
    }

    case 4: {
      setState(289);
      statement_if();
      break;
    }

    case 5: {
      setState(290);
      statement_while();
      break;
    }

    case 6: {
      setState(291);
      statement_foreach();
      break;
    }

    case 7: {
      setState(292);
      statement_inst();
      break;
    }

    case 8: {
      setState(293);
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
  enterRule(_localctx, 30, gscParser::RuleStatement_dev_block);
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
    setState(296);
    match(gscParser::T__0);
    setState(300);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 548368706306178) != 0) || ((((_la - 81) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 81)) & 436155343) != 0)) {
      setState(297);
      statement();
      setState(302);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(303);
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
  enterRule(_localctx, 32, gscParser::RuleStatement_for);
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
    setState(305);
    match(gscParser::T__33);
    setState(306);
    match(gscParser::T__6);
    setState(308);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 4461822080) != 0) || ((((_la - 81) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 81)) & 434058191) != 0)) {
      setState(307);
      expression();
    }
    setState(310);
    match(gscParser::T__4);
    setState(312);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 4461822080) != 0) || ((((_la - 81) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 81)) & 434058191) != 0)) {
      setState(311);
      expression();
    }
    setState(314);
    match(gscParser::T__4);
    setState(316);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 4461822080) != 0) || ((((_la - 81) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 81)) & 434058191) != 0)) {
      setState(315);
      expression();
    }
    setState(318);
    match(gscParser::T__8);
    setState(319);
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
  enterRule(_localctx, 34, gscParser::RuleStatement_while);

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
    match(gscParser::T__34);
    setState(322);
    match(gscParser::T__6);
    setState(323);
    expression();
    setState(324);
    match(gscParser::T__8);
    setState(325);
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
  enterRule(_localctx, 36, gscParser::RuleStatement_dowhile);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(327);
    match(gscParser::T__35);
    setState(328);
    statement();
    setState(329);
    match(gscParser::T__34);
    setState(330);
    match(gscParser::T__6);
    setState(331);
    expression();
    setState(332);
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
  enterRule(_localctx, 38, gscParser::RuleStatement_foreach);
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
    setState(334);
    match(gscParser::T__36);
    setState(335);
    match(gscParser::T__6);
    setState(336);
    match(gscParser::IDENTIFIER);
    setState(339);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__7) {
      setState(337);
      match(gscParser::T__7);
      setState(338);
      match(gscParser::IDENTIFIER);
    }
    setState(341);
    match(gscParser::T__37);
    setState(342);
    expression();
    setState(343);
    match(gscParser::T__8);
    setState(344);
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
  enterRule(_localctx, 40, gscParser::RuleStatement_if);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(346);
    match(gscParser::T__38);
    setState(347);
    match(gscParser::T__6);
    setState(348);
    expression();
    setState(349);
    match(gscParser::T__8);
    setState(350);
    statement();
    setState(353);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 31, _ctx)) {
    case 1: {
      setState(351);
      match(gscParser::T__39);
      setState(352);
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
  enterRule(_localctx, 42, gscParser::RuleStatement_switch);
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
    setState(355);
    match(gscParser::T__40);
    setState(356);
    match(gscParser::T__6);
    setState(357);
    expression();
    setState(358);
    match(gscParser::T__8);
    setState(359);
    match(gscParser::T__16);
    setState(372); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(363);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case gscParser::T__41: {
          setState(360);
          match(gscParser::T__41);
          setState(361);
          const_expr();
          break;
        }

        case gscParser::T__42: {
          setState(362);
          match(gscParser::T__42);
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(365);
      match(gscParser::T__15);
      setState(369);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while ((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & 548368706306178) != 0) || ((((_la - 81) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 81)) & 436155343) != 0)) {
        setState(366);
        statement();
        setState(371);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
      setState(374); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while (_la == gscParser::T__41

    || _la == gscParser::T__42);
    setState(376);
    match(gscParser::T__17);
   
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

gscParser::Array_unpackContext* gscParser::Statement_instContext::array_unpack() {
  return getRuleContext<gscParser::Array_unpackContext>(0);
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
  enterRule(_localctx, 44, gscParser::RuleStatement_inst);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(385);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 35, _ctx)) {
    case 1: {
      setState(378);
      expression();
      break;
    }

    case 2: {
      setState(379);
      operator_inst();
      break;
    }

    case 3: {
      setState(380);
      statement_dowhile();
      break;
    }

    case 4: {
      setState(381);
      function_call_exp();
      break;
    }

    case 5: {
      setState(382);
      nop_def();
      break;
    }

    case 6: {
      setState(383);
      devop_def();
      break;
    }

    case 7: {
      setState(384);
      array_unpack();
      break;
    }

    default:
      break;
    }
    setState(387);
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
  enterRule(_localctx, 46, gscParser::RuleNop_def);
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
    setState(389);
    _la = _input->LA(1);
    if (!(_la == gscParser::T__43

    || _la == gscParser::T__44)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(394);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__6) {
      setState(390);
      match(gscParser::T__6);
      setState(391);
      number();
      setState(392);
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
  enterRule(_localctx, 48, gscParser::RuleDevop_def);
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
    setState(396);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 492581209243648) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(397);
    match(gscParser::T__6);
    setState(398);
    number();
    setState(399);
    match(gscParser::T__8);
   
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
  enterRule(_localctx, 50, gscParser::RuleFunction_component);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(420);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 38, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(403);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 37, _ctx)) {
      case 1: {
        setState(401);
        match(gscParser::IDENTIFIER);
        setState(402);
        match(gscParser::T__29);
        break;
      }

      default:
        break;
      }
      setState(405);
      match(gscParser::IDENTIFIER);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(406);
      match(gscParser::T__23);
      setState(407);
      match(gscParser::T__23);
      setState(408);
      expression();
      setState(409);
      match(gscParser::T__24);
      setState(410);
      match(gscParser::T__24);
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(412);
      match(gscParser::T__23);
      setState(413);
      match(gscParser::T__23);
      setState(414);
      expression();
      setState(415);
      match(gscParser::T__24);
      setState(416);
      match(gscParser::T__24);
      setState(417);
      match(gscParser::T__48);
      setState(418);
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
  enterRule(_localctx, 52, gscParser::RuleOperator_inst);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(422);
    match(gscParser::BUILTIN);
    setState(425);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 39, _ctx)) {
    case 1: {
      setState(423);
      match(gscParser::IDENTIFIER);
      break;
    }

    case 2: {
      setState(424);
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

//----------------- Array_unpackContext ------------------------------------------------------------------

gscParser::Array_unpackContext::Array_unpackContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> gscParser::Array_unpackContext::IDENTIFIER() {
  return getTokens(gscParser::IDENTIFIER);
}

tree::TerminalNode* gscParser::Array_unpackContext::IDENTIFIER(size_t i) {
  return getToken(gscParser::IDENTIFIER, i);
}

gscParser::ExpressionContext* gscParser::Array_unpackContext::expression() {
  return getRuleContext<gscParser::ExpressionContext>(0);
}


size_t gscParser::Array_unpackContext::getRuleIndex() const {
  return gscParser::RuleArray_unpack;
}


std::any gscParser::Array_unpackContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitArray_unpack(this);
  else
    return visitor->visitChildren(this);
}

gscParser::Array_unpackContext* gscParser::array_unpack() {
  Array_unpackContext *_localctx = _tracker.createInstance<Array_unpackContext>(_ctx, getState());
  enterRule(_localctx, 54, gscParser::RuleArray_unpack);
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
    setState(427);
    match(gscParser::T__23);
    setState(428);
    match(gscParser::IDENTIFIER);
    setState(433);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == gscParser::T__7) {
      setState(429);
      match(gscParser::T__7);
      setState(430);
      match(gscParser::IDENTIFIER);
      setState(435);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(436);
    match(gscParser::T__24);
    setState(437);
    match(gscParser::T__13);
    setState(438);
    expression();
   
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
  enterRule(_localctx, 56, gscParser::RuleExpression);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(448);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 41, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(440);
      set_expression();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(441);
      expression0(0);
      setState(442);
      match(gscParser::T__49);
      setState(443);
      expression();
      setState(444);
      match(gscParser::T__15);
      setState(445);
      expression();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(447);
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
  enterRule(_localctx, 58, gscParser::RuleSet_expression);
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
    setState(450);
    left_value(0);
    setState(451);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 4609434218613719040) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(452);
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
  size_t startState = 60;
  enterRecursionRule(_localctx, 60, gscParser::RuleExpression0, precedence);

    

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
    setState(455);
    expression1(0);
    _ctx->stop = _input->LT(-1);
    setState(462);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 42, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression0Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression0);
        setState(457);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(458);
        match(gscParser::T__61);
        setState(459);
        expression1(0); 
      }
      setState(464);
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
  size_t startState = 62;
  enterRecursionRule(_localctx, 62, gscParser::RuleExpression1, precedence);

    

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
    setState(466);
    expression2(0);
    _ctx->stop = _input->LT(-1);
    setState(473);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 43, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression1Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression1);
        setState(468);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(469);
        match(gscParser::T__62);
        setState(470);
        expression2(0); 
      }
      setState(475);
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
  size_t startState = 64;
  enterRecursionRule(_localctx, 64, gscParser::RuleExpression2, precedence);

    

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
    setState(477);
    expression3(0);
    _ctx->stop = _input->LT(-1);
    setState(484);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 44, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression2Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression2);
        setState(479);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(480);
        match(gscParser::T__63);
        setState(481);
        expression3(0); 
      }
      setState(486);
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
  size_t startState = 66;
  enterRecursionRule(_localctx, 66, gscParser::RuleExpression3, precedence);

    

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
    setState(488);
    expression4(0);
    _ctx->stop = _input->LT(-1);
    setState(495);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 45, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression3Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression3);
        setState(490);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(491);
        match(gscParser::T__64);
        setState(492);
        expression4(0); 
      }
      setState(497);
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
  size_t startState = 68;
  enterRecursionRule(_localctx, 68, gscParser::RuleExpression4, precedence);

    

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
    setState(499);
    expression5(0);
    _ctx->stop = _input->LT(-1);
    setState(506);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 46, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression4Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression4);
        setState(501);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(502);
        match(gscParser::T__65);
        setState(503);
        expression5(0); 
      }
      setState(508);
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
  size_t startState = 70;
  enterRecursionRule(_localctx, 70, gscParser::RuleExpression5, precedence);

    

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
    setState(510);
    expression6(0);
    _ctx->stop = _input->LT(-1);
    setState(517);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 47, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression5Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression5);
        setState(512);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(513);
        match(gscParser::T__31);
        setState(514);
        expression6(0); 
      }
      setState(519);
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
  size_t startState = 72;
  enterRecursionRule(_localctx, 72, gscParser::RuleExpression6, precedence);

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
    setState(521);
    expression7(0);
    _ctx->stop = _input->LT(-1);
    setState(528);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 48, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression6Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression6);
        setState(523);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(524);
        _la = _input->LA(1);
        if (!(((((_la - 67) & ~ 0x3fULL) == 0) &&
          ((1ULL << (_la - 67)) & 15) != 0))) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(525);
        expression7(0); 
      }
      setState(530);
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
  size_t startState = 74;
  enterRecursionRule(_localctx, 74, gscParser::RuleExpression7, precedence);

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
    setState(532);
    expression8(0);
    _ctx->stop = _input->LT(-1);
    setState(539);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 49, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression7Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression7);
        setState(534);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(535);
        _la = _input->LA(1);
        if (!(((((_la - 28) & ~ 0x3fULL) == 0) &&
          ((1ULL << (_la - 28)) & 131941395333123) != 0))) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(536);
        expression8(0); 
      }
      setState(541);
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
  size_t startState = 76;
  enterRecursionRule(_localctx, 76, gscParser::RuleExpression8, precedence);

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
    setState(543);
    expression9(0);
    _ctx->stop = _input->LT(-1);
    setState(550);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 50, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression8Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression8);
        setState(545);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(546);
        _la = _input->LA(1);
        if (!(_la == gscParser::T__27

        || _la == gscParser::T__70)) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(547);
        expression9(0); 
      }
      setState(552);
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
  size_t startState = 78;
  enterRecursionRule(_localctx, 78, gscParser::RuleExpression9, precedence);

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
    setState(554);
    expression10(0);
    _ctx->stop = _input->LT(-1);
    setState(561);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 51, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression9Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression9);
        setState(556);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(557);
        _la = _input->LA(1);
        if (!(_la == gscParser::T__74

        || _la == gscParser::T__75)) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(558);
        expression10(0); 
      }
      setState(563);
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
  size_t startState = 80;
  enterRecursionRule(_localctx, 80, gscParser::RuleExpression10, precedence);

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
    setState(565);
    expression11(0);
    _ctx->stop = _input->LT(-1);
    setState(572);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 52, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression10Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression10);
        setState(567);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(568);
        _la = _input->LA(1);
        if (!(_la == gscParser::T__76

        || _la == gscParser::T__77)) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(569);
        expression11(0); 
      }
      setState(574);
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
  size_t startState = 82;
  enterRecursionRule(_localctx, 82, gscParser::RuleExpression11, precedence);

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
    expression12();
    _ctx->stop = _input->LT(-1);
    setState(583);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 53, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression11Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression11);
        setState(578);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(579);
        _la = _input->LA(1);
        if (!(((((_la - 31) & ~ 0x3fULL) == 0) &&
          ((1ULL << (_la - 31)) & 844424930131969) != 0))) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(580);
        expression12(); 
      }
      setState(585);
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
  enterRule(_localctx, 84, gscParser::RuleExpression12);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(595);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 54, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(586);
      _la = _input->LA(1);
      if (!(_la == gscParser::T__80

      || _la == gscParser::T__81)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(587);
      expression13();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(588);
      _la = _input->LA(1);
      if (!(_la == gscParser::T__82

      || _la == gscParser::T__83)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(589);
      left_value(0);
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(590);
      left_value(0);
      setState(591);
      _la = _input->LA(1);
      if (!(_la == gscParser::T__82

      || _la == gscParser::T__83)) {
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
      setState(593);
      is_expression();
      break;
    }

    case 5: {
      enterOuterAlt(_localctx, 5);
      setState(594);
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
  enterRule(_localctx, 86, gscParser::RuleExpression13);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(599);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 55, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(597);
      function_call_exp();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(598);
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
  enterRule(_localctx, 88, gscParser::RuleExpression14);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(604);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 56, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(601);
      const_expr();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(602);
      expression15();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(603);
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
  enterRule(_localctx, 90, gscParser::RuleExpression15);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(606);
    match(gscParser::T__6);
    setState(607);
    expression();
    setState(608);
    match(gscParser::T__8);
   
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
  enterRule(_localctx, 92, gscParser::RuleIs_expression);
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
    setState(610);
    expression13();
    setState(611);
    match(gscParser::T__84);
    setState(613);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__85) {
      setState(612);
      match(gscParser::T__85);
    }
    setState(615);
    _la = _input->LA(1);
    if (!(_la == gscParser::T__19 || ((((_la - 103) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 103)) & 7) != 0))) {
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
  enterRule(_localctx, 94, gscParser::RuleFunction_call_exp);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(627);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 59, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(617);
      function_call(0);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(618);
      expression14();
      setState(620);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (((((_la - 87) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 87)) & 15) != 0)) {
        setState(619);
        _la = _input->LA(1);
        if (!(((((_la - 87) & ~ 0x3fULL) == 0) &&
          ((1ULL << (_la - 87)) & 15) != 0))) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
      }
      setState(622);
      function_component();
      setState(623);
      match(gscParser::T__6);
      setState(624);
      expression_list();
      setState(625);
      match(gscParser::T__8);
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
  size_t startState = 96;
  enterRecursionRule(_localctx, 96, gscParser::RuleFunction_call, precedence);

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
    setState(650);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 63, _ctx)) {
    case 1: {
      setState(631);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (((((_la - 87) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 87)) & 15) != 0)) {
        setState(630);
        _la = _input->LA(1);
        if (!(((((_la - 87) & ~ 0x3fULL) == 0) &&
          ((1ULL << (_la - 87)) & 15) != 0))) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
      }
      setState(633);
      function_component();
      setState(634);
      match(gscParser::T__6);
      setState(635);
      expression_list();
      setState(636);
      match(gscParser::T__8);
      break;
    }

    case 2: {
      setState(640);
      _errHandler->sync(this);
      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 61, _ctx)) {
      case 1: {
        setState(638);
        const_expr();
        break;
      }

      case 2: {
        setState(639);
        expression15();
        break;
      }

      default:
        break;
      }
      setState(643);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (((((_la - 87) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 87)) & 15) != 0)) {
        setState(642);
        _la = _input->LA(1);
        if (!(((((_la - 87) & ~ 0x3fULL) == 0) &&
          ((1ULL << (_la - 87)) & 15) != 0))) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
      }
      setState(645);
      function_component();
      setState(646);
      match(gscParser::T__6);
      setState(647);
      expression_list();
      setState(648);
      match(gscParser::T__8);
      break;
    }

    default:
      break;
    }
    _ctx->stop = _input->LT(-1);
    setState(663);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 65, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Function_callContext>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleFunction_call);
        setState(652);

        if (!(precpred(_ctx, 1))) throw FailedPredicateException(this, "precpred(_ctx, 1)");
        setState(654);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (((((_la - 87) & ~ 0x3fULL) == 0) &&
          ((1ULL << (_la - 87)) & 15) != 0)) {
          setState(653);
          _la = _input->LA(1);
          if (!(((((_la - 87) & ~ 0x3fULL) == 0) &&
            ((1ULL << (_la - 87)) & 15) != 0))) {
          _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
        }
        setState(656);
        function_component();
        setState(657);
        match(gscParser::T__6);
        setState(658);
        expression_list();
        setState(659);
        match(gscParser::T__8); 
      }
      setState(665);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 65, _ctx);
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
  size_t startState = 98;
  enterRecursionRule(_localctx, 98, gscParser::RuleLeft_value, precedence);

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
    setState(690);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 69, _ctx)) {
    case 1: {
      setState(667);
      idf();
      break;
    }

    case 2: {
      setState(671);
      _errHandler->sync(this);
      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 66, _ctx)) {
      case 1: {
        setState(668);
        function_call(0);
        break;
      }

      case 2: {
        setState(669);
        const_expr();
        break;
      }

      case 3: {
        setState(670);
        expression15();
        break;
      }

      default:
        break;
      }
      setState(673);
      _la = _input->LA(1);
      if (!(_la == gscParser::T__90

      || _la == gscParser::T__91)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(679);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case gscParser::IDENTIFIER: {
          setState(674);
          idf();
          break;
        }

        case gscParser::T__6: {
          setState(675);
          match(gscParser::T__6);
          setState(676);
          expression();
          setState(677);
          match(gscParser::T__8);
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      break;
    }

    case 3: {
      setState(684);
      _errHandler->sync(this);
      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 68, _ctx)) {
      case 1: {
        setState(681);
        function_call(0);
        break;
      }

      case 2: {
        setState(682);
        const_expr();
        break;
      }

      case 3: {
        setState(683);
        expression15();
        break;
      }

      default:
        break;
      }
      setState(686);
      match(gscParser::T__23);
      setState(687);
      expression();
      setState(688);
      match(gscParser::T__24);
      break;
    }

    default:
      break;
    }
    _ctx->stop = _input->LT(-1);
    setState(708);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 72, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        setState(706);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 71, _ctx)) {
        case 1: {
          _localctx = _tracker.createInstance<Left_valueContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleLeft_value);
          setState(692);

          if (!(precpred(_ctx, 4))) throw FailedPredicateException(this, "precpred(_ctx, 4)");
          setState(693);
          _la = _input->LA(1);
          if (!(_la == gscParser::T__90

          || _la == gscParser::T__91)) {
          _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(699);
          _errHandler->sync(this);
          switch (_input->LA(1)) {
            case gscParser::IDENTIFIER: {
              setState(694);
              idf();
              break;
            }

            case gscParser::T__6: {
              setState(695);
              match(gscParser::T__6);
              setState(696);
              expression();
              setState(697);
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
          setState(701);

          if (!(precpred(_ctx, 3))) throw FailedPredicateException(this, "precpred(_ctx, 3)");
          setState(702);
          match(gscParser::T__23);
          setState(703);
          expression();
          setState(704);
          match(gscParser::T__24);
          break;
        }

        default:
          break;
        } 
      }
      setState(710);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 72, _ctx);
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

gscParser::Class_initContext* gscParser::Const_exprContext::class_init() {
  return getRuleContext<gscParser::Class_initContext>(0);
}

gscParser::Array_defContext* gscParser::Const_exprContext::array_def() {
  return getRuleContext<gscParser::Array_defContext>(0);
}

gscParser::Struct_defContext* gscParser::Const_exprContext::struct_def() {
  return getRuleContext<gscParser::Struct_defContext>(0);
}

gscParser::FunctionContext* gscParser::Const_exprContext::function() {
  return getRuleContext<gscParser::FunctionContext>(0);
}

gscParser::Const_expr_staticContext* gscParser::Const_exprContext::const_expr_static() {
  return getRuleContext<gscParser::Const_expr_staticContext>(0);
}

tree::TerminalNode* gscParser::Const_exprContext::UNDEFINED_VALUE() {
  return getToken(gscParser::UNDEFINED_VALUE, 0);
}

gscParser::Function_refContext* gscParser::Const_exprContext::function_ref() {
  return getRuleContext<gscParser::Function_refContext>(0);
}

gscParser::Data_refContext* gscParser::Const_exprContext::data_ref() {
  return getRuleContext<gscParser::Data_refContext>(0);
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
  enterRule(_localctx, 100, gscParser::RuleConst_expr);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(719);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 73, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(711);
      class_init();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(712);
      array_def();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(713);
      struct_def();
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(714);
      function();
      break;
    }

    case 5: {
      enterOuterAlt(_localctx, 5);
      setState(715);
      const_expr_static();
      break;
    }

    case 6: {
      enterOuterAlt(_localctx, 6);
      setState(716);
      match(gscParser::UNDEFINED_VALUE);
      break;
    }

    case 7: {
      enterOuterAlt(_localctx, 7);
      setState(717);
      function_ref();
      break;
    }

    case 8: {
      enterOuterAlt(_localctx, 8);
      setState(718);
      data_ref();
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

//----------------- Const_expr_staticContext ------------------------------------------------------------------

gscParser::Const_expr_staticContext::Const_expr_staticContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

gscParser::Vector_valueContext* gscParser::Const_expr_staticContext::vector_value() {
  return getRuleContext<gscParser::Vector_valueContext>(0);
}

gscParser::NumberContext* gscParser::Const_expr_staticContext::number() {
  return getRuleContext<gscParser::NumberContext>(0);
}

tree::TerminalNode* gscParser::Const_expr_staticContext::BOOL_VALUE() {
  return getToken(gscParser::BOOL_VALUE, 0);
}

tree::TerminalNode* gscParser::Const_expr_staticContext::FLOATVAL() {
  return getToken(gscParser::FLOATVAL, 0);
}

tree::TerminalNode* gscParser::Const_expr_staticContext::STRING() {
  return getToken(gscParser::STRING, 0);
}

tree::TerminalNode* gscParser::Const_expr_staticContext::HASHSTRING() {
  return getToken(gscParser::HASHSTRING, 0);
}


size_t gscParser::Const_expr_staticContext::getRuleIndex() const {
  return gscParser::RuleConst_expr_static;
}


std::any gscParser::Const_expr_staticContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitConst_expr_static(this);
  else
    return visitor->visitChildren(this);
}

gscParser::Const_expr_staticContext* gscParser::const_expr_static() {
  Const_expr_staticContext *_localctx = _tracker.createInstance<Const_expr_staticContext>(_ctx, getState());
  enterRule(_localctx, 102, gscParser::RuleConst_expr_static);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(727);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case gscParser::T__6: {
        enterOuterAlt(_localctx, 1);
        setState(721);
        vector_value();
        break;
      }

      case gscParser::INTEGER10:
      case gscParser::INTEGER16:
      case gscParser::INTEGER8:
      case gscParser::INTEGER2:
      case gscParser::IDENTIFIER: {
        enterOuterAlt(_localctx, 2);
        setState(722);
        number();
        break;
      }

      case gscParser::BOOL_VALUE: {
        enterOuterAlt(_localctx, 3);
        setState(723);
        match(gscParser::BOOL_VALUE);
        break;
      }

      case gscParser::FLOATVAL: {
        enterOuterAlt(_localctx, 4);
        setState(724);
        match(gscParser::FLOATVAL);
        break;
      }

      case gscParser::STRING: {
        enterOuterAlt(_localctx, 5);
        setState(725);
        match(gscParser::STRING);
        break;
      }

      case gscParser::HASHSTRING: {
        enterOuterAlt(_localctx, 6);
        setState(726);
        match(gscParser::HASHSTRING);
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
  enterRule(_localctx, 104, gscParser::RuleFunction_ref);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(744);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 76, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(729);
      match(gscParser::T__31);
      setState(732);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 75, _ctx)) {
      case 1: {
        setState(730);
        match(gscParser::IDENTIFIER);
        setState(731);
        match(gscParser::T__29);
        break;
      }

      default:
        break;
      }
      setState(734);
      match(gscParser::IDENTIFIER);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(735);
      match(gscParser::T__92);
      setState(736);
      match(gscParser::IDENTIFIER);
      setState(737);
      match(gscParser::T__27);
      setState(738);
      match(gscParser::PATH);
      setState(739);
      match(gscParser::T__28);
      setState(740);
      match(gscParser::T__29);
      setState(741);
      match(gscParser::IDENTIFIER);
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(742);
      match(gscParser::T__31);
      setState(743);
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

//----------------- Data_refContext ------------------------------------------------------------------

gscParser::Data_refContext::Data_refContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> gscParser::Data_refContext::IDENTIFIER() {
  return getTokens(gscParser::IDENTIFIER);
}

tree::TerminalNode* gscParser::Data_refContext::IDENTIFIER(size_t i) {
  return getToken(gscParser::IDENTIFIER, i);
}


size_t gscParser::Data_refContext::getRuleIndex() const {
  return gscParser::RuleData_ref;
}


std::any gscParser::Data_refContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitData_ref(this);
  else
    return visitor->visitChildren(this);
}

gscParser::Data_refContext* gscParser::data_ref() {
  Data_refContext *_localctx = _tracker.createInstance<Data_refContext>(_ctx, getState());
  enterRule(_localctx, 106, gscParser::RuleData_ref);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(746);
    match(gscParser::T__92);
    setState(747);
    match(gscParser::T__23);
    setState(748);
    match(gscParser::IDENTIFIER);
    setState(749);
    match(gscParser::T__7);
    setState(750);
    match(gscParser::IDENTIFIER);
    setState(751);
    match(gscParser::T__24);
   
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
  enterRule(_localctx, 108, gscParser::RuleNumber);
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
    setState(753);
    _la = _input->LA(1);
    if (!(((((_la - 97) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 97)) & 271) != 0))) {
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
  enterRule(_localctx, 110, gscParser::RuleVector_value);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(755);
    match(gscParser::T__6);
    setState(756);
    expression();
    setState(757);
    match(gscParser::T__7);
    setState(758);
    expression();
    setState(759);
    match(gscParser::T__7);
    setState(760);
    expression();
    setState(761);
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
  enterRule(_localctx, 112, gscParser::RuleArray_def);
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
    setState(763);
    match(gscParser::T__23);
    setState(785);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 4461822080) != 0) || ((((_la - 81) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 81)) & 434058191) != 0)) {
      setState(767);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 77, _ctx)) {
      case 1: {
        setState(764);
        expression();
        setState(765);
        match(gscParser::T__15);
        break;
      }

      default:
        break;
      }
      setState(769);
      expression();
      setState(779);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 79, _ctx);
      while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
        if (alt == 1) {
          setState(770);
          match(gscParser::T__7);
          setState(774);
          _errHandler->sync(this);

          switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 78, _ctx)) {
          case 1: {
            setState(771);
            expression();
            setState(772);
            match(gscParser::T__15);
            break;
          }

          default:
            break;
          }
          setState(776);
          expression(); 
        }
        setState(781);
        _errHandler->sync(this);
        alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 79, _ctx);
      }
      setState(783);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == gscParser::T__7) {
        setState(782);
        match(gscParser::T__7);
      }
    }
    setState(787);
    match(gscParser::T__24);
   
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
  enterRule(_localctx, 114, gscParser::RuleStruct_def);
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
    setState(789);
    match(gscParser::T__16);
    setState(811);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 4461822080) != 0) || ((((_la - 81) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 81)) & 467612623) != 0)) {
      setState(792);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case gscParser::STRUCT_IDENTIFIER: {
          setState(790);
          match(gscParser::STRUCT_IDENTIFIER);
          break;
        }

        case gscParser::T__6:
        case gscParser::T__16:
        case gscParser::T__19:
        case gscParser::T__20:
        case gscParser::T__21:
        case gscParser::T__22:
        case gscParser::T__23:
        case gscParser::T__26:
        case gscParser::T__31:
        case gscParser::T__80:
        case gscParser::T__81:
        case gscParser::T__82:
        case gscParser::T__83:
        case gscParser::T__86:
        case gscParser::T__87:
        case gscParser::T__88:
        case gscParser::T__89:
        case gscParser::T__92:
        case gscParser::T__93:
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
          setState(791);
          expression();
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(794);
      match(gscParser::T__15);
      setState(795);
      expression();
      setState(805);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 84, _ctx);
      while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
        if (alt == 1) {
          setState(796);
          match(gscParser::T__7);
          setState(799);
          _errHandler->sync(this);
          switch (_input->LA(1)) {
            case gscParser::STRUCT_IDENTIFIER: {
              setState(797);
              match(gscParser::STRUCT_IDENTIFIER);
              break;
            }

            case gscParser::T__6:
            case gscParser::T__16:
            case gscParser::T__19:
            case gscParser::T__20:
            case gscParser::T__21:
            case gscParser::T__22:
            case gscParser::T__23:
            case gscParser::T__26:
            case gscParser::T__31:
            case gscParser::T__80:
            case gscParser::T__81:
            case gscParser::T__82:
            case gscParser::T__83:
            case gscParser::T__86:
            case gscParser::T__87:
            case gscParser::T__88:
            case gscParser::T__89:
            case gscParser::T__92:
            case gscParser::T__93:
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
              setState(798);
              expression();
              break;
            }

          default:
            throw NoViableAltException(this);
          }
          setState(801);
          match(gscParser::T__15);
          setState(802);
          expression(); 
        }
        setState(807);
        _errHandler->sync(this);
        alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 84, _ctx);
      }
      setState(809);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == gscParser::T__7) {
        setState(808);
        match(gscParser::T__7);
      }
    }
    setState(813);
    match(gscParser::T__17);
   
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
  enterRule(_localctx, 116, gscParser::RuleClass_init);
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
    setState(815);
    match(gscParser::T__93);
    setState(816);
    match(gscParser::IDENTIFIER);
    setState(817);
    match(gscParser::T__6);
    setState(826);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 4461822080) != 0) || ((((_la - 81) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 81)) & 434058191) != 0)) {
      setState(818);
      expression();
      setState(823);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == gscParser::T__7) {
        setState(819);
        match(gscParser::T__7);
        setState(820);
        expression();
        setState(825);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
    }
    setState(828);
    match(gscParser::T__8);
   
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
  enterRule(_localctx, 118, gscParser::RuleIdf);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(830);
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
    case 30: return expression0Sempred(antlrcpp::downCast<Expression0Context *>(context), predicateIndex);
    case 31: return expression1Sempred(antlrcpp::downCast<Expression1Context *>(context), predicateIndex);
    case 32: return expression2Sempred(antlrcpp::downCast<Expression2Context *>(context), predicateIndex);
    case 33: return expression3Sempred(antlrcpp::downCast<Expression3Context *>(context), predicateIndex);
    case 34: return expression4Sempred(antlrcpp::downCast<Expression4Context *>(context), predicateIndex);
    case 35: return expression5Sempred(antlrcpp::downCast<Expression5Context *>(context), predicateIndex);
    case 36: return expression6Sempred(antlrcpp::downCast<Expression6Context *>(context), predicateIndex);
    case 37: return expression7Sempred(antlrcpp::downCast<Expression7Context *>(context), predicateIndex);
    case 38: return expression8Sempred(antlrcpp::downCast<Expression8Context *>(context), predicateIndex);
    case 39: return expression9Sempred(antlrcpp::downCast<Expression9Context *>(context), predicateIndex);
    case 40: return expression10Sempred(antlrcpp::downCast<Expression10Context *>(context), predicateIndex);
    case 41: return expression11Sempred(antlrcpp::downCast<Expression11Context *>(context), predicateIndex);
    case 48: return function_callSempred(antlrcpp::downCast<Function_callContext *>(context), predicateIndex);
    case 49: return left_valueSempred(antlrcpp::downCast<Left_valueContext *>(context), predicateIndex);

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
