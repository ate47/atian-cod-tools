
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
      "prog", "data", "enum", "struct", "struct_def", "number"
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
  	4,1,18,83,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,1,0,1,0,1,0,
  	3,0,16,8,0,1,0,1,0,5,0,20,8,0,10,0,12,0,23,9,0,1,0,1,0,1,1,1,1,1,1,3,
  	1,30,8,1,1,2,1,2,1,2,1,2,1,2,1,2,3,2,38,8,2,1,2,1,2,1,2,1,2,3,2,44,8,
  	2,5,2,46,8,2,10,2,12,2,49,9,2,3,2,51,8,2,1,2,1,2,1,3,1,3,1,3,1,3,5,3,
  	59,8,3,10,3,12,3,62,9,3,1,3,1,3,1,4,1,4,1,4,1,4,1,4,1,4,1,4,1,4,1,4,3,
  	4,75,8,4,3,4,77,8,4,1,4,1,4,1,5,1,5,1,5,0,0,6,0,2,4,6,8,10,0,1,1,0,12,
  	15,87,0,21,1,0,0,0,2,26,1,0,0,0,4,31,1,0,0,0,6,54,1,0,0,0,8,65,1,0,0,
  	0,10,80,1,0,0,0,12,16,3,2,1,0,13,16,3,6,3,0,14,16,3,4,2,0,15,12,1,0,0,
  	0,15,13,1,0,0,0,15,14,1,0,0,0,16,17,1,0,0,0,17,18,5,1,0,0,18,20,1,0,0,
  	0,19,15,1,0,0,0,20,23,1,0,0,0,21,19,1,0,0,0,21,22,1,0,0,0,22,24,1,0,0,
  	0,23,21,1,0,0,0,24,25,5,0,0,1,25,1,1,0,0,0,26,29,5,16,0,0,27,30,5,18,
  	0,0,28,30,3,10,5,0,29,27,1,0,0,0,29,28,1,0,0,0,30,3,1,0,0,0,31,32,5,2,
  	0,0,32,33,5,16,0,0,33,50,5,3,0,0,34,37,5,18,0,0,35,36,5,4,0,0,36,38,3,
  	10,5,0,37,35,1,0,0,0,37,38,1,0,0,0,38,47,1,0,0,0,39,40,5,5,0,0,40,43,
  	5,18,0,0,41,42,5,4,0,0,42,44,3,10,5,0,43,41,1,0,0,0,43,44,1,0,0,0,44,
  	46,1,0,0,0,45,39,1,0,0,0,46,49,1,0,0,0,47,45,1,0,0,0,47,48,1,0,0,0,48,
  	51,1,0,0,0,49,47,1,0,0,0,50,34,1,0,0,0,50,51,1,0,0,0,51,52,1,0,0,0,52,
  	53,5,6,0,0,53,5,1,0,0,0,54,55,5,7,0,0,55,56,5,16,0,0,56,60,5,3,0,0,57,
  	59,3,8,4,0,58,57,1,0,0,0,59,62,1,0,0,0,60,58,1,0,0,0,60,61,1,0,0,0,61,
  	63,1,0,0,0,62,60,1,0,0,0,63,64,5,6,0,0,64,7,1,0,0,0,65,66,5,16,0,0,66,
  	76,5,16,0,0,67,68,5,8,0,0,68,69,3,10,5,0,69,74,5,9,0,0,70,71,5,8,0,0,
  	71,72,3,10,5,0,72,73,5,9,0,0,73,75,1,0,0,0,74,70,1,0,0,0,74,75,1,0,0,
  	0,75,77,1,0,0,0,76,67,1,0,0,0,76,77,1,0,0,0,77,78,1,0,0,0,78,79,5,1,0,
  	0,79,9,1,0,0,0,80,81,7,0,0,0,81,11,1,0,0,0,10,15,21,29,37,43,47,50,60,
  	74,76
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

std::vector<ddlParser::DataContext *> ddlParser::ProgContext::data() {
  return getRuleContexts<ddlParser::DataContext>();
}

ddlParser::DataContext* ddlParser::ProgContext::data(size_t i) {
  return getRuleContext<ddlParser::DataContext>(i);
}

std::vector<ddlParser::StructContext *> ddlParser::ProgContext::struct_() {
  return getRuleContexts<ddlParser::StructContext>();
}

