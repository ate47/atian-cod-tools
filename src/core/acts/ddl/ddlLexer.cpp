// precompiled header
#include <includes.hpp>


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
      "T__9", "T__10", "T__11", "T__12", "T__13", "T__14", "T__15", "T__16", 
      "T__17", "NEWLINE", "WHITESPACE", "INTEGER10", "INTEGER16", "INTEGER8", 
      "INTEGER2", "IDENTIFIER"
    },
    std::vector<std::string>{
      "DEFAULT_TOKEN_CHANNEL", "HIDDEN"
    },
    std::vector<std::string>{
      "DEFAULT_MODE"
    },
    std::vector<std::string>{
      "", "'version'", "'{'", "'}'", "'['", "']'", "'enum'", "','", "';'", 
      "'struct'", "'uint'", "'int'", "':'", "'string'", "'('", "')'", "'fixed'", 
      "'<'", "'>'"
    },
    std::vector<std::string>{
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
      "", "", "NEWLINE", "WHITESPACE", "INTEGER10", "INTEGER16", "INTEGER8", 
      "INTEGER2", "IDENTIFIER"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,0,25,176,6,-1,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,
  	6,2,7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,
  	7,14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,
  	7,21,2,22,7,22,2,23,7,23,2,24,7,24,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
  	1,1,1,1,2,1,2,1,3,1,3,1,4,1,4,1,5,1,5,1,5,1,5,1,5,1,6,1,6,1,7,1,7,1,8,
  	1,8,1,8,1,8,1,8,1,8,1,8,1,9,1,9,1,9,1,9,1,9,1,10,1,10,1,10,1,10,1,11,
  	1,11,1,12,1,12,1,12,1,12,1,12,1,12,1,12,1,13,1,13,1,14,1,14,1,15,1,15,
  	1,15,1,15,1,15,1,15,1,16,1,16,1,17,1,17,1,18,3,18,117,8,18,1,18,1,18,
  	3,18,121,8,18,1,18,1,18,1,19,1,19,1,19,1,19,1,20,3,20,130,8,20,1,20,1,
  	20,5,20,134,8,20,10,20,12,20,137,9,20,1,21,3,21,140,8,21,1,21,1,21,1,
  	21,4,21,145,8,21,11,21,12,21,146,1,22,3,22,150,8,22,1,22,1,22,5,22,154,
  	8,22,10,22,12,22,157,9,22,1,23,3,23,160,8,23,1,23,1,23,1,23,5,23,165,
  	8,23,10,23,12,23,168,9,23,1,24,1,24,5,24,172,8,24,10,24,12,24,175,9,24,
  	0,0,25,1,1,3,2,5,3,7,4,9,5,11,6,13,7,15,8,17,9,19,10,21,11,23,12,25,13,
  	27,14,29,15,31,16,33,17,35,18,37,19,39,20,41,21,43,22,45,23,47,24,49,
  	25,1,0,10,2,0,9,9,32,32,1,0,49,57,1,0,48,57,2,0,88,88,120,120,3,0,48,
  	57,65,70,97,102,1,0,48,55,2,0,66,66,98,98,1,0,48,49,3,0,65,90,95,95,97,
  	122,4,0,48,57,65,90,95,95,97,122,186,0,1,1,0,0,0,0,3,1,0,0,0,0,5,1,0,
  	0,0,0,7,1,0,0,0,0,9,1,0,0,0,0,11,1,0,0,0,0,13,1,0,0,0,0,15,1,0,0,0,0,
  	17,1,0,0,0,0,19,1,0,0,0,0,21,1,0,0,0,0,23,1,0,0,0,0,25,1,0,0,0,0,27,1,
  	0,0,0,0,29,1,0,0,0,0,31,1,0,0,0,0,33,1,0,0,0,0,35,1,0,0,0,0,37,1,0,0,
  	0,0,39,1,0,0,0,0,41,1,0,0,0,0,43,1,0,0,0,0,45,1,0,0,0,0,47,1,0,0,0,0,
  	49,1,0,0,0,1,51,1,0,0,0,3,59,1,0,0,0,5,61,1,0,0,0,7,63,1,0,0,0,9,65,1,
  	0,0,0,11,67,1,0,0,0,13,72,1,0,0,0,15,74,1,0,0,0,17,76,1,0,0,0,19,83,1,
  	0,0,0,21,88,1,0,0,0,23,92,1,0,0,0,25,94,1,0,0,0,27,101,1,0,0,0,29,103,
  	1,0,0,0,31,105,1,0,0,0,33,111,1,0,0,0,35,113,1,0,0,0,37,120,1,0,0,0,39,
  	124,1,0,0,0,41,129,1,0,0,0,43,139,1,0,0,0,45,149,1,0,0,0,47,159,1,0,0,
  	0,49,169,1,0,0,0,51,52,5,118,0,0,52,53,5,101,0,0,53,54,5,114,0,0,54,55,
  	5,115,0,0,55,56,5,105,0,0,56,57,5,111,0,0,57,58,5,110,0,0,58,2,1,0,0,
  	0,59,60,5,123,0,0,60,4,1,0,0,0,61,62,5,125,0,0,62,6,1,0,0,0,63,64,5,91,
  	0,0,64,8,1,0,0,0,65,66,5,93,0,0,66,10,1,0,0,0,67,68,5,101,0,0,68,69,5,
  	110,0,0,69,70,5,117,0,0,70,71,5,109,0,0,71,12,1,0,0,0,72,73,5,44,0,0,
  	73,14,1,0,0,0,74,75,5,59,0,0,75,16,1,0,0,0,76,77,5,115,0,0,77,78,5,116,
  	0,0,78,79,5,114,0,0,79,80,5,117,0,0,80,81,5,99,0,0,81,82,5,116,0,0,82,
  	18,1,0,0,0,83,84,5,117,0,0,84,85,5,105,0,0,85,86,5,110,0,0,86,87,5,116,
  	0,0,87,20,1,0,0,0,88,89,5,105,0,0,89,90,5,110,0,0,90,91,5,116,0,0,91,
  	22,1,0,0,0,92,93,5,58,0,0,93,24,1,0,0,0,94,95,5,115,0,0,95,96,5,116,0,
  	0,96,97,5,114,0,0,97,98,5,105,0,0,98,99,5,110,0,0,99,100,5,103,0,0,100,
  	26,1,0,0,0,101,102,5,40,0,0,102,28,1,0,0,0,103,104,5,41,0,0,104,30,1,
  	0,0,0,105,106,5,102,0,0,106,107,5,105,0,0,107,108,5,120,0,0,108,109,5,
  	101,0,0,109,110,5,100,0,0,110,32,1,0,0,0,111,112,5,60,0,0,112,34,1,0,
  	0,0,113,114,5,62,0,0,114,36,1,0,0,0,115,117,5,13,0,0,116,115,1,0,0,0,
  	116,117,1,0,0,0,117,118,1,0,0,0,118,121,5,10,0,0,119,121,5,13,0,0,120,
  	116,1,0,0,0,120,119,1,0,0,0,121,122,1,0,0,0,122,123,6,18,0,0,123,38,1,
  	0,0,0,124,125,7,0,0,0,125,126,1,0,0,0,126,127,6,19,0,0,127,40,1,0,0,0,
  	128,130,5,45,0,0,129,128,1,0,0,0,129,130,1,0,0,0,130,131,1,0,0,0,131,
  	135,7,1,0,0,132,134,7,2,0,0,133,132,1,0,0,0,134,137,1,0,0,0,135,133,1,
  	0,0,0,135,136,1,0,0,0,136,42,1,0,0,0,137,135,1,0,0,0,138,140,5,45,0,0,
  	139,138,1,0,0,0,139,140,1,0,0,0,140,141,1,0,0,0,141,142,5,48,0,0,142,
  	144,7,3,0,0,143,145,7,4,0,0,144,143,1,0,0,0,145,146,1,0,0,0,146,144,1,
  	0,0,0,146,147,1,0,0,0,147,44,1,0,0,0,148,150,5,45,0,0,149,148,1,0,0,0,
  	149,150,1,0,0,0,150,151,1,0,0,0,151,155,5,48,0,0,152,154,7,5,0,0,153,
  	152,1,0,0,0,154,157,1,0,0,0,155,153,1,0,0,0,155,156,1,0,0,0,156,46,1,
  	0,0,0,157,155,1,0,0,0,158,160,5,45,0,0,159,158,1,0,0,0,159,160,1,0,0,
  	0,160,161,1,0,0,0,161,162,5,48,0,0,162,166,7,6,0,0,163,165,7,7,0,0,164,
  	163,1,0,0,0,165,168,1,0,0,0,166,164,1,0,0,0,166,167,1,0,0,0,167,48,1,
  	0,0,0,168,166,1,0,0,0,169,173,7,8,0,0,170,172,7,9,0,0,171,170,1,0,0,0,
  	172,175,1,0,0,0,173,171,1,0,0,0,173,174,1,0,0,0,174,50,1,0,0,0,175,173,
  	1,0,0,0,12,0,116,120,129,135,139,146,149,155,159,166,173,1,6,0,0
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
