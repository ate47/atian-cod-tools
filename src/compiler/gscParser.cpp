
// Generated from .\grammar\gsc.g4 by ANTLR 4.13.0


#include "gscVisitor.h"

#include "gscParser.h"


using namespace antlrcpp;

using namespace antlr4;

namespace {

struct GscParserStaticData final {
  GscParserStaticData(std::vector<std::string> ruleNames,
                        std::vector<std::string> literalNames,
                        std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  GscParserStaticData(const GscParserStaticData&) = delete;
  GscParserStaticData(GscParserStaticData&&) = delete;
  GscParserStaticData& operator=(const GscParserStaticData&) = delete;
  GscParserStaticData& operator=(GscParserStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

::antlr4::internal::OnceFlag gscParserOnceFlag;
#if ANTLR4_USE_THREAD_LOCAL_CACHE
static thread_local
#endif
GscParserStaticData *gscParserStaticData = nullptr;

void gscParserInitialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  if (gscParserStaticData != nullptr) {
    return;
  }
#else
  assert(gscParserStaticData == nullptr);
#endif
  auto staticData = std::make_unique<GscParserStaticData>(
    std::vector<std::string>{
      "prog", "include", "namespace", "function", "param_list", "param_val", 
      "expression_list", "statement_block", "statement", "statement_for", 
      "statement_foreach", "statement_if", "statement_inst", "function_call", 
      "operator_inst", "expression", "const_expr", "function_ref", "number", 
      "vector_value", "array_def", "struct_def"
    },
    std::vector<std::string>{
      "", "'#include'", "';'", "'#namespace'", "'function'", "'private'", 
      "'autoexec'", "'event_handler'", "'['", "']'", "'('", "')'", "','", 
      "'='", "'{'", "'}'", "'for'", "'foreach'", "'in'", "'if'", "'else'", 
      "'thread'", "'childthread'", "'::'", "'&'", "'@'", "'<'", "'>'", "'#'", 
      "':'", "", "", "", "", "", "", "", "", "", "", "", "", "", "'undefined'"
    },
    std::vector<std::string>{
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
      "", "", "", "", "", "", "", "", "", "", "", "", "", "NEWLINE", "WHITESPACE", 
      "INTEGER10", "INTEGER16", "INTEGER8", "INTEGER2", "FLOATVAL", "IDENTIFIER", 
      "STRUCT_IDENTIFIER", "PATH", "STRING", "HASHSTRING", "BOOL_VALUE", 
      "UNDEFINED_VALUE"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,43,262,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,7,
  	14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,7,
  	21,1,0,1,0,1,0,5,0,48,8,0,10,0,12,0,51,9,0,1,0,1,0,1,1,1,1,1,1,1,1,1,
  	2,1,2,1,2,1,2,1,3,3,3,64,8,3,1,3,3,3,67,8,3,1,3,3,3,70,8,3,1,3,1,3,1,
  	3,1,3,3,3,76,8,3,1,3,1,3,1,3,1,3,1,3,1,3,1,4,1,4,1,4,5,4,87,8,4,10,4,
  	12,4,90,9,4,3,4,92,8,4,1,5,1,5,1,5,3,5,97,8,5,1,6,1,6,1,6,5,6,102,8,6,
  	10,6,12,6,105,9,6,3,6,107,8,6,1,7,1,7,5,7,111,8,7,10,7,12,7,114,9,7,1,
  	7,1,7,1,8,1,8,1,8,1,8,1,8,3,8,123,8,8,1,9,1,9,1,9,3,9,128,8,9,1,9,1,9,
  	3,9,132,8,9,1,9,1,9,3,9,136,8,9,1,9,1,9,1,9,1,10,1,10,1,10,1,10,1,10,
  	3,10,146,8,10,1,10,1,10,1,10,1,10,1,10,1,11,1,11,1,11,1,11,1,11,1,11,
  	1,11,3,11,160,8,11,1,12,1,12,3,12,164,8,12,1,12,1,12,1,13,3,13,169,8,
  	13,1,13,3,13,172,8,13,1,13,1,13,3,13,176,8,13,1,13,1,13,1,13,1,13,1,13,
  	1,14,1,14,3,14,185,8,14,1,15,1,15,3,15,189,8,15,1,16,1,16,1,16,1,16,1,
  	16,1,16,1,16,1,16,1,16,1,16,3,16,201,8,16,1,17,1,17,1,17,3,17,206,8,17,
  	1,17,1,17,1,17,1,17,1,17,1,17,1,17,1,17,3,17,216,8,17,1,18,1,18,1,19,
  	1,19,1,19,1,19,1,19,1,19,1,19,1,19,1,20,1,20,1,20,1,20,3,20,232,8,20,
  	1,20,3,20,235,8,20,1,20,1,20,3,20,239,8,20,5,20,241,8,20,10,20,12,20,
  	244,9,20,1,20,1,20,1,21,1,21,1,21,1,21,1,21,3,21,253,8,21,5,21,255,8,
  	21,10,21,12,21,258,9,21,1,21,1,21,1,21,0,0,22,0,2,4,6,8,10,12,14,16,18,
  	20,22,24,26,28,30,32,34,36,38,40,42,0,3,2,0,37,37,39,39,1,0,21,22,1,0,
  	32,35,285,0,49,1,0,0,0,2,54,1,0,0,0,4,58,1,0,0,0,6,63,1,0,0,0,8,91,1,
  	0,0,0,10,93,1,0,0,0,12,106,1,0,0,0,14,108,1,0,0,0,16,122,1,0,0,0,18,124,
  	1,0,0,0,20,140,1,0,0,0,22,152,1,0,0,0,24,163,1,0,0,0,26,168,1,0,0,0,28,
  	182,1,0,0,0,30,188,1,0,0,0,32,200,1,0,0,0,34,215,1,0,0,0,36,217,1,0,0,
  	0,38,219,1,0,0,0,40,227,1,0,0,0,42,247,1,0,0,0,44,48,3,6,3,0,45,48,3,
  	2,1,0,46,48,3,4,2,0,47,44,1,0,0,0,47,45,1,0,0,0,47,46,1,0,0,0,48,51,1,
  	0,0,0,49,47,1,0,0,0,49,50,1,0,0,0,50,52,1,0,0,0,51,49,1,0,0,0,52,53,5,
  	0,0,1,53,1,1,0,0,0,54,55,5,1,0,0,55,56,7,0,0,0,56,57,5,2,0,0,57,3,1,0,
  	0,0,58,59,5,3,0,0,59,60,5,37,0,0,60,61,5,2,0,0,61,5,1,0,0,0,62,64,5,4,
  	0,0,63,62,1,0,0,0,63,64,1,0,0,0,64,66,1,0,0,0,65,67,5,5,0,0,66,65,1,0,
  	0,0,66,67,1,0,0,0,67,69,1,0,0,0,68,70,5,6,0,0,69,68,1,0,0,0,69,70,1,0,
  	0,0,70,75,1,0,0,0,71,72,5,7,0,0,72,73,5,8,0,0,73,74,5,37,0,0,74,76,5,
  	9,0,0,75,71,1,0,0,0,75,76,1,0,0,0,76,77,1,0,0,0,77,78,5,37,0,0,78,79,
  	5,10,0,0,79,80,3,8,4,0,80,81,5,11,0,0,81,82,3,14,7,0,82,7,1,0,0,0,83,
  	88,3,10,5,0,84,85,5,12,0,0,85,87,3,10,5,0,86,84,1,0,0,0,87,90,1,0,0,0,
  	88,86,1,0,0,0,88,89,1,0,0,0,89,92,1,0,0,0,90,88,1,0,0,0,91,83,1,0,0,0,
  	91,92,1,0,0,0,92,9,1,0,0,0,93,96,5,37,0,0,94,95,5,13,0,0,95,97,3,30,15,
  	0,96,94,1,0,0,0,96,97,1,0,0,0,97,11,1,0,0,0,98,103,3,30,15,0,99,100,5,
  	12,0,0,100,102,3,30,15,0,101,99,1,0,0,0,102,105,1,0,0,0,103,101,1,0,0,
  	0,103,104,1,0,0,0,104,107,1,0,0,0,105,103,1,0,0,0,106,98,1,0,0,0,106,
  	107,1,0,0,0,107,13,1,0,0,0,108,112,5,14,0,0,109,111,3,16,8,0,110,109,
  	1,0,0,0,111,114,1,0,0,0,112,110,1,0,0,0,112,113,1,0,0,0,113,115,1,0,0,
  	0,114,112,1,0,0,0,115,116,5,15,0,0,116,15,1,0,0,0,117,123,3,14,7,0,118,
  	123,3,18,9,0,119,123,3,22,11,0,120,123,3,20,10,0,121,123,3,24,12,0,122,
  	117,1,0,0,0,122,118,1,0,0,0,122,119,1,0,0,0,122,120,1,0,0,0,122,121,1,
  	0,0,0,123,17,1,0,0,0,124,125,5,16,0,0,125,127,5,10,0,0,126,128,3,30,15,
  	0,127,126,1,0,0,0,127,128,1,0,0,0,128,129,1,0,0,0,129,131,5,2,0,0,130,
  	132,3,30,15,0,131,130,1,0,0,0,131,132,1,0,0,0,132,133,1,0,0,0,133,135,
  	5,2,0,0,134,136,3,30,15,0,135,134,1,0,0,0,135,136,1,0,0,0,136,137,1,0,
  	0,0,137,138,5,11,0,0,138,139,3,16,8,0,139,19,1,0,0,0,140,141,5,17,0,0,
  	141,142,5,10,0,0,142,145,5,37,0,0,143,144,5,12,0,0,144,146,5,37,0,0,145,
  	143,1,0,0,0,145,146,1,0,0,0,146,147,1,0,0,0,147,148,5,18,0,0,148,149,
  	3,30,15,0,149,150,5,11,0,0,150,151,3,16,8,0,151,21,1,0,0,0,152,153,5,
  	19,0,0,153,154,5,10,0,0,154,155,3,30,15,0,155,156,5,11,0,0,156,159,3,
  	16,8,0,157,158,5,20,0,0,158,160,3,16,8,0,159,157,1,0,0,0,159,160,1,0,
  	0,0,160,23,1,0,0,0,161,164,3,26,13,0,162,164,3,28,14,0,163,161,1,0,0,
  	0,163,162,1,0,0,0,163,164,1,0,0,0,164,165,1,0,0,0,165,166,5,2,0,0,166,
  	25,1,0,0,0,167,169,3,30,15,0,168,167,1,0,0,0,168,169,1,0,0,0,169,171,
  	1,0,0,0,170,172,7,1,0,0,171,170,1,0,0,0,171,172,1,0,0,0,172,175,1,0,0,
  	0,173,174,5,37,0,0,174,176,5,23,0,0,175,173,1,0,0,0,175,176,1,0,0,0,176,
  	177,1,0,0,0,177,178,5,37,0,0,178,179,5,10,0,0,179,180,3,12,6,0,180,181,
  	5,11,0,0,181,27,1,0,0,0,182,184,5,37,0,0,183,185,3,30,15,0,184,183,1,
  	0,0,0,184,185,1,0,0,0,185,29,1,0,0,0,186,189,5,37,0,0,187,189,3,32,16,
  	0,188,186,1,0,0,0,188,187,1,0,0,0,189,31,1,0,0,0,190,201,3,38,19,0,191,
  	201,3,40,20,0,192,201,3,42,21,0,193,201,3,36,18,0,194,201,5,42,0,0,195,
  	201,5,36,0,0,196,201,5,40,0,0,197,201,5,41,0,0,198,201,5,43,0,0,199,201,
  	3,34,17,0,200,190,1,0,0,0,200,191,1,0,0,0,200,192,1,0,0,0,200,193,1,0,
  	0,0,200,194,1,0,0,0,200,195,1,0,0,0,200,196,1,0,0,0,200,197,1,0,0,0,200,
  	198,1,0,0,0,200,199,1,0,0,0,201,33,1,0,0,0,202,205,5,24,0,0,203,204,5,
  	37,0,0,204,206,5,23,0,0,205,203,1,0,0,0,205,206,1,0,0,0,206,207,1,0,0,
  	0,207,216,5,37,0,0,208,209,5,25,0,0,209,210,5,37,0,0,210,211,5,26,0,0,
  	211,212,5,39,0,0,212,213,5,27,0,0,213,214,5,23,0,0,214,216,5,37,0,0,215,
  	202,1,0,0,0,215,208,1,0,0,0,216,35,1,0,0,0,217,218,7,2,0,0,218,37,1,0,
  	0,0,219,220,5,10,0,0,220,221,3,30,15,0,221,222,5,12,0,0,222,223,3,30,
  	15,0,223,224,5,12,0,0,224,225,3,30,15,0,225,226,5,11,0,0,226,39,1,0,0,
  	0,227,242,5,8,0,0,228,232,5,41,0,0,229,230,5,28,0,0,230,232,3,36,18,0,
  	231,228,1,0,0,0,231,229,1,0,0,0,232,233,1,0,0,0,233,235,5,29,0,0,234,
  	231,1,0,0,0,234,235,1,0,0,0,235,236,1,0,0,0,236,238,3,30,15,0,237,239,
  	5,12,0,0,238,237,1,0,0,0,238,239,1,0,0,0,239,241,1,0,0,0,240,234,1,0,
  	0,0,241,244,1,0,0,0,242,240,1,0,0,0,242,243,1,0,0,0,243,245,1,0,0,0,244,
  	242,1,0,0,0,245,246,5,9,0,0,246,41,1,0,0,0,247,256,5,14,0,0,248,249,5,
  	38,0,0,249,250,5,29,0,0,250,252,3,30,15,0,251,253,5,12,0,0,252,251,1,
  	0,0,0,252,253,1,0,0,0,253,255,1,0,0,0,254,248,1,0,0,0,255,258,1,0,0,0,
  	256,254,1,0,0,0,256,257,1,0,0,0,257,259,1,0,0,0,258,256,1,0,0,0,259,260,
  	5,15,0,0,260,43,1,0,0,0,33,47,49,63,66,69,75,88,91,96,103,106,112,122,
  	127,131,135,145,159,163,168,171,175,184,188,200,205,215,231,234,238,242,
  	252,256
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  gscParserStaticData = staticData.release();
}

}

gscParser::gscParser(TokenStream *input) : gscParser(input, antlr4::atn::ParserATNSimulatorOptions()) {}

gscParser::gscParser(TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options) : Parser(input) {
  gscParser::initialize();
  _interpreter = new atn::ParserATNSimulator(this, *gscParserStaticData->atn, gscParserStaticData->decisionToDFA, gscParserStaticData->sharedContextCache, options);
}

gscParser::~gscParser() {
  delete _interpreter;
}

const atn::ATN& gscParser::getATN() const {
  return *gscParserStaticData->atn;
}

std::string gscParser::getGrammarFileName() const {
  return "gsc.g4";
}

const std::vector<std::string>& gscParser::getRuleNames() const {
  return gscParserStaticData->ruleNames;
}

const dfa::Vocabulary& gscParser::getVocabulary() const {
  return gscParserStaticData->vocabulary;
}

antlr4::atn::SerializedATNView gscParser::getSerializedATN() const {
  return gscParserStaticData->serializedATN;
}


//----------------- ProgContext ------------------------------------------------------------------

gscParser::ProgContext::ProgContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* gscParser::ProgContext::EOF() {
  return getToken(gscParser::EOF, 0);
}

std::vector<gscParser::FunctionContext *> gscParser::ProgContext::function() {
  return getRuleContexts<gscParser::FunctionContext>();
}

gscParser::FunctionContext* gscParser::ProgContext::function(size_t i) {
  return getRuleContext<gscParser::FunctionContext>(i);
}

std::vector<gscParser::IncludeContext *> gscParser::ProgContext::include() {
  return getRuleContexts<gscParser::IncludeContext>();
}

gscParser::IncludeContext* gscParser::ProgContext::include(size_t i) {
  return getRuleContext<gscParser::IncludeContext>(i);
}

std::vector<gscParser::NamespaceContext *> gscParser::ProgContext::namespace_() {
  return getRuleContexts<gscParser::NamespaceContext>();
}

gscParser::NamespaceContext* gscParser::ProgContext::namespace_(size_t i) {
  return getRuleContext<gscParser::NamespaceContext>(i);
}


size_t gscParser::ProgContext::getRuleIndex() const {
  return gscParser::RuleProg;
}


std::any gscParser::ProgContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitProg(this);
  else
    return visitor->visitChildren(this);
}

gscParser::ProgContext* gscParser::prog() {
  ProgContext *_localctx = _tracker.createInstance<ProgContext>(_ctx, getState());
  enterRule(_localctx, 0, gscParser::RuleProg);
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
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 137438953722) != 0)) {
      setState(47);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case gscParser::T__3:
        case gscParser::T__4:
        case gscParser::T__5:
        case gscParser::T__6:
        case gscParser::IDENTIFIER: {
          setState(44);
          function();
          break;
        }

        case gscParser::T__0: {
          setState(45);
          include();
          break;
        }

        case gscParser::T__2: {
          setState(46);
          namespace_();
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(51);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(52);
    match(gscParser::EOF);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- IncludeContext ------------------------------------------------------------------

gscParser::IncludeContext::IncludeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* gscParser::IncludeContext::IDENTIFIER() {
  return getToken(gscParser::IDENTIFIER, 0);
}

tree::TerminalNode* gscParser::IncludeContext::PATH() {
  return getToken(gscParser::PATH, 0);
}


size_t gscParser::IncludeContext::getRuleIndex() const {
  return gscParser::RuleInclude;
}


std::any gscParser::IncludeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitInclude(this);
  else
    return visitor->visitChildren(this);
}

gscParser::IncludeContext* gscParser::include() {
  IncludeContext *_localctx = _tracker.createInstance<IncludeContext>(_ctx, getState());
  enterRule(_localctx, 2, gscParser::RuleInclude);
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
    match(gscParser::T__0);
    setState(55);
    _la = _input->LA(1);
    if (!(_la == gscParser::IDENTIFIER

    || _la == gscParser::PATH)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(56);
    match(gscParser::T__1);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- NamespaceContext ------------------------------------------------------------------

gscParser::NamespaceContext::NamespaceContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* gscParser::NamespaceContext::IDENTIFIER() {
  return getToken(gscParser::IDENTIFIER, 0);
}


size_t gscParser::NamespaceContext::getRuleIndex() const {
  return gscParser::RuleNamespace;
}


std::any gscParser::NamespaceContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitNamespace(this);
  else
    return visitor->visitChildren(this);
}

gscParser::NamespaceContext* gscParser::namespace_() {
  NamespaceContext *_localctx = _tracker.createInstance<NamespaceContext>(_ctx, getState());
  enterRule(_localctx, 4, gscParser::RuleNamespace);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(58);
    match(gscParser::T__2);
    setState(59);
    match(gscParser::IDENTIFIER);
    setState(60);
    match(gscParser::T__1);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FunctionContext ------------------------------------------------------------------

gscParser::FunctionContext::FunctionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> gscParser::FunctionContext::IDENTIFIER() {
  return getTokens(gscParser::IDENTIFIER);
}

tree::TerminalNode* gscParser::FunctionContext::IDENTIFIER(size_t i) {
  return getToken(gscParser::IDENTIFIER, i);
}

gscParser::Param_listContext* gscParser::FunctionContext::param_list() {
  return getRuleContext<gscParser::Param_listContext>(0);
}

gscParser::Statement_blockContext* gscParser::FunctionContext::statement_block() {
  return getRuleContext<gscParser::Statement_blockContext>(0);
}


size_t gscParser::FunctionContext::getRuleIndex() const {
  return gscParser::RuleFunction;
}


std::any gscParser::FunctionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitFunction(this);
  else
    return visitor->visitChildren(this);
}

gscParser::FunctionContext* gscParser::function() {
  FunctionContext *_localctx = _tracker.createInstance<FunctionContext>(_ctx, getState());
  enterRule(_localctx, 6, gscParser::RuleFunction);
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
    setState(63);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__3) {
      setState(62);
      match(gscParser::T__3);
    }
    setState(66);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__4) {
      setState(65);
      match(gscParser::T__4);
    }
    setState(69);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__5) {
      setState(68);
      match(gscParser::T__5);
    }
    setState(75);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__6) {
      setState(71);
      match(gscParser::T__6);
      setState(72);
      match(gscParser::T__7);
      setState(73);
      match(gscParser::IDENTIFIER);
      setState(74);
      match(gscParser::T__8);
    }
    setState(77);
    match(gscParser::IDENTIFIER);
    setState(78);
    match(gscParser::T__9);
    setState(79);
    param_list();
    setState(80);
    match(gscParser::T__10);
    setState(81);
    statement_block();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Param_listContext ------------------------------------------------------------------

