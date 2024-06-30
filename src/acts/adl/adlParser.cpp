
// Generated from .\grammar\adl.g4 by ANTLR 4.13.0


#include "adlVisitor.h"

#include "adlParser.h"


using namespace antlrcpp;

using namespace antlr4;

namespace {

struct AdlParserStaticData final {
  AdlParserStaticData(std::vector<std::string> ruleNames,
                        std::vector<std::string> literalNames,
                        std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  AdlParserStaticData(const AdlParserStaticData&) = delete;
  AdlParserStaticData(AdlParserStaticData&&) = delete;
  AdlParserStaticData& operator=(const AdlParserStaticData&) = delete;
  AdlParserStaticData& operator=(AdlParserStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

::antlr4::internal::OnceFlag adlParserOnceFlag;
#if ANTLR4_USE_THREAD_LOCAL_CACHE
static thread_local
#endif
AdlParserStaticData *adlParserStaticData = nullptr;

void adlParserInitialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  if (adlParserStaticData != nullptr) {
    return;
  }
#else
  assert(adlParserStaticData == nullptr);
#endif
  auto staticData = std::make_unique<AdlParserStaticData>(
    std::vector<std::string>{
      "prog", "def", "root_def", "include_def", "align_def", "type_def", 
      "struct_def", "flag_def", "enum_def", "customtype_def", "struct_members", 
      "enum_members", "struct_member", "data_member", "data_operator", "enum_member", 
      "idf", "number"
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
  	4,1,32,162,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,7,
  	14,2,15,7,15,2,16,7,16,2,17,7,17,1,0,5,0,38,8,0,10,0,12,0,41,9,0,1,0,
  	1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,1,53,8,1,1,1,3,1,56,8,1,1,2,1,2,
  	1,2,1,3,1,3,1,3,1,4,1,4,1,4,1,5,1,5,1,5,1,5,1,6,1,6,1,6,1,6,1,6,1,6,3,
  	6,77,8,6,1,6,1,6,1,6,1,6,1,6,3,6,84,8,6,1,6,1,6,1,6,1,6,1,6,1,7,1,7,1,
  	7,1,7,1,7,1,7,1,7,1,7,1,8,1,8,1,8,1,8,1,8,1,8,1,8,1,8,1,9,1,9,1,9,1,9,
  	1,10,1,10,1,10,5,10,114,8,10,10,10,12,10,117,9,10,1,11,1,11,1,11,5,11,
  	122,8,11,10,11,12,11,125,9,11,1,11,3,11,128,8,11,3,11,130,8,11,1,12,1,
  	12,3,12,134,8,12,1,13,1,13,3,13,138,8,13,1,13,1,13,1,13,1,13,1,13,3,13,
  	145,8,13,1,14,1,14,1,14,1,15,1,15,3,15,152,8,15,1,15,1,15,3,15,156,8,
  	15,1,16,1,16,1,17,1,17,1,17,0,0,18,0,2,4,6,8,10,12,14,16,18,20,22,24,
  	26,28,30,32,34,0,2,1,0,21,22,1,0,26,29,163,0,39,1,0,0,0,2,52,1,0,0,0,
  	4,57,1,0,0,0,6,60,1,0,0,0,8,63,1,0,0,0,10,66,1,0,0,0,12,70,1,0,0,0,14,
  	90,1,0,0,0,16,98,1,0,0,0,18,106,1,0,0,0,20,115,1,0,0,0,22,129,1,0,0,0,
  	24,133,1,0,0,0,26,135,1,0,0,0,28,146,1,0,0,0,30,151,1,0,0,0,32,157,1,
  	0,0,0,34,159,1,0,0,0,36,38,3,2,1,0,37,36,1,0,0,0,38,41,1,0,0,0,39,37,
  	1,0,0,0,39,40,1,0,0,0,40,42,1,0,0,0,41,39,1,0,0,0,42,43,5,0,0,1,43,1,
  	1,0,0,0,44,53,3,12,6,0,45,53,3,14,7,0,46,53,3,16,8,0,47,53,3,4,2,0,48,
  	53,3,8,4,0,49,53,3,10,5,0,50,53,3,6,3,0,51,53,3,18,9,0,52,44,1,0,0,0,
  	52,45,1,0,0,0,52,46,1,0,0,0,52,47,1,0,0,0,52,48,1,0,0,0,52,49,1,0,0,0,
  	52,50,1,0,0,0,52,51,1,0,0,0,53,55,1,0,0,0,54,56,5,1,0,0,55,54,1,0,0,0,
  	55,56,1,0,0,0,56,3,1,0,0,0,57,58,5,2,0,0,58,59,3,32,16,0,59,5,1,0,0,0,
  	60,61,5,3,0,0,61,62,5,32,0,0,62,7,1,0,0,0,63,64,5,4,0,0,64,65,3,34,17,
  	0,65,9,1,0,0,0,66,67,5,5,0,0,67,68,3,32,16,0,68,69,3,32,16,0,69,11,1,
  	0,0,0,70,76,5,6,0,0,71,72,5,7,0,0,72,73,5,8,0,0,73,74,3,34,17,0,74,75,
  	5,9,0,0,75,77,1,0,0,0,76,71,1,0,0,0,76,77,1,0,0,0,77,83,1,0,0,0,78,79,
  	5,10,0,0,79,80,5,8,0,0,80,81,3,34,17,0,81,82,5,9,0,0,82,84,1,0,0,0,83,
  	78,1,0,0,0,83,84,1,0,0,0,84,85,1,0,0,0,85,86,3,32,16,0,86,87,5,11,0,0,
  	87,88,3,20,10,0,88,89,5,12,0,0,89,13,1,0,0,0,90,91,5,13,0,0,91,92,3,32,
  	16,0,92,93,5,14,0,0,93,94,3,32,16,0,94,95,5,11,0,0,95,96,3,22,11,0,96,
  	97,5,12,0,0,97,15,1,0,0,0,98,99,5,15,0,0,99,100,3,32,16,0,100,101,5,14,
  	0,0,101,102,3,32,16,0,102,103,5,11,0,0,103,104,3,22,11,0,104,105,5,12,
  	0,0,105,17,1,0,0,0,106,107,5,16,0,0,107,108,3,34,17,0,108,109,3,32,16,
  	0,109,19,1,0,0,0,110,111,3,24,12,0,111,112,5,1,0,0,112,114,1,0,0,0,113,
  	110,1,0,0,0,114,117,1,0,0,0,115,113,1,0,0,0,115,116,1,0,0,0,116,21,1,
  	0,0,0,117,115,1,0,0,0,118,123,3,30,15,0,119,120,5,17,0,0,120,122,3,30,
  	15,0,121,119,1,0,0,0,122,125,1,0,0,0,123,121,1,0,0,0,123,124,1,0,0,0,
  	124,127,1,0,0,0,125,123,1,0,0,0,126,128,5,17,0,0,127,126,1,0,0,0,127,
  	128,1,0,0,0,128,130,1,0,0,0,129,118,1,0,0,0,129,130,1,0,0,0,130,23,1,
  	0,0,0,131,134,3,26,13,0,132,134,3,28,14,0,133,131,1,0,0,0,133,132,1,0,
  	0,0,134,25,1,0,0,0,135,137,3,32,16,0,136,138,5,18,0,0,137,136,1,0,0,0,
  	137,138,1,0,0,0,138,139,1,0,0,0,139,144,3,32,16,0,140,141,5,19,0,0,141,
  	142,3,34,17,0,142,143,5,20,0,0,143,145,1,0,0,0,144,140,1,0,0,0,144,145,
  	1,0,0,0,145,27,1,0,0,0,146,147,7,0,0,0,147,148,3,34,17,0,148,29,1,0,0,
  	0,149,152,5,32,0,0,150,152,3,32,16,0,151,149,1,0,0,0,151,150,1,0,0,0,
  	152,155,1,0,0,0,153,154,5,23,0,0,154,156,3,34,17,0,155,153,1,0,0,0,155,
  	156,1,0,0,0,156,31,1,0,0,0,157,158,5,31,0,0,158,33,1,0,0,0,159,160,7,
  	1,0,0,160,35,1,0,0,0,14,39,52,55,76,83,115,123,127,129,133,137,144,151,
  	155
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  adlParserStaticData = staticData.release();
}

}

adlParser::adlParser(TokenStream *input) : adlParser(input, antlr4::atn::ParserATNSimulatorOptions()) {}

adlParser::adlParser(TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options) : Parser(input) {
  adlParser::initialize();
  _interpreter = new atn::ParserATNSimulator(this, *adlParserStaticData->atn, adlParserStaticData->decisionToDFA, adlParserStaticData->sharedContextCache, options);
}

adlParser::~adlParser() {
  delete _interpreter;
}

const atn::ATN& adlParser::getATN() const {
  return *adlParserStaticData->atn;
}

std::string adlParser::getGrammarFileName() const {
  return "adl.g4";
}

const std::vector<std::string>& adlParser::getRuleNames() const {
  return adlParserStaticData->ruleNames;
}

const dfa::Vocabulary& adlParser::getVocabulary() const {
  return adlParserStaticData->vocabulary;
}

antlr4::atn::SerializedATNView adlParser::getSerializedATN() const {
  return adlParserStaticData->serializedATN;
}


//----------------- ProgContext ------------------------------------------------------------------

adlParser::ProgContext::ProgContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* adlParser::ProgContext::EOF() {
  return getToken(adlParser::EOF, 0);
}

std::vector<adlParser::DefContext *> adlParser::ProgContext::def() {
  return getRuleContexts<adlParser::DefContext>();
}

adlParser::DefContext* adlParser::ProgContext::def(size_t i) {
  return getRuleContext<adlParser::DefContext>(i);
}


size_t adlParser::ProgContext::getRuleIndex() const {
  return adlParser::RuleProg;
}


std::any adlParser::ProgContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<adlVisitor*>(visitor))
    return parserVisitor->visitProg(this);
  else
    return visitor->visitChildren(this);
}

adlParser::ProgContext* adlParser::prog() {
  ProgContext *_localctx = _tracker.createInstance<ProgContext>(_ctx, getState());
  enterRule(_localctx, 0, adlParser::RuleProg);
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
    setState(39);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 106620) != 0)) {
      setState(36);
      def();
      setState(41);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(42);
    match(adlParser::EOF);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- DefContext ------------------------------------------------------------------

adlParser::DefContext::DefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

adlParser::Struct_defContext* adlParser::DefContext::struct_def() {
  return getRuleContext<adlParser::Struct_defContext>(0);
}

adlParser::Flag_defContext* adlParser::DefContext::flag_def() {
  return getRuleContext<adlParser::Flag_defContext>(0);
}

adlParser::Enum_defContext* adlParser::DefContext::enum_def() {
  return getRuleContext<adlParser::Enum_defContext>(0);
}

adlParser::Root_defContext* adlParser::DefContext::root_def() {
  return getRuleContext<adlParser::Root_defContext>(0);
}

adlParser::Align_defContext* adlParser::DefContext::align_def() {
  return getRuleContext<adlParser::Align_defContext>(0);
}

adlParser::Type_defContext* adlParser::DefContext::type_def() {
  return getRuleContext<adlParser::Type_defContext>(0);
}

adlParser::Include_defContext* adlParser::DefContext::include_def() {
  return getRuleContext<adlParser::Include_defContext>(0);
}

adlParser::Customtype_defContext* adlParser::DefContext::customtype_def() {
  return getRuleContext<adlParser::Customtype_defContext>(0);
}


size_t adlParser::DefContext::getRuleIndex() const {
  return adlParser::RuleDef;
}


std::any adlParser::DefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<adlVisitor*>(visitor))
    return parserVisitor->visitDef(this);
  else
    return visitor->visitChildren(this);
}

