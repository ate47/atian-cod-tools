// precompiled header
#include <includes.hpp>


// Generated from .\grammar\ddl.g4 by ANTLR 4.13.0


#include "ddlVisitor.h"

#include "ddlParser.h"


using namespace antlrcpp;

using namespace antlr4;

namespace {

struct DdlParserStaticData final {
  DdlParserStaticData(std::vector<std::string> ruleNames,
                        std::vector<std::string> literalNames,
                        std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  DdlParserStaticData(const DdlParserStaticData&) = delete;
  DdlParserStaticData(DdlParserStaticData&&) = delete;
  DdlParserStaticData& operator=(const DdlParserStaticData&) = delete;
  DdlParserStaticData& operator=(DdlParserStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

::antlr4::internal::OnceFlag ddlParserOnceFlag;
#if ANTLR4_USE_THREAD_LOCAL_CACHE
static thread_local
#endif
DdlParserStaticData *ddlParserStaticData = nullptr;

void ddlParserInitialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  if (ddlParserStaticData != nullptr) {
    return;
  }
#else
  assert(ddlParserStaticData == nullptr);
#endif
  auto staticData = std::make_unique<DdlParserStaticData>(
    std::vector<std::string>{
      "prog", "buffer_data", "buffer_flags", "enum", "struct", "struct_def", 
      "struct_type", "number"
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
  	4,1,25,114,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,1,0,4,0,18,8,0,11,0,12,0,19,1,0,1,0,1,1,5,1,25,8,1,10,1,12,1,28,
  	9,1,1,1,1,1,1,1,1,1,1,1,1,1,5,1,36,8,1,10,1,12,1,39,9,1,1,1,1,1,1,2,1,
  	2,1,2,3,2,46,8,2,1,2,1,2,1,3,1,3,1,3,1,3,1,3,1,3,5,3,56,8,3,10,3,12,3,
  	59,9,3,3,3,61,8,3,1,3,1,3,1,3,1,4,1,4,1,4,1,4,5,4,70,8,4,10,4,12,4,73,
  	9,4,1,4,1,4,1,4,1,5,1,5,1,5,1,5,1,5,3,5,83,8,5,1,5,3,5,86,8,5,1,5,1,5,
  	1,6,1,6,1,6,1,6,3,6,94,8,6,1,6,1,6,1,6,1,6,1,6,1,6,1,6,1,6,1,6,1,6,1,
  	6,1,6,3,6,108,8,6,3,6,110,8,6,1,7,1,7,1,7,0,0,8,0,2,4,6,8,10,12,14,0,
  	2,1,0,10,11,1,0,21,24,120,0,17,1,0,0,0,2,26,1,0,0,0,4,42,1,0,0,0,6,49,
  	1,0,0,0,8,65,1,0,0,0,10,77,1,0,0,0,12,109,1,0,0,0,14,111,1,0,0,0,16,18,
  	3,2,1,0,17,16,1,0,0,0,18,19,1,0,0,0,19,17,1,0,0,0,19,20,1,0,0,0,20,21,
  	1,0,0,0,21,22,5,0,0,1,22,1,1,0,0,0,23,25,3,4,2,0,24,23,1,0,0,0,25,28,
  	1,0,0,0,26,24,1,0,0,0,26,27,1,0,0,0,27,29,1,0,0,0,28,26,1,0,0,0,29,30,
  	5,1,0,0,30,31,3,14,7,0,31,37,5,2,0,0,32,36,3,6,3,0,33,36,3,8,4,0,34,36,
  	3,10,5,0,35,32,1,0,0,0,35,33,1,0,0,0,35,34,1,0,0,0,36,39,1,0,0,0,37,35,
  	1,0,0,0,37,38,1,0,0,0,38,40,1,0,0,0,39,37,1,0,0,0,40,41,5,3,0,0,41,3,
  	1,0,0,0,42,43,5,4,0,0,43,45,5,25,0,0,44,46,3,14,7,0,45,44,1,0,0,0,45,
  	46,1,0,0,0,46,47,1,0,0,0,47,48,5,5,0,0,48,5,1,0,0,0,49,50,5,6,0,0,50,
  	51,5,25,0,0,51,60,5,2,0,0,52,57,5,25,0,0,53,54,5,7,0,0,54,56,5,25,0,0,
  	55,53,1,0,0,0,56,59,1,0,0,0,57,55,1,0,0,0,57,58,1,0,0,0,58,61,1,0,0,0,
  	59,57,1,0,0,0,60,52,1,0,0,0,60,61,1,0,0,0,61,62,1,0,0,0,62,63,5,3,0,0,
  	63,64,5,8,0,0,64,7,1,0,0,0,65,66,5,9,0,0,66,67,5,25,0,0,67,71,5,2,0,0,
  	68,70,3,10,5,0,69,68,1,0,0,0,70,73,1,0,0,0,71,69,1,0,0,0,71,72,1,0,0,
  	0,72,74,1,0,0,0,73,71,1,0,0,0,74,75,5,3,0,0,75,76,5,8,0,0,76,9,1,0,0,
  	0,77,78,3,12,6,0,78,85,5,25,0,0,79,82,5,4,0,0,80,83,3,14,7,0,81,83,5,
  	25,0,0,82,80,1,0,0,0,82,81,1,0,0,0,83,84,1,0,0,0,84,86,5,5,0,0,85,79,
  	1,0,0,0,85,86,1,0,0,0,86,87,1,0,0,0,87,88,5,8,0,0,88,11,1,0,0,0,89,110,
  	5,25,0,0,90,93,7,0,0,0,91,92,5,12,0,0,92,94,3,14,7,0,93,91,1,0,0,0,93,
  	94,1,0,0,0,94,108,1,0,0,0,95,96,5,13,0,0,96,97,5,14,0,0,97,98,3,14,7,
  	0,98,99,5,15,0,0,99,108,1,0,0,0,100,101,5,16,0,0,101,102,5,17,0,0,102,
  	103,3,14,7,0,103,104,5,7,0,0,104,105,3,14,7,0,105,106,5,18,0,0,106,108,
  	1,0,0,0,107,90,1,0,0,0,107,95,1,0,0,0,107,100,1,0,0,0,108,110,1,0,0,0,
  	109,89,1,0,0,0,109,107,1,0,0,0,110,13,1,0,0,0,111,112,7,1,0,0,112,15,
  	1,0,0,0,13,19,26,35,37,45,57,60,71,82,85,93,107,109
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  ddlParserStaticData = staticData.release();
}

}

ddlParser::ddlParser(TokenStream *input) : ddlParser(input, antlr4::atn::ParserATNSimulatorOptions()) {}

ddlParser::ddlParser(TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options) : Parser(input) {
  ddlParser::initialize();
  _interpreter = new atn::ParserATNSimulator(this, *ddlParserStaticData->atn, ddlParserStaticData->decisionToDFA, ddlParserStaticData->sharedContextCache, options);
}

ddlParser::~ddlParser() {
  delete _interpreter;
}

const atn::ATN& ddlParser::getATN() const {
  return *ddlParserStaticData->atn;
}

std::string ddlParser::getGrammarFileName() const {
  return "ddl.g4";
}

const std::vector<std::string>& ddlParser::getRuleNames() const {
  return ddlParserStaticData->ruleNames;
}

const dfa::Vocabulary& ddlParser::getVocabulary() const {
  return ddlParserStaticData->vocabulary;
}

antlr4::atn::SerializedATNView ddlParser::getSerializedATN() const {
  return ddlParserStaticData->serializedATN;
}


//----------------- ProgContext ------------------------------------------------------------------

ddlParser::ProgContext::ProgContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* ddlParser::ProgContext::EOF() {
  return getToken(ddlParser::EOF, 0);
}

std::vector<ddlParser::Buffer_dataContext *> ddlParser::ProgContext::buffer_data() {
  return getRuleContexts<ddlParser::Buffer_dataContext>();
}

ddlParser::Buffer_dataContext* ddlParser::ProgContext::buffer_data(size_t i) {
  return getRuleContext<ddlParser::Buffer_dataContext>(i);
}


size_t ddlParser::ProgContext::getRuleIndex() const {
  return ddlParser::RuleProg;
}


std::any ddlParser::ProgContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ddlVisitor*>(visitor))
    return parserVisitor->visitProg(this);
  else
    return visitor->visitChildren(this);
}

ddlParser::ProgContext* ddlParser::prog() {
  ProgContext *_localctx = _tracker.createInstance<ProgContext>(_ctx, getState());
  enterRule(_localctx, 0, ddlParser::RuleProg);
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
    setState(17); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(16);
      buffer_data();
      setState(19); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while (_la == ddlParser::T__0

    || _la == ddlParser::T__3);
    setState(21);
    match(ddlParser::EOF);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Buffer_dataContext ------------------------------------------------------------------

ddlParser::Buffer_dataContext::Buffer_dataContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

ddlParser::NumberContext* ddlParser::Buffer_dataContext::number() {
  return getRuleContext<ddlParser::NumberContext>(0);
}

std::vector<ddlParser::Buffer_flagsContext *> ddlParser::Buffer_dataContext::buffer_flags() {
  return getRuleContexts<ddlParser::Buffer_flagsContext>();
}

ddlParser::Buffer_flagsContext* ddlParser::Buffer_dataContext::buffer_flags(size_t i) {
  return getRuleContext<ddlParser::Buffer_flagsContext>(i);
}

std::vector<ddlParser::EnumContext *> ddlParser::Buffer_dataContext::enum_() {
  return getRuleContexts<ddlParser::EnumContext>();
}

ddlParser::EnumContext* ddlParser::Buffer_dataContext::enum_(size_t i) {
  return getRuleContext<ddlParser::EnumContext>(i);
}

std::vector<ddlParser::StructContext *> ddlParser::Buffer_dataContext::struct_() {
  return getRuleContexts<ddlParser::StructContext>();
}

ddlParser::StructContext* ddlParser::Buffer_dataContext::struct_(size_t i) {
  return getRuleContext<ddlParser::StructContext>(i);
}

std::vector<ddlParser::Struct_defContext *> ddlParser::Buffer_dataContext::struct_def() {
  return getRuleContexts<ddlParser::Struct_defContext>();
}

ddlParser::Struct_defContext* ddlParser::Buffer_dataContext::struct_def(size_t i) {
  return getRuleContext<ddlParser::Struct_defContext>(i);
}


size_t ddlParser::Buffer_dataContext::getRuleIndex() const {
  return ddlParser::RuleBuffer_data;
}


std::any ddlParser::Buffer_dataContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ddlVisitor*>(visitor))
    return parserVisitor->visitBuffer_data(this);
  else
    return visitor->visitChildren(this);
}

ddlParser::Buffer_dataContext* ddlParser::buffer_data() {
  Buffer_dataContext *_localctx = _tracker.createInstance<Buffer_dataContext>(_ctx, getState());
  enterRule(_localctx, 2, ddlParser::RuleBuffer_data);
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
    setState(26);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == ddlParser::T__3) {
      setState(23);
      buffer_flags();
      setState(28);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(29);
    match(ddlParser::T__0);
    setState(30);
    number();
    setState(31);
    match(ddlParser::T__1);
    setState(37);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 33631808) != 0)) {
      setState(35);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case ddlParser::T__5: {
          setState(32);
          enum_();
          break;
        }

        case ddlParser::T__8: {
          setState(33);
          struct_();
          break;
        }

        case ddlParser::T__9:
        case ddlParser::T__10:
        case ddlParser::T__12:
        case ddlParser::T__15:
        case ddlParser::IDENTIFIER: {
          setState(34);
          struct_def();
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(39);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(40);
    match(ddlParser::T__2);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Buffer_flagsContext ------------------------------------------------------------------

ddlParser::Buffer_flagsContext::Buffer_flagsContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* ddlParser::Buffer_flagsContext::IDENTIFIER() {
  return getToken(ddlParser::IDENTIFIER, 0);
}

ddlParser::NumberContext* ddlParser::Buffer_flagsContext::number() {
  return getRuleContext<ddlParser::NumberContext>(0);
}


size_t ddlParser::Buffer_flagsContext::getRuleIndex() const {
  return ddlParser::RuleBuffer_flags;
}


std::any ddlParser::Buffer_flagsContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ddlVisitor*>(visitor))
    return parserVisitor->visitBuffer_flags(this);
  else
    return visitor->visitChildren(this);
}

