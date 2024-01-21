
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
      "prog", "begin", "version", "metatable", "enum", "struct", "struct_def", 
      "number"
    },
    std::vector<std::string>{
      "", "';'", "'begin'", "'version'", "'metatable'", "'enum'", "'{'", 
      "'='", "','", "'}'", "'struct'", "'['", "']'"
    },
    std::vector<std::string>{
      "", "", "", "", "", "", "", "", "", "", "", "", "", "NEWLINE", "WHITESPACE", 
      "INTEGER10", "INTEGER16", "INTEGER8", "INTEGER2", "IDENTIFIER", "PATH", 
      "STRING"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,21,93,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,7,
  	7,7,1,0,1,0,1,0,1,0,1,0,3,0,22,8,0,1,0,1,0,5,0,26,8,0,10,0,12,0,29,9,
  	0,1,0,1,0,1,1,1,1,1,1,1,2,1,2,1,2,1,3,1,3,1,3,1,4,1,4,1,4,1,4,1,4,1,4,
  	3,4,48,8,4,1,4,1,4,1,4,1,4,3,4,54,8,4,5,4,56,8,4,10,4,12,4,59,9,4,3,4,
  	61,8,4,1,4,1,4,1,5,1,5,1,5,1,5,5,5,69,8,5,10,5,12,5,72,9,5,1,5,1,5,1,
  	6,1,6,1,6,1,6,1,6,1,6,1,6,1,6,1,6,3,6,85,8,6,3,6,87,8,6,1,6,1,6,1,7,1,
  	7,1,7,0,0,8,0,2,4,6,8,10,12,14,0,1,1,0,15,18,96,0,27,1,0,0,0,2,32,1,0,
  	0,0,4,35,1,0,0,0,6,38,1,0,0,0,8,41,1,0,0,0,10,64,1,0,0,0,12,75,1,0,0,
  	0,14,90,1,0,0,0,16,22,3,2,1,0,17,22,3,4,2,0,18,22,3,6,3,0,19,22,3,10,
  	5,0,20,22,3,8,4,0,21,16,1,0,0,0,21,17,1,0,0,0,21,18,1,0,0,0,21,19,1,0,
  	0,0,21,20,1,0,0,0,22,23,1,0,0,0,23,24,5,1,0,0,24,26,1,0,0,0,25,21,1,0,
  	0,0,26,29,1,0,0,0,27,25,1,0,0,0,27,28,1,0,0,0,28,30,1,0,0,0,29,27,1,0,
  	0,0,30,31,5,0,0,1,31,1,1,0,0,0,32,33,5,2,0,0,33,34,5,21,0,0,34,3,1,0,
  	0,0,35,36,5,3,0,0,36,37,3,14,7,0,37,5,1,0,0,0,38,39,5,4,0,0,39,40,5,21,
  	0,0,40,7,1,0,0,0,41,42,5,5,0,0,42,43,5,19,0,0,43,60,5,6,0,0,44,47,5,21,
  	0,0,45,46,5,7,0,0,46,48,3,14,7,0,47,45,1,0,0,0,47,48,1,0,0,0,48,57,1,
  	0,0,0,49,50,5,8,0,0,50,53,5,21,0,0,51,52,5,7,0,0,52,54,3,14,7,0,53,51,
  	1,0,0,0,53,54,1,0,0,0,54,56,1,0,0,0,55,49,1,0,0,0,56,59,1,0,0,0,57,55,
  	1,0,0,0,57,58,1,0,0,0,58,61,1,0,0,0,59,57,1,0,0,0,60,44,1,0,0,0,60,61,
  	1,0,0,0,61,62,1,0,0,0,62,63,5,9,0,0,63,9,1,0,0,0,64,65,5,10,0,0,65,66,
  	5,19,0,0,66,70,5,6,0,0,67,69,3,12,6,0,68,67,1,0,0,0,69,72,1,0,0,0,70,
  	68,1,0,0,0,70,71,1,0,0,0,71,73,1,0,0,0,72,70,1,0,0,0,73,74,5,9,0,0,74,
  	11,1,0,0,0,75,76,5,19,0,0,76,86,5,19,0,0,77,78,5,11,0,0,78,79,3,14,7,
  	0,79,84,5,12,0,0,80,81,5,11,0,0,81,82,3,14,7,0,82,83,5,12,0,0,83,85,1,
  	0,0,0,84,80,1,0,0,0,84,85,1,0,0,0,85,87,1,0,0,0,86,77,1,0,0,0,86,87,1,
  	0,0,0,87,88,1,0,0,0,88,89,5,1,0,0,89,13,1,0,0,0,90,91,7,0,0,0,91,15,1,
  	0,0,0,9,21,27,47,53,57,60,70,84,86
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

std::vector<ddlParser::BeginContext *> ddlParser::ProgContext::begin() {
  return getRuleContexts<ddlParser::BeginContext>();
}

ddlParser::BeginContext* ddlParser::ProgContext::begin(size_t i) {
  return getRuleContext<ddlParser::BeginContext>(i);
}

std::vector<ddlParser::VersionContext *> ddlParser::ProgContext::version() {
  return getRuleContexts<ddlParser::VersionContext>();
}

ddlParser::VersionContext* ddlParser::ProgContext::version(size_t i) {
  return getRuleContext<ddlParser::VersionContext>(i);
}

std::vector<ddlParser::MetatableContext *> ddlParser::ProgContext::metatable() {
  return getRuleContexts<ddlParser::MetatableContext>();
}

ddlParser::MetatableContext* ddlParser::ProgContext::metatable(size_t i) {
  return getRuleContext<ddlParser::MetatableContext>(i);
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
    setState(27);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 1084) != 0)) {
      setState(21);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case ddlParser::T__1: {
          setState(16);
          begin();
          break;
        }

        case ddlParser::T__2: {
          setState(17);
          version();
          break;
        }

        case ddlParser::T__3: {
          setState(18);
          metatable();
          break;
        }

        case ddlParser::T__9: {
          setState(19);
          struct_();
          break;
        }

        case ddlParser::T__4: {
          setState(20);
          enum_();
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(23);
      match(ddlParser::T__0);
      setState(29);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(30);
    match(ddlParser::EOF);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BeginContext ------------------------------------------------------------------

ddlParser::BeginContext::BeginContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* ddlParser::BeginContext::STRING() {
  return getToken(ddlParser::STRING, 0);
}


size_t ddlParser::BeginContext::getRuleIndex() const {
  return ddlParser::RuleBegin;
}


std::any ddlParser::BeginContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ddlVisitor*>(visitor))
    return parserVisitor->visitBegin(this);
  else
    return visitor->visitChildren(this);
}