gscParser::Param_listContext::Param_listContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<gscParser::Param_valContext *> gscParser::Param_listContext::param_val() {
  return getRuleContexts<gscParser::Param_valContext>();
}

gscParser::Param_valContext* gscParser::Param_listContext::param_val(size_t i) {
  return getRuleContext<gscParser::Param_valContext>(i);
}


size_t gscParser::Param_listContext::getRuleIndex() const {
  return gscParser::RuleParam_list;
}


std::any gscParser::Param_listContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitParam_list(this);
  else
    return visitor->visitChildren(this);
}

gscParser::Param_listContext* gscParser::param_list() {
  Param_listContext *_localctx = _tracker.createInstance<Param_listContext>(_ctx, getState());
  enterRule(_localctx, 8, gscParser::RuleParam_list);
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
    setState(91);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::IDENTIFIER) {
      setState(83);
      param_val();
      setState(88);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == gscParser::T__11) {
        setState(84);
        match(gscParser::T__11);
        setState(85);
        param_val();
        setState(90);
        _errHandler->sync(this);
        _la = _input->LA(1);
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

//----------------- Param_valContext ------------------------------------------------------------------

gscParser::Param_valContext::Param_valContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* gscParser::Param_valContext::IDENTIFIER() {
  return getToken(gscParser::IDENTIFIER, 0);
}

gscParser::ExpressionContext* gscParser::Param_valContext::expression() {
  return getRuleContext<gscParser::ExpressionContext>(0);
}


size_t gscParser::Param_valContext::getRuleIndex() const {
  return gscParser::RuleParam_val;
}


std::any gscParser::Param_valContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitParam_val(this);
  else
    return visitor->visitChildren(this);
}