adlParser::DefContext* adlParser::def() {
  DefContext *_localctx = _tracker.createInstance<DefContext>(_ctx, getState());
  enterRule(_localctx, 2, adlParser::RuleDef);
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
    setState(52);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case adlParser::T__5: {
        setState(44);
        struct_def();
        break;
      }

      case adlParser::T__12: {
        setState(45);
        flag_def();
        break;
      }

      case adlParser::T__14: {
        setState(46);
        enum_def();
        break;
      }

      case adlParser::T__1: {
        setState(47);
        root_def();
        break;
      }

      case adlParser::T__3: {
        setState(48);
        align_def();
        break;
      }

      case adlParser::T__4: {
        setState(49);
        type_def();
        break;
      }

      case adlParser::T__2: {
        setState(50);
        include_def();
        break;
      }

      case adlParser::T__15: {
        setState(51);
        customtype_def();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    setState(55);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == adlParser::T__0) {
      setState(54);
      match(adlParser::T__0);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Root_defContext ------------------------------------------------------------------

adlParser::Root_defContext::Root_defContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

adlParser::IdfContext* adlParser::Root_defContext::idf() {
  return getRuleContext<adlParser::IdfContext>(0);
}


size_t adlParser::Root_defContext::getRuleIndex() const {
  return adlParser::RuleRoot_def;
}


std::any adlParser::Root_defContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<adlVisitor*>(visitor))
    return parserVisitor->visitRoot_def(this);
  else
    return visitor->visitChildren(this);
}

