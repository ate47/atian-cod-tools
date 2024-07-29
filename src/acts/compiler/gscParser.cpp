
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
      "expression", "set_expression", "expression1", "expression2", "expression3", 
      "expression4", "expression5", "expression6", "expression7", "expression8", 
      "expression9", "expression10", "expression11", "expression12", "expression13", 
      "expression14", "expression15", "is_expression", "function_call_exp", 
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
      "'||'", "'&&'", "'|'", "'^'", "'!='", "'=='", "'!=='", "'==='", "'<='", 
      "'>='", "'<<'", "'>>'", "'+'", "'-'", "'/'", "'%'", "'!'", "'~'", 
      "'++'", "'--'", "'is'", "'not'", "'true'", "'false'", "'thread'", 
      "'childthread'", "'threadendon'", "'builtin'", "'.'", "'@'", "", "", 
      "", "", "", "", "", "", "", "'undefined'"
    },
    std::vector<std::string>{
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
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
  	4,1,103,719,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,7,
  	14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,7,
  	21,2,22,7,22,2,23,7,23,2,24,7,24,2,25,7,25,2,26,7,26,2,27,7,27,2,28,7,
  	28,2,29,7,29,2,30,7,30,2,31,7,31,2,32,7,32,2,33,7,33,2,34,7,34,2,35,7,
  	35,2,36,7,36,2,37,7,37,2,38,7,38,2,39,7,39,2,40,7,40,2,41,7,41,2,42,7,
  	42,2,43,7,43,2,44,7,44,2,45,7,45,2,46,7,46,2,47,7,47,2,48,7,48,2,49,7,
  	49,2,50,7,50,2,51,7,51,1,0,1,0,1,0,1,0,1,0,1,0,1,0,5,0,112,8,0,10,0,12,
  	0,115,9,0,1,0,1,0,1,1,1,1,1,1,1,1,1,2,1,2,1,2,1,2,1,3,1,3,1,3,1,3,1,4,
  	1,4,1,4,1,4,1,4,1,4,1,5,3,5,138,8,5,1,5,3,5,141,8,5,1,5,1,5,1,5,1,5,1,
  	5,3,5,148,8,5,3,5,150,8,5,1,5,1,5,1,5,1,5,3,5,156,8,5,1,5,3,5,159,8,5,
  	1,5,3,5,162,8,5,1,5,1,5,1,5,1,5,3,5,168,8,5,1,5,1,5,1,6,1,6,1,6,1,6,1,
  	6,1,6,1,6,3,6,179,8,6,1,7,1,7,1,7,5,7,184,8,7,10,7,12,7,187,9,7,3,7,189,
  	8,7,1,8,3,8,192,8,8,1,8,1,8,1,8,3,8,197,8,8,1,8,3,8,200,8,8,1,9,1,9,1,
  	9,5,9,205,8,9,10,9,12,9,208,9,9,3,9,210,8,9,1,10,1,10,5,10,214,8,10,10,
  	10,12,10,217,9,10,1,10,1,10,1,11,1,11,3,11,223,8,11,1,11,1,11,1,11,1,
  	11,1,11,1,11,1,11,1,11,3,11,233,8,11,1,12,1,12,5,12,237,8,12,10,12,12,
  	12,240,9,12,1,12,1,12,1,13,1,13,1,13,3,13,247,8,13,1,13,1,13,3,13,251,
  	8,13,1,13,1,13,3,13,255,8,13,1,13,1,13,1,13,1,14,1,14,1,14,1,14,1,14,
  	1,14,1,15,1,15,1,15,1,15,1,15,1,15,1,15,1,16,1,16,1,16,1,16,1,16,3,16,
  	278,8,16,1,16,1,16,1,16,1,16,1,16,1,17,1,17,1,17,1,17,1,17,1,17,1,17,
  	3,17,292,8,17,1,18,1,18,1,18,1,18,1,18,1,18,1,18,1,18,3,18,302,8,18,1,
  	18,1,18,5,18,306,8,18,10,18,12,18,309,9,18,4,18,311,8,18,11,18,12,18,
  	312,1,18,1,18,1,19,1,19,1,19,1,19,1,19,1,19,3,19,323,8,19,1,19,1,19,1,
  	20,1,20,1,20,1,20,1,20,3,20,332,8,20,1,21,1,21,1,21,1,21,1,21,1,22,1,
  	22,3,22,341,8,22,1,22,1,22,1,22,1,22,1,22,1,22,1,22,1,22,1,22,1,22,1,
  	22,1,22,1,22,1,22,1,22,3,22,358,8,22,1,23,1,23,1,23,3,23,363,8,23,1,24,
  	1,24,1,24,1,24,1,24,1,24,1,24,1,24,3,24,373,8,24,1,25,1,25,1,25,1,25,
  	1,26,1,26,1,26,1,26,1,26,1,26,5,26,385,8,26,10,26,12,26,388,9,26,1,27,
  	1,27,1,27,1,27,1,27,1,27,5,27,396,8,27,10,27,12,27,399,9,27,1,28,1,28,
  	1,28,1,28,1,28,1,28,5,28,407,8,28,10,28,12,28,410,9,28,1,29,1,29,1,29,
  	1,29,1,29,1,29,5,29,418,8,29,10,29,12,29,421,9,29,1,30,1,30,1,30,1,30,
  	1,30,1,30,5,30,429,8,30,10,30,12,30,432,9,30,1,31,1,31,1,31,1,31,1,31,
  	1,31,5,31,440,8,31,10,31,12,31,443,9,31,1,32,1,32,1,32,1,32,1,32,1,32,
  	5,32,451,8,32,10,32,12,32,454,9,32,1,33,1,33,1,33,1,33,1,33,1,33,5,33,
  	462,8,33,10,33,12,33,465,9,33,1,34,1,34,1,34,1,34,1,34,1,34,5,34,473,
  	8,34,10,34,12,34,476,9,34,1,35,1,35,1,35,1,35,1,35,1,35,5,35,484,8,35,
  	10,35,12,35,487,9,35,1,36,1,36,1,36,1,36,1,36,1,36,5,36,495,8,36,10,36,
  	12,36,498,9,36,1,37,1,37,1,37,1,37,1,37,1,37,1,37,1,37,1,37,3,37,509,
  	8,37,1,38,1,38,3,38,513,8,38,1,39,1,39,1,39,3,39,518,8,39,1,40,1,40,1,
  	40,1,40,1,41,1,41,1,41,3,41,527,8,41,1,41,1,41,1,42,1,42,1,42,3,42,534,
  	8,42,1,42,1,42,1,42,1,42,1,42,3,42,541,8,42,1,43,1,43,3,43,545,8,43,1,
  	43,1,43,1,43,1,43,1,43,1,43,1,43,3,43,554,8,43,1,43,3,43,557,8,43,1,43,
  	1,43,1,43,1,43,1,43,3,43,564,8,43,1,43,1,43,3,43,568,8,43,1,43,1,43,1,
  	43,1,43,1,43,5,43,575,8,43,10,43,12,43,578,9,43,1,44,1,44,1,44,1,44,1,
  	44,3,44,585,8,44,1,44,1,44,1,44,1,44,1,44,1,44,3,44,593,8,44,1,44,1,44,
  	1,44,3,44,598,8,44,1,44,1,44,1,44,1,44,3,44,604,8,44,1,44,1,44,1,44,1,
  	44,1,44,1,44,1,44,3,44,613,8,44,1,44,1,44,1,44,1,44,1,44,5,44,620,8,44,
  	10,44,12,44,623,9,44,1,45,1,45,1,45,1,45,1,45,1,45,1,45,1,45,1,45,1,45,
  	1,45,3,45,636,8,45,1,46,1,46,1,46,3,46,641,8,46,1,46,1,46,1,46,1,46,1,
  	46,1,46,1,46,1,46,1,46,1,46,3,46,653,8,46,1,47,1,47,1,48,1,48,1,48,1,
  	48,1,48,1,48,1,48,1,48,1,49,1,49,1,49,1,49,3,49,669,8,49,1,49,1,49,1,
  	49,1,49,1,49,3,49,676,8,49,1,49,5,49,679,8,49,10,49,12,49,682,9,49,1,
  	49,3,49,685,8,49,3,49,687,8,49,1,49,1,49,1,50,1,50,1,50,3,50,694,8,50,
  	1,50,1,50,1,50,1,50,1,50,3,50,701,8,50,1,50,1,50,5,50,705,8,50,10,50,
  	12,50,708,9,50,1,50,3,50,711,8,50,3,50,713,8,50,1,50,1,50,1,51,1,51,1,
  	51,0,13,52,54,56,58,60,62,64,66,68,70,72,86,88,52,0,2,4,6,8,10,12,14,
  	16,18,20,22,24,26,28,30,32,34,36,38,40,42,44,46,48,50,52,54,56,58,60,
  	62,64,66,68,70,72,74,76,78,80,82,84,86,88,90,92,94,96,98,100,102,0,18,
  	1,0,3,4,2,0,99,99,101,101,1,0,8,9,1,0,25,26,1,0,41,42,1,0,43,45,2,0,10,
  	10,48,58,1,0,63,66,2,0,21,22,67,68,2,0,21,21,67,67,1,0,69,70,1,0,71,72,
  	2,0,25,25,73,74,1,0,75,76,1,0,77,78,3,0,11,11,81,82,98,99,1,0,83,86,2,
  	0,91,94,99,99,780,0,113,1,0,0,0,2,118,1,0,0,0,4,122,1,0,0,0,6,126,1,0,
  	0,0,8,130,1,0,0,0,10,137,1,0,0,0,12,171,1,0,0,0,14,188,1,0,0,0,16,199,
  	1,0,0,0,18,209,1,0,0,0,20,211,1,0,0,0,22,222,1,0,0,0,24,234,1,0,0,0,26,
  	243,1,0,0,0,28,259,1,0,0,0,30,265,1,0,0,0,32,272,1,0,0,0,34,284,1,0,0,
  	0,36,293,1,0,0,0,38,322,1,0,0,0,40,326,1,0,0,0,42,333,1,0,0,0,44,357,
  	1,0,0,0,46,359,1,0,0,0,48,372,1,0,0,0,50,374,1,0,0,0,52,378,1,0,0,0,54,
  	389,1,0,0,0,56,400,1,0,0,0,58,411,1,0,0,0,60,422,1,0,0,0,62,433,1,0,0,
  	0,64,444,1,0,0,0,66,455,1,0,0,0,68,466,1,0,0,0,70,477,1,0,0,0,72,488,
  	1,0,0,0,74,508,1,0,0,0,76,512,1,0,0,0,78,517,1,0,0,0,80,519,1,0,0,0,82,
  	523,1,0,0,0,84,540,1,0,0,0,86,563,1,0,0,0,88,603,1,0,0,0,90,635,1,0,0,
  	0,92,652,1,0,0,0,94,654,1,0,0,0,96,656,1,0,0,0,98,664,1,0,0,0,100,690,
  	1,0,0,0,102,716,1,0,0,0,104,112,5,1,0,0,105,112,5,2,0,0,106,112,3,10,
  	5,0,107,112,3,2,1,0,108,112,3,4,2,0,109,112,3,6,3,0,110,112,3,8,4,0,111,
  	104,1,0,0,0,111,105,1,0,0,0,111,106,1,0,0,0,111,107,1,0,0,0,111,108,1,
  	0,0,0,111,109,1,0,0,0,111,110,1,0,0,0,112,115,1,0,0,0,113,111,1,0,0,0,
  	113,114,1,0,0,0,114,116,1,0,0,0,115,113,1,0,0,0,116,117,5,0,0,1,117,1,
  	1,0,0,0,118,119,7,0,0,0,119,120,7,1,0,0,120,121,5,5,0,0,121,3,1,0,0,0,
  	122,123,5,6,0,0,123,124,5,99,0,0,124,125,5,5,0,0,125,5,1,0,0,0,126,127,
  	5,7,0,0,127,128,7,1,0,0,128,129,5,5,0,0,129,7,1,0,0,0,130,131,7,2,0,0,
  	131,132,5,99,0,0,132,133,5,10,0,0,133,134,3,48,24,0,134,135,5,5,0,0,135,
  	9,1,0,0,0,136,138,5,11,0,0,137,136,1,0,0,0,137,138,1,0,0,0,138,140,1,
  	0,0,0,139,141,5,12,0,0,140,139,1,0,0,0,140,141,1,0,0,0,141,149,1,0,0,
  	0,142,147,5,13,0,0,143,144,5,14,0,0,144,145,3,94,47,0,145,146,5,15,0,
  	0,146,148,1,0,0,0,147,143,1,0,0,0,147,148,1,0,0,0,148,150,1,0,0,0,149,
  	142,1,0,0,0,149,150,1,0,0,0,150,155,1,0,0,0,151,152,5,16,0,0,152,153,
  	5,17,0,0,153,154,5,99,0,0,154,156,5,18,0,0,155,151,1,0,0,0,155,156,1,
  	0,0,0,156,158,1,0,0,0,157,159,3,12,6,0,158,157,1,0,0,0,158,159,1,0,0,
  	0,159,161,1,0,0,0,160,162,5,99,0,0,161,160,1,0,0,0,161,162,1,0,0,0,162,
  	163,1,0,0,0,163,164,5,14,0,0,164,165,3,14,7,0,165,167,5,15,0,0,166,168,
  	5,19,0,0,167,166,1,0,0,0,167,168,1,0,0,0,168,169,1,0,0,0,169,170,3,20,
  	10,0,170,11,1,0,0,0,171,172,5,20,0,0,172,178,5,99,0,0,173,174,5,21,0,
  	0,174,175,5,101,0,0,175,176,5,22,0,0,176,177,5,23,0,0,177,179,5,99,0,
  	0,178,173,1,0,0,0,178,179,1,0,0,0,179,13,1,0,0,0,180,185,3,16,8,0,181,
  	182,5,24,0,0,182,184,3,16,8,0,183,181,1,0,0,0,184,187,1,0,0,0,185,183,
  	1,0,0,0,185,186,1,0,0,0,186,189,1,0,0,0,187,185,1,0,0,0,188,180,1,0,0,
  	0,188,189,1,0,0,0,189,15,1,0,0,0,190,192,7,3,0,0,191,190,1,0,0,0,191,
  	192,1,0,0,0,192,193,1,0,0,0,193,196,5,99,0,0,194,195,5,10,0,0,195,197,
  	3,48,24,0,196,194,1,0,0,0,196,197,1,0,0,0,197,200,1,0,0,0,198,200,5,27,
  	0,0,199,191,1,0,0,0,199,198,1,0,0,0,200,17,1,0,0,0,201,206,3,48,24,0,
  	202,203,5,24,0,0,203,205,3,48,24,0,204,202,1,0,0,0,205,208,1,0,0,0,206,
  	204,1,0,0,0,206,207,1,0,0,0,207,210,1,0,0,0,208,206,1,0,0,0,209,201,1,
  	0,0,0,209,210,1,0,0,0,210,19,1,0,0,0,211,215,5,28,0,0,212,214,3,22,11,
  	0,213,212,1,0,0,0,214,217,1,0,0,0,215,213,1,0,0,0,215,216,1,0,0,0,216,
  	218,1,0,0,0,217,215,1,0,0,0,218,219,5,29,0,0,219,21,1,0,0,0,220,221,5,
  	99,0,0,221,223,5,30,0,0,222,220,1,0,0,0,222,223,1,0,0,0,223,232,1,0,0,
  	0,224,233,3,20,10,0,225,233,3,24,12,0,226,233,3,26,13,0,227,233,3,34,
  	17,0,228,233,3,28,14,0,229,233,3,32,16,0,230,233,3,38,19,0,231,233,3,
  	36,18,0,232,224,1,0,0,0,232,225,1,0,0,0,232,226,1,0,0,0,232,227,1,0,0,
  	0,232,228,1,0,0,0,232,229,1,0,0,0,232,230,1,0,0,0,232,231,1,0,0,0,233,
  	23,1,0,0,0,234,238,5,1,0,0,235,237,3,22,11,0,236,235,1,0,0,0,237,240,
  	1,0,0,0,238,236,1,0,0,0,238,239,1,0,0,0,239,241,1,0,0,0,240,238,1,0,0,
  	0,241,242,5,2,0,0,242,25,1,0,0,0,243,244,5,31,0,0,244,246,5,14,0,0,245,
  	247,3,48,24,0,246,245,1,0,0,0,246,247,1,0,0,0,247,248,1,0,0,0,248,250,
  	5,5,0,0,249,251,3,48,24,0,250,249,1,0,0,0,250,251,1,0,0,0,251,252,1,0,
  	0,0,252,254,5,5,0,0,253,255,3,48,24,0,254,253,1,0,0,0,254,255,1,0,0,0,
  	255,256,1,0,0,0,256,257,5,15,0,0,257,258,3,22,11,0,258,27,1,0,0,0,259,
  	260,5,32,0,0,260,261,5,14,0,0,261,262,3,48,24,0,262,263,5,15,0,0,263,
  	264,3,22,11,0,264,29,1,0,0,0,265,266,5,33,0,0,266,267,3,22,11,0,267,268,
  	5,32,0,0,268,269,5,14,0,0,269,270,3,48,24,0,270,271,5,15,0,0,271,31,1,
  	0,0,0,272,273,5,34,0,0,273,274,5,14,0,0,274,277,5,99,0,0,275,276,5,24,
  	0,0,276,278,5,99,0,0,277,275,1,0,0,0,277,278,1,0,0,0,278,279,1,0,0,0,
  	279,280,5,35,0,0,280,281,3,48,24,0,281,282,5,15,0,0,282,283,3,22,11,0,
  	283,33,1,0,0,0,284,285,5,36,0,0,285,286,5,14,0,0,286,287,3,48,24,0,287,
  	288,5,15,0,0,288,291,3,22,11,0,289,290,5,37,0,0,290,292,3,22,11,0,291,
  	289,1,0,0,0,291,292,1,0,0,0,292,35,1,0,0,0,293,294,5,38,0,0,294,295,5,
  	14,0,0,295,296,3,48,24,0,296,297,5,15,0,0,297,310,5,28,0,0,298,299,5,
  	39,0,0,299,302,3,90,45,0,300,302,5,40,0,0,301,298,1,0,0,0,301,300,1,0,
  	0,0,302,303,1,0,0,0,303,307,5,30,0,0,304,306,3,22,11,0,305,304,1,0,0,
  	0,306,309,1,0,0,0,307,305,1,0,0,0,307,308,1,0,0,0,308,311,1,0,0,0,309,
  	307,1,0,0,0,310,301,1,0,0,0,311,312,1,0,0,0,312,310,1,0,0,0,312,313,1,
  	0,0,0,313,314,1,0,0,0,314,315,5,29,0,0,315,37,1,0,0,0,316,323,3,48,24,
  	0,317,323,3,46,23,0,318,323,3,30,15,0,319,323,3,84,42,0,320,323,3,40,
  	20,0,321,323,3,42,21,0,322,316,1,0,0,0,322,317,1,0,0,0,322,318,1,0,0,
  	0,322,319,1,0,0,0,322,320,1,0,0,0,322,321,1,0,0,0,323,324,1,0,0,0,324,
  	325,5,5,0,0,325,39,1,0,0,0,326,331,7,4,0,0,327,328,5,14,0,0,328,329,3,
  	94,47,0,329,330,5,15,0,0,330,332,1,0,0,0,331,327,1,0,0,0,331,332,1,0,
  	0,0,332,41,1,0,0,0,333,334,7,5,0,0,334,335,5,14,0,0,335,336,3,94,47,0,
  	336,337,5,15,0,0,337,43,1,0,0,0,338,339,5,99,0,0,339,341,5,23,0,0,340,
  	338,1,0,0,0,340,341,1,0,0,0,341,342,1,0,0,0,342,358,5,99,0,0,343,344,
  	5,17,0,0,344,345,5,17,0,0,345,346,3,48,24,0,346,347,5,18,0,0,347,348,
  	5,18,0,0,348,358,1,0,0,0,349,350,5,17,0,0,350,351,5,17,0,0,351,352,3,
  	48,24,0,352,353,5,18,0,0,353,354,5,18,0,0,354,355,5,46,0,0,355,356,5,
  	99,0,0,356,358,1,0,0,0,357,340,1,0,0,0,357,343,1,0,0,0,357,349,1,0,0,
  	0,358,45,1,0,0,0,359,362,5,96,0,0,360,363,5,99,0,0,361,363,3,48,24,0,
  	362,360,1,0,0,0,362,361,1,0,0,0,362,363,1,0,0,0,363,47,1,0,0,0,364,373,
  	3,50,25,0,365,366,3,52,26,0,366,367,5,47,0,0,367,368,3,48,24,0,368,369,
  	5,30,0,0,369,370,3,48,24,0,370,373,1,0,0,0,371,373,3,52,26,0,372,364,
  	1,0,0,0,372,365,1,0,0,0,372,371,1,0,0,0,373,49,1,0,0,0,374,375,3,88,44,
  	0,375,376,7,6,0,0,376,377,3,48,24,0,377,51,1,0,0,0,378,379,6,26,-1,0,
  	379,380,3,54,27,0,380,386,1,0,0,0,381,382,10,2,0,0,382,383,5,59,0,0,383,
  	385,3,54,27,0,384,381,1,0,0,0,385,388,1,0,0,0,386,384,1,0,0,0,386,387,
  	1,0,0,0,387,53,1,0,0,0,388,386,1,0,0,0,389,390,6,27,-1,0,390,391,3,56,
  	28,0,391,397,1,0,0,0,392,393,10,2,0,0,393,394,5,60,0,0,394,396,3,56,28,
  	0,395,392,1,0,0,0,396,399,1,0,0,0,397,395,1,0,0,0,397,398,1,0,0,0,398,
  	55,1,0,0,0,399,397,1,0,0,0,400,401,6,28,-1,0,401,402,3,58,29,0,402,408,
  	1,0,0,0,403,404,10,2,0,0,404,405,5,61,0,0,405,407,3,58,29,0,406,403,1,
  	0,0,0,407,410,1,0,0,0,408,406,1,0,0,0,408,409,1,0,0,0,409,57,1,0,0,0,
  	410,408,1,0,0,0,411,412,6,29,-1,0,412,413,3,60,30,0,413,419,1,0,0,0,414,
  	415,10,2,0,0,415,416,5,62,0,0,416,418,3,60,30,0,417,414,1,0,0,0,418,421,
  	1,0,0,0,419,417,1,0,0,0,419,420,1,0,0,0,420,59,1,0,0,0,421,419,1,0,0,
  	0,422,423,6,30,-1,0,423,424,3,62,31,0,424,430,1,0,0,0,425,426,10,2,0,
  	0,426,427,5,26,0,0,427,429,3,62,31,0,428,425,1,0,0,0,429,432,1,0,0,0,
  	430,428,1,0,0,0,430,431,1,0,0,0,431,61,1,0,0,0,432,430,1,0,0,0,433,434,
  	6,31,-1,0,434,435,3,64,32,0,435,441,1,0,0,0,436,437,10,2,0,0,437,438,
  	7,7,0,0,438,440,3,64,32,0,439,436,1,0,0,0,440,443,1,0,0,0,441,439,1,0,
  	0,0,441,442,1,0,0,0,442,63,1,0,0,0,443,441,1,0,0,0,444,445,6,32,-1,0,
  	445,446,3,66,33,0,446,452,1,0,0,0,447,448,10,2,0,0,448,449,7,8,0,0,449,
  	451,3,66,33,0,450,447,1,0,0,0,451,454,1,0,0,0,452,450,1,0,0,0,452,453,
  	1,0,0,0,453,65,1,0,0,0,454,452,1,0,0,0,455,456,6,33,-1,0,456,457,3,68,
  	34,0,457,463,1,0,0,0,458,459,10,2,0,0,459,460,7,9,0,0,460,462,3,68,34,
  	0,461,458,1,0,0,0,462,465,1,0,0,0,463,461,1,0,0,0,463,464,1,0,0,0,464,
  	67,1,0,0,0,465,463,1,0,0,0,466,467,6,34,-1,0,467,468,3,70,35,0,468,474,
  	1,0,0,0,469,470,10,2,0,0,470,471,7,10,0,0,471,473,3,70,35,0,472,469,1,
  	0,0,0,473,476,1,0,0,0,474,472,1,0,0,0,474,475,1,0,0,0,475,69,1,0,0,0,
  	476,474,1,0,0,0,477,478,6,35,-1,0,478,479,3,72,36,0,479,485,1,0,0,0,480,
  	481,10,2,0,0,481,482,7,11,0,0,482,484,3,72,36,0,483,480,1,0,0,0,484,487,
  	1,0,0,0,485,483,1,0,0,0,485,486,1,0,0,0,486,71,1,0,0,0,487,485,1,0,0,
  	0,488,489,6,36,-1,0,489,490,3,74,37,0,490,496,1,0,0,0,491,492,10,2,0,
  	0,492,493,7,12,0,0,493,495,3,74,37,0,494,491,1,0,0,0,495,498,1,0,0,0,
  	496,494,1,0,0,0,496,497,1,0,0,0,497,73,1,0,0,0,498,496,1,0,0,0,499,500,
  	7,13,0,0,500,509,3,76,38,0,501,502,7,14,0,0,502,509,3,88,44,0,503,504,
  	3,88,44,0,504,505,7,14,0,0,505,509,1,0,0,0,506,509,3,82,41,0,507,509,
  	3,76,38,0,508,499,1,0,0,0,508,501,1,0,0,0,508,503,1,0,0,0,508,506,1,0,
  	0,0,508,507,1,0,0,0,509,75,1,0,0,0,510,513,3,84,42,0,511,513,3,78,39,
  	0,512,510,1,0,0,0,512,511,1,0,0,0,513,77,1,0,0,0,514,518,3,90,45,0,515,
  	518,3,80,40,0,516,518,3,88,44,0,517,514,1,0,0,0,517,515,1,0,0,0,517,516,
  	1,0,0,0,518,79,1,0,0,0,519,520,5,14,0,0,520,521,3,48,24,0,521,522,5,15,
  	0,0,522,81,1,0,0,0,523,524,3,76,38,0,524,526,5,79,0,0,525,527,5,80,0,
  	0,526,525,1,0,0,0,526,527,1,0,0,0,527,528,1,0,0,0,528,529,7,15,0,0,529,
  	83,1,0,0,0,530,541,3,86,43,0,531,533,3,78,39,0,532,534,7,16,0,0,533,532,
  	1,0,0,0,533,534,1,0,0,0,534,535,1,0,0,0,535,536,3,44,22,0,536,537,5,14,
  	0,0,537,538,3,18,9,0,538,539,5,15,0,0,539,541,1,0,0,0,540,530,1,0,0,0,
  	540,531,1,0,0,0,541,85,1,0,0,0,542,544,6,43,-1,0,543,545,7,16,0,0,544,
  	543,1,0,0,0,544,545,1,0,0,0,545,546,1,0,0,0,546,547,3,44,22,0,547,548,
  	5,14,0,0,548,549,3,18,9,0,549,550,5,15,0,0,550,564,1,0,0,0,551,554,3,
  	90,45,0,552,554,3,80,40,0,553,551,1,0,0,0,553,552,1,0,0,0,554,556,1,0,
  	0,0,555,557,7,16,0,0,556,555,1,0,0,0,556,557,1,0,0,0,557,558,1,0,0,0,
  	558,559,3,44,22,0,559,560,5,14,0,0,560,561,3,18,9,0,561,562,5,15,0,0,
  	562,564,1,0,0,0,563,542,1,0,0,0,563,553,1,0,0,0,564,576,1,0,0,0,565,567,
  	10,1,0,0,566,568,7,16,0,0,567,566,1,0,0,0,567,568,1,0,0,0,568,569,1,0,
  	0,0,569,570,3,44,22,0,570,571,5,14,0,0,571,572,3,18,9,0,572,573,5,15,
  	0,0,573,575,1,0,0,0,574,565,1,0,0,0,575,578,1,0,0,0,576,574,1,0,0,0,576,
  	577,1,0,0,0,577,87,1,0,0,0,578,576,1,0,0,0,579,580,6,44,-1,0,580,604,
  	3,102,51,0,581,585,3,86,43,0,582,585,3,90,45,0,583,585,3,80,40,0,584,
  	581,1,0,0,0,584,582,1,0,0,0,584,583,1,0,0,0,585,586,1,0,0,0,586,592,5,
  	87,0,0,587,593,3,102,51,0,588,589,5,14,0,0,589,590,3,48,24,0,590,591,
  	5,15,0,0,591,593,1,0,0,0,592,587,1,0,0,0,592,588,1,0,0,0,593,604,1,0,
  	0,0,594,598,3,86,43,0,595,598,3,90,45,0,596,598,3,80,40,0,597,594,1,0,
  	0,0,597,595,1,0,0,0,597,596,1,0,0,0,598,599,1,0,0,0,599,600,5,17,0,0,
  	600,601,3,48,24,0,601,602,5,18,0,0,602,604,1,0,0,0,603,579,1,0,0,0,603,
  	584,1,0,0,0,603,597,1,0,0,0,604,621,1,0,0,0,605,606,10,4,0,0,606,612,
  	5,87,0,0,607,613,3,102,51,0,608,609,5,14,0,0,609,610,3,48,24,0,610,611,
  	5,15,0,0,611,613,1,0,0,0,612,607,1,0,0,0,612,608,1,0,0,0,613,620,1,0,
  	0,0,614,615,10,3,0,0,615,616,5,17,0,0,616,617,3,48,24,0,617,618,5,18,
  	0,0,618,620,1,0,0,0,619,605,1,0,0,0,619,614,1,0,0,0,620,623,1,0,0,0,621,
  	619,1,0,0,0,621,622,1,0,0,0,622,89,1,0,0,0,623,621,1,0,0,0,624,636,3,
  	96,48,0,625,636,3,98,49,0,626,636,3,100,50,0,627,636,3,94,47,0,628,636,
  	3,10,5,0,629,636,5,97,0,0,630,636,5,95,0,0,631,636,5,102,0,0,632,636,
  	5,103,0,0,633,636,5,98,0,0,634,636,3,92,46,0,635,624,1,0,0,0,635,625,
  	1,0,0,0,635,626,1,0,0,0,635,627,1,0,0,0,635,628,1,0,0,0,635,629,1,0,0,
  	0,635,630,1,0,0,0,635,631,1,0,0,0,635,632,1,0,0,0,635,633,1,0,0,0,635,
  	634,1,0,0,0,636,91,1,0,0,0,637,640,5,26,0,0,638,639,5,99,0,0,639,641,
  	5,23,0,0,640,638,1,0,0,0,640,641,1,0,0,0,641,642,1,0,0,0,642,653,5,99,
  	0,0,643,644,5,88,0,0,644,645,5,99,0,0,645,646,5,21,0,0,646,647,5,101,
  	0,0,647,648,5,22,0,0,648,649,5,23,0,0,649,653,5,99,0,0,650,651,5,26,0,
  	0,651,653,3,88,44,0,652,637,1,0,0,0,652,643,1,0,0,0,652,650,1,0,0,0,653,
  	93,1,0,0,0,654,655,7,17,0,0,655,95,1,0,0,0,656,657,5,14,0,0,657,658,3,
  	48,24,0,658,659,5,24,0,0,659,660,3,48,24,0,660,661,5,24,0,0,661,662,3,
  	48,24,0,662,663,5,15,0,0,663,97,1,0,0,0,664,686,5,17,0,0,665,666,3,48,
  	24,0,666,667,5,30,0,0,667,669,1,0,0,0,668,665,1,0,0,0,668,669,1,0,0,0,
  	669,670,1,0,0,0,670,680,3,48,24,0,671,675,5,24,0,0,672,673,3,48,24,0,
  	673,674,5,30,0,0,674,676,1,0,0,0,675,672,1,0,0,0,675,676,1,0,0,0,676,
  	677,1,0,0,0,677,679,3,48,24,0,678,671,1,0,0,0,679,682,1,0,0,0,680,678,
  	1,0,0,0,680,681,1,0,0,0,681,684,1,0,0,0,682,680,1,0,0,0,683,685,5,24,
  	0,0,684,683,1,0,0,0,684,685,1,0,0,0,685,687,1,0,0,0,686,668,1,0,0,0,686,
  	687,1,0,0,0,687,688,1,0,0,0,688,689,5,18,0,0,689,99,1,0,0,0,690,712,5,
  	28,0,0,691,694,5,100,0,0,692,694,3,48,24,0,693,691,1,0,0,0,693,692,1,
  	0,0,0,694,695,1,0,0,0,695,696,5,30,0,0,696,706,3,48,24,0,697,700,5,24,
  	0,0,698,701,5,100,0,0,699,701,3,48,24,0,700,698,1,0,0,0,700,699,1,0,0,
  	0,701,702,1,0,0,0,702,703,5,30,0,0,703,705,3,48,24,0,704,697,1,0,0,0,
  	705,708,1,0,0,0,706,704,1,0,0,0,706,707,1,0,0,0,707,710,1,0,0,0,708,706,
  	1,0,0,0,709,711,5,24,0,0,710,709,1,0,0,0,710,711,1,0,0,0,711,713,1,0,
  	0,0,712,693,1,0,0,0,712,713,1,0,0,0,713,714,1,0,0,0,714,715,5,29,0,0,
  	715,101,1,0,0,0,716,717,5,99,0,0,717,103,1,0,0,0,79,111,113,137,140,147,
  	149,155,158,161,167,178,185,188,191,196,199,206,209,215,222,232,238,246,
  	250,254,277,291,301,307,312,322,331,340,357,362,372,386,397,408,419,430,
  	441,452,463,474,485,496,508,512,517,526,533,540,544,553,556,563,567,576,
  	584,592,597,603,612,619,621,635,640,652,668,675,680,684,686,693,700,706,
  	710,712
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
    setState(113);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 1145822) != 0) || _la == gscParser::IDENTIFIER) {
      setState(111);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case gscParser::T__0: {
          setState(104);
          match(gscParser::T__0);
          break;
        }

        case gscParser::T__1: {
          setState(105);
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
          setState(106);
          function();
          break;
        }

        case gscParser::T__2:
        case gscParser::T__3: {
          setState(107);
          include();
          break;
        }

        case gscParser::T__5: {
          setState(108);
          namespace_();
          break;
        }

        case gscParser::T__6: {
          setState(109);
          filenamespace();
          break;
        }

        case gscParser::T__7:
        case gscParser::T__8: {
          setState(110);
          constexpr_();
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(115);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(116);
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
    setState(118);
    _la = _input->LA(1);
    if (!(_la == gscParser::T__2

    || _la == gscParser::T__3)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(119);
    _la = _input->LA(1);
    if (!(_la == gscParser::IDENTIFIER

    || _la == gscParser::PATH)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(120);
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
    setState(122);
    match(gscParser::T__5);
    setState(123);
    match(gscParser::IDENTIFIER);
    setState(124);
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
    setState(126);
    match(gscParser::T__6);
    setState(127);
    _la = _input->LA(1);
    if (!(_la == gscParser::IDENTIFIER

    || _la == gscParser::PATH)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(128);
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
    setState(130);
    _la = _input->LA(1);
    if (!(_la == gscParser::T__7

    || _la == gscParser::T__8)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(131);
    match(gscParser::IDENTIFIER);
    setState(132);
    match(gscParser::T__9);
    setState(133);
    expression();
    setState(134);
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
    setState(137);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__10) {
      setState(136);
      match(gscParser::T__10);
    }
    setState(140);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__11) {
      setState(139);
      match(gscParser::T__11);
    }
    setState(149);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__12) {
      setState(142);
      match(gscParser::T__12);
      setState(147);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 4, _ctx)) {
      case 1: {
        setState(143);
        match(gscParser::T__13);
        setState(144);
        number();
        setState(145);
        match(gscParser::T__14);
        break;
      }

      default:
        break;
      }
    }
    setState(155);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__15) {
      setState(151);
      match(gscParser::T__15);
      setState(152);
      match(gscParser::T__16);
      setState(153);
      match(gscParser::IDENTIFIER);
      setState(154);
      match(gscParser::T__17);
    }
    setState(158);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__19) {
      setState(157);
      detour_info();
    }
    setState(161);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::IDENTIFIER) {
      setState(160);
      match(gscParser::IDENTIFIER);
    }
    setState(163);
    match(gscParser::T__13);
    setState(164);
    param_list();
    setState(165);
    match(gscParser::T__14);
    setState(167);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__18) {
      setState(166);
      match(gscParser::T__18);
    }
    setState(169);
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
    setState(171);
    match(gscParser::T__19);
    setState(172);
    match(gscParser::IDENTIFIER);
    setState(178);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__20) {
      setState(173);
      match(gscParser::T__20);
      setState(174);
      match(gscParser::PATH);
      setState(175);
      match(gscParser::T__21);
      setState(176);
      match(gscParser::T__22);
      setState(177);
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
    setState(188);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 234881024) != 0) || _la == gscParser::IDENTIFIER) {
      setState(180);
      param_val();
      setState(185);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == gscParser::T__23) {
        setState(181);
        match(gscParser::T__23);
        setState(182);
        param_val();
        setState(187);
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
    setState(199);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case gscParser::T__24:
      case gscParser::T__25:
      case gscParser::IDENTIFIER: {
        enterOuterAlt(_localctx, 1);
        setState(191);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == gscParser::T__24

        || _la == gscParser::T__25) {
          setState(190);
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
        setState(193);
        match(gscParser::IDENTIFIER);
        setState(196);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == gscParser::T__9) {
          setState(194);
          match(gscParser::T__9);
          setState(195);
          expression();
        }
        break;
      }

      case gscParser::T__26: {
        enterOuterAlt(_localctx, 2);
        setState(198);
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
    setState(209);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 336820224) != 0) || ((((_la - 75) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 75)) & 434056975) != 0)) {
      setState(201);
      expression();
      setState(206);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == gscParser::T__23) {
        setState(202);
        match(gscParser::T__23);
        setState(203);
        expression();
        setState(208);
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
    setState(211);
    match(gscParser::T__27);
    setState(215);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 68545867380738) != 0) || ((((_la - 75) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 75)) & 436154127) != 0)) {
      setState(212);
      statement();
      setState(217);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(218);
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
    setState(222);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 19, _ctx)) {
    case 1: {
      setState(220);
      match(gscParser::IDENTIFIER);
      setState(221);
      match(gscParser::T__29);
      break;
    }

    default:
      break;
    }
    setState(232);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 20, _ctx)) {
    case 1: {
      setState(224);
      statement_block();
      break;
    }

    case 2: {
      setState(225);
      statement_dev_block();
      break;
    }

    case 3: {
      setState(226);
      statement_for();
      break;
    }

    case 4: {
      setState(227);
      statement_if();
      break;
    }

    case 5: {
      setState(228);
      statement_while();
      break;
    }

    case 6: {
      setState(229);
      statement_foreach();
      break;
    }

    case 7: {
      setState(230);
      statement_inst();
      break;
    }

    case 8: {
      setState(231);
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
    setState(234);
    match(gscParser::T__0);
    setState(238);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 68545867380738) != 0) || ((((_la - 75) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 75)) & 436154127) != 0)) {
      setState(235);
      statement();
      setState(240);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(241);
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
    setState(243);
    match(gscParser::T__30);
    setState(244);
    match(gscParser::T__13);
    setState(246);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 336820224) != 0) || ((((_la - 75) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 75)) & 434056975) != 0)) {
      setState(245);
      expression();
    }
    setState(248);
    match(gscParser::T__4);
    setState(250);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 336820224) != 0) || ((((_la - 75) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 75)) & 434056975) != 0)) {
      setState(249);
      expression();
    }
    setState(252);
    match(gscParser::T__4);
    setState(254);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 336820224) != 0) || ((((_la - 75) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 75)) & 434056975) != 0)) {
      setState(253);
      expression();
    }
    setState(256);
    match(gscParser::T__14);
    setState(257);
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
    setState(259);
    match(gscParser::T__31);
    setState(260);
    match(gscParser::T__13);
    setState(261);
    expression();
    setState(262);
    match(gscParser::T__14);
    setState(263);
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
    setState(265);
    match(gscParser::T__32);
    setState(266);
    statement();
    setState(267);
    match(gscParser::T__31);
    setState(268);
    match(gscParser::T__13);
    setState(269);
    expression();
    setState(270);
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
    setState(272);
    match(gscParser::T__33);
    setState(273);
    match(gscParser::T__13);
    setState(274);
    match(gscParser::IDENTIFIER);
    setState(277);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__23) {
      setState(275);
      match(gscParser::T__23);
      setState(276);
      match(gscParser::IDENTIFIER);
    }
    setState(279);
    match(gscParser::T__34);
    setState(280);
    expression();
    setState(281);
    match(gscParser::T__14);
    setState(282);
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
    setState(284);
    match(gscParser::T__35);
    setState(285);
    match(gscParser::T__13);
    setState(286);
    expression();
    setState(287);
    match(gscParser::T__14);
    setState(288);
    statement();
    setState(291);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 26, _ctx)) {
    case 1: {
      setState(289);
      match(gscParser::T__36);
      setState(290);
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
    setState(293);
    match(gscParser::T__37);
    setState(294);
    match(gscParser::T__13);
    setState(295);
    expression();
    setState(296);
    match(gscParser::T__14);
    setState(297);
    match(gscParser::T__27);
    setState(310); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(301);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case gscParser::T__38: {
          setState(298);
          match(gscParser::T__38);
          setState(299);
          const_expr();
          break;
        }

        case gscParser::T__39: {
          setState(300);
          match(gscParser::T__39);
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(303);
      match(gscParser::T__29);
      setState(307);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while ((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & 68545867380738) != 0) || ((((_la - 75) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 75)) & 436154127) != 0)) {
        setState(304);
        statement();
        setState(309);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
      setState(312); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while (_la == gscParser::T__38

    || _la == gscParser::T__39);
    setState(314);
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
    setState(322);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 30, _ctx)) {
    case 1: {
      setState(316);
      expression();
      break;
    }

    case 2: {
      setState(317);
      operator_inst();
      break;
    }

    case 3: {
      setState(318);
      statement_dowhile();
      break;
    }

    case 4: {
      setState(319);
      function_call_exp();
      break;
    }

    case 5: {
      setState(320);
      nop_def();
      break;
    }

    case 6: {
      setState(321);
      devop_def();
      break;
    }

    default:
      break;
    }
    setState(324);
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
    setState(326);
    _la = _input->LA(1);
    if (!(_la == gscParser::T__40

    || _la == gscParser::T__41)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(331);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__13) {
      setState(327);
      match(gscParser::T__13);
      setState(328);
      number();
      setState(329);
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
    setState(333);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 61572651155456) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(334);
    match(gscParser::T__13);
    setState(335);
    number();
    setState(336);
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
    setState(357);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 33, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(340);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 32, _ctx)) {
      case 1: {
        setState(338);
        match(gscParser::IDENTIFIER);
        setState(339);
        match(gscParser::T__22);
        break;
      }

      default:
        break;
      }
      setState(342);
      match(gscParser::IDENTIFIER);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(343);
      match(gscParser::T__16);
      setState(344);
      match(gscParser::T__16);
      setState(345);
      expression();
      setState(346);
      match(gscParser::T__17);
      setState(347);
      match(gscParser::T__17);
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(349);
      match(gscParser::T__16);
      setState(350);
      match(gscParser::T__16);
      setState(351);
      expression();
      setState(352);
      match(gscParser::T__17);
      setState(353);
      match(gscParser::T__17);
      setState(354);
      match(gscParser::T__45);
      setState(355);
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
    setState(359);
    match(gscParser::BUILTIN);
    setState(362);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 34, _ctx)) {
    case 1: {
      setState(360);
      match(gscParser::IDENTIFIER);
      break;
    }

    case 2: {
      setState(361);
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
  enterRule(_localctx, 48, gscParser::RuleExpression);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(372);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 35, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(364);
      set_expression();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(365);
      expression1(0);
      setState(366);
      match(gscParser::T__46);
      setState(367);
      expression();
      setState(368);
      match(gscParser::T__29);
      setState(369);
      expression();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(371);
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
    setState(374);
    left_value(0);
    setState(375);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 576179277326713856) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(376);
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
  size_t startState = 52;
  enterRecursionRule(_localctx, 52, gscParser::RuleExpression1, precedence);

    

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
    setState(379);
    expression2(0);
    _ctx->stop = _input->LT(-1);
    setState(386);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 36, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression1Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression1);
        setState(381);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(382);
        match(gscParser::T__58);
        setState(383);
        expression2(0); 
      }
      setState(388);
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
  size_t startState = 54;
  enterRecursionRule(_localctx, 54, gscParser::RuleExpression2, precedence);

    

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
    setState(390);
    expression3(0);
    _ctx->stop = _input->LT(-1);
    setState(397);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 37, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression2Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression2);
        setState(392);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(393);
        match(gscParser::T__59);
        setState(394);
        expression3(0); 
      }
      setState(399);
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
  size_t startState = 56;
  enterRecursionRule(_localctx, 56, gscParser::RuleExpression3, precedence);

    

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
    setState(401);
    expression4(0);
    _ctx->stop = _input->LT(-1);
    setState(408);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 38, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression3Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression3);
        setState(403);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(404);
        match(gscParser::T__60);
        setState(405);
        expression4(0); 
      }
      setState(410);
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
  size_t startState = 58;
  enterRecursionRule(_localctx, 58, gscParser::RuleExpression4, precedence);

    

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
    setState(412);
    expression5(0);
    _ctx->stop = _input->LT(-1);
    setState(419);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 39, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression4Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression4);
        setState(414);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(415);
        match(gscParser::T__61);
        setState(416);
        expression5(0); 
      }
      setState(421);
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
  size_t startState = 60;
  enterRecursionRule(_localctx, 60, gscParser::RuleExpression5, precedence);

    

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
    setState(423);
    expression6(0);
    _ctx->stop = _input->LT(-1);
    setState(430);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 40, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression5Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression5);
        setState(425);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(426);
        match(gscParser::T__25);
        setState(427);
        expression6(0); 
      }
      setState(432);
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
  size_t startState = 62;
  enterRecursionRule(_localctx, 62, gscParser::RuleExpression6, precedence);

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
    setState(434);
    expression7(0);
    _ctx->stop = _input->LT(-1);
    setState(441);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 41, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression6Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression6);
        setState(436);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(437);
        _la = _input->LA(1);
        if (!(((((_la - 63) & ~ 0x3fULL) == 0) &&
          ((1ULL << (_la - 63)) & 15) != 0))) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(438);
        expression7(0); 
      }
      setState(443);
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
  size_t startState = 64;
  enterRecursionRule(_localctx, 64, gscParser::RuleExpression7, precedence);

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
    setState(445);
    expression8(0);
    _ctx->stop = _input->LT(-1);
    setState(452);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 42, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression7Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression7);
        setState(447);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(448);
        _la = _input->LA(1);
        if (!(((((_la - 21) & ~ 0x3fULL) == 0) &&
          ((1ULL << (_la - 21)) & 211106232532995) != 0))) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(449);
        expression8(0); 
      }
      setState(454);
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
  size_t startState = 66;
  enterRecursionRule(_localctx, 66, gscParser::RuleExpression8, precedence);

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
    setState(456);
    expression9(0);
    _ctx->stop = _input->LT(-1);
    setState(463);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 43, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression8Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression8);
        setState(458);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(459);
        _la = _input->LA(1);
        if (!(_la == gscParser::T__20

        || _la == gscParser::T__66)) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(460);
        expression9(0); 
      }
      setState(465);
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
  size_t startState = 68;
  enterRecursionRule(_localctx, 68, gscParser::RuleExpression9, precedence);

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
    setState(467);
    expression10(0);
    _ctx->stop = _input->LT(-1);
    setState(474);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 44, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression9Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression9);
        setState(469);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(470);
        _la = _input->LA(1);
        if (!(_la == gscParser::T__68

        || _la == gscParser::T__69)) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(471);
        expression10(0); 
      }
      setState(476);
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
  size_t startState = 70;
  enterRecursionRule(_localctx, 70, gscParser::RuleExpression10, precedence);

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
    setState(478);
    expression11(0);
    _ctx->stop = _input->LT(-1);
    setState(485);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 45, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression10Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression10);
        setState(480);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(481);
        _la = _input->LA(1);
        if (!(_la == gscParser::T__70

        || _la == gscParser::T__71)) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(482);
        expression11(0); 
      }
      setState(487);
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
  size_t startState = 72;
  enterRecursionRule(_localctx, 72, gscParser::RuleExpression11, precedence);

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
    setState(489);
    expression12();
    _ctx->stop = _input->LT(-1);
    setState(496);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 46, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Expression11Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression11);
        setState(491);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(492);
        _la = _input->LA(1);
        if (!(((((_la - 25) & ~ 0x3fULL) == 0) &&
          ((1ULL << (_la - 25)) & 844424930131969) != 0))) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(493);
        expression12(); 
      }
      setState(498);
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
  enterRule(_localctx, 74, gscParser::RuleExpression12);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(508);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 47, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(499);
      _la = _input->LA(1);
      if (!(_la == gscParser::T__74

      || _la == gscParser::T__75)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(500);
      expression13();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(501);
      _la = _input->LA(1);
      if (!(_la == gscParser::T__76

      || _la == gscParser::T__77)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(502);
      left_value(0);
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(503);
      left_value(0);
      setState(504);
      _la = _input->LA(1);
      if (!(_la == gscParser::T__76

      || _la == gscParser::T__77)) {
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
      setState(506);
      is_expression();
      break;
    }

    case 5: {
      enterOuterAlt(_localctx, 5);
      setState(507);
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
  enterRule(_localctx, 76, gscParser::RuleExpression13);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(512);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 48, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(510);
      function_call_exp();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(511);
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
  enterRule(_localctx, 78, gscParser::RuleExpression14);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(517);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 49, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(514);
      const_expr();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(515);
      expression15();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(516);
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
  enterRule(_localctx, 80, gscParser::RuleExpression15);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(519);
    match(gscParser::T__13);
    setState(520);
    expression();
    setState(521);
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
  enterRule(_localctx, 82, gscParser::RuleIs_expression);
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
    setState(523);
    expression13();
    setState(524);
    match(gscParser::T__78);
    setState(526);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__79) {
      setState(525);
      match(gscParser::T__79);
    }
    setState(528);
    _la = _input->LA(1);
    if (!(_la == gscParser::T__10 || ((((_la - 81) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 81)) & 393219) != 0))) {
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
  enterRule(_localctx, 84, gscParser::RuleFunction_call_exp);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(540);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 52, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(530);
      function_call(0);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(531);
      expression14();
      setState(533);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (((((_la - 83) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 83)) & 15) != 0)) {
        setState(532);
        _la = _input->LA(1);
        if (!(((((_la - 83) & ~ 0x3fULL) == 0) &&
          ((1ULL << (_la - 83)) & 15) != 0))) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
      }
      setState(535);
      function_component();
      setState(536);
      match(gscParser::T__13);
      setState(537);
      expression_list();
      setState(538);
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
  size_t startState = 86;
  enterRecursionRule(_localctx, 86, gscParser::RuleFunction_call, precedence);

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
    setState(563);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 56, _ctx)) {
    case 1: {
      setState(544);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (((((_la - 83) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 83)) & 15) != 0)) {
        setState(543);
        _la = _input->LA(1);
        if (!(((((_la - 83) & ~ 0x3fULL) == 0) &&
          ((1ULL << (_la - 83)) & 15) != 0))) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
      }
      setState(546);
      function_component();
      setState(547);
      match(gscParser::T__13);
      setState(548);
      expression_list();
      setState(549);
      match(gscParser::T__14);
      break;
    }

    case 2: {
      setState(553);
      _errHandler->sync(this);
      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 54, _ctx)) {
      case 1: {
        setState(551);
        const_expr();
        break;
      }

      case 2: {
        setState(552);
        expression15();
        break;
      }

      default:
        break;
      }
      setState(556);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (((((_la - 83) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 83)) & 15) != 0)) {
        setState(555);
        _la = _input->LA(1);
        if (!(((((_la - 83) & ~ 0x3fULL) == 0) &&
          ((1ULL << (_la - 83)) & 15) != 0))) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
      }
      setState(558);
      function_component();
      setState(559);
      match(gscParser::T__13);
      setState(560);
      expression_list();
      setState(561);
      match(gscParser::T__14);
      break;
    }

    default:
      break;
    }
    _ctx->stop = _input->LT(-1);
    setState(576);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 58, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Function_callContext>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleFunction_call);
        setState(565);

        if (!(precpred(_ctx, 1))) throw FailedPredicateException(this, "precpred(_ctx, 1)");
        setState(567);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (((((_la - 83) & ~ 0x3fULL) == 0) &&
          ((1ULL << (_la - 83)) & 15) != 0)) {
          setState(566);
          _la = _input->LA(1);
          if (!(((((_la - 83) & ~ 0x3fULL) == 0) &&
            ((1ULL << (_la - 83)) & 15) != 0))) {
          _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
        }
        setState(569);
        function_component();
        setState(570);
        match(gscParser::T__13);
        setState(571);
        expression_list();
        setState(572);
        match(gscParser::T__14); 
      }
      setState(578);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 58, _ctx);
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
  size_t startState = 88;
  enterRecursionRule(_localctx, 88, gscParser::RuleLeft_value, precedence);

    

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
    setState(603);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 62, _ctx)) {
    case 1: {
      setState(580);
      idf();
      break;
    }

    case 2: {
      setState(584);
      _errHandler->sync(this);
      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 59, _ctx)) {
      case 1: {
        setState(581);
        function_call(0);
        break;
      }

      case 2: {
        setState(582);
        const_expr();
        break;
      }

      case 3: {
        setState(583);
        expression15();
        break;
      }

      default:
        break;
      }
      setState(586);
      match(gscParser::T__86);
      setState(592);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case gscParser::IDENTIFIER: {
          setState(587);
          idf();
          break;
        }

        case gscParser::T__13: {
          setState(588);
          match(gscParser::T__13);
          setState(589);
          expression();
          setState(590);
          match(gscParser::T__14);
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      break;
    }

    case 3: {
      setState(597);
      _errHandler->sync(this);
      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 61, _ctx)) {
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
      match(gscParser::T__16);
      setState(600);
      expression();
      setState(601);
      match(gscParser::T__17);
      break;
    }

    default:
      break;
    }
    _ctx->stop = _input->LT(-1);
    setState(621);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 65, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        setState(619);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 64, _ctx)) {
        case 1: {
          _localctx = _tracker.createInstance<Left_valueContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleLeft_value);
          setState(605);

          if (!(precpred(_ctx, 4))) throw FailedPredicateException(this, "precpred(_ctx, 4)");
          setState(606);
          match(gscParser::T__86);
          setState(612);
          _errHandler->sync(this);
          switch (_input->LA(1)) {
            case gscParser::IDENTIFIER: {
              setState(607);
              idf();
              break;
            }

            case gscParser::T__13: {
              setState(608);
              match(gscParser::T__13);
              setState(609);
              expression();
              setState(610);
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
          setState(614);

          if (!(precpred(_ctx, 3))) throw FailedPredicateException(this, "precpred(_ctx, 3)");
          setState(615);
          match(gscParser::T__16);
          setState(616);
          expression();
          setState(617);
          match(gscParser::T__17);
          break;
        }

        default:
          break;
        } 
      }
      setState(623);
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
  enterRule(_localctx, 90, gscParser::RuleConst_expr);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(635);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 66, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(624);
      vector_value();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(625);
      array_def();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(626);
      struct_def();
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(627);
      number();
      break;
    }

    case 5: {
      enterOuterAlt(_localctx, 5);
      setState(628);
      function();
      break;
    }

    case 6: {
      enterOuterAlt(_localctx, 6);
      setState(629);
      match(gscParser::BOOL_VALUE);
      break;
    }

    case 7: {
      enterOuterAlt(_localctx, 7);
      setState(630);
      match(gscParser::FLOATVAL);
      break;
    }

    case 8: {
      enterOuterAlt(_localctx, 8);
      setState(631);
      match(gscParser::STRING);
      break;
    }

    case 9: {
      enterOuterAlt(_localctx, 9);
      setState(632);
      match(gscParser::HASHSTRING);
      break;
    }

    case 10: {
      enterOuterAlt(_localctx, 10);
      setState(633);
      match(gscParser::UNDEFINED_VALUE);
      break;
    }

    case 11: {
      enterOuterAlt(_localctx, 11);
      setState(634);
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
  enterRule(_localctx, 92, gscParser::RuleFunction_ref);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(652);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 68, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(637);
      match(gscParser::T__25);
      setState(640);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 67, _ctx)) {
      case 1: {
        setState(638);
        match(gscParser::IDENTIFIER);
        setState(639);
        match(gscParser::T__22);
        break;
      }

      default:
        break;
      }
      setState(642);
      match(gscParser::IDENTIFIER);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(643);
      match(gscParser::T__87);
      setState(644);
      match(gscParser::IDENTIFIER);
      setState(645);
      match(gscParser::T__20);
      setState(646);
      match(gscParser::PATH);
      setState(647);
      match(gscParser::T__21);
      setState(648);
      match(gscParser::T__22);
      setState(649);
      match(gscParser::IDENTIFIER);
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(650);
      match(gscParser::T__25);
      setState(651);
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
  enterRule(_localctx, 94, gscParser::RuleNumber);
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
    setState(654);
    _la = _input->LA(1);
    if (!(((((_la - 91) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 91)) & 271) != 0))) {
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
  enterRule(_localctx, 96, gscParser::RuleVector_value);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(656);
    match(gscParser::T__13);
    setState(657);
    expression();
    setState(658);
    match(gscParser::T__23);
    setState(659);
    expression();
    setState(660);
    match(gscParser::T__23);
    setState(661);
    expression();
    setState(662);
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
  enterRule(_localctx, 98, gscParser::RuleArray_def);
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
    setState(664);
    match(gscParser::T__16);
    setState(686);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 336820224) != 0) || ((((_la - 75) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 75)) & 434056975) != 0)) {
      setState(668);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 69, _ctx)) {
      case 1: {
        setState(665);
        expression();
        setState(666);
        match(gscParser::T__29);
        break;
      }

      default:
        break;
      }
      setState(670);
      expression();
      setState(680);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 71, _ctx);
      while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
        if (alt == 1) {
          setState(671);
          match(gscParser::T__23);
          setState(675);
          _errHandler->sync(this);

          switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 70, _ctx)) {
          case 1: {
            setState(672);
            expression();
            setState(673);
            match(gscParser::T__29);
            break;
          }

          default:
            break;
          }
          setState(677);
          expression(); 
        }
        setState(682);
        _errHandler->sync(this);
        alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 71, _ctx);
      }
      setState(684);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == gscParser::T__23) {
        setState(683);
        match(gscParser::T__23);
      }
    }
    setState(688);
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
  enterRule(_localctx, 100, gscParser::RuleStruct_def);
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
    setState(690);
    match(gscParser::T__27);
    setState(712);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 336820224) != 0) || ((((_la - 75) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 75)) & 467611407) != 0)) {
      setState(693);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case gscParser::STRUCT_IDENTIFIER: {
          setState(691);
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
        case gscParser::T__74:
        case gscParser::T__75:
        case gscParser::T__76:
        case gscParser::T__77:
        case gscParser::T__82:
        case gscParser::T__83:
        case gscParser::T__84:
        case gscParser::T__85:
        case gscParser::T__87:
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
          setState(692);
          expression();
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(695);
      match(gscParser::T__29);
      setState(696);
      expression();
      setState(706);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 76, _ctx);
      while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
        if (alt == 1) {
          setState(697);
          match(gscParser::T__23);
          setState(700);
          _errHandler->sync(this);
          switch (_input->LA(1)) {
            case gscParser::STRUCT_IDENTIFIER: {
              setState(698);
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
            case gscParser::T__74:
            case gscParser::T__75:
            case gscParser::T__76:
            case gscParser::T__77:
            case gscParser::T__82:
            case gscParser::T__83:
            case gscParser::T__84:
            case gscParser::T__85:
            case gscParser::T__87:
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
              setState(699);
              expression();
              break;
            }

          default:
            throw NoViableAltException(this);
          }
          setState(702);
          match(gscParser::T__29);
          setState(703);
          expression(); 
        }
        setState(708);
        _errHandler->sync(this);
        alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 76, _ctx);
      }
      setState(710);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == gscParser::T__23) {
        setState(709);
        match(gscParser::T__23);
      }
    }
    setState(714);
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
  enterRule(_localctx, 102, gscParser::RuleIdf);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(716);
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
    case 26: return expression1Sempred(antlrcpp::downCast<Expression1Context *>(context), predicateIndex);
    case 27: return expression2Sempred(antlrcpp::downCast<Expression2Context *>(context), predicateIndex);
    case 28: return expression3Sempred(antlrcpp::downCast<Expression3Context *>(context), predicateIndex);
    case 29: return expression4Sempred(antlrcpp::downCast<Expression4Context *>(context), predicateIndex);
    case 30: return expression5Sempred(antlrcpp::downCast<Expression5Context *>(context), predicateIndex);
    case 31: return expression6Sempred(antlrcpp::downCast<Expression6Context *>(context), predicateIndex);
    case 32: return expression7Sempred(antlrcpp::downCast<Expression7Context *>(context), predicateIndex);
    case 33: return expression8Sempred(antlrcpp::downCast<Expression8Context *>(context), predicateIndex);
    case 34: return expression9Sempred(antlrcpp::downCast<Expression9Context *>(context), predicateIndex);
    case 35: return expression10Sempred(antlrcpp::downCast<Expression10Context *>(context), predicateIndex);
    case 36: return expression11Sempred(antlrcpp::downCast<Expression11Context *>(context), predicateIndex);
    case 43: return function_callSempred(antlrcpp::downCast<Function_callContext *>(context), predicateIndex);
    case 44: return left_valueSempred(antlrcpp::downCast<Left_valueContext *>(context), predicateIndex);

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
