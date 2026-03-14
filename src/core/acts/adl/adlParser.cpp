// precompiled header
#include <includes.hpp>


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
      "number", "number_op1", "number_op2", "number_op3", "number_op4", 
      "number_op5", "number_op6", "idf", "number_raw"
    },
    std::vector<std::string>{
      "", "';'", "'#root'", "'#include'", "'#align'", "'typedef'", "'struct'", 
      "'align'", "'('", "')'", "'size'", "'{'", "'}'", "'flag'", "':'", 
      "'enum'", "'customtype'", "','", "'*'", "'['", "']'", "'$padding'", 
      "'$assert_offset'", "'='", "'|'", "'^'", "'&'", "'<<'", "'>>'", "'+'", 
      "'-'", "'/'", "'%'", "'~'"
    },
    std::vector<std::string>{
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
      "NEWLINE", "WHITESPACE", "INTEGER10", "INTEGER16", "INTEGER8", "INTEGER2", 
      "BOOL_VALUE", "IDENTIFIER", "STRING"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,42,251,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,7,
  	14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,7,
  	21,2,22,7,22,2,23,7,23,2,24,7,24,1,0,5,0,52,8,0,10,0,12,0,55,9,0,1,0,
  	1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,1,67,8,1,1,1,3,1,70,8,1,1,2,1,2,
  	1,2,1,3,1,3,1,3,1,4,1,4,1,4,1,5,1,5,1,5,1,5,1,6,1,6,1,6,1,6,1,6,1,6,3,
  	6,91,8,6,1,6,1,6,1,6,1,6,1,6,3,6,98,8,6,1,6,1,6,1,6,1,6,1,6,1,7,1,7,1,
  	7,1,7,1,7,1,7,1,7,1,7,1,8,1,8,1,8,1,8,1,8,1,8,1,8,1,8,1,9,1,9,1,9,1,9,
  	1,10,1,10,1,10,5,10,128,8,10,10,10,12,10,131,9,10,1,11,1,11,1,11,5,11,
  	136,8,11,10,11,12,11,139,9,11,1,11,3,11,142,8,11,3,11,144,8,11,1,12,1,
  	12,3,12,148,8,12,1,13,1,13,3,13,152,8,13,1,13,1,13,1,13,1,13,1,13,3,13,
  	159,8,13,1,14,1,14,1,14,1,15,1,15,3,15,166,8,15,1,15,1,15,3,15,170,8,
  	15,1,16,1,16,1,16,1,16,1,16,1,16,5,16,178,8,16,10,16,12,16,181,9,16,1,
  	17,1,17,1,17,1,17,1,17,1,17,5,17,189,8,17,10,17,12,17,192,9,17,1,18,1,
  	18,1,18,1,18,1,18,1,18,5,18,200,8,18,10,18,12,18,203,9,18,1,19,1,19,1,
  	19,1,19,1,19,1,19,5,19,211,8,19,10,19,12,19,214,9,19,1,20,1,20,1,20,1,
  	20,1,20,1,20,5,20,222,8,20,10,20,12,20,225,9,20,1,21,1,21,1,21,1,21,1,
  	21,1,21,5,21,233,8,21,10,21,12,21,236,9,21,1,22,1,22,1,22,1,22,1,22,1,
  	22,1,22,3,22,245,8,22,1,23,1,23,1,24,1,24,1,24,0,6,32,34,36,38,40,42,
  	25,0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,38,40,42,44,46,
  	48,0,5,1,0,21,22,1,0,27,28,1,0,29,30,2,0,18,18,31,32,1,0,36,39,253,0,
  	53,1,0,0,0,2,66,1,0,0,0,4,71,1,0,0,0,6,74,1,0,0,0,8,77,1,0,0,0,10,80,
  	1,0,0,0,12,84,1,0,0,0,14,104,1,0,0,0,16,112,1,0,0,0,18,120,1,0,0,0,20,
  	129,1,0,0,0,22,143,1,0,0,0,24,147,1,0,0,0,26,149,1,0,0,0,28,160,1,0,0,
  	0,30,165,1,0,0,0,32,171,1,0,0,0,34,182,1,0,0,0,36,193,1,0,0,0,38,204,
  	1,0,0,0,40,215,1,0,0,0,42,226,1,0,0,0,44,244,1,0,0,0,46,246,1,0,0,0,48,
  	248,1,0,0,0,50,52,3,2,1,0,51,50,1,0,0,0,52,55,1,0,0,0,53,51,1,0,0,0,53,
  	54,1,0,0,0,54,56,1,0,0,0,55,53,1,0,0,0,56,57,5,0,0,1,57,1,1,0,0,0,58,
  	67,3,12,6,0,59,67,3,14,7,0,60,67,3,16,8,0,61,67,3,4,2,0,62,67,3,8,4,0,
  	63,67,3,10,5,0,64,67,3,6,3,0,65,67,3,18,9,0,66,58,1,0,0,0,66,59,1,0,0,
  	0,66,60,1,0,0,0,66,61,1,0,0,0,66,62,1,0,0,0,66,63,1,0,0,0,66,64,1,0,0,
  	0,66,65,1,0,0,0,67,69,1,0,0,0,68,70,5,1,0,0,69,68,1,0,0,0,69,70,1,0,0,
  	0,70,3,1,0,0,0,71,72,5,2,0,0,72,73,3,46,23,0,73,5,1,0,0,0,74,75,5,3,0,
  	0,75,76,5,42,0,0,76,7,1,0,0,0,77,78,5,4,0,0,78,79,3,32,16,0,79,9,1,0,
  	0,0,80,81,5,5,0,0,81,82,3,46,23,0,82,83,3,46,23,0,83,11,1,0,0,0,84,90,
  	5,6,0,0,85,86,5,7,0,0,86,87,5,8,0,0,87,88,3,32,16,0,88,89,5,9,0,0,89,
  	91,1,0,0,0,90,85,1,0,0,0,90,91,1,0,0,0,91,97,1,0,0,0,92,93,5,10,0,0,93,
  	94,5,8,0,0,94,95,3,32,16,0,95,96,5,9,0,0,96,98,1,0,0,0,97,92,1,0,0,0,
  	97,98,1,0,0,0,98,99,1,0,0,0,99,100,3,46,23,0,100,101,5,11,0,0,101,102,
  	3,20,10,0,102,103,5,12,0,0,103,13,1,0,0,0,104,105,5,13,0,0,105,106,3,
  	46,23,0,106,107,5,14,0,0,107,108,3,46,23,0,108,109,5,11,0,0,109,110,3,
  	22,11,0,110,111,5,12,0,0,111,15,1,0,0,0,112,113,5,15,0,0,113,114,3,46,
  	23,0,114,115,5,14,0,0,115,116,3,46,23,0,116,117,5,11,0,0,117,118,3,22,
  	11,0,118,119,5,12,0,0,119,17,1,0,0,0,120,121,5,16,0,0,121,122,3,32,16,
  	0,122,123,3,46,23,0,123,19,1,0,0,0,124,125,3,24,12,0,125,126,5,1,0,0,
  	126,128,1,0,0,0,127,124,1,0,0,0,128,131,1,0,0,0,129,127,1,0,0,0,129,130,
  	1,0,0,0,130,21,1,0,0,0,131,129,1,0,0,0,132,137,3,30,15,0,133,134,5,17,
  	0,0,134,136,3,30,15,0,135,133,1,0,0,0,136,139,1,0,0,0,137,135,1,0,0,0,
  	137,138,1,0,0,0,138,141,1,0,0,0,139,137,1,0,0,0,140,142,5,17,0,0,141,
  	140,1,0,0,0,141,142,1,0,0,0,142,144,1,0,0,0,143,132,1,0,0,0,143,144,1,
  	0,0,0,144,23,1,0,0,0,145,148,3,26,13,0,146,148,3,28,14,0,147,145,1,0,
  	0,0,147,146,1,0,0,0,148,25,1,0,0,0,149,151,3,46,23,0,150,152,5,18,0,0,
  	151,150,1,0,0,0,151,152,1,0,0,0,152,153,1,0,0,0,153,158,3,46,23,0,154,
  	155,5,19,0,0,155,156,3,32,16,0,156,157,5,20,0,0,157,159,1,0,0,0,158,154,
  	1,0,0,0,158,159,1,0,0,0,159,27,1,0,0,0,160,161,7,0,0,0,161,162,3,32,16,
  	0,162,29,1,0,0,0,163,166,5,42,0,0,164,166,3,46,23,0,165,163,1,0,0,0,165,
  	164,1,0,0,0,166,169,1,0,0,0,167,168,5,23,0,0,168,170,3,32,16,0,169,167,
  	1,0,0,0,169,170,1,0,0,0,170,31,1,0,0,0,171,172,6,16,-1,0,172,173,3,34,
  	17,0,173,179,1,0,0,0,174,175,10,2,0,0,175,176,5,24,0,0,176,178,3,34,17,
  	0,177,174,1,0,0,0,178,181,1,0,0,0,179,177,1,0,0,0,179,180,1,0,0,0,180,
  	33,1,0,0,0,181,179,1,0,0,0,182,183,6,17,-1,0,183,184,3,36,18,0,184,190,
  	1,0,0,0,185,186,10,2,0,0,186,187,5,25,0,0,187,189,3,36,18,0,188,185,1,
  	0,0,0,189,192,1,0,0,0,190,188,1,0,0,0,190,191,1,0,0,0,191,35,1,0,0,0,
  	192,190,1,0,0,0,193,194,6,18,-1,0,194,195,3,38,19,0,195,201,1,0,0,0,196,
  	197,10,2,0,0,197,198,5,26,0,0,198,200,3,38,19,0,199,196,1,0,0,0,200,203,
  	1,0,0,0,201,199,1,0,0,0,201,202,1,0,0,0,202,37,1,0,0,0,203,201,1,0,0,
  	0,204,205,6,19,-1,0,205,206,3,40,20,0,206,212,1,0,0,0,207,208,10,2,0,
  	0,208,209,7,1,0,0,209,211,3,40,20,0,210,207,1,0,0,0,211,214,1,0,0,0,212,
  	210,1,0,0,0,212,213,1,0,0,0,213,39,1,0,0,0,214,212,1,0,0,0,215,216,6,
  	20,-1,0,216,217,3,42,21,0,217,223,1,0,0,0,218,219,10,2,0,0,219,220,7,
  	2,0,0,220,222,3,42,21,0,221,218,1,0,0,0,222,225,1,0,0,0,223,221,1,0,0,
  	0,223,224,1,0,0,0,224,41,1,0,0,0,225,223,1,0,0,0,226,227,6,21,-1,0,227,
  	228,3,44,22,0,228,234,1,0,0,0,229,230,10,2,0,0,230,231,7,3,0,0,231,233,
  	3,44,22,0,232,229,1,0,0,0,233,236,1,0,0,0,234,232,1,0,0,0,234,235,1,0,
  	0,0,235,43,1,0,0,0,236,234,1,0,0,0,237,238,5,33,0,0,238,245,3,48,24,0,
  	239,245,3,48,24,0,240,241,5,8,0,0,241,242,3,32,16,0,242,243,5,9,0,0,243,
  	245,1,0,0,0,244,237,1,0,0,0,244,239,1,0,0,0,244,240,1,0,0,0,245,45,1,
  	0,0,0,246,247,5,41,0,0,247,47,1,0,0,0,248,249,7,4,0,0,249,49,1,0,0,0,
  	21,53,66,69,90,97,129,137,141,143,147,151,158,165,169,179,190,201,212,
  	223,234,244
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
    setState(53);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 106620) != 0)) {
      setState(50);
      def();
      setState(55);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(56);
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
    setState(66);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case adlParser::T__5: {
        setState(58);
        struct_def();
        break;
      }

      case adlParser::T__12: {
        setState(59);
        flag_def();
        break;
      }

      case adlParser::T__14: {
        setState(60);
        enum_def();
        break;
      }

      case adlParser::T__1: {
        setState(61);
        root_def();
        break;
      }

      case adlParser::T__3: {
        setState(62);
        align_def();
        break;
      }

      case adlParser::T__4: {
        setState(63);
        type_def();
        break;
      }

      case adlParser::T__2: {
        setState(64);
        include_def();
        break;
      }

      case adlParser::T__15: {
        setState(65);
        customtype_def();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    setState(69);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == adlParser::T__0) {
      setState(68);
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
    setState(71);
    match(adlParser::T__1);
    setState(72);
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
    setState(74);
    match(adlParser::T__2);
    setState(75);
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
    setState(77);
    match(adlParser::T__3);
    setState(78);
    number(0);
   
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
    setState(80);
    match(adlParser::T__4);
    setState(81);
    idf();
    setState(82);
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
    setState(84);
    match(adlParser::T__5);
    setState(90);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == adlParser::T__6) {
      setState(85);
      match(adlParser::T__6);
      setState(86);
      match(adlParser::T__7);
      setState(87);
      number(0);
      setState(88);
      match(adlParser::T__8);
    }
    setState(97);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == adlParser::T__9) {
      setState(92);
      match(adlParser::T__9);
      setState(93);
      match(adlParser::T__7);
      setState(94);
      number(0);
      setState(95);
      match(adlParser::T__8);
    }
    setState(99);
    idf();
    setState(100);
    match(adlParser::T__10);
    setState(101);
    struct_members();
    setState(102);
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
    setState(104);
    match(adlParser::T__12);
    setState(105);
    idf();
    setState(106);
    match(adlParser::T__13);
    setState(107);
    idf();
    setState(108);
    match(adlParser::T__10);
    setState(109);
    enum_members();
    setState(110);
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
    setState(112);
    match(adlParser::T__14);
    setState(113);
    idf();
    setState(114);
    match(adlParser::T__13);
    setState(115);
    idf();
    setState(116);
    match(adlParser::T__10);
    setState(117);
    enum_members();
    setState(118);
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
    setState(120);
    match(adlParser::T__15);
    setState(121);
    number(0);
    setState(122);
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
    setState(129);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 2199029547008) != 0)) {
      setState(124);
      struct_member();

      setState(125);
      match(adlParser::T__0);
      setState(131);
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
    setState(143);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == adlParser::IDENTIFIER

    || _la == adlParser::STRING) {
      setState(132);
      enum_member();
      setState(137);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 6, _ctx);
      while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
        if (alt == 1) {
          setState(133);
          match(adlParser::T__16);
          setState(134);
          enum_member(); 
        }
        setState(139);
        _errHandler->sync(this);
        alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 6, _ctx);
      }
      setState(141);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == adlParser::T__16) {
        setState(140);
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
    setState(147);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case adlParser::IDENTIFIER: {
        enterOuterAlt(_localctx, 1);
        setState(145);
        data_member();
        break;
      }

      case adlParser::T__20:
      case adlParser::T__21: {
        enterOuterAlt(_localctx, 2);
        setState(146);
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
    setState(149);
    idf();
    setState(151);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == adlParser::T__17) {
      setState(150);
      match(adlParser::T__17);
    }
    setState(153);
    idf();
    setState(158);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == adlParser::T__18) {
      setState(154);
      match(adlParser::T__18);
      setState(155);
      number(0);
      setState(156);
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
    setState(160);
    _la = _input->LA(1);
    if (!(_la == adlParser::T__20

    || _la == adlParser::T__21)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(161);
    number(0);
   
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
    setState(165);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case adlParser::STRING: {
        setState(163);
        match(adlParser::STRING);
        break;
      }

      case adlParser::IDENTIFIER: {
        setState(164);
        idf();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    setState(169);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == adlParser::T__22) {
      setState(167);
      match(adlParser::T__22);
      setState(168);
      number(0);
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

adlParser::NumberContext::NumberContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

adlParser::Number_op1Context* adlParser::NumberContext::number_op1() {
  return getRuleContext<adlParser::Number_op1Context>(0);
}

adlParser::NumberContext* adlParser::NumberContext::number() {
  return getRuleContext<adlParser::NumberContext>(0);
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
   return number(0);
}

adlParser::NumberContext* adlParser::number(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  adlParser::NumberContext *_localctx = _tracker.createInstance<NumberContext>(_ctx, parentState);
  adlParser::NumberContext *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 32;
  enterRecursionRule(_localctx, 32, adlParser::RuleNumber, precedence);

    

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(172);
    number_op1(0);
    _ctx->stop = _input->LT(-1);
    setState(179);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 14, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<NumberContext>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleNumber);
        setState(174);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(175);
        match(adlParser::T__23);
        setState(176);
        number_op1(0); 
      }
      setState(181);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 14, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- Number_op1Context ------------------------------------------------------------------

adlParser::Number_op1Context::Number_op1Context(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

adlParser::Number_op2Context* adlParser::Number_op1Context::number_op2() {
  return getRuleContext<adlParser::Number_op2Context>(0);
}

adlParser::Number_op1Context* adlParser::Number_op1Context::number_op1() {
  return getRuleContext<adlParser::Number_op1Context>(0);
}


size_t adlParser::Number_op1Context::getRuleIndex() const {
  return adlParser::RuleNumber_op1;
}


std::any adlParser::Number_op1Context::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<adlVisitor*>(visitor))
    return parserVisitor->visitNumber_op1(this);
  else
    return visitor->visitChildren(this);
}


adlParser::Number_op1Context* adlParser::number_op1() {
   return number_op1(0);
}

adlParser::Number_op1Context* adlParser::number_op1(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  adlParser::Number_op1Context *_localctx = _tracker.createInstance<Number_op1Context>(_ctx, parentState);
  adlParser::Number_op1Context *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 34;
  enterRecursionRule(_localctx, 34, adlParser::RuleNumber_op1, precedence);

    

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(183);
    number_op2(0);
    _ctx->stop = _input->LT(-1);
    setState(190);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 15, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Number_op1Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleNumber_op1);
        setState(185);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(186);
        match(adlParser::T__24);
        setState(187);
        number_op2(0); 
      }
      setState(192);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 15, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- Number_op2Context ------------------------------------------------------------------