adlParser::Root_defContext* adlParser::root_def() {
  Root_defContext *_localctx = _tracker.createInstance<Root_defContext>(_ctx, getState());
  enterRule(_localctx, 4, adlParser::RuleRoot_def);

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
    match(adlParser::T__1);
    setState(58);
    idf();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Include_defContext ------------------------------------------------------------------

adlParser::Include_defContext::Include_defContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* adlParser::Include_defContext::STRING() {
  return getToken(adlParser::STRING, 0);
}


size_t adlParser::Include_defContext::getRuleIndex() const {
  return adlParser::RuleInclude_def;
}


std::any adlParser::Include_defContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<adlVisitor*>(visitor))
    return parserVisitor->visitInclude_def(this);
  else
    return visitor->visitChildren(this);
}

adlParser::Include_defContext* adlParser::include_def() {
  Include_defContext *_localctx = _tracker.createInstance<Include_defContext>(_ctx, getState());
  enterRule(_localctx, 6, adlParser::RuleInclude_def);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(60);
    match(adlParser::T__2);
    setState(61);
    match(adlParser::STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Align_defContext ------------------------------------------------------------------

adlParser::Align_defContext::Align_defContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

adlParser::NumberContext* adlParser::Align_defContext::number() {
  return getRuleContext<adlParser::NumberContext>(0);
}


size_t adlParser::Align_defContext::getRuleIndex() const {
  return adlParser::RuleAlign_def;
}


std::any adlParser::Align_defContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<adlVisitor*>(visitor))
    return parserVisitor->visitAlign_def(this);
  else
    return visitor->visitChildren(this);
}