gscParser::Param_valContext* gscParser::param_val() {
  Param_valContext *_localctx = _tracker.createInstance<Param_valContext>(_ctx, getState());
  enterRule(_localctx, 10, gscParser::RuleParam_val);
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
    setState(93);
    match(gscParser::IDENTIFIER);
    setState(96);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__12) {
      setState(94);
      match(gscParser::T__12);
      setState(95);
      expression();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Expression_listContext ------------------------------------------------------------------

gscParser::Expression_listContext::Expression_listContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<gscParser::ExpressionContext *> gscParser::Expression_listContext::expression() {
  return getRuleContexts<gscParser::ExpressionContext>();
}

gscParser::ExpressionContext* gscParser::Expression_listContext::expression(size_t i) {
  return getRuleContext<gscParser::ExpressionContext>(i);
}


size_t gscParser::Expression_listContext::getRuleIndex() const {
  return gscParser::RuleExpression_list;
}


std::any gscParser::Expression_listContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitExpression_list(this);
  else
    return visitor->visitChildren(this);
}

gscParser::Expression_listContext* gscParser::expression_list() {
  Expression_listContext *_localctx = _tracker.createInstance<Expression_listContext>(_ctx, getState());
  enterRule(_localctx, 12, gscParser::RuleExpression_list);
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
    setState(106);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 16763307705600) != 0)) {
      setState(98);
      expression();
      setState(103);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == gscParser::T__11) {
        setState(99);
        match(gscParser::T__11);
        setState(100);
        expression();
        setState(105);
        _errHandler->sync(this);
        _la = _input->LA(1);
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

//----------------- Statement_blockContext ------------------------------------------------------------------

gscParser::Statement_blockContext::Statement_blockContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<gscParser::StatementContext *> gscParser::Statement_blockContext::statement() {
  return getRuleContexts<gscParser::StatementContext>();
}

gscParser::StatementContext* gscParser::Statement_blockContext::statement(size_t i) {
  return getRuleContext<gscParser::StatementContext>(i);
}


size_t gscParser::Statement_blockContext::getRuleIndex() const {
  return gscParser::RuleStatement_block;
}


std::any gscParser::Statement_blockContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitStatement_block(this);
  else
    return visitor->visitChildren(this);
}