ddlParser::Buffer_flagsContext* ddlParser::buffer_flags() {
  Buffer_flagsContext *_localctx = _tracker.createInstance<Buffer_flagsContext>(_ctx, getState());
  enterRule(_localctx, 4, ddlParser::RuleBuffer_flags);
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
    setState(42);
    match(ddlParser::T__3);
    setState(43);
    match(ddlParser::IDENTIFIER);
    setState(45);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 31457280) != 0)) {
      setState(44);
      number();
    }
    setState(47);
    match(ddlParser::T__4);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- EnumContext ------------------------------------------------------------------

ddlParser::EnumContext::EnumContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> ddlParser::EnumContext::IDENTIFIER() {
  return getTokens(ddlParser::IDENTIFIER);
}

tree::TerminalNode* ddlParser::EnumContext::IDENTIFIER(size_t i) {
  return getToken(ddlParser::IDENTIFIER, i);
}


size_t ddlParser::EnumContext::getRuleIndex() const {
  return ddlParser::RuleEnum;
}


std::any ddlParser::EnumContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ddlVisitor*>(visitor))
    return parserVisitor->visitEnum(this);
  else
    return visitor->visitChildren(this);
}

ddlParser::EnumContext* ddlParser::enum_() {
  EnumContext *_localctx = _tracker.createInstance<EnumContext>(_ctx, getState());
  enterRule(_localctx, 6, ddlParser::RuleEnum);
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
    setState(49);
    match(ddlParser::T__5);
    setState(50);
    match(ddlParser::IDENTIFIER);
    setState(51);
    match(ddlParser::T__1);
    setState(60);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == ddlParser::IDENTIFIER) {
      setState(52);
      match(ddlParser::IDENTIFIER);
      setState(57);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == ddlParser::T__6) {
        setState(53);
        match(ddlParser::T__6);
        setState(54);
        match(ddlParser::IDENTIFIER);
        setState(59);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
    }
    setState(62);
    match(ddlParser::T__2);
    setState(63);
    match(ddlParser::T__7);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- StructContext ------------------------------------------------------------------

