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
      "prog", "buffer_data", "enum", "struct", "struct_def", "number"
    },
    std::vector<std::string>{
      "", "'version'", "'{'", "'}'", "'enum'", "'='", "','", "';'", "'struct'", 
      "'['", "']'"
    },
    std::vector<std::string>{
      "", "", "", "", "", "", "", "", "", "", "", "NEWLINE", "WHITESPACE", 
      "INTEGER10", "INTEGER16", "INTEGER8", "INTEGER2", "IDENTIFIER", "PATH", 
      "STRING"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,19,87,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,1,0,5,0,14,
  	8,0,10,0,12,0,17,9,0,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,5,1,27,8,1,10,1,
  	12,1,30,9,1,1,1,1,1,1,2,1,2,1,2,1,2,1,2,1,2,3,2,40,8,2,1,2,1,2,1,2,1,
  	2,3,2,46,8,2,5,2,48,8,2,10,2,12,2,51,9,2,3,2,53,8,2,1,2,1,2,1,2,1,3,1,
  	3,1,3,1,3,5,3,62,8,3,10,3,12,3,65,9,3,1,3,1,3,1,3,1,4,1,4,1,4,1,4,1,4,
  	1,4,1,4,1,4,1,4,3,4,79,8,4,3,4,81,8,4,1,4,1,4,1,5,1,5,1,5,0,0,6,0,2,4,
  	6,8,10,0,2,2,0,17,17,19,19,1,0,13,16,91,0,15,1,0,0,0,2,20,1,0,0,0,4,33,
  	1,0,0,0,6,57,1,0,0,0,8,69,1,0,0,0,10,84,1,0,0,0,12,14,3,2,1,0,13,12,1,
  	0,0,0,14,17,1,0,0,0,15,13,1,0,0,0,15,16,1,0,0,0,16,18,1,0,0,0,17,15,1,
  	0,0,0,18,19,5,0,0,1,19,1,1,0,0,0,20,21,5,1,0,0,21,22,3,10,5,0,22,28,5,
  	2,0,0,23,27,3,4,2,0,24,27,3,6,3,0,25,27,3,8,4,0,26,23,1,0,0,0,26,24,1,
  	0,0,0,26,25,1,0,0,0,27,30,1,0,0,0,28,26,1,0,0,0,28,29,1,0,0,0,29,31,1,
  	0,0,0,30,28,1,0,0,0,31,32,5,3,0,0,32,3,1,0,0,0,33,34,5,4,0,0,34,35,5,
  	17,0,0,35,52,5,2,0,0,36,39,7,0,0,0,37,38,5,5,0,0,38,40,3,10,5,0,39,37,
  	1,0,0,0,39,40,1,0,0,0,40,49,1,0,0,0,41,42,5,6,0,0,42,45,7,0,0,0,43,44,
  	5,5,0,0,44,46,3,10,5,0,45,43,1,0,0,0,45,46,1,0,0,0,46,48,1,0,0,0,47,41,
  	1,0,0,0,48,51,1,0,0,0,49,47,1,0,0,0,49,50,1,0,0,0,50,53,1,0,0,0,51,49,
  	1,0,0,0,52,36,1,0,0,0,52,53,1,0,0,0,53,54,1,0,0,0,54,55,5,3,0,0,55,56,
  	5,7,0,0,56,5,1,0,0,0,57,58,5,8,0,0,58,59,5,17,0,0,59,63,5,2,0,0,60,62,
  	3,8,4,0,61,60,1,0,0,0,62,65,1,0,0,0,63,61,1,0,0,0,63,64,1,0,0,0,64,66,
  	1,0,0,0,65,63,1,0,0,0,66,67,5,3,0,0,67,68,5,7,0,0,68,7,1,0,0,0,69,70,
  	5,17,0,0,70,80,5,17,0,0,71,72,5,9,0,0,72,73,3,10,5,0,73,78,5,10,0,0,74,
  	75,5,9,0,0,75,76,3,10,5,0,76,77,5,10,0,0,77,79,1,0,0,0,78,74,1,0,0,0,
  	78,79,1,0,0,0,79,81,1,0,0,0,80,71,1,0,0,0,80,81,1,0,0,0,81,82,1,0,0,0,
  	82,83,5,7,0,0,83,9,1,0,0,0,84,85,7,1,0,0,85,11,1,0,0,0,10,15,26,28,39,
  	45,49,52,63,78,80
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
    setState(15);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == ddlParser::T__0) {
      setState(12);
      buffer_data();
      setState(17);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(18);
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
    setState(20);
    match(ddlParser::T__0);
    setState(21);
    number();
    setState(22);
    match(ddlParser::T__1);
    setState(28);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 131344) != 0)) {
      setState(26);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case ddlParser::T__3: {
          setState(23);
          enum_();
          break;
        }

        case ddlParser::T__7: {
          setState(24);
          struct_();
          break;
        }

        case ddlParser::IDENTIFIER: {
          setState(25);
          struct_def();
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(30);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(31);
    match(ddlParser::T__2);
   
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

std::vector<tree::TerminalNode *> ddlParser::EnumContext::STRING() {
  return getTokens(ddlParser::STRING);
}

tree::TerminalNode* ddlParser::EnumContext::STRING(size_t i) {
  return getToken(ddlParser::STRING, i);
}

std::vector<ddlParser::NumberContext *> ddlParser::EnumContext::number() {
  return getRuleContexts<ddlParser::NumberContext>();
}

ddlParser::NumberContext* ddlParser::EnumContext::number(size_t i) {
  return getRuleContext<ddlParser::NumberContext>(i);
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
  enterRule(_localctx, 4, ddlParser::RuleEnum);
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
    setState(33);
    match(ddlParser::T__3);
    setState(34);
    match(ddlParser::IDENTIFIER);
    setState(35);
    match(ddlParser::T__1);
    setState(52);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == ddlParser::IDENTIFIER

    || _la == ddlParser::STRING) {
      setState(36);
      _la = _input->LA(1);
      if (!(_la == ddlParser::IDENTIFIER

      || _la == ddlParser::STRING)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(39);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == ddlParser::T__4) {
        setState(37);
        match(ddlParser::T__4);
        setState(38);
        number();
      }
      setState(49);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == ddlParser::T__5) {
        setState(41);
        match(ddlParser::T__5);
        setState(42);
        _la = _input->LA(1);
        if (!(_la == ddlParser::IDENTIFIER

        || _la == ddlParser::STRING)) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(45);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == ddlParser::T__4) {
          setState(43);
          match(ddlParser::T__4);
          setState(44);
          number();
        }
        setState(51);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
    }
    setState(54);
    match(ddlParser::T__2);
    setState(55);
    match(ddlParser::T__6);
   
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
  enterRule(_localctx, 6, ddlParser::RuleStruct);
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
    setState(57);
    match(ddlParser::T__7);
    setState(58);
    match(ddlParser::IDENTIFIER);
    setState(59);
    match(ddlParser::T__1);
    setState(63);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == ddlParser::IDENTIFIER) {
      setState(60);
      struct_def();
      setState(65);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(66);
    match(ddlParser::T__2);
    setState(67);
    match(ddlParser::T__6);
   
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

std::vector<tree::TerminalNode *> ddlParser::Struct_defContext::IDENTIFIER() {
  return getTokens(ddlParser::IDENTIFIER);
}

tree::TerminalNode* ddlParser::Struct_defContext::IDENTIFIER(size_t i) {
  return getToken(ddlParser::IDENTIFIER, i);
}

std::vector<ddlParser::NumberContext *> ddlParser::Struct_defContext::number() {
  return getRuleContexts<ddlParser::NumberContext>();
}

ddlParser::NumberContext* ddlParser::Struct_defContext::number(size_t i) {
  return getRuleContext<ddlParser::NumberContext>(i);
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
  enterRule(_localctx, 8, ddlParser::RuleStruct_def);
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
    setState(69);
    match(ddlParser::IDENTIFIER);
    setState(70);
    match(ddlParser::IDENTIFIER);
    setState(80);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == ddlParser::T__8) {
      setState(71);
      match(ddlParser::T__8);
      setState(72);
      number();
      setState(73);
      match(ddlParser::T__9);
      setState(78);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == ddlParser::T__8) {
        setState(74);
        match(ddlParser::T__8);
        setState(75);
        number();
        setState(76);
        match(ddlParser::T__9);
      }
    }
    setState(82);
    match(ddlParser::T__6);
   
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
  enterRule(_localctx, 10, ddlParser::RuleNumber);
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
    setState(84);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 122880) != 0))) {
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