adlParser::Align_defContext* adlParser::align_def() {
  Align_defContext *_localctx = _tracker.createInstance<Align_defContext>(_ctx, getState());
  enterRule(_localctx, 8, adlParser::RuleAlign_def);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(63);
    match(adlParser::T__3);
    setState(64);
    number();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Type_defContext ------------------------------------------------------------------

adlParser::Type_defContext::Type_defContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<adlParser::IdfContext *> adlParser::Type_defContext::idf() {
  return getRuleContexts<adlParser::IdfContext>();
}

adlParser::IdfContext* adlParser::Type_defContext::idf(size_t i) {
  return getRuleContext<adlParser::IdfContext>(i);
}


size_t adlParser::Type_defContext::getRuleIndex() const {
  return adlParser::RuleType_def;
}


std::any adlParser::Type_defContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<adlVisitor*>(visitor))
    return parserVisitor->visitType_def(this);
  else
    return visitor->visitChildren(this);
}

adlParser::Type_defContext* adlParser::type_def() {
  Type_defContext *_localctx = _tracker.createInstance<Type_defContext>(_ctx, getState());
  enterRule(_localctx, 10, adlParser::RuleType_def);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(66);
    match(adlParser::T__4);
    setState(67);
    idf();
    setState(68);
    idf();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Struct_defContext ------------------------------------------------------------------

adlParser::Struct_defContext::Struct_defContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

adlParser::IdfContext* adlParser::Struct_defContext::idf() {
  return getRuleContext<adlParser::IdfContext>(0);
}

adlParser::Struct_membersContext* adlParser::Struct_defContext::struct_members() {
  return getRuleContext<adlParser::Struct_membersContext>(0);
}

std::vector<adlParser::NumberContext *> adlParser::Struct_defContext::number() {
  return getRuleContexts<adlParser::NumberContext>();
}

adlParser::NumberContext* adlParser::Struct_defContext::number(size_t i) {
  return getRuleContext<adlParser::NumberContext>(i);
}


size_t adlParser::Struct_defContext::getRuleIndex() const {
  return adlParser::RuleStruct_def;
}


std::any adlParser::Struct_defContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<adlVisitor*>(visitor))
    return parserVisitor->visitStruct_def(this);
  else
    return visitor->visitChildren(this);
}