ddlParser::StructContext::StructContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* ddlParser::StructContext::IDENTIFIER() {
  return getToken(ddlParser::IDENTIFIER, 0);
}

std::vector<ddlParser::Struct_defContext *> ddlParser::StructContext::struct_def() {
  return getRuleContexts<ddlParser::Struct_defContext>();
}

ddlParser::Struct_defContext* ddlParser::StructContext::struct_def(size_t i) {
  return getRuleContext<ddlParser::Struct_defContext>(i);
}


size_t ddlParser::StructContext::getRuleIndex() const {
  return ddlParser::RuleStruct;
}


std::any ddlParser::StructContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ddlVisitor*>(visitor))
    return parserVisitor->visitStruct(this);
  else
    return visitor->visitChildren(this);
}

ddlParser::StructContext* ddlParser::struct_() {
  StructContext *_localctx = _tracker.createInstance<StructContext>(_ctx, getState());
  enterRule(_localctx, 8, ddlParser::RuleStruct);
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
    setState(65);
    match(ddlParser::T__8);
    setState(66);
    match(ddlParser::IDENTIFIER);
    setState(67);
    match(ddlParser::T__1);
    setState(71);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 33631232) != 0)) {
      setState(68);
      struct_def();
      setState(73);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(74);
    match(ddlParser::T__2);
    setState(75);
    match(ddlParser::T__7);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Struct_defContext ------------------------------------------------------------------

ddlParser::Struct_defContext::Struct_defContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

ddlParser::Struct_typeContext* ddlParser::Struct_defContext::struct_type() {
  return getRuleContext<ddlParser::Struct_typeContext>(0);
}

std::vector<tree::TerminalNode *> ddlParser::Struct_defContext::IDENTIFIER() {
  return getTokens(ddlParser::IDENTIFIER);
}

tree::TerminalNode* ddlParser::Struct_defContext::IDENTIFIER(size_t i) {
  return getToken(ddlParser::IDENTIFIER, i);
}

ddlParser::NumberContext* ddlParser::Struct_defContext::number() {
  return getRuleContext<ddlParser::NumberContext>(0);
}


size_t ddlParser::Struct_defContext::getRuleIndex() const {
  return ddlParser::RuleStruct_def;
}


std::any ddlParser::Struct_defContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ddlVisitor*>(visitor))
    return parserVisitor->visitStruct_def(this);
  else
    return visitor->visitChildren(this);
}

