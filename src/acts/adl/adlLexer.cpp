
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
      "T__17", "T__18", "T__19", "T__20", "T__21", "T__22", "NEWLINE", "WHITESPACE", 
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
      "'enum'", "'customtype'", "','", "'*'", "'['", "']'", "'$padding'", 
      "'$assert_offset'", "'='"
    },
    std::vector<std::string>{
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
      "", "", "", "", "", "", "", "NEWLINE", "WHITESPACE", "INTEGER10", 
      "INTEGER16", "INTEGER8", "INTEGER2", "BOOL_VALUE", "IDENTIFIER", "STRING"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,0,32,263,6,-1,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,
  	6,2,7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,
  	7,14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,
  	7,21,2,22,7,22,2,23,7,23,2,24,7,24,2,25,7,25,2,26,7,26,2,27,7,27,2,28,
  	7,28,2,29,7,29,2,30,7,30,2,31,7,31,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,
  	2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,4,1,4,
  	1,4,1,4,1,4,1,4,1,4,1,4,1,5,1,5,1,5,1,5,1,5,1,5,1,5,1,6,1,6,1,6,1,6,1,
  	6,1,6,1,7,1,7,1,8,1,8,1,9,1,9,1,9,1,9,1,9,1,10,1,10,1,11,1,11,1,12,1,
  	12,1,12,1,12,1,12,1,13,1,13,1,14,1,14,1,14,1,14,1,14,1,15,1,15,1,15,1,
  	15,1,15,1,15,1,15,1,15,1,15,1,15,1,15,1,16,1,16,1,17,1,17,1,18,1,18,1,
  	19,1,19,1,20,1,20,1,20,1,20,1,20,1,20,1,20,1,20,1,20,1,21,1,21,1,21,1,
  	21,1,21,1,21,1,21,1,21,1,21,1,21,1,21,1,21,1,21,1,21,1,21,1,22,1,22,1,
  	23,3,23,182,8,23,1,23,1,23,3,23,186,8,23,1,23,1,23,1,24,1,24,1,24,1,24,
  	1,25,3,25,195,8,25,1,25,1,25,5,25,199,8,25,10,25,12,25,202,9,25,1,26,
  	3,26,205,8,26,1,26,1,26,1,26,4,26,210,8,26,11,26,12,26,211,1,27,3,27,
  	215,8,27,1,27,1,27,5,27,219,8,27,10,27,12,27,222,9,27,1,28,3,28,225,8,
  	28,1,28,1,28,1,28,5,28,230,8,28,10,28,12,28,233,9,28,1,29,1,29,1,29,1,
  	29,1,29,1,29,1,29,1,29,1,29,3,29,244,8,29,1,30,1,30,5,30,248,8,30,10,
  	30,12,30,251,9,30,1,31,1,31,1,31,1,31,5,31,257,8,31,10,31,12,31,260,9,
  	31,1,31,1,31,0,0,32,1,1,3,2,5,3,7,4,9,5,11,6,13,7,15,8,17,9,19,10,21,
  	11,23,12,25,13,27,14,29,15,31,16,33,17,35,18,37,19,39,20,41,21,43,22,
  	45,23,47,24,49,25,51,26,53,27,55,28,57,29,59,30,61,31,63,32,1,0,11,2,
  	0,9,9,32,32,1,0,49,57,1,0,48,57,2,0,88,88,120,120,3,0,48,57,65,70,97,
  	102,1,0,48,55,2,0,66,66,98,98,1,0,48,49,3,0,65,90,95,95,97,122,4,0,48,
  	57,65,90,95,95,97,122,2,0,34,34,92,92,276,0,1,1,0,0,0,0,3,1,0,0,0,0,5,
  	1,0,0,0,0,7,1,0,0,0,0,9,1,0,0,0,0,11,1,0,0,0,0,13,1,0,0,0,0,15,1,0,0,
  	0,0,17,1,0,0,0,0,19,1,0,0,0,0,21,1,0,0,0,0,23,1,0,0,0,0,25,1,0,0,0,0,
  	27,1,0,0,0,0,29,1,0,0,0,0,31,1,0,0,0,0,33,1,0,0,0,0,35,1,0,0,0,0,37,1,
  	0,0,0,0,39,1,0,0,0,0,41,1,0,0,0,0,43,1,0,0,0,0,45,1,0,0,0,0,47,1,0,0,
  	0,0,49,1,0,0,0,0,51,1,0,0,0,0,53,1,0,0,0,0,55,1,0,0,0,0,57,1,0,0,0,0,
  	59,1,0,0,0,0,61,1,0,0,0,0,63,1,0,0,0,1,65,1,0,0,0,3,67,1,0,0,0,5,73,1,
  	0,0,0,7,82,1,0,0,0,9,89,1,0,0,0,11,97,1,0,0,0,13,104,1,0,0,0,15,110,1,
  	0,0,0,17,112,1,0,0,0,19,114,1,0,0,0,21,119,1,0,0,0,23,121,1,0,0,0,25,
  	123,1,0,0,0,27,128,1,0,0,0,29,130,1,0,0,0,31,135,1,0,0,0,33,146,1,0,0,
  	0,35,148,1,0,0,0,37,150,1,0,0,0,39,152,1,0,0,0,41,154,1,0,0,0,43,163,
  	1,0,0,0,45,178,1,0,0,0,47,185,1,0,0,0,49,189,1,0,0,0,51,194,1,0,0,0,53,
  	204,1,0,0,0,55,214,1,0,0,0,57,224,1,0,0,0,59,243,1,0,0,0,61,245,1,0,0,
  	0,63,252,1,0,0,0,65,66,5,59,0,0,66,2,1,0,0,0,67,68,5,35,0,0,68,69,5,114,
  	0,0,69,70,5,111,0,0,70,71,5,111,0,0,71,72,5,116,0,0,72,4,1,0,0,0,73,74,
  	5,35,0,0,74,75,5,105,0,0,75,76,5,110,0,0,76,77,5,99,0,0,77,78,5,108,0,
  	0,78,79,5,117,0,0,79,80,5,100,0,0,80,81,5,101,0,0,81,6,1,0,0,0,82,83,
  	5,35,0,0,83,84,5,97,0,0,84,85,5,108,0,0,85,86,5,105,0,0,86,87,5,103,0,
  	0,87,88,5,110,0,0,88,8,1,0,0,0,89,90,5,116,0,0,90,91,5,121,0,0,91,92,
  	5,112,0,0,92,93,5,101,0,0,93,94,5,100,0,0,94,95,5,101,0,0,95,96,5,102,
  	0,0,96,10,1,0,0,0,97,98,5,115,0,0,98,99,5,116,0,0,99,100,5,114,0,0,100,
  	101,5,117,0,0,101,102,5,99,0,0,102,103,5,116,0,0,103,12,1,0,0,0,104,105,
  	5,97,0,0,105,106,5,108,0,0,106,107,5,105,0,0,107,108,5,103,0,0,108,109,
  	5,110,0,0,109,14,1,0,0,0,110,111,5,40,0,0,111,16,1,0,0,0,112,113,5,41,
  	0,0,113,18,1,0,0,0,114,115,5,115,0,0,115,116,5,105,0,0,116,117,5,122,
  	0,0,117,118,5,101,0,0,118,20,1,0,0,0,119,120,5,123,0,0,120,22,1,0,0,0,
  	121,122,5,125,0,0,122,24,1,0,0,0,123,124,5,102,0,0,124,125,5,108,0,0,
  	125,126,5,97,0,0,126,127,5,103,0,0,127,26,1,0,0,0,128,129,5,58,0,0,129,
  	28,1,0,0,0,130,131,5,101,0,0,131,132,5,110,0,0,132,133,5,117,0,0,133,
  	134,5,109,0,0,134,30,1,0,0,0,135,136,5,99,0,0,136,137,5,117,0,0,137,138,
  	5,115,0,0,138,139,5,116,0,0,139,140,5,111,0,0,140,141,5,109,0,0,141,142,
  	5,116,0,0,142,143,5,121,0,0,143,144,5,112,0,0,144,145,5,101,0,0,145,32,
  	1,0,0,0,146,147,5,44,0,0,147,34,1,0,0,0,148,149,5,42,0,0,149,36,1,0,0,
  	0,150,151,5,91,0,0,151,38,1,0,0,0,152,153,5,93,0,0,153,40,1,0,0,0,154,
  	155,5,36,0,0,155,156,5,112,0,0,156,157,5,97,0,0,157,158,5,100,0,0,158,
  	159,5,100,0,0,159,160,5,105,0,0,160,161,5,110,0,0,161,162,5,103,0,0,162,
  	42,1,0,0,0,163,164,5,36,0,0,164,165,5,97,0,0,165,166,5,115,0,0,166,167,
  	5,115,0,0,167,168,5,101,0,0,168,169,5,114,0,0,169,170,5,116,0,0,170,171,
  	5,95,0,0,171,172,5,111,0,0,172,173,5,102,0,0,173,174,5,102,0,0,174,175,
  	5,115,0,0,175,176,5,101,0,0,176,177,5,116,0,0,177,44,1,0,0,0,178,179,
  	5,61,0,0,179,46,1,0,0,0,180,182,5,13,0,0,181,180,1,0,0,0,181,182,1,0,
  	0,0,182,183,1,0,0,0,183,186,5,10,0,0,184,186,5,13,0,0,185,181,1,0,0,0,
  	185,184,1,0,0,0,186,187,1,0,0,0,187,188,6,23,0,0,188,48,1,0,0,0,189,190,
  	7,0,0,0,190,191,1,0,0,0,191,192,6,24,0,0,192,50,1,0,0,0,193,195,5,45,
  	0,0,194,193,1,0,0,0,194,195,1,0,0,0,195,196,1,0,0,0,196,200,7,1,0,0,197,
  	199,7,2,0,0,198,197,1,0,0,0,199,202,1,0,0,0,200,198,1,0,0,0,200,201,1,
  	0,0,0,201,52,1,0,0,0,202,200,1,0,0,0,203,205,5,45,0,0,204,203,1,0,0,0,
  	204,205,1,0,0,0,205,206,1,0,0,0,206,207,5,48,0,0,207,209,7,3,0,0,208,
  	210,7,4,0,0,209,208,1,0,0,0,210,211,1,0,0,0,211,209,1,0,0,0,211,212,1,
  	0,0,0,212,54,1,0,0,0,213,215,5,45,0,0,214,213,1,0,0,0,214,215,1,0,0,0,
  	215,216,1,0,0,0,216,220,5,48,0,0,217,219,7,5,0,0,218,217,1,0,0,0,219,
  	222,1,0,0,0,220,218,1,0,0,0,220,221,1,0,0,0,221,56,1,0,0,0,222,220,1,
  	0,0,0,223,225,5,45,0,0,224,223,1,0,0,0,224,225,1,0,0,0,225,226,1,0,0,
  	0,226,227,5,48,0,0,227,231,7,6,0,0,228,230,7,7,0,0,229,228,1,0,0,0,230,
  	233,1,0,0,0,231,229,1,0,0,0,231,232,1,0,0,0,232,58,1,0,0,0,233,231,1,
  	0,0,0,234,235,5,116,0,0,235,236,5,114,0,0,236,237,5,117,0,0,237,244,5,
  	101,0,0,238,239,5,102,0,0,239,240,5,97,0,0,240,241,5,108,0,0,241,242,
  	5,115,0,0,242,244,5,101,0,0,243,234,1,0,0,0,243,238,1,0,0,0,244,60,1,
  	0,0,0,245,249,7,8,0,0,246,248,7,9,0,0,247,246,1,0,0,0,248,251,1,0,0,0,
  	249,247,1,0,0,0,249,250,1,0,0,0,250,62,1,0,0,0,251,249,1,0,0,0,252,258,
  	5,34,0,0,253,257,8,10,0,0,254,255,5,92,0,0,255,257,9,0,0,0,256,253,1,
  	0,0,0,256,254,1,0,0,0,257,260,1,0,0,0,258,256,1,0,0,0,258,259,1,0,0,0,
  	259,261,1,0,0,0,260,258,1,0,0,0,261,262,5,34,0,0,262,64,1,0,0,0,15,0,
  	181,185,194,200,204,211,214,220,224,231,243,249,256,258,1,6,0,0
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