ddlParser::BeginContext* ddlParser::begin() {
  BeginContext *_localctx = _tracker.createInstance<BeginContext>(_ctx, getState());
  enterRule(_localctx, 2, ddlParser::RuleBegin);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(32);
    match(ddlParser::T__1);
    setState(33);
    match(ddlParser::STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- VersionContext ------------------------------------------------------------------

ddlParser::VersionContext::VersionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

ddlParser::NumberContext* ddlParser::VersionContext::number() {
  return getRuleContext<ddlParser::NumberContext>(0);
}


size_t ddlParser::VersionContext::getRuleIndex() const {
  return ddlParser::RuleVersion;
}


std::any ddlParser::VersionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ddlVisitor*>(visitor))
    return parserVisitor->visitVersion(this);
  else
    return visitor->visitChildren(this);
}

ddlParser::VersionContext* ddlParser::version() {
  VersionContext *_localctx = _tracker.createInstance<VersionContext>(_ctx, getState());
  enterRule(_localctx, 4, ddlParser::RuleVersion);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(35);
    match(ddlParser::T__2);
    setState(36);
    number();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- MetatableContext ------------------------------------------------------------------

ddlParser::MetatableContext::MetatableContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* ddlParser::MetatableContext::STRING() {
  return getToken(ddlParser::STRING, 0);
}


size_t ddlParser::MetatableContext::getRuleIndex() const {
  return ddlParser::RuleMetatable;
}


std::any ddlParser::MetatableContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ddlVisitor*>(visitor))
    return parserVisitor->visitMetatable(this);
  else
    return visitor->visitChildren(this);
}

ddlParser::MetatableContext* ddlParser::metatable() {
  MetatableContext *_localctx = _tracker.createInstance<MetatableContext>(_ctx, getState());
  enterRule(_localctx, 6, ddlParser::RuleMetatable);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(38);
    match(ddlParser::T__3);
    setState(39);
    match(ddlParser::STRING);
   
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
  enterRule(_localctx, 8, ddlParser::RuleEnum);
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
    setState(41);
    match(ddlParser::T__4);
    setState(42);
    match(ddlParser::IDENTIFIER);
    setState(43);
    match(ddlParser::T__5);
    setState(60);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == ddlParser::STRING) {
      setState(44);
      match(ddlParser::STRING);
      setState(47);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == ddlParser::T__6) {
        setState(45);
        match(ddlParser::T__6);
        setState(46);
        number();
      }
      setState(57);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == ddlParser::T__7) {
        setState(49);
        match(ddlParser::T__7);
        setState(50);
        match(ddlParser::STRING);
        setState(53);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == ddlParser::T__6) {
          setState(51);
          match(ddlParser::T__6);
          setState(52);
          number();
        }
        setState(59);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
    }
    setState(62);
    match(ddlParser::T__8);
   
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
  enterRule(_localctx, 10, ddlParser::RuleStruct);
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
    setState(64);
    match(ddlParser::T__9);
    setState(65);
    match(ddlParser::IDENTIFIER);
    setState(66);
    match(ddlParser::T__5);
    setState(70);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == ddlParser::IDENTIFIER) {
      setState(67);
      struct_def();
      setState(72);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(73);
    match(ddlParser::T__8);
   
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
  enterRule(_localctx, 12, ddlParser::RuleStruct_def);
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
    setState(75);
    match(ddlParser::IDENTIFIER);
    setState(76);
    match(ddlParser::IDENTIFIER);
    setState(86);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == ddlParser::T__10) {
      setState(77);
      match(ddlParser::T__10);
      setState(78);
      number();
      setState(79);
      match(ddlParser::T__11);
      setState(84);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == ddlParser::T__10) {
        setState(80);
        match(ddlParser::T__10);
        setState(81);
        number();
        setState(82);
        match(ddlParser::T__11);
      }
    }
    setState(88);
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
    setState(90);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 491520) != 0))) {
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