gscParser::Statement_blockContext* gscParser::statement_block() {
  Statement_blockContext *_localctx = _tracker.createInstance<Statement_blockContext>(_ctx, getState());
  enterRule(_localctx, 14, gscParser::RuleStatement_block);
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
    setState(108);
    match(gscParser::T__13);
    setState(112);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 16763314717956) != 0)) {
      setState(109);
      statement();
      setState(114);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(115);
    match(gscParser::T__14);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- StatementContext ------------------------------------------------------------------

gscParser::StatementContext::StatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

gscParser::Statement_blockContext* gscParser::StatementContext::statement_block() {
  return getRuleContext<gscParser::Statement_blockContext>(0);
}

gscParser::Statement_forContext* gscParser::StatementContext::statement_for() {
  return getRuleContext<gscParser::Statement_forContext>(0);
}

gscParser::Statement_ifContext* gscParser::StatementContext::statement_if() {
  return getRuleContext<gscParser::Statement_ifContext>(0);
}

gscParser::Statement_foreachContext* gscParser::StatementContext::statement_foreach() {
  return getRuleContext<gscParser::Statement_foreachContext>(0);
}

gscParser::Statement_instContext* gscParser::StatementContext::statement_inst() {
  return getRuleContext<gscParser::Statement_instContext>(0);
}


size_t gscParser::StatementContext::getRuleIndex() const {
  return gscParser::RuleStatement;
}


std::any gscParser::StatementContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitStatement(this);
  else
    return visitor->visitChildren(this);
}