adlParser::Number_op2Context::Number_op2Context(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

adlParser::Number_op3Context* adlParser::Number_op2Context::number_op3() {
  return getRuleContext<adlParser::Number_op3Context>(0);
}

adlParser::Number_op2Context* adlParser::Number_op2Context::number_op2() {
  return getRuleContext<adlParser::Number_op2Context>(0);
}


size_t adlParser::Number_op2Context::getRuleIndex() const {
  return adlParser::RuleNumber_op2;
}


std::any adlParser::Number_op2Context::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<adlVisitor*>(visitor))
    return parserVisitor->visitNumber_op2(this);
  else
    return visitor->visitChildren(this);
}


adlParser::Number_op2Context* adlParser::number_op2() {
   return number_op2(0);
}

adlParser::Number_op2Context* adlParser::number_op2(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  adlParser::Number_op2Context *_localctx = _tracker.createInstance<Number_op2Context>(_ctx, parentState);
  adlParser::Number_op2Context *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 36;
  enterRecursionRule(_localctx, 36, adlParser::RuleNumber_op2, precedence);

    

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(194);
    number_op3(0);
    _ctx->stop = _input->LT(-1);
    setState(201);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 16, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Number_op2Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleNumber_op2);
        setState(196);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(197);
        match(adlParser::T__25);
        setState(198);
        number_op3(0); 
      }
      setState(203);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 16, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- Number_op3Context ------------------------------------------------------------------