ddlParser::StructContext* ddlParser::ProgContext::struct_(size_t i) {
  return getRuleContext<ddlParser::StructContext>(i);
}

std::vector<ddlParser::EnumContext *> ddlParser::ProgContext::enum_() {
  return getRuleContexts<ddlParser::EnumContext>();
}

ddlParser::EnumContext* ddlParser::ProgContext::enum_(size_t i) {
  return getRuleContext<ddlParser::EnumContext>(i);
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
    setState(21);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 65668) != 0)) {
      setState(15);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case ddlParser::IDENTIFIER: {
          setState(12);
          data();
          break;
        }

        case ddlParser::T__6: {
          setState(13);
          struct_();
          break;
        }

        case ddlParser::T__1: {
          setState(14);
          enum_();
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(17);
      match(ddlParser::T__0);
      setState(23);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(24);
    match(ddlParser::EOF);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- DataContext ------------------------------------------------------------------

ddlParser::DataContext::DataContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* ddlParser::DataContext::IDENTIFIER() {
  return getToken(ddlParser::IDENTIFIER, 0);
}

tree::TerminalNode* ddlParser::DataContext::STRING() {
  return getToken(ddlParser::STRING, 0);
}

ddlParser::NumberContext* ddlParser::DataContext::number() {
  return getRuleContext<ddlParser::NumberContext>(0);
}


size_t ddlParser::DataContext::getRuleIndex() const {
  return ddlParser::RuleData;
}


std::any ddlParser::DataContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ddlVisitor*>(visitor))
    return parserVisitor->visitData(this);
  else
    return visitor->visitChildren(this);
}

ddlParser::DataContext* ddlParser::data() {
  DataContext *_localctx = _tracker.createInstance<DataContext>(_ctx, getState());
  enterRule(_localctx, 2, ddlParser::RuleData);

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
    match(ddlParser::IDENTIFIER);
    setState(29);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case ddlParser::STRING: {
        setState(27);
        match(ddlParser::STRING);
        break;
      }

      case ddlParser::INTEGER10:
      case ddlParser::INTEGER16:
      case ddlParser::INTEGER8:
      case ddlParser::INTEGER2: {
        setState(28);
        number();
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

//----------------- EnumContext ------------------------------------------------------------------

ddlParser::EnumContext::EnumContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* ddlParser::EnumContext::IDENTIFIER() {
  return getToken(ddlParser::IDENTIFIER, 0);
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
    setState(31);
    match(ddlParser::T__1);
    setState(32);
    match(ddlParser::IDENTIFIER);
    setState(33);
    match(ddlParser::T__2);
    setState(50);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == ddlParser::STRING) {
      setState(34);
      match(ddlParser::STRING);
      setState(37);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == ddlParser::T__3) {
        setState(35);
        match(ddlParser::T__3);
        setState(36);
        number();
      }
      setState(47);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == ddlParser::T__4) {
        setState(39);
        match(ddlParser::T__4);
        setState(40);
        match(ddlParser::STRING);
        setState(43);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == ddlParser::T__3) {
          setState(41);
          match(ddlParser::T__3);
          setState(42);
          number();
        }
        setState(49);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
    }
    setState(52);
    match(ddlParser::T__5);
   
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
    setState(54);
    match(ddlParser::T__6);
    setState(55);
    match(ddlParser::IDENTIFIER);
    setState(56);
    match(ddlParser::T__2);
    setState(60);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == ddlParser::IDENTIFIER) {
      setState(57);
      struct_def();
      setState(62);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(63);
    match(ddlParser::T__5);
   
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
    setState(65);
    match(ddlParser::IDENTIFIER);
    setState(66);
    match(ddlParser::IDENTIFIER);
    setState(76);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == ddlParser::T__7) {
      setState(67);
      match(ddlParser::T__7);
      setState(68);
      number();
      setState(69);
      match(ddlParser::T__8);
      setState(74);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == ddlParser::T__7) {
        setState(70);
        match(ddlParser::T__7);
        setState(71);
        number();
        setState(72);
        match(ddlParser::T__8);
      }
    }
    setState(78);
    match(ddlParser::T__0);
   
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
    setState(80);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 61440) != 0))) {
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