adlParser::Struct_defContext* adlParser::struct_def() {
  Struct_defContext *_localctx = _tracker.createInstance<Struct_defContext>(_ctx, getState());
  enterRule(_localctx, 12, adlParser::RuleStruct_def);
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
    setState(70);
    match(adlParser::T__5);
    setState(76);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == adlParser::T__6) {
      setState(71);
      match(adlParser::T__6);
      setState(72);
      match(adlParser::T__7);
      setState(73);
      number();
      setState(74);
      match(adlParser::T__8);
    }
    setState(83);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == adlParser::T__9) {
      setState(78);
      match(adlParser::T__9);
      setState(79);
      match(adlParser::T__7);
      setState(80);
      number();
      setState(81);
      match(adlParser::T__8);
    }
    setState(85);
    idf();
    setState(86);
    match(adlParser::T__10);
    setState(87);
    struct_members();
    setState(88);
    match(adlParser::T__11);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Flag_defContext ------------------------------------------------------------------

adlParser::Flag_defContext::Flag_defContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<adlParser::IdfContext *> adlParser::Flag_defContext::idf() {
  return getRuleContexts<adlParser::IdfContext>();
}

adlParser::IdfContext* adlParser::Flag_defContext::idf(size_t i) {
  return getRuleContext<adlParser::IdfContext>(i);
}

adlParser::Enum_membersContext* adlParser::Flag_defContext::enum_members() {
  return getRuleContext<adlParser::Enum_membersContext>(0);
}


size_t adlParser::Flag_defContext::getRuleIndex() const {
  return adlParser::RuleFlag_def;
}


std::any adlParser::Flag_defContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<adlVisitor*>(visitor))
    return parserVisitor->visitFlag_def(this);
  else
    return visitor->visitChildren(this);
}

adlParser::Flag_defContext* adlParser::flag_def() {
  Flag_defContext *_localctx = _tracker.createInstance<Flag_defContext>(_ctx, getState());
  enterRule(_localctx, 14, adlParser::RuleFlag_def);

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
    match(adlParser::T__12);
    setState(91);
    idf();
    setState(92);
    match(adlParser::T__13);
    setState(93);
    idf();
    setState(94);
    match(adlParser::T__10);
    setState(95);
    enum_members();
    setState(96);
    match(adlParser::T__11);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Enum_defContext ------------------------------------------------------------------

adlParser::Enum_defContext::Enum_defContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<adlParser::IdfContext *> adlParser::Enum_defContext::idf() {
  return getRuleContexts<adlParser::IdfContext>();
}

adlParser::IdfContext* adlParser::Enum_defContext::idf(size_t i) {
  return getRuleContext<adlParser::IdfContext>(i);
}

adlParser::Enum_membersContext* adlParser::Enum_defContext::enum_members() {
  return getRuleContext<adlParser::Enum_membersContext>(0);
}


size_t adlParser::Enum_defContext::getRuleIndex() const {
  return adlParser::RuleEnum_def;
}


std::any adlParser::Enum_defContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<adlVisitor*>(visitor))
    return parserVisitor->visitEnum_def(this);
  else
    return visitor->visitChildren(this);
}