adlParser::Number_op3Context::Number_op3Context(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

adlParser::Number_op4Context* adlParser::Number_op3Context::number_op4() {
  return getRuleContext<adlParser::Number_op4Context>(0);
}

adlParser::Number_op3Context* adlParser::Number_op3Context::number_op3() {
  return getRuleContext<adlParser::Number_op3Context>(0);
}


size_t adlParser::Number_op3Context::getRuleIndex() const {
  return adlParser::RuleNumber_op3;
}


std::any adlParser::Number_op3Context::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<adlVisitor*>(visitor))
    return parserVisitor->visitNumber_op3(this);
  else
    return visitor->visitChildren(this);
}


adlParser::Number_op3Context* adlParser::number_op3() {
   return number_op3(0);
}

adlParser::Number_op3Context* adlParser::number_op3(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  adlParser::Number_op3Context *_localctx = _tracker.createInstance<Number_op3Context>(_ctx, parentState);
  adlParser::Number_op3Context *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 38;
  enterRecursionRule(_localctx, 38, adlParser::RuleNumber_op3, precedence);

    size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(205);
    number_op4(0);
    _ctx->stop = _input->LT(-1);
    setState(212);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 17, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Number_op3Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleNumber_op3);
        setState(207);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(208);
        _la = _input->LA(1);
        if (!(_la == adlParser::T__26

        || _la == adlParser::T__27)) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(209);
        number_op4(0); 
      }
      setState(214);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 17, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- Number_op4Context ------------------------------------------------------------------

