
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
      "T__17", "T__18", "T__19", "T__20", "NEWLINE", "WHITESPACE", "INTEGER10", 
      "INTEGER16", "INTEGER8", "INTEGER2", "BOOL_VALUE", "IDENTIFIER", "STRING"
    },
    std::vector<std::string>{
      "DEFAULT_TOKEN_CHANNEL", "HIDDEN"
    },
    std::vector<std::string>{
      "DEFAULT_MODE"
    },
    std::vector<std::string>{
      "", "';'", "'#root'", "'#align'", "'typedef'", "'struct'", "'align'", 
      "'('", "')'", "'size'", "'{'", "'}'", "'flag'", "':'", "'enum'", "','", 
      "'*'", "'['", "']'", "'$padding'", "'$assert_offset'", "'='"
    },
    std::vector<std::string>{
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
      "", "", "", "", "", "NEWLINE", "WHITESPACE", "INTEGER10", "INTEGER16", 
      "INTEGER8", "INTEGER2", "BOOL_VALUE", "IDENTIFIER", "STRING"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,0,30,239,6,-1,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,
  	6,2,7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,
  	7,14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,
  	7,21,2,22,7,22,2,23,7,23,2,24,7,24,2,25,7,25,2,26,7,26,2,27,7,27,2,28,
  	7,28,2,29,7,29,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,2,1,2,1,2,1,2,1,
  	2,1,2,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,4,1,4,1,4,1,4,1,4,1,4,1,4,1,5,
  	1,5,1,5,1,5,1,5,1,5,1,6,1,6,1,7,1,7,1,8,1,8,1,8,1,8,1,8,1,9,1,9,1,10,
  	1,10,1,11,1,11,1,11,1,11,1,11,1,12,1,12,1,13,1,13,1,13,1,13,1,13,1,14,
  	1,14,1,15,1,15,1,16,1,16,1,17,1,17,1,18,1,18,1,18,1,18,1,18,1,18,1,18,
  	1,18,1,18,1,19,1,19,1,19,1,19,1,19,1,19,1,19,1,19,1,19,1,19,1,19,1,19,
  	1,19,1,19,1,19,1,20,1,20,1,21,3,21,158,8,21,1,21,1,21,3,21,162,8,21,1,
  	21,1,21,1,22,1,22,1,22,1,22,1,23,3,23,171,8,23,1,23,1,23,5,23,175,8,23,
  	10,23,12,23,178,9,23,1,24,3,24,181,8,24,1,24,1,24,1,24,4,24,186,8,24,
  	11,24,12,24,187,1,25,3,25,191,8,25,1,25,1,25,5,25,195,8,25,10,25,12,25,
  	198,9,25,1,26,3,26,201,8,26,1,26,1,26,1,26,5,26,206,8,26,10,26,12,26,
  	209,9,26,1,27,1,27,1,27,1,27,1,27,1,27,1,27,1,27,1,27,3,27,220,8,27,1,
  	28,1,28,5,28,224,8,28,10,28,12,28,227,9,28,1,29,1,29,1,29,1,29,5,29,233,
  	8,29,10,29,12,29,236,9,29,1,29,1,29,0,0,30,1,1,3,2,5,3,7,4,9,5,11,6,13,
  	7,15,8,17,9,19,10,21,11,23,12,25,13,27,14,29,15,31,16,33,17,35,18,37,
  	19,39,20,41,21,43,22,45,23,47,24,49,25,51,26,53,27,55,28,57,29,59,30,
  	1,0,11,2,0,9,9,32,32,1,0,49,57,1,0,48,57,2,0,88,88,120,120,3,0,48,57,
  	65,70,97,102,1,0,48,55,2,0,66,66,98,98,1,0,48,49,3,0,65,90,95,95,97,122,
  	4,0,48,57,65,90,95,95,97,122,2,0,34,34,92,92,252,0,1,1,0,0,0,0,3,1,0,
  	0,0,0,5,1,0,0,0,0,7,1,0,0,0,0,9,1,0,0,0,0,11,1,0,0,0,0,13,1,0,0,0,0,15,
  	1,0,0,0,0,17,1,0,0,0,0,19,1,0,0,0,0,21,1,0,0,0,0,23,1,0,0,0,0,25,1,0,
  	0,0,0,27,1,0,0,0,0,29,1,0,0,0,0,31,1,0,0,0,0,33,1,0,0,0,0,35,1,0,0,0,
  	0,37,1,0,0,0,0,39,1,0,0,0,0,41,1,0,0,0,0,43,1,0,0,0,0,45,1,0,0,0,0,47,
  	1,0,0,0,0,49,1,0,0,0,0,51,1,0,0,0,0,53,1,0,0,0,0,55,1,0,0,0,0,57,1,0,
  	0,0,0,59,1,0,0,0,1,61,1,0,0,0,3,63,1,0,0,0,5,69,1,0,0,0,7,76,1,0,0,0,
  	9,84,1,0,0,0,11,91,1,0,0,0,13,97,1,0,0,0,15,99,1,0,0,0,17,101,1,0,0,0,
  	19,106,1,0,0,0,21,108,1,0,0,0,23,110,1,0,0,0,25,115,1,0,0,0,27,117,1,
  	0,0,0,29,122,1,0,0,0,31,124,1,0,0,0,33,126,1,0,0,0,35,128,1,0,0,0,37,
  	130,1,0,0,0,39,139,1,0,0,0,41,154,1,0,0,0,43,161,1,0,0,0,45,165,1,0,0,
  	0,47,170,1,0,0,0,49,180,1,0,0,0,51,190,1,0,0,0,53,200,1,0,0,0,55,219,
  	1,0,0,0,57,221,1,0,0,0,59,228,1,0,0,0,61,62,5,59,0,0,62,2,1,0,0,0,63,
  	64,5,35,0,0,64,65,5,114,0,0,65,66,5,111,0,0,66,67,5,111,0,0,67,68,5,116,
  	0,0,68,4,1,0,0,0,69,70,5,35,0,0,70,71,5,97,0,0,71,72,5,108,0,0,72,73,
  	5,105,0,0,73,74,5,103,0,0,74,75,5,110,0,0,75,6,1,0,0,0,76,77,5,116,0,
  	0,77,78,5,121,0,0,78,79,5,112,0,0,79,80,5,101,0,0,80,81,5,100,0,0,81,
  	82,5,101,0,0,82,83,5,102,0,0,83,8,1,0,0,0,84,85,5,115,0,0,85,86,5,116,
  	0,0,86,87,5,114,0,0,87,88,5,117,0,0,88,89,5,99,0,0,89,90,5,116,0,0,90,
  	10,1,0,0,0,91,92,5,97,0,0,92,93,5,108,0,0,93,94,5,105,0,0,94,95,5,103,
  	0,0,95,96,5,110,0,0,96,12,1,0,0,0,97,98,5,40,0,0,98,14,1,0,0,0,99,100,
  	5,41,0,0,100,16,1,0,0,0,101,102,5,115,0,0,102,103,5,105,0,0,103,104,5,
  	122,0,0,104,105,5,101,0,0,105,18,1,0,0,0,106,107,5,123,0,0,107,20,1,0,
  	0,0,108,109,5,125,0,0,109,22,1,0,0,0,110,111,5,102,0,0,111,112,5,108,
  	0,0,112,113,5,97,0,0,113,114,5,103,0,0,114,24,1,0,0,0,115,116,5,58,0,
  	0,116,26,1,0,0,0,117,118,5,101,0,0,118,119,5,110,0,0,119,120,5,117,0,
  	0,120,121,5,109,0,0,121,28,1,0,0,0,122,123,5,44,0,0,123,30,1,0,0,0,124,
  	125,5,42,0,0,125,32,1,0,0,0,126,127,5,91,0,0,127,34,1,0,0,0,128,129,5,
  	93,0,0,129,36,1,0,0,0,130,131,5,36,0,0,131,132,5,112,0,0,132,133,5,97,
  	0,0,133,134,5,100,0,0,134,135,5,100,0,0,135,136,5,105,0,0,136,137,5,110,
  	0,0,137,138,5,103,0,0,138,38,1,0,0,0,139,140,5,36,0,0,140,141,5,97,0,
  	0,141,142,5,115,0,0,142,143,5,115,0,0,143,144,5,101,0,0,144,145,5,114,
  	0,0,145,146,5,116,0,0,146,147,5,95,0,0,147,148,5,111,0,0,148,149,5,102,
  	0,0,149,150,5,102,0,0,150,151,5,115,0,0,151,152,5,101,0,0,152,153,5,116,
  	0,0,153,40,1,0,0,0,154,155,5,61,0,0,155,42,1,0,0,0,156,158,5,13,0,0,157,
  	156,1,0,0,0,157,158,1,0,0,0,158,159,1,0,0,0,159,162,5,10,0,0,160,162,
  	5,13,0,0,161,157,1,0,0,0,161,160,1,0,0,0,162,163,1,0,0,0,163,164,6,21,
  	0,0,164,44,1,0,0,0,165,166,7,0,0,0,166,167,1,0,0,0,167,168,6,22,0,0,168,
  	46,1,0,0,0,169,171,5,45,0,0,170,169,1,0,0,0,170,171,1,0,0,0,171,172,1,
  	0,0,0,172,176,7,1,0,0,173,175,7,2,0,0,174,173,1,0,0,0,175,178,1,0,0,0,
  	176,174,1,0,0,0,176,177,1,0,0,0,177,48,1,0,0,0,178,176,1,0,0,0,179,181,
  	5,45,0,0,180,179,1,0,0,0,180,181,1,0,0,0,181,182,1,0,0,0,182,183,5,48,
  	0,0,183,185,7,3,0,0,184,186,7,4,0,0,185,184,1,0,0,0,186,187,1,0,0,0,187,
  	185,1,0,0,0,187,188,1,0,0,0,188,50,1,0,0,0,189,191,5,45,0,0,190,189,1,
  	0,0,0,190,191,1,0,0,0,191,192,1,0,0,0,192,196,5,48,0,0,193,195,7,5,0,
  	0,194,193,1,0,0,0,195,198,1,0,0,0,196,194,1,0,0,0,196,197,1,0,0,0,197,
  	52,1,0,0,0,198,196,1,0,0,0,199,201,5,45,0,0,200,199,1,0,0,0,200,201,1,
  	0,0,0,201,202,1,0,0,0,202,203,5,48,0,0,203,207,7,6,0,0,204,206,7,7,0,
  	0,205,204,1,0,0,0,206,209,1,0,0,0,207,205,1,0,0,0,207,208,1,0,0,0,208,
  	54,1,0,0,0,209,207,1,0,0,0,210,211,5,116,0,0,211,212,5,114,0,0,212,213,
  	5,117,0,0,213,220,5,101,0,0,214,215,5,102,0,0,215,216,5,97,0,0,216,217,
  	5,108,0,0,217,218,5,115,0,0,218,220,5,101,0,0,219,210,1,0,0,0,219,214,
  	1,0,0,0,220,56,1,0,0,0,221,225,7,8,0,0,222,224,7,9,0,0,223,222,1,0,0,
  	0,224,227,1,0,0,0,225,223,1,0,0,0,225,226,1,0,0,0,226,58,1,0,0,0,227,
  	225,1,0,0,0,228,234,5,34,0,0,229,233,8,10,0,0,230,231,5,92,0,0,231,233,
  	9,0,0,0,232,229,1,0,0,0,232,230,1,0,0,0,233,236,1,0,0,0,234,232,1,0,0,
  	0,234,235,1,0,0,0,235,237,1,0,0,0,236,234,1,0,0,0,237,238,5,34,0,0,238,
  	60,1,0,0,0,15,0,157,161,170,176,180,187,190,196,200,207,219,225,232,234,
  	1,6,0,0
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