adlParser::Enum_defContext* adlParser::enum_def() {
  Enum_defContext *_localctx = _tracker.createInstance<Enum_defContext>(_ctx, getState());
  enterRule(_localctx, 16, adlParser::RuleEnum_def);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(98);
    match(adlParser::T__14);
    setState(99);
    idf();
    setState(100);
    match(adlParser::T__13);
    setState(101);
    idf();
    setState(102);
    match(adlParser::T__10);
    setState(103);
    enum_members();
    setState(104);
    match(adlParser::T__11);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Customtype_defContext ------------------------------------------------------------------

adlParser::Customtype_defContext::Customtype_defContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

adlParser::NumberContext* adlParser::Customtype_defContext::number() {
  return getRuleContext<adlParser::NumberContext>(0);
}

adlParser::IdfContext* adlParser::Customtype_defContext::idf() {
  return getRuleContext<adlParser::IdfContext>(0);
}


size_t adlParser::Customtype_defContext::getRuleIndex() const {
  return adlParser::RuleCustomtype_def;
}


std::any adlParser::Customtype_defContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<adlVisitor*>(visitor))
    return parserVisitor->visitCustomtype_def(this);
  else
    return visitor->visitChildren(this);
}

adlParser::Customtype_defContext* adlParser::customtype_def() {
  Customtype_defContext *_localctx = _tracker.createInstance<Customtype_defContext>(_ctx, getState());
  enterRule(_localctx, 18, adlParser::RuleCustomtype_def);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(106);
    match(adlParser::T__15);
    setState(107);
    number();
    setState(108);
    idf();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Struct_membersContext ------------------------------------------------------------------

adlParser::Struct_membersContext::Struct_membersContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<adlParser::Struct_memberContext *> adlParser::Struct_membersContext::struct_member() {
  return getRuleContexts<adlParser::Struct_memberContext>();
}

adlParser::Struct_memberContext* adlParser::Struct_membersContext::struct_member(size_t i) {
  return getRuleContext<adlParser::Struct_memberContext>(i);
}


size_t adlParser::Struct_membersContext::getRuleIndex() const {
  return adlParser::RuleStruct_members;
}


std::any adlParser::Struct_membersContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<adlVisitor*>(visitor))
    return parserVisitor->visitStruct_members(this);
  else
    return visitor->visitChildren(this);
}

adlParser::Struct_membersContext* adlParser::struct_members() {
  Struct_membersContext *_localctx = _tracker.createInstance<Struct_membersContext>(_ctx, getState());
  enterRule(_localctx, 20, adlParser::RuleStruct_members);
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
    setState(115);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 2153775104) != 0)) {
      setState(110);
      struct_member();
      setState(111);
      match(adlParser::T__0);
      setState(117);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Enum_membersContext ------------------------------------------------------------------

adlParser::Enum_membersContext::Enum_membersContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<adlParser::Enum_memberContext *> adlParser::Enum_membersContext::enum_member() {
  return getRuleContexts<adlParser::Enum_memberContext>();
}

adlParser::Enum_memberContext* adlParser::Enum_membersContext::enum_member(size_t i) {
  return getRuleContext<adlParser::Enum_memberContext>(i);
}


size_t adlParser::Enum_membersContext::getRuleIndex() const {
  return adlParser::RuleEnum_members;
}


std::any adlParser::Enum_membersContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<adlVisitor*>(visitor))
    return parserVisitor->visitEnum_members(this);
  else
    return visitor->visitChildren(this);
}