gscParser::StatementContext* gscParser::statement() {
  StatementContext *_localctx = _tracker.createInstance<StatementContext>(_ctx, getState());
  enterRule(_localctx, 16, gscParser::RuleStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(122);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 12, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(117);
      statement_block();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(118);
      statement_for();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(119);
      statement_if();
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(120);
      statement_foreach();
      break;
    }

    case 5: {
      enterOuterAlt(_localctx, 5);
      setState(121);
      statement_inst();
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Statement_forContext ------------------------------------------------------------------

gscParser::Statement_forContext::Statement_forContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

gscParser::StatementContext* gscParser::Statement_forContext::statement() {
  return getRuleContext<gscParser::StatementContext>(0);
}

std::vector<gscParser::ExpressionContext *> gscParser::Statement_forContext::expression() {
  return getRuleContexts<gscParser::ExpressionContext>();
}

gscParser::ExpressionContext* gscParser::Statement_forContext::expression(size_t i) {
  return getRuleContext<gscParser::ExpressionContext>(i);
}


size_t gscParser::Statement_forContext::getRuleIndex() const {
  return gscParser::RuleStatement_for;
}


std::any gscParser::Statement_forContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitStatement_for(this);
  else
    return visitor->visitChildren(this);
}

gscParser::Statement_forContext* gscParser::statement_for() {
  Statement_forContext *_localctx = _tracker.createInstance<Statement_forContext>(_ctx, getState());
  enterRule(_localctx, 18, gscParser::RuleStatement_for);
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
    setState(124);
    match(gscParser::T__15);
    setState(125);
    match(gscParser::T__9);
    setState(127);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 16763307705600) != 0)) {
      setState(126);
      expression();
    }
    setState(129);
    match(gscParser::T__1);
    setState(131);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 16763307705600) != 0)) {
      setState(130);
      expression();
    }
    setState(133);
    match(gscParser::T__1);
    setState(135);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 16763307705600) != 0)) {
      setState(134);
      expression();
    }
    setState(137);
    match(gscParser::T__10);
    setState(138);
    statement();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Statement_foreachContext ------------------------------------------------------------------

gscParser::Statement_foreachContext::Statement_foreachContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> gscParser::Statement_foreachContext::IDENTIFIER() {
  return getTokens(gscParser::IDENTIFIER);
}

tree::TerminalNode* gscParser::Statement_foreachContext::IDENTIFIER(size_t i) {
  return getToken(gscParser::IDENTIFIER, i);
}

gscParser::ExpressionContext* gscParser::Statement_foreachContext::expression() {
  return getRuleContext<gscParser::ExpressionContext>(0);
}

gscParser::StatementContext* gscParser::Statement_foreachContext::statement() {
  return getRuleContext<gscParser::StatementContext>(0);
}


size_t gscParser::Statement_foreachContext::getRuleIndex() const {
  return gscParser::RuleStatement_foreach;
}


std::any gscParser::Statement_foreachContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitStatement_foreach(this);
  else
    return visitor->visitChildren(this);
}

gscParser::Statement_foreachContext* gscParser::statement_foreach() {
  Statement_foreachContext *_localctx = _tracker.createInstance<Statement_foreachContext>(_ctx, getState());
  enterRule(_localctx, 20, gscParser::RuleStatement_foreach);
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
    setState(140);
    match(gscParser::T__16);
    setState(141);
    match(gscParser::T__9);
    setState(142);
    match(gscParser::IDENTIFIER);
    setState(145);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__11) {
      setState(143);
      match(gscParser::T__11);
      setState(144);
      match(gscParser::IDENTIFIER);
    }
    setState(147);
    match(gscParser::T__17);
    setState(148);
    expression();
    setState(149);
    match(gscParser::T__10);
    setState(150);
    statement();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Statement_ifContext ------------------------------------------------------------------

gscParser::Statement_ifContext::Statement_ifContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

gscParser::ExpressionContext* gscParser::Statement_ifContext::expression() {
  return getRuleContext<gscParser::ExpressionContext>(0);
}

std::vector<gscParser::StatementContext *> gscParser::Statement_ifContext::statement() {
  return getRuleContexts<gscParser::StatementContext>();
}

gscParser::StatementContext* gscParser::Statement_ifContext::statement(size_t i) {
  return getRuleContext<gscParser::StatementContext>(i);
}


