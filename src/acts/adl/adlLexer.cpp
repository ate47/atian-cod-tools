
// Generated from .\grammar\adl.g4 by ANTLR 4.13.0


#include "adlLexer.h"


using namespace antlr4;



using namespace antlr4;

namespace {

struct AdlLexerStaticData final {
  AdlLexerStaticData(std::vector<std::string> ruleNames,
                          std::vector<std::string> channelNames,
                          std::vector<std::string> modeNames,
                          std::vector<std::string> literalNames,
                          std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), channelNames(std::move(channelNames)),
        modeNames(std::move(modeNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  AdlLexerStaticData(const AdlLexerStaticData&) = delete;
  AdlLexerStaticData(AdlLexerStaticData&&) = delete;
  AdlLexerStaticData& operator=(const AdlLexerStaticData&) = delete;
  AdlLexerStaticData& operator=(AdlLexerStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> channelNames;
  const std::vector<std::string> modeNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

::antlr4::internal::OnceFlag adllexerLexerOnceFlag;
#if ANTLR4_USE_THREAD_LOCAL_CACHE
static thread_local
#endif
AdlLexerStaticData *adllexerLexerStaticData = nullptr;

void adllexerLexerInitialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  if (adllexerLexerStaticData != nullptr) {
    return;
  }
#else
  assert(adllexerLexerStaticData == nullptr);
#endif
  auto staticData = std::make_unique<AdlLexerStaticData>(
    std::vector<std::string>{
      "T__0", "T__1", "T__2", "T__3", "T__4", "T__5", "T__6", "T__7", "T__8", 
      "T__9", "T__10", "T__11", "T__12", "T__13", "T__14", "T__15", "T__16", 
      "T__17", "T__18", "T__19", "T__20", "T__21", "NEWLINE", "WHITESPACE", 
      "INTEGER10", "INTEGER16", "INTEGER8", "INTEGER2", "BOOL_VALUE", "IDENTIFIER", 
      "STRING"
    },
    std::vector<std::string>{
      "DEFAULT_TOKEN_CHANNEL", "HIDDEN"
    },
    std::vector<std::string>{
      "DEFAULT_MODE"
    },
    std::vector<std::string>{
      "", "';'", "'#root'", "'#include'", "'#align'", "'typedef'", "'struct'", 
      "'align'", "'('", "')'", "'size'", "'{'", "'}'", "'flag'", "':'", 
      "'enum'", "','", "'*'", "'['", "']'", "'$padding'", "'$assert_offset'", 
      "'='"
    },
    std::vector<std::string>{
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
      "", "", "", "", "", "", "NEWLINE", "WHITESPACE", "INTEGER10", "INTEGER16", 
      "INTEGER8", "INTEGER2", "BOOL_VALUE", "IDENTIFIER", "STRING"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,0,31,250,6,-1,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,
  	6,2,7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,
  	7,14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,
  	7,21,2,22,7,22,2,23,7,23,2,24,7,24,2,25,7,25,2,26,7,26,2,27,7,27,2,28,
  	7,28,2,29,7,29,2,30,7,30,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,2,1,2,
  	1,2,1,2,1,2,1,2,1,2,1,2,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,4,1,4,1,4,1,4,1,
  	4,1,4,1,4,1,4,1,5,1,5,1,5,1,5,1,5,1,5,1,5,1,6,1,6,1,6,1,6,1,6,1,6,1,7,
  	1,7,1,8,1,8,1,9,1,9,1,9,1,9,1,9,1,10,1,10,1,11,1,11,1,12,1,12,1,12,1,
  	12,1,12,1,13,1,13,1,14,1,14,1,14,1,14,1,14,1,15,1,15,1,16,1,16,1,17,1,
  	17,1,18,1,18,1,19,1,19,1,19,1,19,1,19,1,19,1,19,1,19,1,19,1,20,1,20,1,
  	20,1,20,1,20,1,20,1,20,1,20,1,20,1,20,1,20,1,20,1,20,1,20,1,20,1,21,1,
  	21,1,22,3,22,169,8,22,1,22,1,22,3,22,173,8,22,1,22,1,22,1,23,1,23,1,23,
  	1,23,1,24,3,24,182,8,24,1,24,1,24,5,24,186,8,24,10,24,12,24,189,9,24,
  	1,25,3,25,192,8,25,1,25,1,25,1,25,4,25,197,8,25,11,25,12,25,198,1,26,
  	3,26,202,8,26,1,26,1,26,5,26,206,8,26,10,26,12,26,209,9,26,1,27,3,27,
  	212,8,27,1,27,1,27,1,27,5,27,217,8,27,10,27,12,27,220,9,27,1,28,1,28,
  	1,28,1,28,1,28,1,28,1,28,1,28,1,28,3,28,231,8,28,1,29,1,29,5,29,235,8,
  	29,10,29,12,29,238,9,29,1,30,1,30,1,30,1,30,5,30,244,8,30,10,30,12,30,
  	247,9,30,1,30,1,30,0,0,31,1,1,3,2,5,3,7,4,9,5,11,6,13,7,15,8,17,9,19,
  	10,21,11,23,12,25,13,27,14,29,15,31,16,33,17,35,18,37,19,39,20,41,21,
  	43,22,45,23,47,24,49,25,51,26,53,27,55,28,57,29,59,30,61,31,1,0,11,2,
  	0,9,9,32,32,1,0,49,57,1,0,48,57,2,0,88,88,120,120,3,0,48,57,65,70,97,
  	102,1,0,48,55,2,0,66,66,98,98,1,0,48,49,3,0,65,90,95,95,97,122,4,0,48,
  	57,65,90,95,95,97,122,2,0,34,34,92,92,263,0,1,1,0,0,0,0,3,1,0,0,0,0,5,
  	1,0,0,0,0,7,1,0,0,0,0,9,1,0,0,0,0,11,1,0,0,0,0,13,1,0,0,0,0,15,1,0,0,
  	0,0,17,1,0,0,0,0,19,1,0,0,0,0,21,1,0,0,0,0,23,1,0,0,0,0,25,1,0,0,0,0,
  	27,1,0,0,0,0,29,1,0,0,0,0,31,1,0,0,0,0,33,1,0,0,0,0,35,1,0,0,0,0,37,1,
  	0,0,0,0,39,1,0,0,0,0,41,1,0,0,0,0,43,1,0,0,0,0,45,1,0,0,0,0,47,1,0,0,
  	0,0,49,1,0,0,0,0,51,1,0,0,0,0,53,1,0,0,0,0,55,1,0,0,0,0,57,1,0,0,0,0,
  	59,1,0,0,0,0,61,1,0,0,0,1,63,1,0,0,0,3,65,1,0,0,0,5,71,1,0,0,0,7,80,1,
  	0,0,0,9,87,1,0,0,0,11,95,1,0,0,0,13,102,1,0,0,0,15,108,1,0,0,0,17,110,
  	1,0,0,0,19,112,1,0,0,0,21,117,1,0,0,0,23,119,1,0,0,0,25,121,1,0,0,0,27,
  	126,1,0,0,0,29,128,1,0,0,0,31,133,1,0,0,0,33,135,1,0,0,0,35,137,1,0,0,
  	0,37,139,1,0,0,0,39,141,1,0,0,0,41,150,1,0,0,0,43,165,1,0,0,0,45,172,
  	1,0,0,0,47,176,1,0,0,0,49,181,1,0,0,0,51,191,1,0,0,0,53,201,1,0,0,0,55,
  	211,1,0,0,0,57,230,1,0,0,0,59,232,1,0,0,0,61,239,1,0,0,0,63,64,5,59,0,
  	0,64,2,1,0,0,0,65,66,5,35,0,0,66,67,5,114,0,0,67,68,5,111,0,0,68,69,5,
  	111,0,0,69,70,5,116,0,0,70,4,1,0,0,0,71,72,5,35,0,0,72,73,5,105,0,0,73,
  	74,5,110,0,0,74,75,5,99,0,0,75,76,5,108,0,0,76,77,5,117,0,0,77,78,5,100,
  	0,0,78,79,5,101,0,0,79,6,1,0,0,0,80,81,5,35,0,0,81,82,5,97,0,0,82,83,
  	5,108,0,0,83,84,5,105,0,0,84,85,5,103,0,0,85,86,5,110,0,0,86,8,1,0,0,
  	0,87,88,5,116,0,0,88,89,5,121,0,0,89,90,5,112,0,0,90,91,5,101,0,0,91,
  	92,5,100,0,0,92,93,5,101,0,0,93,94,5,102,0,0,94,10,1,0,0,0,95,96,5,115,
  	0,0,96,97,5,116,0,0,97,98,5,114,0,0,98,99,5,117,0,0,99,100,5,99,0,0,100,
  	101,5,116,0,0,101,12,1,0,0,0,102,103,5,97,0,0,103,104,5,108,0,0,104,105,
  	5,105,0,0,105,106,5,103,0,0,106,107,5,110,0,0,107,14,1,0,0,0,108,109,
  	5,40,0,0,109,16,1,0,0,0,110,111,5,41,0,0,111,18,1,0,0,0,112,113,5,115,
  	0,0,113,114,5,105,0,0,114,115,5,122,0,0,115,116,5,101,0,0,116,20,1,0,
  	0,0,117,118,5,123,0,0,118,22,1,0,0,0,119,120,5,125,0,0,120,24,1,0,0,0,
  	121,122,5,102,0,0,122,123,5,108,0,0,123,124,5,97,0,0,124,125,5,103,0,
  	0,125,26,1,0,0,0,126,127,5,58,0,0,127,28,1,0,0,0,128,129,5,101,0,0,129,
  	130,5,110,0,0,130,131,5,117,0,0,131,132,5,109,0,0,132,30,1,0,0,0,133,
  	134,5,44,0,0,134,32,1,0,0,0,135,136,5,42,0,0,136,34,1,0,0,0,137,138,5,
  	91,0,0,138,36,1,0,0,0,139,140,5,93,0,0,140,38,1,0,0,0,141,142,5,36,0,
  	0,142,143,5,112,0,0,143,144,5,97,0,0,144,145,5,100,0,0,145,146,5,100,
  	0,0,146,147,5,105,0,0,147,148,5,110,0,0,148,149,5,103,0,0,149,40,1,0,
  	0,0,150,151,5,36,0,0,151,152,5,97,0,0,152,153,5,115,0,0,153,154,5,115,
  	0,0,154,155,5,101,0,0,155,156,5,114,0,0,156,157,5,116,0,0,157,158,5,95,
  	0,0,158,159,5,111,0,0,159,160,5,102,0,0,160,161,5,102,0,0,161,162,5,115,
  	0,0,162,163,5,101,0,0,163,164,5,116,0,0,164,42,1,0,0,0,165,166,5,61,0,
  	0,166,44,1,0,0,0,167,169,5,13,0,0,168,167,1,0,0,0,168,169,1,0,0,0,169,
  	170,1,0,0,0,170,173,5,10,0,0,171,173,5,13,0,0,172,168,1,0,0,0,172,171,
  	1,0,0,0,173,174,1,0,0,0,174,175,6,22,0,0,175,46,1,0,0,0,176,177,7,0,0,
  	0,177,178,1,0,0,0,178,179,6,23,0,0,179,48,1,0,0,0,180,182,5,45,0,0,181,
  	180,1,0,0,0,181,182,1,0,0,0,182,183,1,0,0,0,183,187,7,1,0,0,184,186,7,
  	2,0,0,185,184,1,0,0,0,186,189,1,0,0,0,187,185,1,0,0,0,187,188,1,0,0,0,
  	188,50,1,0,0,0,189,187,1,0,0,0,190,192,5,45,0,0,191,190,1,0,0,0,191,192,
  	1,0,0,0,192,193,1,0,0,0,193,194,5,48,0,0,194,196,7,3,0,0,195,197,7,4,
  	0,0,196,195,1,0,0,0,197,198,1,0,0,0,198,196,1,0,0,0,198,199,1,0,0,0,199,
  	52,1,0,0,0,200,202,5,45,0,0,201,200,1,0,0,0,201,202,1,0,0,0,202,203,1,
  	0,0,0,203,207,5,48,0,0,204,206,7,5,0,0,205,204,1,0,0,0,206,209,1,0,0,
  	0,207,205,1,0,0,0,207,208,1,0,0,0,208,54,1,0,0,0,209,207,1,0,0,0,210,
  	212,5,45,0,0,211,210,1,0,0,0,211,212,1,0,0,0,212,213,1,0,0,0,213,214,
  	5,48,0,0,214,218,7,6,0,0,215,217,7,7,0,0,216,215,1,0,0,0,217,220,1,0,
  	0,0,218,216,1,0,0,0,218,219,1,0,0,0,219,56,1,0,0,0,220,218,1,0,0,0,221,
  	222,5,116,0,0,222,223,5,114,0,0,223,224,5,117,0,0,224,231,5,101,0,0,225,
  	226,5,102,0,0,226,227,5,97,0,0,227,228,5,108,0,0,228,229,5,115,0,0,229,
  	231,5,101,0,0,230,221,1,0,0,0,230,225,1,0,0,0,231,58,1,0,0,0,232,236,
  	7,8,0,0,233,235,7,9,0,0,234,233,1,0,0,0,235,238,1,0,0,0,236,234,1,0,0,
  	0,236,237,1,0,0,0,237,60,1,0,0,0,238,236,1,0,0,0,239,245,5,34,0,0,240,
  	244,8,10,0,0,241,242,5,92,0,0,242,244,9,0,0,0,243,240,1,0,0,0,243,241,
  	1,0,0,0,244,247,1,0,0,0,245,243,1,0,0,0,245,246,1,0,0,0,246,248,1,0,0,
  	0,247,245,1,0,0,0,248,249,5,34,0,0,249,62,1,0,0,0,15,0,168,172,181,187,
  	191,198,201,207,211,218,230,236,243,245,1,6,0,0
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  adllexerLexerStaticData = staticData.release();
}

}

adlLexer::adlLexer(CharStream *input) : Lexer(input) {
  adlLexer::initialize();
  _interpreter = new atn::LexerATNSimulator(this, *adllexerLexerStaticData->atn, adllexerLexerStaticData->decisionToDFA, adllexerLexerStaticData->sharedContextCache);
}

adlLexer::~adlLexer() {
  delete _interpreter;
}

std::string adlLexer::getGrammarFileName() const {
  return "adl.g4";
}

const std::vector<std::string>& adlLexer::getRuleNames() const {
  return adllexerLexerStaticData->ruleNames;
}

const std::vector<std::string>& adlLexer::getChannelNames() const {
  return adllexerLexerStaticData->channelNames;
}

const std::vector<std::string>& adlLexer::getModeNames() const {
  return adllexerLexerStaticData->modeNames;
}

const dfa::Vocabulary& adlLexer::getVocabulary() const {
  return adllexerLexerStaticData->vocabulary;
}

antlr4::atn::SerializedATNView adlLexer::getSerializedATN() const {
  return adllexerLexerStaticData->serializedATN;
}

const atn::ATN& adlLexer::getATN() const {
  return *adllexerLexerStaticData->atn;
}




void adlLexer::initialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  adllexerLexerInitialize();
#else
  ::antlr4::internal::call_once(adllexerLexerOnceFlag, adllexerLexerInitialize);
#endif
}