adlParser::Enum_membersContext* adlParser::enum_members() {
  Enum_membersContext *_localctx = _tracker.createInstance<Enum_membersContext>(_ctx, getState());
  enterRule(_localctx, 22, adlParser::RuleEnum_members);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(129);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == adlParser::IDENTIFIER

    || _la == adlParser::STRING) {
      setState(118);
      enum_member();
      setState(123);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 6, _ctx);
      while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
        if (alt == 1) {
          setState(119);
          match(adlParser::T__16);
          setState(120);
          enum_member(); 
        }
        setState(125);
        _errHandler->sync(this);
        alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 6, _ctx);
      }
      setState(127);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == adlParser::T__16) {
        setState(126);
        match(adlParser::T__16);
      }
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Struct_memberContext ------------------------------------------------------------------

adlParser::Struct_memberContext::Struct_memberContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

adlParser::Data_memberContext* adlParser::Struct_memberContext::data_member() {
  return getRuleContext<adlParser::Data_memberContext>(0);
}

adlParser::Data_operatorContext* adlParser::Struct_memberContext::data_operator() {
  return getRuleContext<adlParser::Data_operatorContext>(0);
}


size_t adlParser::Struct_memberContext::getRuleIndex() const {
  return adlParser::RuleStruct_member;
}


std::any adlParser::Struct_memberContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<adlVisitor*>(visitor))
    return parserVisitor->visitStruct_member(this);
  else
    return visitor->visitChildren(this);
}

adlParser::Struct_memberContext* adlParser::struct_member() {
  Struct_memberContext *_localctx = _tracker.createInstance<Struct_memberContext>(_ctx, getState());
  enterRule(_localctx, 24, adlParser::RuleStruct_member);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(133);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case adlParser::IDENTIFIER: {
        enterOuterAlt(_localctx, 1);
        setState(131);
        data_member();
        break;
      }

      case adlParser::T__20:
      case adlParser::T__21: {
        enterOuterAlt(_localctx, 2);
        setState(132);
        data_operator();
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

//----------------- Data_memberContext ------------------------------------------------------------------

adlParser::Data_memberContext::Data_memberContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<adlParser::IdfContext *> adlParser::Data_memberContext::idf() {
  return getRuleContexts<adlParser::IdfContext>();
}

adlParser::IdfContext* adlParser::Data_memberContext::idf(size_t i) {
  return getRuleContext<adlParser::IdfContext>(i);
}

adlParser::NumberContext* adlParser::Data_memberContext::number() {
  return getRuleContext<adlParser::NumberContext>(0);
}


size_t adlParser::Data_memberContext::getRuleIndex() const {
  return adlParser::RuleData_member;
}


std::any adlParser::Data_memberContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<adlVisitor*>(visitor))
    return parserVisitor->visitData_member(this);
  else
    return visitor->visitChildren(this);
}

adlParser::Data_memberContext* adlParser::data_member() {
  Data_memberContext *_localctx = _tracker.createInstance<Data_memberContext>(_ctx, getState());
  enterRule(_localctx, 26, adlParser::RuleData_member);
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
    setState(135);
    idf();
    setState(137);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == adlParser::T__17) {
      setState(136);
      match(adlParser::T__17);
    }
    setState(139);
    idf();
    setState(144);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == adlParser::T__18) {
      setState(140);
      match(adlParser::T__18);
      setState(141);
      number();
      setState(142);
      match(adlParser::T__19);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Data_operatorContext ------------------------------------------------------------------

adlParser::Data_operatorContext::Data_operatorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

adlParser::NumberContext* adlParser::Data_operatorContext::number() {
  return getRuleContext<adlParser::NumberContext>(0);
}


size_t adlParser::Data_operatorContext::getRuleIndex() const {
  return adlParser::RuleData_operator;
}


std::any adlParser::Data_operatorContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<adlVisitor*>(visitor))
    return parserVisitor->visitData_operator(this);
  else
    return visitor->visitChildren(this);
}