size_t gscParser::Statement_ifContext::getRuleIndex() const {
  return gscParser::RuleStatement_if;
}


std::any gscParser::Statement_ifContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitStatement_if(this);
  else
    return visitor->visitChildren(this);
}

gscParser::Statement_ifContext* gscParser::statement_if() {
  Statement_ifContext *_localctx = _tracker.createInstance<Statement_ifContext>(_ctx, getState());
  enterRule(_localctx, 22, gscParser::RuleStatement_if);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(152);
    match(gscParser::T__18);
    setState(153);
    match(gscParser::T__9);
    setState(154);
    expression();
    setState(155);
    match(gscParser::T__10);
    setState(156);
    statement();
    setState(159);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 17, _ctx)) {
    case 1: {
      setState(157);
      match(gscParser::T__19);
      setState(158);
      statement();
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Statement_instContext ------------------------------------------------------------------

gscParser::Statement_instContext::Statement_instContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

gscParser::Function_callContext* gscParser::Statement_instContext::function_call() {
  return getRuleContext<gscParser::Function_callContext>(0);
}

gscParser::Operator_instContext* gscParser::Statement_instContext::operator_inst() {
  return getRuleContext<gscParser::Operator_instContext>(0);
}


size_t gscParser::Statement_instContext::getRuleIndex() const {
  return gscParser::RuleStatement_inst;
}


std::any gscParser::Statement_instContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitStatement_inst(this);
  else
    return visitor->visitChildren(this);
}

gscParser::Statement_instContext* gscParser::statement_inst() {
  Statement_instContext *_localctx = _tracker.createInstance<Statement_instContext>(_ctx, getState());
  enterRule(_localctx, 24, gscParser::RuleStatement_inst);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(163);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 18, _ctx)) {
    case 1: {
      setState(161);
      function_call();
      break;
    }

    case 2: {
      setState(162);
      operator_inst();
      break;
    }

    default:
      break;
    }
    setState(165);
    match(gscParser::T__1);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Function_callContext ------------------------------------------------------------------

gscParser::Function_callContext::Function_callContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> gscParser::Function_callContext::IDENTIFIER() {
  return getTokens(gscParser::IDENTIFIER);
}

tree::TerminalNode* gscParser::Function_callContext::IDENTIFIER(size_t i) {
  return getToken(gscParser::IDENTIFIER, i);
}

gscParser::Expression_listContext* gscParser::Function_callContext::expression_list() {
  return getRuleContext<gscParser::Expression_listContext>(0);
}

gscParser::ExpressionContext* gscParser::Function_callContext::expression() {
  return getRuleContext<gscParser::ExpressionContext>(0);
}


size_t gscParser::Function_callContext::getRuleIndex() const {
  return gscParser::RuleFunction_call;
}


std::any gscParser::Function_callContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitFunction_call(this);
  else
    return visitor->visitChildren(this);
}

gscParser::Function_callContext* gscParser::function_call() {
  Function_callContext *_localctx = _tracker.createInstance<Function_callContext>(_ctx, getState());
  enterRule(_localctx, 26, gscParser::RuleFunction_call);
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
    setState(168);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 19, _ctx)) {
    case 1: {
      setState(167);
      expression();
      break;
    }

    default:
      break;
    }
    setState(171);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == gscParser::T__20

    || _la == gscParser::T__21) {
      setState(170);
      _la = _input->LA(1);
      if (!(_la == gscParser::T__20

      || _la == gscParser::T__21)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
    }
    setState(175);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 21, _ctx)) {
    case 1: {
      setState(173);
      match(gscParser::IDENTIFIER);
      setState(174);
      match(gscParser::T__22);
      break;
    }

    default:
      break;
    }
    setState(177);
    match(gscParser::IDENTIFIER);
    setState(178);
    match(gscParser::T__9);
    setState(179);
    expression_list();
    setState(180);
    match(gscParser::T__10);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Operator_instContext ------------------------------------------------------------------

gscParser::Operator_instContext::Operator_instContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* gscParser::Operator_instContext::IDENTIFIER() {
  return getToken(gscParser::IDENTIFIER, 0);
}

gscParser::ExpressionContext* gscParser::Operator_instContext::expression() {
  return getRuleContext<gscParser::ExpressionContext>(0);
}


size_t gscParser::Operator_instContext::getRuleIndex() const {
  return gscParser::RuleOperator_inst;
}


std::any gscParser::Operator_instContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitOperator_inst(this);
  else
    return visitor->visitChildren(this);
}

gscParser::Operator_instContext* gscParser::operator_inst() {
  Operator_instContext *_localctx = _tracker.createInstance<Operator_instContext>(_ctx, getState());
  enterRule(_localctx, 28, gscParser::RuleOperator_inst);
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
    setState(182);
    match(gscParser::IDENTIFIER);
    setState(184);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 16763307705600) != 0)) {
      setState(183);
      expression();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExpressionContext ------------------------------------------------------------------

gscParser::ExpressionContext::ExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* gscParser::ExpressionContext::IDENTIFIER() {
  return getToken(gscParser::IDENTIFIER, 0);
}

gscParser::Const_exprContext* gscParser::ExpressionContext::const_expr() {
  return getRuleContext<gscParser::Const_exprContext>(0);
}


size_t gscParser::ExpressionContext::getRuleIndex() const {
  return gscParser::RuleExpression;
}


std::any gscParser::ExpressionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitExpression(this);
  else
    return visitor->visitChildren(this);
}