adlParser::Number_op4Context::Number_op4Context(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

adlParser::Number_op5Context* adlParser::Number_op4Context::number_op5() {
  return getRuleContext<adlParser::Number_op5Context>(0);
}

adlParser::Number_op4Context* adlParser::Number_op4Context::number_op4() {
  return getRuleContext<adlParser::Number_op4Context>(0);
}


size_t adlParser::Number_op4Context::getRuleIndex() const {
  return adlParser::RuleNumber_op4;
}


std::any adlParser::Number_op4Context::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<adlVisitor*>(visitor))
    return parserVisitor->visitNumber_op4(this);
  else
    return visitor->visitChildren(this);
}


adlParser::Number_op4Context* adlParser::number_op4() {
   return number_op4(0);
}

adlParser::Number_op4Context* adlParser::number_op4(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  adlParser::Number_op4Context *_localctx = _tracker.createInstance<Number_op4Context>(_ctx, parentState);
  adlParser::Number_op4Context *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 40;
  enterRecursionRule(_localctx, 40, adlParser::RuleNumber_op4, precedence);

    size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(216);
    number_op5(0);
    _ctx->stop = _input->LT(-1);
    setState(223);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 18, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Number_op4Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleNumber_op4);
        setState(218);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(219);
        _la = _input->LA(1);
        if (!(_la == adlParser::T__28

        || _la == adlParser::T__29)) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(220);
        number_op5(0); 
      }
      setState(225);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 18, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- Number_op5Context ------------------------------------------------------------------