adlParser::Data_operatorContext* adlParser::data_operator() {
  Data_operatorContext *_localctx = _tracker.createInstance<Data_operatorContext>(_ctx, getState());
  enterRule(_localctx, 28, adlParser::RuleData_operator);
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
    setState(146);
    _la = _input->LA(1);
    if (!(_la == adlParser::T__20

    || _la == adlParser::T__21)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(147);
    number();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Enum_memberContext ------------------------------------------------------------------

adlParser::Enum_memberContext::Enum_memberContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* adlParser::Enum_memberContext::STRING() {
  return getToken(adlParser::STRING, 0);
}

adlParser::IdfContext* adlParser::Enum_memberContext::idf() {
  return getRuleContext<adlParser::IdfContext>(0);
}

adlParser::NumberContext* adlParser::Enum_memberContext::number() {
  return getRuleContext<adlParser::NumberContext>(0);
}


size_t adlParser::Enum_memberContext::getRuleIndex() const {
  return adlParser::RuleEnum_member;
}


std::any adlParser::Enum_memberContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<adlVisitor*>(visitor))
    return parserVisitor->visitEnum_member(this);
  else
    return visitor->visitChildren(this);
}

adlParser::Enum_memberContext* adlParser::enum_member() {
  Enum_memberContext *_localctx = _tracker.createInstance<Enum_memberContext>(_ctx, getState());
  enterRule(_localctx, 30, adlParser::RuleEnum_member);
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
    setState(151);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case adlParser::STRING: {
        setState(149);
        match(adlParser::STRING);
        break;
      }

      case adlParser::IDENTIFIER: {
        setState(150);
        idf();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    setState(155);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == adlParser::T__22) {
      setState(153);
      match(adlParser::T__22);
      setState(154);
      number();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- IdfContext ------------------------------------------------------------------

adlParser::IdfContext::IdfContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* adlParser::IdfContext::IDENTIFIER() {
  return getToken(adlParser::IDENTIFIER, 0);
}


size_t adlParser::IdfContext::getRuleIndex() const {
  return adlParser::RuleIdf;
}


std::any adlParser::IdfContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<adlVisitor*>(visitor))
    return parserVisitor->visitIdf(this);
  else
    return visitor->visitChildren(this);
}

adlParser::IdfContext* adlParser::idf() {
  IdfContext *_localctx = _tracker.createInstance<IdfContext>(_ctx, getState());
  enterRule(_localctx, 32, adlParser::RuleIdf);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(157);
    match(adlParser::IDENTIFIER);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- NumberContext ------------------------------------------------------------------

adlParser::NumberContext::NumberContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* adlParser::NumberContext::INTEGER10() {
  return getToken(adlParser::INTEGER10, 0);
}

tree::TerminalNode* adlParser::NumberContext::INTEGER16() {
  return getToken(adlParser::INTEGER16, 0);
}

tree::TerminalNode* adlParser::NumberContext::INTEGER8() {
  return getToken(adlParser::INTEGER8, 0);
}

tree::TerminalNode* adlParser::NumberContext::INTEGER2() {
  return getToken(adlParser::INTEGER2, 0);
}


size_t adlParser::NumberContext::getRuleIndex() const {
  return adlParser::RuleNumber;
}


std::any adlParser::NumberContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<adlVisitor*>(visitor))
    return parserVisitor->visitNumber(this);
  else
    return visitor->visitChildren(this);
}

adlParser::NumberContext* adlParser::number() {
  NumberContext *_localctx = _tracker.createInstance<NumberContext>(_ctx, getState());
  enterRule(_localctx, 34, adlParser::RuleNumber);
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
    setState(159);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 1006632960) != 0))) {
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

void adlParser::initialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  adlParserInitialize();
#else
  ::antlr4::internal::call_once(adlParserOnceFlag, adlParserInitialize);
#endif
}