gscParser::ExpressionContext* gscParser::expression() {
  ExpressionContext *_localctx = _tracker.createInstance<ExpressionContext>(_ctx, getState());
  enterRule(_localctx, 30, gscParser::RuleExpression);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(188);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case gscParser::IDENTIFIER: {
        enterOuterAlt(_localctx, 1);
        setState(186);
        match(gscParser::IDENTIFIER);
        break;
      }

      case gscParser::T__7:
      case gscParser::T__9:
      case gscParser::T__13:
      case gscParser::T__23:
      case gscParser::T__24:
      case gscParser::INTEGER10:
      case gscParser::INTEGER16:
      case gscParser::INTEGER8:
      case gscParser::INTEGER2:
      case gscParser::FLOATVAL:
      case gscParser::STRING:
      case gscParser::HASHSTRING:
      case gscParser::BOOL_VALUE:
      case gscParser::UNDEFINED_VALUE: {
        enterOuterAlt(_localctx, 2);
        setState(187);
        const_expr();
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

//----------------- Const_exprContext ------------------------------------------------------------------

gscParser::Const_exprContext::Const_exprContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

gscParser::Vector_valueContext* gscParser::Const_exprContext::vector_value() {
  return getRuleContext<gscParser::Vector_valueContext>(0);
}

gscParser::Array_defContext* gscParser::Const_exprContext::array_def() {
  return getRuleContext<gscParser::Array_defContext>(0);
}

gscParser::Struct_defContext* gscParser::Const_exprContext::struct_def() {
  return getRuleContext<gscParser::Struct_defContext>(0);
}

gscParser::NumberContext* gscParser::Const_exprContext::number() {
  return getRuleContext<gscParser::NumberContext>(0);
}

tree::TerminalNode* gscParser::Const_exprContext::BOOL_VALUE() {
  return getToken(gscParser::BOOL_VALUE, 0);
}

tree::TerminalNode* gscParser::Const_exprContext::FLOATVAL() {
  return getToken(gscParser::FLOATVAL, 0);
}

tree::TerminalNode* gscParser::Const_exprContext::STRING() {
  return getToken(gscParser::STRING, 0);
}

tree::TerminalNode* gscParser::Const_exprContext::HASHSTRING() {
  return getToken(gscParser::HASHSTRING, 0);
}

tree::TerminalNode* gscParser::Const_exprContext::UNDEFINED_VALUE() {
  return getToken(gscParser::UNDEFINED_VALUE, 0);
}

gscParser::Function_refContext* gscParser::Const_exprContext::function_ref() {
  return getRuleContext<gscParser::Function_refContext>(0);
}


size_t gscParser::Const_exprContext::getRuleIndex() const {
  return gscParser::RuleConst_expr;
}


std::any gscParser::Const_exprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitConst_expr(this);
  else
    return visitor->visitChildren(this);
}

gscParser::Const_exprContext* gscParser::const_expr() {
  Const_exprContext *_localctx = _tracker.createInstance<Const_exprContext>(_ctx, getState());
  enterRule(_localctx, 32, gscParser::RuleConst_expr);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(200);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case gscParser::T__9: {
        enterOuterAlt(_localctx, 1);
        setState(190);
        vector_value();
        break;
      }

      case gscParser::T__7: {
        enterOuterAlt(_localctx, 2);
        setState(191);
        array_def();
        break;
      }

      case gscParser::T__13: {
        enterOuterAlt(_localctx, 3);
        setState(192);
        struct_def();
        break;
      }

      case gscParser::INTEGER10:
      case gscParser::INTEGER16:
      case gscParser::INTEGER8:
      case gscParser::INTEGER2: {
        enterOuterAlt(_localctx, 4);
        setState(193);
        number();
        break;
      }

      case gscParser::BOOL_VALUE: {
        enterOuterAlt(_localctx, 5);
        setState(194);
        match(gscParser::BOOL_VALUE);
        break;
      }

      case gscParser::FLOATVAL: {
        enterOuterAlt(_localctx, 6);
        setState(195);
        match(gscParser::FLOATVAL);
        break;
      }

      case gscParser::STRING: {
        enterOuterAlt(_localctx, 7);
        setState(196);
        match(gscParser::STRING);
        break;
      }

      case gscParser::HASHSTRING: {
        enterOuterAlt(_localctx, 8);
        setState(197);
        match(gscParser::HASHSTRING);
        break;
      }

      case gscParser::UNDEFINED_VALUE: {
        enterOuterAlt(_localctx, 9);
        setState(198);
        match(gscParser::UNDEFINED_VALUE);
        break;
      }

      case gscParser::T__23:
      case gscParser::T__24: {
        enterOuterAlt(_localctx, 10);
        setState(199);
        function_ref();
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

//----------------- Function_refContext ------------------------------------------------------------------

gscParser::Function_refContext::Function_refContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> gscParser::Function_refContext::IDENTIFIER() {
  return getTokens(gscParser::IDENTIFIER);
}

tree::TerminalNode* gscParser::Function_refContext::IDENTIFIER(size_t i) {
  return getToken(gscParser::IDENTIFIER, i);
}

tree::TerminalNode* gscParser::Function_refContext::PATH() {
  return getToken(gscParser::PATH, 0);
}


size_t gscParser::Function_refContext::getRuleIndex() const {
  return gscParser::RuleFunction_ref;
}


std::any gscParser::Function_refContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitFunction_ref(this);
  else
    return visitor->visitChildren(this);
}

gscParser::Function_refContext* gscParser::function_ref() {
  Function_refContext *_localctx = _tracker.createInstance<Function_refContext>(_ctx, getState());
  enterRule(_localctx, 34, gscParser::RuleFunction_ref);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(215);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case gscParser::T__23: {
        enterOuterAlt(_localctx, 1);
        setState(202);
        match(gscParser::T__23);
        setState(205);
        _errHandler->sync(this);

        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 25, _ctx)) {
        case 1: {
          setState(203);
          match(gscParser::IDENTIFIER);
          setState(204);
          match(gscParser::T__22);
          break;
        }

        default:
          break;
        }
        setState(207);
        match(gscParser::IDENTIFIER);
        break;
      }

      case gscParser::T__24: {
        enterOuterAlt(_localctx, 2);
        setState(208);
        match(gscParser::T__24);
        setState(209);
        match(gscParser::IDENTIFIER);
        setState(210);
        match(gscParser::T__25);
        setState(211);
        match(gscParser::PATH);
        setState(212);
        match(gscParser::T__26);
        setState(213);
        match(gscParser::T__22);
        setState(214);
        match(gscParser::IDENTIFIER);
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

gscParser::NumberContext::NumberContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* gscParser::NumberContext::INTEGER10() {
  return getToken(gscParser::INTEGER10, 0);
}

tree::TerminalNode* gscParser::NumberContext::INTEGER16() {
  return getToken(gscParser::INTEGER16, 0);
}

tree::TerminalNode* gscParser::NumberContext::INTEGER8() {
  return getToken(gscParser::INTEGER8, 0);
}

tree::TerminalNode* gscParser::NumberContext::INTEGER2() {
  return getToken(gscParser::INTEGER2, 0);
}


size_t gscParser::NumberContext::getRuleIndex() const {
  return gscParser::RuleNumber;
}


std::any gscParser::NumberContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitNumber(this);
  else
    return visitor->visitChildren(this);
}

gscParser::NumberContext* gscParser::number() {
  NumberContext *_localctx = _tracker.createInstance<NumberContext>(_ctx, getState());
  enterRule(_localctx, 36, gscParser::RuleNumber);
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
    setState(217);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 64424509440) != 0))) {
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

//----------------- Vector_valueContext ------------------------------------------------------------------

gscParser::Vector_valueContext::Vector_valueContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<gscParser::ExpressionContext *> gscParser::Vector_valueContext::expression() {
  return getRuleContexts<gscParser::ExpressionContext>();
}

gscParser::ExpressionContext* gscParser::Vector_valueContext::expression(size_t i) {
  return getRuleContext<gscParser::ExpressionContext>(i);
}


size_t gscParser::Vector_valueContext::getRuleIndex() const {
  return gscParser::RuleVector_value;
}


std::any gscParser::Vector_valueContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitVector_value(this);
  else
    return visitor->visitChildren(this);
}