ddlParser::Struct_defContext* ddlParser::struct_def() {
  Struct_defContext *_localctx = _tracker.createInstance<Struct_defContext>(_ctx, getState());
  enterRule(_localctx, 10, ddlParser::RuleStruct_def);
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
    setState(77);
    struct_type();
    setState(78);
    match(ddlParser::IDENTIFIER);
    setState(85);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == ddlParser::T__3) {
      setState(79);
      match(ddlParser::T__3);
      setState(82);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case ddlParser::INTEGER10:
        case ddlParser::INTEGER16:
        case ddlParser::INTEGER8:
        case ddlParser::INTEGER2: {
          setState(80);
          number();
          break;
        }

        case ddlParser::IDENTIFIER: {
          setState(81);
          match(ddlParser::IDENTIFIER);
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(84);
      match(ddlParser::T__4);
    }
    setState(87);
    match(ddlParser::T__7);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Struct_typeContext ------------------------------------------------------------------

ddlParser::Struct_typeContext::Struct_typeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* ddlParser::Struct_typeContext::IDENTIFIER() {
  return getToken(ddlParser::IDENTIFIER, 0);
}

std::vector<ddlParser::NumberContext *> ddlParser::Struct_typeContext::number() {
  return getRuleContexts<ddlParser::NumberContext>();
}

ddlParser::NumberContext* ddlParser::Struct_typeContext::number(size_t i) {
  return getRuleContext<ddlParser::NumberContext>(i);
}


size_t ddlParser::Struct_typeContext::getRuleIndex() const {
  return ddlParser::RuleStruct_type;
}


std::any ddlParser::Struct_typeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ddlVisitor*>(visitor))
    return parserVisitor->visitStruct_type(this);
  else
    return visitor->visitChildren(this);
}

