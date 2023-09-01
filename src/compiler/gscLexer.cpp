
// Generated from .\grammar\gsc.g4 by ANTLR 4.13.0


#include "gscLexer.h"


using namespace antlr4;



using namespace antlr4;

namespace {

struct GscLexerStaticData final {
  GscLexerStaticData(std::vector<std::string> ruleNames,
                          std::vector<std::string> channelNames,
                          std::vector<std::string> modeNames,
                          std::vector<std::string> literalNames,
                          std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), channelNames(std::move(channelNames)),
        modeNames(std::move(modeNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  GscLexerStaticData(const GscLexerStaticData&) = delete;
  GscLexerStaticData(GscLexerStaticData&&) = delete;
  GscLexerStaticData& operator=(const GscLexerStaticData&) = delete;
  GscLexerStaticData& operator=(GscLexerStaticData&&) = delete;

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

::antlr4::internal::OnceFlag gsclexerLexerOnceFlag;
#if ANTLR4_USE_THREAD_LOCAL_CACHE
static thread_local
#endif
GscLexerStaticData *gsclexerLexerStaticData = nullptr;

void gsclexerLexerInitialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  if (gsclexerLexerStaticData != nullptr) {
    return;
  }
#else
  assert(gsclexerLexerStaticData == nullptr);
#endif
  auto staticData = std::make_unique<GscLexerStaticData>(
    std::vector<std::string>{
      "T__0", "T__1", "T__2", "T__3", "T__4", "T__5", "IDENTIFIER", "PATH", 
      "SEMICO", "NEWLINE", "WHITESPACE"
    },
    std::vector<std::string>{
      "DEFAULT_TOKEN_CHANNEL", "HIDDEN"
    },
    std::vector<std::string>{
      "DEFAULT_MODE"
    },
    std::vector<std::string>{
      "", "'#include'", "'function'", "'('", "')'", "'{'", "'}'", "", "", 
      "';'"
    },
    std::vector<std::string>{
      "", "", "", "", "", "", "", "IDENTIFIER", "PATH", "SEMICO", "NEWLINE", 
      "WHITESPACE"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,0,11,76,6,-1,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,
  	6,2,7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
  	0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,2,1,3,1,3,1,4,1,4,1,5,1,5,
  	1,6,1,6,5,6,52,8,6,10,6,12,6,55,9,6,1,7,4,7,58,8,7,11,7,12,7,59,1,8,1,
  	8,1,9,3,9,65,8,9,1,9,1,9,3,9,69,8,9,1,9,1,9,1,10,1,10,1,10,1,10,0,0,11,
  	1,1,3,2,5,3,7,4,9,5,11,6,13,7,15,8,17,9,19,10,21,11,1,0,4,3,0,65,90,95,
  	95,97,122,4,0,48,57,65,90,95,95,97,122,5,0,46,57,65,90,92,92,95,95,97,
  	122,2,0,9,9,32,32,79,0,1,1,0,0,0,0,3,1,0,0,0,0,5,1,0,0,0,0,7,1,0,0,0,
  	0,9,1,0,0,0,0,11,1,0,0,0,0,13,1,0,0,0,0,15,1,0,0,0,0,17,1,0,0,0,0,19,
  	1,0,0,0,0,21,1,0,0,0,1,23,1,0,0,0,3,32,1,0,0,0,5,41,1,0,0,0,7,43,1,0,
  	0,0,9,45,1,0,0,0,11,47,1,0,0,0,13,49,1,0,0,0,15,57,1,0,0,0,17,61,1,0,
  	0,0,19,68,1,0,0,0,21,72,1,0,0,0,23,24,5,35,0,0,24,25,5,105,0,0,25,26,
  	5,110,0,0,26,27,5,99,0,0,27,28,5,108,0,0,28,29,5,117,0,0,29,30,5,100,
  	0,0,30,31,5,101,0,0,31,2,1,0,0,0,32,33,5,102,0,0,33,34,5,117,0,0,34,35,
  	5,110,0,0,35,36,5,99,0,0,36,37,5,116,0,0,37,38,5,105,0,0,38,39,5,111,
  	0,0,39,40,5,110,0,0,40,4,1,0,0,0,41,42,5,40,0,0,42,6,1,0,0,0,43,44,5,
  	41,0,0,44,8,1,0,0,0,45,46,5,123,0,0,46,10,1,0,0,0,47,48,5,125,0,0,48,
  	12,1,0,0,0,49,53,7,0,0,0,50,52,7,1,0,0,51,50,1,0,0,0,52,55,1,0,0,0,53,
  	51,1,0,0,0,53,54,1,0,0,0,54,14,1,0,0,0,55,53,1,0,0,0,56,58,7,2,0,0,57,
  	56,1,0,0,0,58,59,1,0,0,0,59,57,1,0,0,0,59,60,1,0,0,0,60,16,1,0,0,0,61,
  	62,5,59,0,0,62,18,1,0,0,0,63,65,5,13,0,0,64,63,1,0,0,0,64,65,1,0,0,0,
  	65,66,1,0,0,0,66,69,5,10,0,0,67,69,5,13,0,0,68,64,1,0,0,0,68,67,1,0,0,
  	0,69,70,1,0,0,0,70,71,6,9,0,0,71,20,1,0,0,0,72,73,7,3,0,0,73,74,1,0,0,
  	0,74,75,6,10,0,0,75,22,1,0,0,0,5,0,53,59,64,68,1,6,0,0
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  gsclexerLexerStaticData = staticData.release();
}

}

gscLexer::gscLexer(CharStream *input) : Lexer(input) {
  gscLexer::initialize();
  _interpreter = new atn::LexerATNSimulator(this, *gsclexerLexerStaticData->atn, gsclexerLexerStaticData->decisionToDFA, gsclexerLexerStaticData->sharedContextCache);
}

gscLexer::~gscLexer() {
  delete _interpreter;
}

std::string gscLexer::getGrammarFileName() const {
  return "gsc.g4";
}

const std::vector<std::string>& gscLexer::getRuleNames() const {
  return gsclexerLexerStaticData->ruleNames;
}

const std::vector<std::string>& gscLexer::getChannelNames() const {
  return gsclexerLexerStaticData->channelNames;
}

const std::vector<std::string>& gscLexer::getModeNames() const {
  return gsclexerLexerStaticData->modeNames;
}

const dfa::Vocabulary& gscLexer::getVocabulary() const {
  return gsclexerLexerStaticData->vocabulary;
}

antlr4::atn::SerializedATNView gscLexer::getSerializedATN() const {
  return gsclexerLexerStaticData->serializedATN;
}

const atn::ATN& gscLexer::getATN() const {
  return *gsclexerLexerStaticData->atn;
}




void gscLexer::initialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  gsclexerLexerInitialize();
#else
  ::antlr4::internal::call_once(gsclexerLexerOnceFlag, gsclexerLexerInitialize);
#endif
}