gscParser::Vector_valueContext* gscParser::vector_value() {
  Vector_valueContext *_localctx = _tracker.createInstance<Vector_valueContext>(_ctx, getState());
  enterRule(_localctx, 38, gscParser::RuleVector_value);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(219);
    match(gscParser::T__9);
    setState(220);
    expression();
    setState(221);
    match(gscParser::T__11);
    setState(222);
    expression();
    setState(223);
    match(gscParser::T__11);
    setState(224);
    expression();
    setState(225);
    match(gscParser::T__10);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Array_defContext ------------------------------------------------------------------

gscParser::Array_defContext::Array_defContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<gscParser::ExpressionContext *> gscParser::Array_defContext::expression() {
  return getRuleContexts<gscParser::ExpressionContext>();
}

gscParser::ExpressionContext* gscParser::Array_defContext::expression(size_t i) {
  return getRuleContext<gscParser::ExpressionContext>(i);
}

std::vector<tree::TerminalNode *> gscParser::Array_defContext::HASHSTRING() {
  return getTokens(gscParser::HASHSTRING);
}

tree::TerminalNode* gscParser::Array_defContext::HASHSTRING(size_t i) {
  return getToken(gscParser::HASHSTRING, i);
}

std::vector<gscParser::NumberContext *> gscParser::Array_defContext::number() {
  return getRuleContexts<gscParser::NumberContext>();
}

gscParser::NumberContext* gscParser::Array_defContext::number(size_t i) {
  return getRuleContext<gscParser::NumberContext>(i);
}


size_t gscParser::Array_defContext::getRuleIndex() const {
  return gscParser::RuleArray_def;
}


std::any gscParser::Array_defContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitArray_def(this);
  else
    return visitor->visitChildren(this);
}

gscParser::Array_defContext* gscParser::array_def() {
  Array_defContext *_localctx = _tracker.createInstance<Array_defContext>(_ctx, getState());
  enterRule(_localctx, 40, gscParser::RuleArray_def);
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
    setState(227);
    match(gscParser::T__7);
    setState(242);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 16763576141056) != 0)) {
      setState(234);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 28, _ctx)) {
      case 1: {
        setState(231);
        _errHandler->sync(this);
        switch (_input->LA(1)) {
          case gscParser::HASHSTRING: {
            setState(228);
            match(gscParser::HASHSTRING);
            break;
          }

          case gscParser::T__27: {
            setState(229);
            match(gscParser::T__27);
            setState(230);
            number();
            break;
          }

        default:
          throw NoViableAltException(this);
        }
        setState(233);
        match(gscParser::T__28);
        break;
      }

      default:
        break;
      }
      setState(236);
      expression();
      setState(238);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == gscParser::T__11) {
        setState(237);
        match(gscParser::T__11);
      }
      setState(244);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(245);
    match(gscParser::T__8);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Struct_defContext ------------------------------------------------------------------

gscParser::Struct_defContext::Struct_defContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> gscParser::Struct_defContext::STRUCT_IDENTIFIER() {
  return getTokens(gscParser::STRUCT_IDENTIFIER);
}

tree::TerminalNode* gscParser::Struct_defContext::STRUCT_IDENTIFIER(size_t i) {
  return getToken(gscParser::STRUCT_IDENTIFIER, i);
}

std::vector<gscParser::ExpressionContext *> gscParser::Struct_defContext::expression() {
  return getRuleContexts<gscParser::ExpressionContext>();
}

gscParser::ExpressionContext* gscParser::Struct_defContext::expression(size_t i) {
  return getRuleContext<gscParser::ExpressionContext>(i);
}


size_t gscParser::Struct_defContext::getRuleIndex() const {
  return gscParser::RuleStruct_def;
}


std::any gscParser::Struct_defContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<gscVisitor*>(visitor))
    return parserVisitor->visitStruct_def(this);
  else
    return visitor->visitChildren(this);
}

gscParser::Struct_defContext* gscParser::struct_def() {
  Struct_defContext *_localctx = _tracker.createInstance<Struct_defContext>(_ctx, getState());
  enterRule(_localctx, 42, gscParser::RuleStruct_def);
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
    setState(247);
    match(gscParser::T__13);
    setState(256);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == gscParser::STRUCT_IDENTIFIER) {
      setState(248);
      match(gscParser::STRUCT_IDENTIFIER);
      setState(249);
      match(gscParser::T__28);
      setState(250);
      expression();
      setState(252);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == gscParser::T__11) {
        setState(251);
        match(gscParser::T__11);
      }
      setState(258);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(259);
    match(gscParser::T__14);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

void gscParser::initialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  gscParserInitialize();
#else
  ::antlr4::internal::call_once(gscParserOnceFlag, gscParserInitialize);
#endif
}