adlParser::Number_op5Context::Number_op5Context(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

adlParser::Number_op6Context* adlParser::Number_op5Context::number_op6() {
  return getRuleContext<adlParser::Number_op6Context>(0);
}

adlParser::Number_op5Context* adlParser::Number_op5Context::number_op5() {
  return getRuleContext<adlParser::Number_op5Context>(0);
}


size_t adlParser::Number_op5Context::getRuleIndex() const {
  return adlParser::RuleNumber_op5;
}


std::any adlParser::Number_op5Context::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<adlVisitor*>(visitor))
    return parserVisitor->visitNumber_op5(this);
  else
    return visitor->visitChildren(this);
}


adlParser::Number_op5Context* adlParser::number_op5() {
   return number_op5(0);
}

adlParser::Number_op5Context* adlParser::number_op5(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  adlParser::Number_op5Context *_localctx = _tracker.createInstance<Number_op5Context>(_ctx, parentState);
  adlParser::Number_op5Context *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 42;
  enterRecursionRule(_localctx, 42, adlParser::RuleNumber_op5, precedence);

    size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(227);
    number_op6();
    _ctx->stop = _input->LT(-1);
    setState(234);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 19, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<Number_op5Context>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleNumber_op5);
        setState(229);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(230);
        _la = _input->LA(1);
        if (!((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & 6442713088) != 0))) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(231);
        number_op6(); 
      }
      setState(236);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 19, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- Number_op6Context ------------------------------------------------------------------