ddlParser::Struct_typeContext* ddlParser::struct_type() {
  Struct_typeContext *_localctx = _tracker.createInstance<Struct_typeContext>(_ctx, getState());
  enterRule(_localctx, 12, ddlParser::RuleStruct_type);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(109);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case ddlParser::IDENTIFIER: {
        enterOuterAlt(_localctx, 1);
        setState(89);
        match(ddlParser::IDENTIFIER);
        break;
      }

      case ddlParser::T__9:
      case ddlParser::T__10:
      case ddlParser::T__12:
      case ddlParser::T__15: {
        enterOuterAlt(_localctx, 2);
        setState(107);
        _errHandler->sync(this);
        switch (_input->LA(1)) {
          case ddlParser::T__9:
          case ddlParser::T__10: {
            setState(90);
            _la = _input->LA(1);
            if (!(_la == ddlParser::T__9

            || _la == ddlParser::T__10)) {
            _errHandler->recoverInline(this);
            }
            else {
              _errHandler->reportMatch(this);
              consume();
            }
            setState(93);
            _errHandler->sync(this);

            _la = _input->LA(1);
            if (_la == ddlParser::T__11) {
              setState(91);
              match(ddlParser::T__11);
              setState(92);
              number();
            }
            break;
          }

          case ddlParser::T__12: {
            setState(95);
            match(ddlParser::T__12);
            setState(96);
            match(ddlParser::T__13);
            setState(97);
            number();
            setState(98);
            match(ddlParser::T__14);
            break;
          }

          case ddlParser::T__15: {
            setState(100);
            match(ddlParser::T__15);
            setState(101);
            match(ddlParser::T__16);
            setState(102);
            number();
            setState(103);
            match(ddlParser::T__6);
            setState(104);
            number();
            setState(105);
            match(ddlParser::T__17);
            break;
          }

        default:
          throw NoViableAltException(this);
        }
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

//----------------- NumberContext ------------------------------------------------------------------

ddlParser::NumberContext::NumberContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* ddlParser::NumberContext::INTEGER10() {
  return getToken(ddlParser::INTEGER10, 0);
}

tree::TerminalNode* ddlParser::NumberContext::INTEGER16() {
  return getToken(ddlParser::INTEGER16, 0);
}

tree::TerminalNode* ddlParser::NumberContext::INTEGER8() {
  return getToken(ddlParser::INTEGER8, 0);
}

tree::TerminalNode* ddlParser::NumberContext::INTEGER2() {
  return getToken(ddlParser::INTEGER2, 0);
}


size_t ddlParser::NumberContext::getRuleIndex() const {
  return ddlParser::RuleNumber;
}


std::any ddlParser::NumberContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ddlVisitor*>(visitor))
    return parserVisitor->visitNumber(this);
  else
    return visitor->visitChildren(this);
}

ddlParser::NumberContext* ddlParser::number() {
  NumberContext *_localctx = _tracker.createInstance<NumberContext>(_ctx, getState());
  enterRule(_localctx, 14, ddlParser::RuleNumber);
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
    setState(111);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 31457280) != 0))) {
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

void ddlParser::initialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  ddlParserInitialize();
#else
  ::antlr4::internal::call_once(ddlParserOnceFlag, ddlParserInitialize);
#endif
}
