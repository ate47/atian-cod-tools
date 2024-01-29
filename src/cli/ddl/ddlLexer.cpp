
// Generated from .\grammar\ddl.g4 by ANTLR 4.13.0


#include "ddlLexer.h"


using namespace antlr4;



using namespace antlr4;

namespace {

struct DdlLexerStaticData final {
  DdlLexerStaticData(std::vector<std::string> ruleNames,
                          std::vector<std::string> channelNames,
                          std::vector<std::string> modeNames,
                          std::vector<std::string> literalNames,
                          std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), channelNames(std::move(channelNames)),
        modeNames(std::move(modeNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  DdlLexerStaticData(const DdlLexerStaticData&) = delete;
  DdlLexerStaticData(DdlLexerStaticData&&) = delete;
  DdlLexerStaticData& operator=(const DdlLexerStaticData&) = delete;
  DdlLexerStaticData& operator=(DdlLexerStaticData&&) = delete;

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

::antlr4::internal::OnceFlag ddllexerLexerOnceFlag;
#if ANTLR4_USE_THREAD_LOCAL_CACHE
static thread_local
#endif
DdlLexerStaticData *ddllexerLexerStaticData = nullptr;

void ddllexerLexerInitialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  if (ddllexerLexerStaticData != nullptr) {
    return;
  }
#else
  assert(ddllexerLexerStaticData == nullptr);
#endif
  auto staticData = std::make_unique<DdlLexerStaticData>(
    std::vector<std::string>{
      "T__0", "T__1", "T__2", "T__3", "T__4", "T__5", "T__6", "T__7", "T__8", 
      "NEWLINE", "WHITESPACE", "INTEGER10", "INTEGER16", "INTEGER8", "INTEGER2", 
      "IDENTIFIER", "PATH", "STRING"
    },
    std::vector<std::string>{
      "DEFAULT_TOKEN_CHANNEL", "HIDDEN"
    },
    std::vector<std::string>{
      "DEFAULT_MODE"
    },
    std::vector<std::string>{
      "", "';'", "'enum'", "'{'", "'='", "','", "'}'", "'struct'", "'['", 
      "']'"
    },
    std::vector<std::string>{
      "", "", "", "", "", "", "", "", "", "", "NEWLINE", "WHITESPACE", "INTEGER10", 
      "INTEGER16", "INTEGER8", "INTEGER2", "IDENTIFIER", "PATH", "STRING"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,0,18,142,6,-1,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,
  	6,2,7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,
  	7,14,2,15,7,15,2,16,7,16,2,17,7,17,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,2,1,
  	2,1,3,1,3,1,4,1,4,1,5,1,5,1,6,1,6,1,6,1,6,1,6,1,6,1,6,1,7,1,7,1,8,1,8,
  	1,9,3,9,65,8,9,1,9,1,9,3,9,69,8,9,1,9,1,9,1,10,1,10,1,10,1,10,1,11,3,
  	11,78,8,11,1,11,1,11,5,11,82,8,11,10,11,12,11,85,9,11,1,12,3,12,88,8,
  	12,1,12,1,12,1,12,1,12,4,12,94,8,12,11,12,12,12,95,1,13,3,13,99,8,13,
  	1,13,1,13,5,13,103,8,13,10,13,12,13,106,9,13,1,14,3,14,109,8,14,1,14,
  	1,14,1,14,1,14,5,14,115,8,14,10,14,12,14,118,9,14,1,15,1,15,5,15,122,
  	8,15,10,15,12,15,125,9,15,1,16,4,16,128,8,16,11,16,12,16,129,1,17,1,17,
  	1,17,1,17,5,17,136,8,17,10,17,12,17,139,9,17,1,17,1,17,0,0,18,1,1,3,2,
  	5,3,7,4,9,5,11,6,13,7,15,8,17,9,19,10,21,11,23,12,25,13,27,14,29,15,31,
  	16,33,17,35,18,1,0,10,2,0,9,9,32,32,1,0,49,57,1,0,48,57,2,0,48,57,97,
  	102,1,0,48,55,1,0,48,49,3,0,65,90,95,95,97,122,4,0,48,57,65,90,95,95,
  	97,122,5,0,46,57,65,90,92,92,95,95,97,122,2,0,34,34,92,92,155,0,1,1,0,
  	0,0,0,3,1,0,0,0,0,5,1,0,0,0,0,7,1,0,0,0,0,9,1,0,0,0,0,11,1,0,0,0,0,13,
  	1,0,0,0,0,15,1,0,0,0,0,17,1,0,0,0,0,19,1,0,0,0,0,21,1,0,0,0,0,23,1,0,
  	0,0,0,25,1,0,0,0,0,27,1,0,0,0,0,29,1,0,0,0,0,31,1,0,0,0,0,33,1,0,0,0,
  	0,35,1,0,0,0,1,37,1,0,0,0,3,39,1,0,0,0,5,44,1,0,0,0,7,46,1,0,0,0,9,48,
  	1,0,0,0,11,50,1,0,0,0,13,52,1,0,0,0,15,59,1,0,0,0,17,61,1,0,0,0,19,68,
  	1,0,0,0,21,72,1,0,0,0,23,77,1,0,0,0,25,87,1,0,0,0,27,98,1,0,0,0,29,108,
  	1,0,0,0,31,119,1,0,0,0,33,127,1,0,0,0,35,131,1,0,0,0,37,38,5,59,0,0,38,
  	2,1,0,0,0,39,40,5,101,0,0,40,41,5,110,0,0,41,42,5,117,0,0,42,43,5,109,
  	0,0,43,4,1,0,0,0,44,45,5,123,0,0,45,6,1,0,0,0,46,47,5,61,0,0,47,8,1,0,
  	0,0,48,49,5,44,0,0,49,10,1,0,0,0,50,51,5,125,0,0,51,12,1,0,0,0,52,53,
  	5,115,0,0,53,54,5,116,0,0,54,55,5,114,0,0,55,56,5,117,0,0,56,57,5,99,
  	0,0,57,58,5,116,0,0,58,14,1,0,0,0,59,60,5,91,0,0,60,16,1,0,0,0,61,62,
  	5,93,0,0,62,18,1,0,0,0,63,65,5,13,0,0,64,63,1,0,0,0,64,65,1,0,0,0,65,
  	66,1,0,0,0,66,69,5,10,0,0,67,69,5,13,0,0,68,64,1,0,0,0,68,67,1,0,0,0,
  	69,70,1,0,0,0,70,71,6,9,0,0,71,20,1,0,0,0,72,73,7,0,0,0,73,74,1,0,0,0,
  	74,75,6,10,0,0,75,22,1,0,0,0,76,78,5,45,0,0,77,76,1,0,0,0,77,78,1,0,0,
  	0,78,79,1,0,0,0,79,83,7,1,0,0,80,82,7,2,0,0,81,80,1,0,0,0,82,85,1,0,0,
  	0,83,81,1,0,0,0,83,84,1,0,0,0,84,24,1,0,0,0,85,83,1,0,0,0,86,88,5,45,
  	0,0,87,86,1,0,0,0,87,88,1,0,0,0,88,89,1,0,0,0,89,90,5,48,0,0,90,91,5,
  	120,0,0,91,93,1,0,0,0,92,94,7,3,0,0,93,92,1,0,0,0,94,95,1,0,0,0,95,93,
  	1,0,0,0,95,96,1,0,0,0,96,26,1,0,0,0,97,99,5,45,0,0,98,97,1,0,0,0,98,99,
  	1,0,0,0,99,100,1,0,0,0,100,104,5,48,0,0,101,103,7,4,0,0,102,101,1,0,0,
  	0,103,106,1,0,0,0,104,102,1,0,0,0,104,105,1,0,0,0,105,28,1,0,0,0,106,
  	104,1,0,0,0,107,109,5,45,0,0,108,107,1,0,0,0,108,109,1,0,0,0,109,110,
  	1,0,0,0,110,111,5,48,0,0,111,112,5,98,0,0,112,116,1,0,0,0,113,115,7,5,
  	0,0,114,113,1,0,0,0,115,118,1,0,0,0,116,114,1,0,0,0,116,117,1,0,0,0,117,
  	30,1,0,0,0,118,116,1,0,0,0,119,123,7,6,0,0,120,122,7,7,0,0,121,120,1,
  	0,0,0,122,125,1,0,0,0,123,121,1,0,0,0,123,124,1,0,0,0,124,32,1,0,0,0,
  	125,123,1,0,0,0,126,128,7,8,0,0,127,126,1,0,0,0,128,129,1,0,0,0,129,127,
  	1,0,0,0,129,130,1,0,0,0,130,34,1,0,0,0,131,137,5,34,0,0,132,136,8,9,0,
  	0,133,134,5,92,0,0,134,136,9,0,0,0,135,132,1,0,0,0,135,133,1,0,0,0,136,
  	139,1,0,0,0,137,135,1,0,0,0,137,138,1,0,0,0,138,140,1,0,0,0,139,137,1,
  	0,0,0,140,141,5,34,0,0,141,36,1,0,0,0,15,0,64,68,77,83,87,95,98,104,108,
  	116,123,129,135,137,1,6,0,0
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  ddllexerLexerStaticData = staticData.release();
}

}

ddlLexer::ddlLexer(CharStream *input) : Lexer(input) {
  ddlLexer::initialize();
  _interpreter = new atn::LexerATNSimulator(this, *ddllexerLexerStaticData->atn, ddllexerLexerStaticData->decisionToDFA, ddllexerLexerStaticData->sharedContextCache);
}

ddlLexer::~ddlLexer() {
  delete _interpreter;
}

std::string ddlLexer::getGrammarFileName() const {
  return "ddl.g4";
}

const std::vector<std::string>& ddlLexer::getRuleNames() const {
  return ddllexerLexerStaticData->ruleNames;
}

const std::vector<std::string>& ddlLexer::getChannelNames() const {
  return ddllexerLexerStaticData->channelNames;
}

const std::vector<std::string>& ddlLexer::getModeNames() const {
  return ddllexerLexerStaticData->modeNames;
}

const dfa::Vocabulary& ddlLexer::getVocabulary() const {
  return ddllexerLexerStaticData->vocabulary;
}

antlr4::atn::SerializedATNView ddlLexer::getSerializedATN() const {
  return ddllexerLexerStaticData->serializedATN;
}

const atn::ATN& ddlLexer::getATN() const {
  return *ddllexerLexerStaticData->atn;
}




void ddlLexer::initialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  ddllexerLexerInitialize();
#else
  ::antlr4::internal::call_once(ddllexerLexerOnceFlag, ddllexerLexerInitialize);
#endif
}