adlParser::Number_op6Context::Number_op6Context(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

adlParser::Number_rawContext* adlParser::Number_op6Context::number_raw() {
  return getRuleContext<adlParser::Number_rawContext>(0);
}

adlParser::NumberContext* adlParser::Number_op6Context::number() {
  return getRuleContext<adlParser::NumberContext>(0);
}


size_t adlParser::Number_op6Context::getRuleIndex() const {
  return adlParser::RuleNumber_op6;
}


std::any adlParser::Number_op6Context::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<adlVisitor*>(visitor))
    return parserVisitor->visitNumber_op6(this);
  else
    return visitor->visitChildren(this);
}

adlParser::Number_op6Context* adlParser::number_op6() {
  Number_op6Context *_localctx = _tracker.createInstance<Number_op6Context>(_ctx, getState());
  enterRule(_localctx, 44, adlParser::RuleNumber_op6);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(244);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case adlParser::T__32: {
        enterOuterAlt(_localctx, 1);
        setState(237);
        match(adlParser::T__32);
        setState(238);
        number_raw();
        break;
      }

      case adlParser::INTEGER10:
      case adlParser::INTEGER16:
      case adlParser::INTEGER8:
      case adlParser::INTEGER2: {
        enterOuterAlt(_localctx, 2);
        setState(239);
        number_raw();
        break;
      }

      case adlParser::T__7: {
        enterOuterAlt(_localctx, 3);
        setState(240);
        match(adlParser::T__7);
        setState(241);
        number(0);
        setState(242);
        match(adlParser::T__8);
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
  enterRule(_localctx, 46, adlParser::RuleIdf);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(246);
    match(adlParser::IDENTIFIER);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Number_rawContext ------------------------------------------------------------------

adlParser::Number_rawContext::Number_rawContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* adlParser::Number_rawContext::INTEGER10() {
  return getToken(adlParser::INTEGER10, 0);
}

tree::TerminalNode* adlParser::Number_rawContext::INTEGER16() {
  return getToken(adlParser::INTEGER16, 0);
}

tree::TerminalNode* adlParser::Number_rawContext::INTEGER8() {
  return getToken(adlParser::INTEGER8, 0);
}

tree::TerminalNode* adlParser::Number_rawContext::INTEGER2() {
  return getToken(adlParser::INTEGER2, 0);
}


size_t adlParser::Number_rawContext::getRuleIndex() const {
  return adlParser::RuleNumber_raw;
}


std::any adlParser::Number_rawContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<adlVisitor*>(visitor))
    return parserVisitor->visitNumber_raw(this);
  else
    return visitor->visitChildren(this);
}

adlParser::Number_rawContext* adlParser::number_raw() {
  Number_rawContext *_localctx = _tracker.createInstance<Number_rawContext>(_ctx, getState());
  enterRule(_localctx, 48, adlParser::RuleNumber_raw);
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
    setState(248);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 1030792151040) != 0))) {
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

bool adlParser::sempred(RuleContext *context, size_t ruleIndex, size_t predicateIndex) {
  switch (ruleIndex) {
    case 16: return numberSempred(antlrcpp::downCast<NumberContext *>(context), predicateIndex);
    case 17: return number_op1Sempred(antlrcpp::downCast<Number_op1Context *>(context), predicateIndex);
    case 18: return number_op2Sempred(antlrcpp::downCast<Number_op2Context *>(context), predicateIndex);
    case 19: return number_op3Sempred(antlrcpp::downCast<Number_op3Context *>(context), predicateIndex);
    case 20: return number_op4Sempred(antlrcpp::downCast<Number_op4Context *>(context), predicateIndex);
    case 21: return number_op5Sempred(antlrcpp::downCast<Number_op5Context *>(context), predicateIndex);

  default:
    break;
  }
  return true;
}

bool adlParser::numberSempred(NumberContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 0: return precpred(_ctx, 2);

  default:
    break;
  }
  return true;
}

bool adlParser::number_op1Sempred(Number_op1Context *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 1: return precpred(_ctx, 2);

  default:
    break;
  }
  return true;
}

bool adlParser::number_op2Sempred(Number_op2Context *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 2: return precpred(_ctx, 2);

  default:
    break;
  }
  return true;
}

bool adlParser::number_op3Sempred(Number_op3Context *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 3: return precpred(_ctx, 2);

  default:
    break;
  }
  return true;
}

bool adlParser::number_op4Sempred(Number_op4Context *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 4: return precpred(_ctx, 2);

  default:
    break;
  }
  return true;
}

bool adlParser::number_op5Sempred(Number_op5Context *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 5: return precpred(_ctx, 2);

  default:
    break;
  }
  return true;
}

void adlParser::initialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  adlParserInitialize();
#else
  ::antlr4::internal::call_once(adlParserOnceFlag, adlParserInitialize);
#endif
}
