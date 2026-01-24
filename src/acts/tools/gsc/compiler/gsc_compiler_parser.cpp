#include <includes.hpp>
#include <tools/gsc/compiler/gsc_compiler_parser.hpp>
#include <tools/gsc/compiler/gsc_compiler_script_object.hpp>

namespace tool::gsc::compiler {

    std::string ParseString(TerminalNode* term, size_t start) {
        std::string node = term->getText();

        if (node.length() <= start) return {};

        size_t end{ node.length() };

        // string val, remove delimiter
        if ((node[start] == '"' || node[start] == '\'') && node[node.length() - 1] == node[start]) {
            end--;
            start++;
        }

        std::unique_ptr<char[]> newStr = std::make_unique<char[]>(end - start + 1);
        char* newStrWriter = &newStr[0];

        // format string
        for (size_t i = start; i < end; i++) {
            if (node[i] != '\\') {
                *(newStrWriter++) = node[i];
                continue; // default case
            }

            i++;

            assert(i < node.length() && "bad format, \\ before end");

            switch (node[i]) {
            case 'n':
                *(newStrWriter++) = '\n';
                break;
            case 't':
                *(newStrWriter++) = '\t';
                break;
            case 'r':
                *(newStrWriter++) = '\r';
                break;
            case 'b':
                *(newStrWriter++) = '\b';
                break;
            default:
                *(newStrWriter++) = node[i];
                break;
            }
        }
        *(newStrWriter++) = 0; // end char
        return &newStr[0];
    }
    
    constexpr byte INVALID_VECTOR_FLAGS = 0xFF;

    bool FindConstVectorCommon(float& x, float& y, float& z, float& common) {
        common = x;
        if (y != 0) {
            if (!common) {
                common = y;
            }
            else if (y != common && y != -common) {
                return false;
            }
        }
        if (z != 0) {
            if (!common) {
                common = z;
            }
            else if (z != common && z != -common) {
                return false;
            }
        }

        if (common != 0 && common != 1 && common != -1) {
            if (common < 0) {
                common *= -1;
            }
            x /= common;
            y /= common;
            z /= common;
            return true;
        }
        // no scale required
        return false;
    }

    byte ToConstVectorFlags(float x, float y, float z) {
        // 0 = 0, 1 = -1, 2 = 1
        byte flag{};

        if (x == -1) flag |= 1 << 4;
        else if (x == 1) flag |= 2 << 4;
        else if (x != 0) return INVALID_VECTOR_FLAGS;

        if (y == -1) flag |= 1 << 2;
        else if (y == 1) flag |= 2 << 2;
        else if (y != 0) return INVALID_VECTOR_FLAGS;

        if (z == -1) flag |= 1;
        else if (z == 1) flag |= 2;
        else if (z != 0) return INVALID_VECTOR_FLAGS;

        return flag;
    }

    bool AddHashNode(ParseTree* tree, char type, const std::string& sub, FunctionObject& fobj, CompileObject& obj) {
        obj.AddHash(sub);
        auto ith = obj.vmInfo->hashesFunc.find(type);

        if (ith == obj.vmInfo->hashesFunc.end()) {
            obj.info.PrintLineMessage(core::logs::LVL_ERROR, tree, std::format("Hash type not available for this vm: {}", type));
            return false;
        }

        const char* ss = sub.c_str();

        uint64_t val;

        if (!hash::TryHashPattern(ss, val)) {
            val = ith->second.hashFunc(ss);
            if (!val) {
                obj.info.PrintLineMessage(core::logs::LVL_ERROR, tree, std::format("Can't hash the string '{}' with the type {}", sub, type));
                return false;
            }
        }

        switch (ith->second.size) {
        case 8: fobj.AddNode(tree, new AscmNodeData<uint64_t>((uint64_t)val, ith->second.opCode)); break;
        case 4: fobj.AddNode(tree, new AscmNodeData<uint32_t>((uint32_t)val, ith->second.opCode)); break;
        case 2: fobj.AddNode(tree, new AscmNodeData<uint16_t>((uint16_t)val, ith->second.opCode)); break;
        case 1: fobj.AddNode(tree, new AscmNodeData<uint8_t>((uint8_t)val, ith->second.opCode)); break;
        default: {
            obj.info.PrintLineMessage(core::logs::LVL_ERROR, tree, std::format("Invalid hash size definition: {} / {} bytes", type, ith->second.size));
            return false;
        }
        }
        return true;
    }


    bool ParseFieldNode(ParseTree* exp, gscParser& parser, CompileObject& obj, FunctionObject& fobj) {
        if (!exp) {
            obj.info.PrintLineMessage(core::logs::LVL_ERROR, exp, "empty tree error");
            return false;
        }
        if (exp->getTreeType() == TREE_ERROR) {
            obj.info.PrintLineMessage(core::logs::LVL_ERROR, exp, "detected tree error, bad syntax?");
            return false;
        }
        if (exp->getTreeType() == TREE_RULE) {
            RuleContext* rule = dynamic_cast<RuleContext*>(exp);



            // search that we don't have a children in error, after that we'll assume the tree is right for a depth of 2
            if (std::find_if(rule->children.begin(), rule->children.end(), [](const ParseTree* tree) -> bool { return tree->getTreeType() == TREE_ERROR; })
                != rule->children.end()) {
                obj.info.PrintLineMessage(core::logs::LVL_ERROR, exp, "detected tree children error, bad syntax?");
                return false;
            }

            switch (rule->getRuleIndex()) {
            case gscParser::RuleExpression:
            case gscParser::RuleExpression0:
            case gscParser::RuleExpression1:
            case gscParser::RuleExpression2:
            case gscParser::RuleExpression3:
            case gscParser::RuleExpression4:
            case gscParser::RuleExpression5:
            case gscParser::RuleExpression6:
            case gscParser::RuleExpression7:
            case gscParser::RuleExpression8:
            case gscParser::RuleExpression9:
            case gscParser::RuleExpression10:
            case gscParser::RuleExpression11:
            case gscParser::RuleExpression12:
            case gscParser::RuleExpression13:
            case gscParser::RuleExpression14:
            case gscParser::RuleIdf: {
                if (rule->children.size() == 1) {
                    return ParseFieldNode(rule->children[0], parser, obj, fobj);
                }
                obj.info.PrintLineMessage(core::logs::LVL_ERROR, exp, std::format("Not a valid lvalue: {} ({})", rule->getText(), rule->getRuleIndex()));
                return false;
            }
            case gscParser::RuleExpression15:
                return ParseFieldNode(rule->children[1], parser, obj, fobj);
            case gscParser::RuleLeft_value: {
                if (rule->children.size() == 1) {
                    return ParseFieldNode(rule->children[0], parser, obj, fobj);
                }

                if (rule->children[0]->getTreeType() == TREE_RULE) {
                    RuleContext* first = dynamic_cast<RuleContext*>(rule->children[0]);

                    std::string second = rule->children[1]->getText();

                    if (second == "." || second == "?.") {
                        if (second == "?.") {
                            // assume that it'll fail by itself if it is undefined so we don't need to do anything
                            obj.info.PrintLineMessage(core::logs::LVL_WARNING, exp, std::format("Usage of ?. in a left value: {}", rule->getText()));
                        }
                        // object access
                        if (IS_IDF(rule->children[2])) {
                            if (!ParseExpressionNode(first, parser, obj, fobj, true)) {
                                return false;
                            }
                            std::string fieldText = rule->children[2]->getText();

                            if (fieldText == "size") {
                                obj.info.PrintLineMessage(core::logs::LVL_ERROR, exp, std::format(".size can't be used as a lvalue: {}", exp->getText()));
                                return false;
                            }
                            else {
                                // use identifier
                                fobj.AddNode(rule->children[2], new AscmNodeOpCode(OPCODE_CastFieldObject));
                                fobj.AddNode(rule->children[2], fobj.CreateFieldHash(fieldText, OPCODE_EvalFieldVariableRef));
                            }
                        }
                        else {
                            if (!ParseExpressionNode(rule->children[3], parser, obj, fobj, true)) {
                                return false;
                            }
                            fobj.AddNode(rule->children[3], new AscmNodeOpCode(OPCODE_CastCanon));

                            if (!ParseExpressionNode(first, parser, obj, fobj, true)) {
                                return false;
                            }
                            fobj.AddNode(first, new AscmNodeOpCode(OPCODE_CastFieldObject));
                            fobj.AddNode(first, new AscmNodeOpCode(OPCODE_EvalFieldVariableOnStackRef));
                        }
                        return true;
                    }
                    else if (second == "[") {
                        // array access

                        if (!ParseExpressionNode(rule->children[2], parser, obj, fobj, true)) {
                            return false;
                        }

                        if (!ParseFieldNode(first, parser, obj, fobj)) {
                            return false;
                        }

                        fobj.AddNode(first, new AscmNodeOpCode(OPCODE_EvalArrayRef));
                        return true;
                    }
                    else {
                        obj.info.PrintLineMessage(core::logs::LVL_ERROR, exp, std::format("Unknown left value type: {}", second));
                        return false;
                    }
                }
                else {
                    obj.info.PrintLineMessage(core::logs::LVL_ERROR, exp, std::format("Unknown lvalue tree type: {}", exp->getText()));
                    return false;
                }
                obj.info.PrintLineMessage(core::logs::LVL_ERROR, exp, std::format("Unhandled lvalue type: {}", exp->getText()));
                return false;
            }
            case gscParser::RuleConst_expr:
            case gscParser::RuleConst_expr_static:
            case gscParser::RuleNumber:
            case gscParser::RuleVector_value:
            case gscParser::RuleClass_init:
            case gscParser::RuleSet_expression:
            case gscParser::RuleArray_def:
            case gscParser::RuleStruct_def:
            case gscParser::RuleFunction:
            case gscParser::RuleFunction_ref:
            case gscParser::RuleStatement:
            case gscParser::RuleStatement_if:
            case gscParser::RuleStatement_while:
            case gscParser::RuleStatement_dowhile:
            case gscParser::RuleStatement_for:
            case gscParser::RuleStatement_switch:
            case gscParser::RuleStatement_foreach:
            case gscParser::RuleStatement_inst:
            case gscParser::RuleNop_def:
            case gscParser::RuleDevop_def:
            case gscParser::RuleStatement_block:
            case gscParser::RuleStatement_dev_block:
            case gscParser::RuleOperator_inst:
            case gscParser::RuleArray_unpack:
            case gscParser::RuleFunction_call:
            case gscParser::RuleFunction_call_exp:
                obj.info.PrintLineMessage(core::logs::LVL_ERROR, exp, std::format("Not a valid lvalue: {} ({})", rule->getText(), rule->getRuleIndex()));
                return false;
            }


            obj.info.PrintLineMessage(core::logs::LVL_ERROR, rule, std::format("Unhandled lvalue rule: {} ({})", rule->getText(), rule->getRuleIndex()));
            return false;
        }

        if (exp->getTreeType() != TREE_TERMINAL) {
            obj.info.PrintLineMessage(core::logs::LVL_WARNING, exp, std::format("Unknown tree type: {}", exp->getText()));
            return false;
        }
        TerminalNode* term = dynamic_cast<TerminalNode*>(exp);


        size_t len = term->getText().size();

        switch (term->getSymbol()->getType()) {
        case gscParser::IDENTIFIER: {
            std::string varName = term->getText();

            auto ceit = obj.constexprs.find(hash::Hash64(varName.c_str()));

            if (ceit != obj.constexprs.end()) {
                ParseTree* pt = ceit->second;

                if (!ParseFieldNode(pt, parser, obj, fobj)) {
                    obj.info.PrintLineMessage(core::logs::LVL_ERROR, term, std::format("error when using constexpr {}", varName));
                    return false;
                }
                return true;
            }

            if (varName == "self") {
                obj.info.PrintLineMessage(core::logs::LVL_ERROR, term, "self can't be used as a lvalue");
                return false;
            }

            auto gvarIt = fobj.m_vmInfo->globalvars.find(obj.vmInfo->HashField(varName));

            if (gvarIt != fobj.m_vmInfo->globalvars.end()) {
                GlobalVariableDef& gv = gvarIt->second;

                obj.info.PrintLineMessage(core::logs::LVL_WARNING, term, std::format("The {} global variable can't be used as a lvalue", gv.name));
                return false;
            }
            auto [varerr, keyVarL] = fobj.RegisterVar(varName, true, 0);

            if (varerr) {
                obj.info.PrintLineMessage(core::logs::LVL_ERROR, term, varerr);
                return false;
            }

            fobj.AddNode(term, new AscmNodeVariable(keyVarL->id, OPCODE_EvalLocalVariableRefCached));
            return true;
        }
        case gscParser::UNDEFINED_VALUE:
        case gscParser::BOOL_VALUE:
        case gscParser::FLOATVAL:
        case gscParser::INTEGER10:
        case gscParser::INTEGER16:
        case gscParser::INTEGER8:
        case gscParser::INTEGER2:
        case gscParser::HASHSTRING:
        case gscParser::STRING:
            obj.info.PrintLineMessage(core::logs::LVL_ERROR, exp, std::format("Not a valid lvalue: {}", term->getText()));
            return false;
        }

        obj.info.PrintLineMessage(core::logs::LVL_ERROR, exp, std::format("Unhandled lvalue terminal: {}", term->getText()));
        return false;
    }
    
    bool ParseExpressionNode(ParseTree* exp, gscParser& parser, CompileObject& obj, FunctionObject& fobj, bool expressVal) {
        if (!exp) {
            obj.info.PrintLineMessage(core::logs::LVL_ERROR, exp, "Empty tree error");
            return false;
        }
        if (exp->getTreeType() == TREE_ERROR) {
            obj.info.PrintLineMessage(core::logs::LVL_ERROR, exp, "Detected tree error, bad syntax?");
            return false;
        }

        if (exp->getTreeType() == TREE_RULE) {
            RuleContext* rule = dynamic_cast<RuleContext*>(exp);

            // search that we don't have a children in error, after that we'll assume the tree is right for a depth of 2
            if (std::find_if(rule->children.begin(), rule->children.end(), [](const ParseTree* tree) -> bool { return tree->getTreeType() == TREE_ERROR; })
                != rule->children.end()) {
                obj.info.PrintLineMessage(core::logs::LVL_ERROR, exp, "Detected tree children error, bad syntax?");
                return false;
            }

            switch (rule->getRuleIndex()) {
            case gscParser::RuleStatement: {
                if (expressVal) {
                    obj.info.PrintLineMessage(core::logs::LVL_ERROR, rule, "Can't express a statement");
                    return false;
                }

                if (rule->children.size() > 1) {
                    // define location

                    std::string locName = rule->children[0]->getText();

                    FunctionJumpLoc& loc = fobj.m_jumpLocs[locName];

                    if (loc.defined) {
                        obj.info.PrintLineMessage(core::logs::LVL_ERROR, rule, std::format("The location {} was defined twice!", locName));
                        return false;
                    }

                    loc.defined = true;

                    if (!loc.node) {
                        loc.node = new AscmNode();
                    }

                    fobj.AddNode(rule, loc.node);
                }

                return ParseExpressionNode(rule->children[rule->children.size() - 1], parser, obj, fobj, expressVal);
            }
            case gscParser::RuleStatement_if: {
                bool ok{ true };

                bool extracted;
                int64_t v = obj.NumberNodeValue(rule->children[2], false, &extracted);

                if (extracted) {
                    if (v) {
                        // no else
                        return ParseExpressionNode(rule->children[4], parser, obj, fobj, false);
                    }
                    else {
                        if (rule->children.size() <= 5) {
                            // no else
                            return true;
                        }
                        else {
                            // only else
                            return ParseExpressionNode(rule->children[6], parser, obj, fobj, false);
                        }
                    }
                }

                AscmNode* elseStart = new AscmNode();
                if (!ParseExpressionNode(rule->children[2], parser, obj, fobj, true)) {
                    ok = false;
                }

                fobj.AddNode(rule->children[2], new AscmNodeJump(elseStart, OPCODE_JumpOnFalse));

                if (!ParseExpressionNode(rule->children[4], parser, obj, fobj, false)) {
                    ok = false;
                }


                if (rule->children.size() > 5) { // else
                    AscmNode* endElse = new AscmNode();
                    fobj.AddNode(rule->children[5], new AscmNodeJump(endElse, OPCODE_Jump));
                    fobj.AddNode(rule->children[6], elseStart);

                    if (!ParseExpressionNode(rule->children[6], parser, obj, fobj, false)) {
                        ok = false;
                    }

                    fobj.AddNode(rule->children[rule->children.size() - 1], endElse);
                }
                else {
                    fobj.AddNode(rule->children[rule->children.size() - 1], elseStart);
                }

                return ok;
            }
            case gscParser::RuleStatement_while: {
                bool ok{ true };

                bool extracted;
                int64_t v = obj.NumberNodeValue(rule->children[2], false, &extracted);

                if (!extracted || v) {
                    AscmNode* loopBreak = new AscmNode();
                    AscmNode* loopContinue = new AscmNode();

                    fobj.PushContinueNode(loopContinue);
                    fobj.PushBreakNode(loopBreak);

                    fobj.AddNode(rule, loopContinue);

                    if (!extracted) {
                        // the value is constantly true, we don't need to write the condition
                        if (!ParseExpressionNode(rule->children[2], parser, obj, fobj, true)) {
                            ok = false;
                        }

                        fobj.AddNode(rule->children[2], new AscmNodeJump(loopBreak, OPCODE_JumpOnFalse));
                    }

                    if (!ParseExpressionNode(rule->children[4], parser, obj, fobj, false)) {
                        ok = false;
                    }

                    // TODO: add next node
                    fobj.AddNode(rule->children[4], new AscmNodeJump(loopContinue, OPCODE_Jump));

                    fobj.AddNode(rule->children[4], loopBreak);

                    fobj.PopContinueNode();
                    fobj.PopBreakNode();
                }

                return ok;
            }
            case gscParser::RuleStatement_dowhile: {
                bool ok{ true };
                AscmNode* loopStart = new AscmNode();
                AscmNode* loopBreak = new AscmNode();
                AscmNode* loopContinue = new AscmNode();

                fobj.PushContinueNode(loopContinue);
                fobj.PushBreakNode(loopBreak);

                fobj.AddNode(rule, loopStart);

                if (!ParseExpressionNode(rule->children[1], parser, obj, fobj, false)) {
                    ok = false;
                }

                fobj.AddNode(rule, loopContinue);

                bool extracted;
                int64_t v = obj.NumberNodeValue(rule->children[4], false, &extracted);

                if (extracted) {
                    if (v) {
                        fobj.AddNode(rule->children[4], new AscmNodeJump(loopStart, OPCODE_Jump));
                    }
                }
                else {
                    if (!ParseExpressionNode(rule->children[4], parser, obj, fobj, true)) {
                        ok = false;
                    }
                    fobj.AddNode(rule->children[4], new AscmNodeJump(loopStart, OPCODE_JumpOnTrue));
                }


                // TODO: add next node
                fobj.AddNode(rule->children[4], loopBreak);

                fobj.PopContinueNode();
                fobj.PopBreakNode();

                return ok;
            }
            case gscParser::RuleStatement_for: {
                bool err{};
                size_t i = 2;

                // init expression
                if (IS_RULE_TYPE(rule->children[i], gscParser::RuleExpression) && !ParseExpressionNode(rule->children[i++], parser, obj, fobj, false)) {
                    err = true;
                }

                i++; // skip ';'
                AscmNode* loopStart = new AscmNode();
                AscmNode* loopBreak = new AscmNode();
                AscmNode* loopContinue = new AscmNode();

                fobj.AddNode(rule, loopStart);


                // if expression
                if (IS_RULE_TYPE(rule->children[i], gscParser::RuleExpression)) {
                    if (!ParseExpressionNode(rule->children[i++], parser, obj, fobj, true)) {
                        err = true;
                    }
                    else {
                        fobj.AddNode(rule->children[i - 1], new AscmNodeJump(loopBreak, OPCODE_JumpOnFalse));
                    }
                }

                i++; // skip ';'

                fobj.PushContinueNode(loopContinue);
                fobj.PushBreakNode(loopBreak);

                if (!ParseExpressionNode(rule->children[rule->children.size() - 1], parser, obj, fobj, false)) {
                    err = true;
                }

                fobj.PopContinueNode();
                fobj.PopBreakNode();

                // delta expression
                fobj.AddNode(rule->children[rule->children.size() - 1], loopContinue);

                if (IS_RULE_TYPE(rule->children[i], gscParser::RuleExpression) && !ParseExpressionNode(rule->children[i++], parser, obj, fobj, false)) {
                    err = true;
                }

                fobj.AddNode(rule->children[i - 1], new AscmNodeJump(loopStart, OPCODE_Jump));
                fobj.AddNode(rule->children[i - 1], loopBreak);

                return !err;
            }
            case gscParser::RuleStatement_switch: {
                //uint64_t switchType{ obj.gscHandler->buildFlags & tool::gsc::GOHF_SWITCH_TYPE_MASK };
                auto [err, var] = fobj.RegisterVarRnd();

                if (err) {
                    obj.info.PrintLineMessage(core::logs::LVL_ERROR, rule, std::format("Error when registering switch variable: {}", err));
                    return false;
                }

                /*
                  var = expression1

                  if (var == case1) goto LOC_1
                  if (var == case2) goto LOC_2
                  ...
                  goto LOC_DEF
                  LOC_1:
                    ...
                  LOC_2:
                    ...
                    ...
                  LOC_DEF:
                    ...
                  LOC_ENDSWITCH:
                    ...


                 */
                bool ok{ true };

                if (!ParseExpressionNode(rule->children[2], parser, obj, fobj, true)) {
                    ok = false;
                }

                fobj.AddNode(rule->children[2], new AscmNodeVariable(var->id, OPCODE_EvalLocalVariableRefCached));
                fobj.AddNode(rule->children[2], new AscmNodeOpCode(OPCODE_SetVariableField));


                // statement_switch: 'switch' '(' expression ')' '{' (('case' const_expr | 'default') ':' (statement)*)+'}';

                struct JumpData {
                    size_t startStmt;
                    size_t endStmt;
                    AscmNode* jmpNode;
                };

                // build jump table
                std::vector<JumpData> jmpTable{};
                JumpData defaultCase{ 0, 0, nullptr };

                bool findDefault{};
                for (size_t i = 5; i < rule->children.size() - 1;) {
                    std::string caseType = rule->children[i]->getText();

                    if (caseType == "case") {
                        i++; // 'case'
                        AscmNode* jmpNode = new AscmNode();

                        if (!ParseExpressionNode(rule->children[i], parser, obj, fobj, true)) {
                            ok = false;
                        }
                        fobj.AddNode(rule->children[i], new AscmNodeVariable(var->id, OPCODE_EvalLocalVariableCached));
                        fobj.AddNode(rule->children[i], new AscmNodeOpCode(OPCODE_Equal));


                        fobj.AddNode(rule->children[i], new AscmNodeJump(jmpNode, OPCODE_JumpOnTrue));

                        i += 2; // const_expr ':'

                        size_t start = i;
                        while (IS_RULE_TYPE(rule->children[i], gscParser::RuleStatement)) {
                            i++;
                        }
                        jmpTable.emplace_back(start, i, jmpNode);

                        continue;
                    }

                    if (caseType == "default") {
                        if (defaultCase.jmpNode) {
                            obj.info.PrintLineMessage(core::logs::LVL_ERROR, rule->children[i], "A switch can't have more than one default block");
                            ok = false;
                        }
                        else {
                            defaultCase.jmpNode = new AscmNode();
                        }
                        i += 2; // 'default' ':'
                        defaultCase.startStmt = i;
                        while (IS_RULE_TYPE(rule->children[i], gscParser::RuleStatement)) {
                            i++;
                        }
                        defaultCase.endStmt = i;
                        jmpTable.emplace_back(defaultCase);
                        continue;
                    }

                    obj.info.PrintLineMessage(core::logs::LVL_ERROR, rule->children[i], std::format("Unknown case type: {}/{}", caseType, i));
                    ok = false;
                    i++;
                }

                // push content
                AscmNode* endSwitchNode = new AscmNode();
                fobj.PushBreakNode(endSwitchNode);

                if (defaultCase.jmpNode) {
                    fobj.AddNode(rule, new AscmNodeJump(defaultCase.jmpNode, OPCODE_Jump));
                }
                else {
                    fobj.AddNode(rule, new AscmNodeJump(endSwitchNode, OPCODE_Jump));
                }

                for (JumpData& caseElem : jmpTable) {
                    fobj.AddNode(rule->children[caseElem.startStmt], caseElem.jmpNode);

                    for (size_t stmt = caseElem.startStmt; stmt < caseElem.endStmt; stmt++) {
                        if (!ParseExpressionNode(rule->children[stmt], parser, obj, fobj, false)) {
                            ok = false;
                        }
                    }
                }

                fobj.AddNode(rule, endSwitchNode);

                fobj.PopBreakNode();

                return ok;
            }
            case gscParser::RuleStatement_foreach: {
                auto [var1err, arrayVal] = fobj.RegisterVarRnd();

                if (var1err) {
                    obj.info.PrintLineMessage(core::logs::LVL_ERROR, rule, std::format("Error when registering foreach variable: {}", var1err));
                    return false;
                }

                // foreach (key, value in level.var_4afb8f5a) {

                FunctionVar* keyVar;
                FunctionVar* valueVar;

                if (IS_IDF(rule->children[4])) {
                    // key,value in
                    auto [var3err, keyVarL] = fobj.RegisterVar(rule->children[2]->getText(), true);
                    auto [var4err, valueVarL] = fobj.RegisterVar(rule->children[4]->getText(), true);

                    if (var3err) {
                        obj.info.PrintLineMessage(core::logs::LVL_ERROR, rule, std::format("Error when registering foreach key variable: {}", var3err));
                        return false;
                    }
                    if (var4err) {
                        obj.info.PrintLineMessage(core::logs::LVL_ERROR, rule, std::format("Error when registering foreach value variable: {}", var4err));
                        return false;
                    }

                    keyVar = keyVarL;
                    valueVar = valueVarL;
                }
                else {
                    // value in 
                    auto [var3err, keyVarL] = fobj.RegisterVarRnd();
                    auto [var4err, valueVarL] = fobj.RegisterVar(rule->children[2]->getText(), true);

                    if (var3err) {
                        obj.info.PrintLineMessage(core::logs::LVL_ERROR, rule, std::format("Error when registering foreach key variable: {}", var3err));
                        return false;
                    }
                    if (var4err) {
                        obj.info.PrintLineMessage(core::logs::LVL_ERROR, rule, std::format("Error when registering foreach value variable: {}", var4err));
                        return false;
                    }

                    keyVar = keyVarL;
                    valueVar = valueVarL;
                }

                bool ok{ true };

                ParseTree* arrVal = rule->children[rule->children.size() - 3];

                if (!ParseExpressionNode(arrVal, parser, obj, fobj, true)) {
                    ok = false;
                }

                uint64_t forEachType = obj.gscHandler->buildFlags & tool::gsc::GOHF_FOREACH_TYPE_MASK;

                switch (forEachType) {
                case tool::gsc::GOHF_FOREACH_TYPE_T8: {
                    auto [var2err, iteratorVal] = fobj.RegisterVarRnd();
                    if (var2err) {
                        obj.info.PrintLineMessage(core::logs::LVL_ERROR, rule, std::format("Error when registering foreach iterator variable: {}", var2err));
                        return false;
                    }

                    // array = ...;
                    fobj.AddNode(arrVal, new AscmNodeVariable(arrayVal->id, OPCODE_EvalLocalVariableRefCached));
                    fobj.AddNode(arrVal, new AscmNodeOpCode(OPCODE_SetVariableField));
                    // key = firstarray(array);
                    fobj.AddNode(arrVal, new AscmNodeVariable(arrayVal->id, OPCODE_FirstArrayKeyCached));
                    fobj.AddNode(arrVal, new AscmNodeVariable(keyVar->id, OPCODE_EvalLocalVariableRefCached));
                    fobj.AddNode(arrVal, new AscmNodeOpCode(OPCODE_SetVariableField));

                    AscmNode* loopBreak = new AscmNode();
                    AscmNode* loopContinue = new AscmNode();
                    AscmNode* loopIt = new AscmNode();

                    fobj.PushBreakNode(loopBreak);
                    fobj.PushContinueNode(loopContinue);

                    fobj.AddNode(arrVal, loopIt);
                    // jumpiffalse(isdefined(key)) loopBreak
                    fobj.AddNode(arrVal, new AscmNodeVariable(keyVar->id, OPCODE_EvalLocalVariableCached));
                    fobj.AddNode(arrVal, new AscmNodeOpCode(OPCODE_IsDefined));
                    fobj.AddNode(arrVal, new AscmNodeJump(loopBreak, OPCODE_JumpOnFalse));

                    // value = array[key];
                    fobj.AddNode(arrVal, new AscmNodeVariable(keyVar->id, OPCODE_EvalLocalVariableCached));
                    fobj.AddNode(arrVal, new AscmNodeVariable(arrayVal->id, OPCODE_EvalLocalVariableCached));
                    fobj.AddNode(arrVal, new AscmNodeOpCode(OPCODE_EvalArray));
                    fobj.AddNode(arrVal, new AscmNodeVariable(valueVar->id, OPCODE_EvalLocalVariableRefCached));
                    fobj.AddNode(arrVal, new AscmNodeOpCode(OPCODE_SetVariableField));

                    // nextarray(array, key, iterator)
                    fobj.AddNode(arrVal, new AscmNodeVariable(keyVar->id, OPCODE_EvalLocalVariableCached));
                    fobj.AddNode(arrVal, new AscmNodeVariable(arrayVal->id, OPCODE_EvalLocalVariableCached));
                    fobj.AddNode(arrVal, new AscmNodeVariable(iteratorVal->id, OPCODE_SetNextArrayKeyCached));

                    if (!ParseExpressionNode(rule->children[rule->children.size() - 1], parser, obj, fobj, false)) {
                        ok = false;
                    }

                    fobj.AddNode(rule->children[rule->children.size() - 1], loopContinue);
                    // key = iterator;
                    fobj.AddNode(rule->children[rule->children.size() - 1], new AscmNodeVariable(iteratorVal->id, OPCODE_EvalLocalVariableCached));
                    fobj.AddNode(rule->children[rule->children.size() - 1], new AscmNodeVariable(keyVar->id, OPCODE_EvalLocalVariableRefCached));
                    fobj.AddNode(rule->children[rule->children.size() - 1], new AscmNodeOpCode(OPCODE_SetVariableField));

                    // loop back
                    fobj.AddNode(rule->children[rule->children.size() - 1], new AscmNodeJump(loopIt, OPCODE_Jump));

                    fobj.PopBreakNode();
                    fobj.PopContinueNode();
                    // end
                    fobj.AddNode(rule->children[rule->children.size() - 1], loopBreak);
                }
                                                    break;
                case tool::gsc::GOHF_FOREACH_TYPE_T9: {
                    auto [varnexterr, nextVar] = fobj.RegisterVarRnd();

                    if (varnexterr) {
                        obj.info.PrintLineMessage(core::logs::LVL_ERROR, rule, std::format("Error when registering foreach next variable: {}", varnexterr));
                        ok = false;
                    }

                    auto [var2err, iteratorVal] = fobj.RegisterVarRnd();
                    if (var2err) {
                        obj.info.PrintLineMessage(core::logs::LVL_ERROR, rule, std::format("Error when registering foreach iterator variable: {}", var2err));
                        return false;
                    }

                    // array = ...;
                    fobj.AddNode(arrVal, new AscmNodeVariable(arrayVal->id, OPCODE_EvalLocalVariableRefCached));
                    fobj.AddNode(arrVal, new AscmNodeOpCode(OPCODE_SetVariableField));
                    // iterator = firstarray(array);
                    fobj.AddNode(arrVal, new AscmNodeVariable(arrayVal->id, OPCODE_EvalLocalVariableCached));
                    fobj.AddNode(arrVal, new AscmNodeOpCode(OPCODE_FirstArrayKey));
                    fobj.AddNode(arrVal, new AscmNodeVariable(iteratorVal->id, OPCODE_SetLocalVariableCached));

                    AscmNode* loopBreak = new AscmNode();
                    AscmNode* loopContinue = new AscmNode();
                    AscmNode* loopIt = new AscmNode();

                    fobj.PushBreakNode(loopBreak);
                    fobj.PushContinueNode(loopContinue);

                    fobj.AddNode(arrVal, loopIt);
                    // jumpiffalse(isdefined(iterator)) loopBreak
                    fobj.AddNode(arrVal, new AscmNodeVariable(iteratorVal->id, OPCODE_EvalLocalVariableCached));
                    fobj.AddNode(arrVal, new AscmNodeOpCode(OPCODE_IsDefined));
                    fobj.AddNode(arrVal, new AscmNodeJump(loopBreak, OPCODE_JumpOnFalse));

                    // key = iteratorkey(iterator);
                    fobj.AddNode(arrVal, new AscmNodeVariable(iteratorVal->id, OPCODE_EvalLocalVariableCached));
                    fobj.AddNode(arrVal, new AscmNodeOpCode(OPCODE_T9_IteratorKey));
                    fobj.AddNode(arrVal, new AscmNodeVariable(keyVar->id, OPCODE_SetLocalVariableCached));

                    // val = iteratorval(iterator);
                    fobj.AddNode(arrVal, new AscmNodeVariable(iteratorVal->id, OPCODE_EvalLocalVariableCached));
                    fobj.AddNode(arrVal, new AscmNodeOpCode(OPCODE_T9_IteratorVal));
                    fobj.AddNode(arrVal, new AscmNodeVariable(valueVar->id, OPCODE_SetLocalVariableCached));

                    // next = iteratornext(iterator);
                    fobj.AddNode(arrVal, new AscmNodeVariable(iteratorVal->id, OPCODE_EvalLocalVariableCached));
                    fobj.AddNode(arrVal, new AscmNodeOpCode(OPCODE_T9_IteratorNext));
                    fobj.AddNode(arrVal, new AscmNodeVariable(nextVar->id, OPCODE_SetLocalVariableCached));

                    if (!ParseExpressionNode(rule->children[rule->children.size() - 1], parser, obj, fobj, false)) {
                        ok = false;
                    }

                    fobj.AddNode(rule->children[rule->children.size() - 1], loopContinue);
                    // key = iterator;
                    fobj.AddNode(rule->children[rule->children.size() - 1], new AscmNodeVariable(nextVar->id, OPCODE_EvalLocalVariableCached));
                    fobj.AddNode(rule->children[rule->children.size() - 1], new AscmNodeVariable(iteratorVal->id, OPCODE_EvalLocalVariableRefCached));
                    fobj.AddNode(rule->children[rule->children.size() - 1], new AscmNodeOpCode(OPCODE_SetVariableField));

                    // loop back
                    fobj.AddNode(rule->children[rule->children.size() - 1], new AscmNodeJump(loopIt, OPCODE_Jump));

                    fobj.PopBreakNode();
                    fobj.PopContinueNode();
                    // end
                    fobj.AddNode(rule->children[rule->children.size() - 1], loopBreak);
                }
                                                    break;
                case tool::gsc::GOHF_FOREACH_TYPE_JUP: {
                    // array = ...;
                    fobj.AddNode(arrVal, new AscmNodeVariable(arrayVal->id, OPCODE_SetLocalVariableCached));
                    // key = getfirstarraykey(array);
                    fobj.AddNode(arrVal, new AscmNodeOpCode(OPCODE_PreScriptCall));
                    fobj.AddNode(arrVal, new AscmNodeVariable(arrayVal->id, OPCODE_EvalLocalVariableCached));
                    uint64_t fakHash = obj.vmInfo->HashField("getfirstarraykey");
                    AscmNodeFunctionCall* fakNode = new AscmNodeFunctionCall(OPCODE_CallBuiltinFunction, 0, 1, fakHash, obj.currentNamespace, obj.vmInfo);
                    obj.AddImport(fakNode, obj.currentNamespace, fakHash, 1, tool::gsc::FUNCTION | tool::gsc::GET_CALL);
                    fobj.AddNode(arrVal, fakNode);
                    fobj.AddNode(arrVal, new AscmNodeVariable(keyVar->id, OPCODE_SetLocalVariableCached));

                    AscmNode* loopBreak = new AscmNode();
                    AscmNode* loopContinue = new AscmNode();
                    AscmNode* loopIt = new AscmNode();

                    fobj.PushBreakNode(loopBreak);
                    fobj.PushContinueNode(loopContinue);

                    fobj.AddNode(arrVal, loopIt);
                    // jumpiffalse(isdefined(key)) loopBreak
                    fobj.AddNode(arrVal, new AscmNodeVariable(keyVar->id, OPCODE_EvalLocalVariableCached));
                    fobj.AddNode(arrVal, new AscmNodeOpCode(OPCODE_IsDefined));
                    fobj.AddNode(arrVal, new AscmNodeJump(loopBreak, OPCODE_JumpOnFalse));

                    // val = array[key];
                    fobj.AddNode(arrVal, new AscmNodeVariable(keyVar->id, OPCODE_EvalLocalVariableCached));
                    fobj.AddNode(arrVal, new AscmNodeVariable(arrayVal->id, OPCODE_EvalLocalVariableCached));
                    fobj.AddNode(arrVal, new AscmNodeOpCode(OPCODE_EvalArray));
                    fobj.AddNode(arrVal, new AscmNodeVariable(valueVar->id, OPCODE_SetLocalVariableCached));


                    if (!ParseExpressionNode(rule->children[rule->children.size() - 1], parser, obj, fobj, false)) {
                        ok = false;
                    }

                    fobj.AddNode(rule->children[rule->children.size() - 1], loopContinue);
                    // key = getnextarraykey(array, key);
                    fobj.AddNode(arrVal, new AscmNodeOpCode(OPCODE_PreScriptCall));
                    fobj.AddNode(rule->children[rule->children.size() - 1], new AscmNodeVariable(keyVar->id, OPCODE_EvalLocalVariableCached));
                    fobj.AddNode(rule->children[rule->children.size() - 1], new AscmNodeVariable(arrayVal->id, OPCODE_EvalLocalVariableCached));

                    uint64_t gakHash = obj.vmInfo->HashField("getnextarraykey");
                    AscmNodeFunctionCall* gakNode = new AscmNodeFunctionCall(OPCODE_CallBuiltinFunction, 0, 2, gakHash, obj.currentNamespace, obj.vmInfo);
                    obj.AddImport(gakNode, obj.currentNamespace, gakHash, 2, tool::gsc::FUNCTION | tool::gsc::GET_CALL);
                    fobj.AddNode(arrVal, gakNode);
                    fobj.AddNode(rule->children[rule->children.size() - 1], new AscmNodeVariable(keyVar->id, OPCODE_SetLocalVariableCached));

                    // loop back
                    fobj.AddNode(rule->children[rule->children.size() - 1], new AscmNodeJump(loopIt, OPCODE_Jump));

                    fobj.PopBreakNode();
                    fobj.PopContinueNode();
                    // end
                    fobj.AddNode(rule->children[rule->children.size() - 1], loopBreak);

                    //fobj.AddNode(rule->children[rule->children.size() - 1], new AscmNodeVariable(keyVar->id, OPCODE_IW_ClearFieldVariableRef));
                    //fobj.AddNode(rule->children[rule->children.size() - 1], new AscmNodeVariable(arrayVal->id, OPCODE_IW_ClearFieldVariableRef));
                }
                                                     break;
                default:
                    obj.info.PrintLineMessage(core::logs::LVL_ERROR, rule, std::format("foreach not implemented for vm {} (0x{:x})", obj.vmInfo->name, forEachType));
                    ok = false;
                    break;
                }

                return ok;
            }
            case gscParser::RuleNop_def: {
                if (expressVal) {
                    obj.info.PrintLineMessage(core::logs::LVL_ERROR, rule, "Can't express a nop value");
                    return false;
                }
                size_t count{ 1 };
                if (rule->children.size() > 1) {
                    int64_t val{ obj.NumberNodeValue(rule->children[2]) };
                    if (val < 0) {
                        obj.info.PrintLineMessage(core::logs::LVL_ERROR, rule, "Can't define a negative amount of nop");
                        return false;
                    }
                    count = (size_t)val;
                }

                for (size_t i = 0; i < count; i++) {
                    fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_Nop));
                }

                return true;
            }
            case gscParser::RuleArray_unpack: {
                auto [varerr, arrVar] = fobj.RegisterVarRnd();
                if (varerr) {
                    obj.info.PrintLineMessage(core::logs::LVL_ERROR, rule, std::format("Error when registering array unpack variable: {}", varerr));
                    return false;
                }

                if (!ParseExpressionNode(rule->children[rule->children.size() - 1], parser, obj, fobj, true)) {
                    return false;
                }
                fobj.AddNode(rule, new AscmNodeVariable(arrVar->id, OPCODE_EvalLocalVariableRefCached));
                fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_SetVariableField));

                size_t numEntries{ 1 + (rule->children.size() - 5) / 2 };

                for (int i = (int)(numEntries - 1); i >= 0; i--) {
                    auto* child{ rule->children[1 + i * 2] };
                    const std::string unpackName{ child->getText() };

                    auto [varerr2, arrVar2] = fobj.RegisterVar(unpackName, true);
                    if (varerr2) {
                        obj.info.PrintLineMessage(core::logs::LVL_ERROR, child, std::format("Error when registering array unpack variable {}: {}", unpackName, varerr2));
                        return false;
                    }

                    fobj.AddNode(child, obj.BuildAscmNodeData(i));
                    fobj.AddNode(child, new AscmNodeVariable(arrVar->id, OPCODE_EvalLocalVariableCached));
                    fobj.AddNode(child, new AscmNodeOpCode(OPCODE_EvalArray));
                    fobj.AddNode(child, new AscmNodeVariable(arrVar2->id, OPCODE_EvalLocalVariableRefCached));
                    fobj.AddNode(child, new AscmNodeOpCode(OPCODE_SetVariableField));
                }

                return true;
            }
            case gscParser::RuleDevop_def: {
                if (expressVal) {
                    obj.info.PrintLineMessage(core::logs::LVL_ERROR, rule, "Can't express a devop value");
                    return false;
                }
                int64_t val{ obj.NumberNodeValue(rule->children[2]) };
                if (val < 0) {
                    obj.info.PrintLineMessage(core::logs::LVL_ERROR, rule, "Invalid devop value (Negative)");
                    return false;
                }
                if (obj.vmInfo->HasFlag(VmFlags::VMF_OPCODE_U16)) {
                    if (val > 0xFFFF) {
                        obj.info.PrintLineMessage(core::logs::LVL_ERROR, rule, "Invalid devop value (Too large)");
                        return false;
                    }
                }
                else {
                    if (val > 0xFF) {
                        obj.info.PrintLineMessage(core::logs::LVL_ERROR, rule, "Invalid devop value (Too large)");
                        return false;
                    }
                }

                fobj.AddNode(rule, new AscmNodeRawOpCode((uint16_t)val));

                return true;
            }
            case gscParser::RuleStatement_inst: {
                if (rule->children.size() == 1) {
                    if (expressVal) {
                        obj.info.PrintLineMessage(core::logs::LVL_ERROR, rule, "Can't express this value");
                        return false;
                    }
                    return true; // empty instruction
                }
                return ParseExpressionNode(rule->children[0], parser, obj, fobj, expressVal);
            }
            case gscParser::RuleStatement_block: {
                if (expressVal) {
                    obj.info.PrintLineMessage(core::logs::LVL_ERROR, rule, "Can't express this value");
                    return false;
                }
                bool ok{ true };
                for (size_t i = 1; i < rule->children.size() - 1; i++) {
                    ParseTree* stmt = rule->children[i];

                    if (!ParseExpressionNode(stmt, parser, obj, fobj, false)) {
                        ok = false; // do not return false so we can have multiple issues
                    }
                }
                return ok;
            }
            case gscParser::RuleStatement_dev_block: {
                if (expressVal) {
                    obj.info.PrintLineMessage(core::logs::LVL_ERROR, rule, "Can't express this value");
                    return false;
                }
                bool ok{ true };
                AscmNode* endBlock = new AscmNode();
                AscmNode* djmp = new AscmNodeJump(endBlock, OPCODE_DevblockBegin);
                obj.m_devBlocks.push_back(djmp);
                fobj.AddNode(rule->children[0], djmp);
                obj.devBlockDepth++;
                for (size_t i = 1; i < rule->children.size() - 1; i++) {
                    ParseTree* stmt = rule->children[i];

                    if (!ParseExpressionNode(stmt, parser, obj, fobj, false)) {
                        ok = false;
                    }
                }
                obj.devBlockDepth--;
                fobj.AddNode(rule->children[rule->children.size() - 1], endBlock);
                return ok;
            }
            case gscParser::RuleOperator_inst: {
                if (expressVal) {
                    obj.info.PrintLineMessage(core::logs::LVL_ERROR, rule, "Can't express this value");
                    return false;
                }
                std::string idf = rule->children[0]->getText();

                if (idf == "break") {
                    if (rule->children.size() > 1) {
                        obj.info.PrintLineMessage(core::logs::LVL_ERROR, rule, "Can't specify jump location with break");
                        return false;
                    }

                    AscmNode* loc = fobj.PeekBreakNode();

                    if (!loc) {
                        obj.info.PrintLineMessage(core::logs::LVL_ERROR, rule, "Not in a break context");
                        return false;
                    }

                    fobj.AddNode(rule, new AscmNodeJump(loc, OPCODE_Jump));
                    return true;
                }

                if (idf == "continue") {
                    if (rule->children.size() > 1) {
                        obj.info.PrintLineMessage(core::logs::LVL_ERROR, rule, "Can't specify jump location with continue");
                        return false;
                    }

                    AscmNode* loc = fobj.PeekContinueNode();

                    if (!loc) {
                        obj.info.PrintLineMessage(core::logs::LVL_ERROR, rule, "Not in a continue context");
                        return false;
                    }

                    fobj.AddNode(rule, new AscmNodeJump(loc, OPCODE_Jump));
                    return true;
                }

                if (idf == "goto") {
                    if (rule->children.size() <= 1 && !IS_IDF(rule->children[1])) {
                        obj.info.PrintLineMessage(core::logs::LVL_ERROR, rule, "goto should be used with a jump location");
                        return false;
                    }

                    FunctionJumpLoc& loc = fobj.m_jumpLocs[rule->children[1]->getText()];
                    if (!loc.node) {
                        loc.node = new AscmNode();
                    }
                    if (!loc.def) {
                        loc.def = rule;
                    }

                    fobj.AddNode(rule, new AscmNodeJump(loc.node, OPCODE_Jump));
                    return true;
                }

                if (idf == "jumpdev") {
                    if (rule->children.size() <= 1 && !IS_IDF(rule->children[1])) {
                        obj.info.PrintLineMessage(core::logs::LVL_ERROR, rule, "jumpdev should be used with a jump location");
                        return false;
                    }

                    FunctionJumpLoc& loc = fobj.m_jumpLocs[rule->children[1]->getText()];
                    if (!loc.node) {
                        loc.node = new AscmNode();
                    }
                    if (!loc.def) {
                        loc.def = rule;
                    }

                    AscmNode* djmp = new AscmNodeJump(loc.node, OPCODE_DevblockBegin);
                    obj.m_devBlocks.push_back(djmp);
                    fobj.AddNode(rule, djmp);
                    return true;
                }

                if (idf == "return") {
                    if (rule->children.size() <= 1) {
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_End));
                        return true;
                    }

                    if (!ParseExpressionNode(rule->children[1], parser, obj, fobj, true)) {
                        return false;
                    }

                    fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_Return));
                    return true;
                }

                if (idf == "wait") {
                    if (rule->children.size() <= 1) {
                        obj.info.PrintLineMessage(core::logs::LVL_ERROR, rule, "wait should be used with a value");
                        return false;
                    }

                    if (!ParseExpressionNode(rule->children[1], parser, obj, fobj, true)) {
                        return false;
                    }

                    fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_Wait));
                    return true;
                }

                obj.info.PrintLineMessage(core::logs::LVL_ERROR, rule, std::format("Unknown operator type {}", idf));
                return false;
            }
            case gscParser::RuleFunction_call:
            case gscParser::RuleFunction_call_exp: {
                if (rule->children.size() == 1) {
                    return ParseExpressionNode(rule->children[0], parser, obj, fobj, expressVal);
                }
                uint64_t compilerCallHash{};
                uint64_t funcNspHash{};
                uint64_t funcHash{};
                ParseTree* ptrTree{};
                ParseTree* selfTree{};
                AscmNode* devCallInlineEnd{};

                int flags{};
                byte importFlags{};
                size_t idx{};
                if (rule->children[idx]->getTreeType() == TREE_RULE && !IS_RULE_TYPE(rule->children[idx], gscParser::RuleFunction_component)) {
                    selfTree = rule->children[idx];
                    flags |= FCF_METHOD;
                    idx++;
                }
                if (rule->children[idx]->getTreeType() == TREE_TERMINAL) {
                    std::string callModifier = rule->children[idx]->getText();
                    if (callModifier == "thread") {
                        flags |= FCF_THREAD;
                    }
                    else if (callModifier == "childthread" || callModifier == "threadendon") { // old compatibility
                        flags |= FCF_CHILDTHREAD;
                    }
                    else if (callModifier == "builtin") {
                        flags |= FCF_BUILTIN;
                    }
                    else {
                        obj.info.PrintLineMessage(core::logs::LVL_ERROR, rule->children[idx], std::format("Unknown call modifier {}", callModifier));
                        return false;
                    }
                    idx++;
                }
                ParseTree* functionComp = rule->children[idx];

                if (!IS_RULE_TYPE(functionComp, gscParser::RuleFunction_component)) {
                    obj.info.PrintLineMessage(core::logs::LVL_ERROR, functionComp, std::format("Not a function component {}", functionComp->getText()));
                    return false;
                }

                ParseTree* paramsList = rule->children[rule->children.size() - 2];
                if (!IS_RULE_TYPE(paramsList, gscParser::RuleExpression_list)) {
                    obj.info.PrintLineMessage(core::logs::LVL_ERROR, paramsList, std::format("Not a params list {}", paramsList->getText()));
                    return false;
                }

                if (functionComp->children.size() == 1) {
                    // func
                    importFlags |= tool::gsc::GET_CALL;
                    funcNspHash = obj.currentNamespace;
                    std::string funcName = functionComp->children[0]->getText();

                    funcHash = obj.vmInfo->HashField(funcName.c_str());

                    auto funcIt = obj.vmInfo->opFuncs.find(funcHash);

                    if (funcIt != obj.vmInfo->opFuncs.end()) {
                        // internal function call
                        FunctionOperator& f = funcIt->second;
                        if (f.HasFlag(tool::gsc::opcode::VPFD_SELF_PARAM) && !(flags & FCF_METHOD)) {
                            obj.info.PrintLineMessage(core::logs::LVL_ERROR, functionComp, std::format("Operator '{}' should have a caller, Usage: {}", funcName, f.usage));
                            return false;
                        }
                        if (expressVal && !f.HasFlag(tool::gsc::opcode::VPFD_RETURN_VALUE)) {
                            obj.info.PrintLineMessage(core::logs::LVL_ERROR, functionComp, std::format("Operator '{}' doesn't return a value, Usage: {}", funcName, f.usage));
                            return false;
                        }
                        if (flags & (FCF_THREAD | FCF_CHILDTHREAD | FCF_BUILTIN)) {
                            obj.info.PrintLineMessage(core::logs::LVL_ERROR, functionComp, std::format("Operator '{}' can't have a call modifier, Usage: {}", funcName, f.usage));
                            return false;
                        }

                        if (f.HasFlag(tool::gsc::opcode::VPFD_USE_PRE_SCRIPT_CALL)) {
                            fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_PreScriptCall));
                        }

                        bool paramError{};
                        int paramCount{};
                        uint64_t hashVal{};
                        std::string strVal{};

                        if (f.HasFlag(tool::gsc::opcode::VPFD_UNPACK)) {
                            if (expressVal) {
                                obj.info.PrintLineMessage(core::logs::LVL_ERROR, functionComp, std::format("Operator '{}' can't express a value, Usage: {}", funcName, f.usage));
                                paramError = true;
                            }
                            else if (!paramsList->children.size()) {
                                obj.info.PrintLineMessage(core::logs::LVL_ERROR, functionComp, std::format("Operator '{}' needs at least one param, Usage: {}", funcName, f.usage));
                                paramError = true;
                            }
                            else {
                                if (!ParseExpressionNode(paramsList->children[0], parser, obj, fobj, true)) {
                                    paramError = true;
                                }

                                paramCount++;

                                for (int i = (int)paramsList->children.size() - 1; i > 1; i -= 2) {
                                    ParseTree* pt = paramsList->children[i];
                                    while (pt && pt->getTreeType() == TREE_RULE && pt->children.size() == 1) {
                                        pt = pt->children[0];
                                    }
                                    if (!IS_IDF(pt)) {
                                        obj.info.PrintLineMessage(core::logs::LVL_ERROR, functionComp, std::format("Operator '{}' needs to be unpacked with variables, Usage: {}", funcName, f.usage));
                                        return false;
                                    }
                                    paramCount++;
                                }
                            }
                        }
                        else {
                            int removedStart{};
                            if (f.HasFlag(VPFD_HASH_PARAM)) {
                                if (paramsList->children.empty()) {
                                    obj.info.PrintLineMessage(core::logs::LVL_ERROR, functionComp, std::format("Operator '{}' needs to have at least one param, Usage: {}", funcName, f.usage));
                                    return false;
                                }

                                ParseTree* hashParam{ paramsList->children[removedStart] };

                                if (!obj.TryHashNodeValue(hashParam, hashVal)) {
                                    obj.info.PrintLineMessage(core::logs::LVL_ERROR, functionComp, std::format("Operator '{}' should start with a valid hash param, Usage: {}", funcName, f.usage));
                                    return false;
                                }

                                removedStart += 2; // use the first param as hash
                            }
                            if (f.HasFlag(VPFD_STRING_PARAM)) {
                                if (paramsList->children.empty()) {
                                    obj.info.PrintLineMessage(core::logs::LVL_ERROR, functionComp, std::format("Operator '{}' needs to have at least one param, Usage: {}", funcName, f.usage));
                                    return false;
                                }

                                ParseTree* strParam{ paramsList->children[removedStart] };

                                if (!obj.TryStringNodeValue(strParam, strVal)) {
                                    obj.info.PrintLineMessage(core::logs::LVL_ERROR, functionComp, std::format("Operator '{}' should have a valid string param, Usage: {}", funcName, f.usage));
                                    return false;
                                }

                                removedStart += 2; // use the first param as hash
                            }
                            for (int i = (int)paramsList->children.size() - 1; i >= removedStart; i -= 2) {
                                if (!ParseExpressionNode(paramsList->children[i], parser, obj, fobj, true)) {
                                    paramError = true;
                                }
                                paramCount++;
                            }
                        }

                        // add self
                        if (flags & FCF_METHOD) {
                            if (!ParseExpressionNode(selfTree, parser, obj, fobj, true)) {
                                obj.info.PrintLineMessage(core::logs::LVL_ERROR, functionComp, "Error when parsing caller");
                                return false;
                            }
                        }

                        if (paramError) {
                            obj.info.PrintLineMessage(core::logs::LVL_ERROR, paramsList, "Error when parsing param list");
                            return false;
                        }

                        if (paramCount < f.minParam) {
                            obj.info.PrintLineMessage(core::logs::LVL_ERROR, paramsList, std::format("Not enought params for operator '{}', Usage: {}", funcName, f.usage));
                            return false;
                        }

                        if (paramCount > f.maxParam) {
                            obj.info.PrintLineMessage(core::logs::LVL_ERROR, paramsList, std::format("Too many params for operator '{}', Usage: {}", funcName, f.usage));
                            return false;
                        }
                        fobj.AddNode(rule, new AscmNodeOpCode(f.opCode));

                        if (f.HasFlag(tool::gsc::opcode::VPFD_USE_COUNT)) {
                            fobj.AddNode(rule, new AscmNodeRawData<byte>((byte)paramCount));
                        }
                        if (f.HasFlag(VPFD_HASH_PARAM)) {
                            fobj.AddNode(rule, new AscmNodeRawData<uint64_t>(hashVal));
                        }
                        if (f.HasFlag(VPFD_STRING_PARAM)) {
                            AscmNodeRaw* node = new AscmNodeRaw();
                            node->data.insert(node->data.end(), (byte*)strVal.data(), (byte*)strVal.data() + strVal.size() + 1);
                            fobj.AddNode(rule, node);
                        }

                        if (!expressVal && f.HasFlag(tool::gsc::opcode::VPFD_RETURN_VALUE)) {
                            fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_DecTop));
                        }

                        if (f.HasFlag(tool::gsc::opcode::VPFD_UNPACK)) {
                            for (size_t i = 1; i < paramCount; i++) {
                                ParseTree* pnode = paramsList->children[i * 2];
                                auto [errVar, var] = fobj.RegisterVar(pnode->getText(), true);
                                if (errVar) {
                                    obj.info.PrintLineMessage(core::logs::LVL_ERROR, pnode, std::format("Can't register variable: {}", errVar));
                                    return false;
                                }

                                // register var
                                fobj.AddNode(pnode, new AscmNodeVariable(var->id, OPCODE_IW_SetWaittillVariableFieldCached));
                            }

                            fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_ClearParams));
                        }

                        return true;
                    }
                    // test dev call
                    if (!obj.config.noDevCallInline && !obj.devBlockDepth && fobj.m_vmInfo->devCallsNames.find(funcHash & hash::MASK60) != fobj.m_vmInfo->devCallsNames.end()) {
                        if (!expressVal) { // can't inline a returned value
                            devCallInlineEnd = new AscmNode();
                            fobj.AddNode(rule, new AscmNodeJump(devCallInlineEnd, OPCODE_DevblockBegin));

                            obj.devBlockDepth++;
                        }
                    }
                    obj.AddHash(funcName);
                }
                else if (functionComp->children.size() == 3) {
                    // namespace::func

                    std::string funcNspName = functionComp->children[0]->getText();
                    std::string funcName = functionComp->children[2]->getText();

                    obj.AddHash(funcName);
                    funcHash = obj.vmInfo->HashField(funcName.c_str());

                    if (IS_TERMINAL_TYPE(functionComp->children[0], gscParser::PATH)) {
                        // does not accept hashed file names
                        if (!obj.vmInfo->HasFlag(VmFlags::VMF_FULL_FILE_NAMESPACE)) {
                            // technically we can assume that the filename and the namespace are the same, but it wouldn't be
                            // a good thing to allow.
                            obj.info.PrintLineMessage(core::logs::LVL_ERROR, paramsList, "");
                            return false;
                        }

                        obj.AddInclude(funcNspName);
                        obj.AddHash(funcNspName);
                        importFlags |= tool::gsc::ACTS_USE_FULL_NAMESPACE;
                        funcNspHash = obj.vmInfo->HashField(funcNspName.c_str());
                    }
                    else if (utils::EqualIgnoreCase("compiler", funcNspName)) {
                        compilerCallHash = funcHash;
                        importFlags |= tool::gsc::GET_CALL;
                        funcNspHash = obj.currentNamespace;

                        if (!obj.vmInfo->compilerHookFunctionName) {
                            obj.info.PrintLineMessage(core::logs::LVL_ERROR, paramsList, "no compiler:: function hook name defined for this vm");
                            return false;
                        }
                        funcHash = obj.vmInfo->compilerHookFunctionName;
                    }
                    else {
                        obj.AddHash(funcNspName);
                        funcNspHash = obj.vmInfo->HashField(funcNspName.c_str());
                    }
                }
                else if (functionComp->children.size() == 5) {
                    // [ [ espression ] ]
                    ptrTree = functionComp->children[2];
                    flags |= FCF_POINTER;
                }
                else if (functionComp->children.size() == 7) {
                    if (flags & FCF_METHOD) {
                        obj.info.PrintLineMessage(core::logs::LVL_ERROR, functionComp, "A class call can't have a self caller");
                        return false;
                    }
                    // [ [ espression ] ] -> func
                    std::string funcName = functionComp->children[6]->getText();
                    obj.AddHash(funcName);
                    funcHash = obj.vmInfo->HashField(funcName.c_str());

                    ptrTree = functionComp->children[2];
                    flags |= FCF_POINTER_CLASS | FCF_POINTER;
                }
                else {
                    obj.info.PrintLineMessage(core::logs::LVL_ERROR, functionComp, std::format("Function call not implemented {}", functionComp->getText()));
                    return false;
                }

                fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_PreScriptCall));

                // push params
                bool paramError{};
                int paramCount{};
                for (int i = (int)paramsList->children.size() - 1; i >= 0; i -= 2) {
                    if (!ParseExpressionNode(paramsList->children[i], parser, obj, fobj, true)) {
                        paramError = true;
                    }
                    paramCount++;
                }

                if (compilerCallHash) {
                    // compiler::test(1, 2, 3) = hookfunc(#test, 1, 2, 3)
                    fobj.AddNode(functionComp->children[2], obj.BuildAscmNodeData((int64_t)compilerCallHash));
                    paramCount++;
                }

                // add self
                if (flags & FCF_METHOD) {
                    if (!ParseExpressionNode(selfTree, parser, obj, fobj, true)) {
                        obj.info.PrintLineMessage(core::logs::LVL_ERROR, functionComp, "Error when parsing caller");
                        return false;
                    }
                }

                // add ptr
                if (flags & FCF_POINTER) {
                    if (!ParseExpressionNode(ptrTree, parser, obj, fobj, true)) {
                        obj.info.PrintLineMessage(core::logs::LVL_ERROR, functionComp, "Error when parsing pointer");
                        return false;
                    }
                }

                if (paramError) {
                    obj.info.PrintLineMessage(core::logs::LVL_ERROR, paramsList, "Error when parsing param list");
                    return false;
                }

                if (paramCount >= 256) {
                    obj.info.PrintLineMessage(core::logs::LVL_ERROR, paramsList, "Too many parameters for call");
                    return false;
                }

                OPCode opcode;
                if (flags & FCF_POINTER) {
                    if (flags & FCF_POINTER_CLASS) {

                        if (flags & FCF_THREAD) {
                            opcode = OPCODE_ClassFunctionThreadCall;
                        }
                        else if (flags & FCF_CHILDTHREAD) {
                            opcode = OPCODE_ClassFunctionThreadCallEndOn;
                        }
                        else if (flags & FCF_BUILTIN) {
                            obj.info.PrintLineMessage(core::logs::LVL_ERROR, paramsList, "Class pointer can't be used with builtin calls");
                            return false;
                        }
                        else {
                            opcode = OPCODE_ClassFunctionCall;
                        }
                    }
                    else {
                        if (flags & FCF_METHOD) {
                            if (flags & FCF_THREAD) {
                                opcode = OPCODE_ScriptMethodThreadCallPointer;
                            }
                            else if (flags & FCF_CHILDTHREAD) {
                                opcode = OPCODE_ScriptMethodThreadCallPointerEndOn;
                            }
                            else if (flags & FCF_BUILTIN) {
                                opcode = OPCODE_IW_BuiltinMethodCallPointer;
                            }
                            else {
                                opcode = OPCODE_ScriptMethodCallPointer;
                            }
                        }
                        else {
                            if (flags & FCF_THREAD) {
                                opcode = OPCODE_ScriptThreadCallPointer;
                            }
                            else if (flags & FCF_CHILDTHREAD) {
                                opcode = OPCODE_ScriptThreadCallPointerEndOn;
                            }
                            else if (flags & FCF_BUILTIN) {
                                opcode = OPCODE_IW_BuiltinFunctionCallPointer;
                            }
                            else {
                                opcode = OPCODE_ScriptFunctionCallPointer;
                            }
                        }
                    }
                    fobj.AddNode(rule, new AscmNodeFunctionCall(opcode, flags, (byte)paramCount, funcHash, funcNspHash, obj.vmInfo));
                }
                else {
                    if (flags & FCF_METHOD) {
                        if (flags & FCF_THREAD) {
                            opcode = OPCODE_ScriptMethodThreadCall;
                            importFlags |= tool::gsc::T8GSCImportFlags::METHOD_THREAD;
                        }
                        else if (flags & FCF_CHILDTHREAD) {
                            opcode = OPCODE_ScriptMethodThreadCallEndOn;
                            importFlags |= tool::gsc::T8GSCImportFlags::METHOD_CHILDTHREAD;
                        }
                        else if (flags & FCF_BUILTIN) {
                            obj.info.PrintLineMessage(core::logs::LVL_ERROR, paramsList, "builtin modifier can only be used with pointer calls");
                            return false;
                        }
                        else {
                            opcode = OPCODE_ScriptMethodCall;
                            importFlags |= tool::gsc::T8GSCImportFlags::METHOD;
                        }
                    }
                    else {
                        if (flags & FCF_THREAD) {
                            opcode = OPCODE_ScriptThreadCall;
                            importFlags |= tool::gsc::T8GSCImportFlags::FUNCTION_THREAD;
                        }
                        else if (flags & FCF_CHILDTHREAD) {
                            opcode = OPCODE_ScriptThreadCallEndOn;
                            importFlags |= tool::gsc::T8GSCImportFlags::FUNCTION_CHILDTHREAD;
                        }
                        else if (flags & FCF_BUILTIN) {
                            obj.info.PrintLineMessage(core::logs::LVL_ERROR, paramsList, "builtin modifier can only be used with pointer calls");
                            return false;
                        }
                        else {
                            opcode = OPCODE_ScriptFunctionCall;
                            importFlags |= tool::gsc::T8GSCImportFlags::FUNCTION;
                        }
                    }

                    AscmNodeFunctionCall* funcCall = new AscmNodeFunctionCall(opcode, flags, (byte)paramCount, funcHash, funcNspHash, obj.vmInfo);

                    if (!(flags & FCF_POINTER_CLASS)) {
                        // class aren't imported
                        obj.AddImport(funcCall, funcNspHash, funcHash, paramCount, importFlags);
                    }

                    fobj.AddNode(rule, funcCall);
                }


                if (!expressVal) {
                    fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_DecTop));
                }
                if (devCallInlineEnd) {
                    obj.devBlockDepth--;
                    fobj.AddNode(rule, devCallInlineEnd);
                }
                return true;
            }
            case gscParser::RuleExpression:
            case gscParser::RuleExpression0:
            case gscParser::RuleExpression1:
            case gscParser::RuleExpression2:
            case gscParser::RuleExpression3:
            case gscParser::RuleExpression4:
            case gscParser::RuleExpression5:
            case gscParser::RuleExpression6:
            case gscParser::RuleExpression7:
            case gscParser::RuleExpression8:
            case gscParser::RuleExpression9:
            case gscParser::RuleExpression10:
            case gscParser::RuleExpression11:
            case gscParser::RuleExpression12:
            case gscParser::RuleIdf: {
                if (rule->children.size() == 1) {
                    // simple rules recursion
                    return ParseExpressionNode(rule->children[0], parser, obj, fobj, expressVal);
                }
                if (rule->children.size() == 2) {
                    // (++|--|~|!) exp
                    if (rule->children[0]->getTreeType() == TREE_TERMINAL) {
                        // ++/--/~/!

                        std::string op = rule->children[0]->getText();
                        if (op == "!") {
                            if (!ParseExpressionNode(rule->children[1], parser, obj, fobj, true)) {
                                return false;
                            }
                            fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_BoolNot));

                            if (!expressVal) {
                                fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_DecTop));
                            }
                        }
                        else if (op == "~") {
                            if (!ParseExpressionNode(rule->children[1], parser, obj, fobj, true)) {
                                return false;
                            }
                            fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_BoolComplement));

                            if (!expressVal) {
                                fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_DecTop));
                            }
                        }
                        else if (op == "++") {
                            // ++var
                            if (!ParseFieldNode(rule->children[1], parser, obj, fobj)) {
                                return false; // can't parse field
                            }
                            fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_Inc));
                            if (expressVal) {
                                if (!ParseExpressionNode(rule->children[1], parser, obj, fobj, true)) {
                                    return false;
                                }
                            }
                        }
                        else if (op == "--") {
                            // --var
                            if (!ParseFieldNode(rule->children[1], parser, obj, fobj)) {
                                return false; // can't parse field
                            }
                            fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_Dec));
                            if (expressVal) {
                                if (!ParseExpressionNode(rule->children[1], parser, obj, fobj, true)) {
                                    return false;
                                }
                            }
                        }
                        else {
                            obj.info.PrintLineMessage(core::logs::LVL_ERROR, rule->children[0], std::format("Unhandled operator: {}", op));
                            return false;
                        }
                    }
                    else {
                        // ++/--

                        std::string op = rule->children[1]->getText();
                        if (op == "++") {
                            // var++
                            if (expressVal) {
                                if (!ParseExpressionNode(rule->children[0], parser, obj, fobj, true)) {
                                    return false; // can't parse field
                                }
                            }
                            if (!ParseFieldNode(rule->children[0], parser, obj, fobj)) {
                                return false; // can't parse field
                            }
                            fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_Inc));
                        }
                        else if (op == "--") {
                            // var--
                            if (expressVal) {
                                if (!ParseExpressionNode(rule->children[0], parser, obj, fobj, true)) {
                                    return false; // can't parse field
                                }
                            }
                            if (!ParseFieldNode(rule->children[0], parser, obj, fobj)) {
                                return false; // can't parse field
                            }
                            fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_Dec));
                        }
                        else {
                            obj.info.PrintLineMessage(core::logs::LVL_ERROR, rule->children[1], std::format("Unhandled operator: {}", op));
                            return false;
                        }
                    }

                    return true;
                }
                if (rule->children.size() == 5) {
                    // e ? e : e
                    bool ok{ true };

                    if (!ParseExpressionNode(rule->children[0], parser, obj, fobj, true)) {
                        ok = false;
                    }

                    AscmNode* caseNo = new AscmNode();
                    fobj.AddNode(rule, new AscmNodeJump(caseNo, OPCODE_JumpOnFalse));

                    if (!ParseExpressionNode(rule->children[2], parser, obj, fobj, true)) {
                        ok = false;
                    }

                    AscmNode* caseEnd = new AscmNode();
                    fobj.AddNode(rule, new AscmNodeJump(caseEnd, OPCODE_Jump));

                    fobj.AddNode(rule->children[3], caseNo);

                    if (!ParseExpressionNode(rule->children[4], parser, obj, fobj, true)) {
                        ok = false;
                    }

                    fobj.AddNode(rule->children[4], caseEnd);

                    return ok;
                }
                if (rule->children.size() != 3) {
                    obj.info.PrintLineMessage(core::logs::LVL_ERROR, rule, std::format("Unknown expression, excepted 3 children: {}", rule->getText()));
                    return false;
                }

                std::string op = rule->children[1]->getText();

                if (op == "||" || op == "&&") {
                    // boolean operators are defined using jumps, we need to handle them
                    // push op left
                    bool ok{ true };
                    if (!ParseExpressionNode(rule->children[0], parser, obj, fobj, true)) {
                        ok = false;
                    }
                    AscmNode* after = new AscmNode();

                    fobj.AddNode(rule, new AscmNodeJump(after, op == "&&" ? OPCODE_JumpOnFalseExpr : OPCODE_JumpOnTrueExpr));

                    // push op right
                    if (!ParseExpressionNode(rule->children[2], parser, obj, fobj, true)) {
                        ok = false;
                    }

                    // after the operator
                    fobj.AddNode(rule->children[2], after);

                    if (!expressVal) {
                        fobj.AddNode(rule->children[2], new AscmNodeOpCode(OPCODE_DecTop));
                    }
                    return ok;
                }
                else if (op == "??") {
                    bool ok{ true };

                    if (obj.HasOpCode(OPCODE_JumpOnDefinedExpr)) {
                        if (!ParseExpressionNode(rule->children[0], parser, obj, fobj, true)) {
                            ok = false;
                        }
                        AscmNode* end = new AscmNode();
                        fobj.AddNode(rule, new AscmNodeJump(end, OPCODE_JumpOnDefinedExpr));

                        if (!ParseExpressionNode(rule->children[2], parser, obj, fobj, true)) {
                            ok = false;
                        }

                        fobj.AddNode(rule, end);
                    }
                    else {
                        auto [verr, tmp] = fobj.GetSpecialTmpVar();

                        if (verr) {
                            obj.info.PrintLineMessage(core::logs::LVL_ERROR, rule->children[1], std::format("Can't create temp variable for ?? operation: {}", verr));
                            return false;
                        }

                        if (!ParseExpressionNode(rule->children[0], parser, obj, fobj, true)) {
                            ok = false;
                        }

                        fobj.AddNode(rule, new AscmNodeVariable(tmp->id, OPCODE_EvalLocalVariableRefCached));
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_SetVariableField));

                        AscmNode* onDefined = new AscmNode();
                        fobj.AddNode(rule, new AscmNodeVariable(tmp->id, OPCODE_EvalLocalVariableCached));
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_IsDefined));
                        fobj.AddNode(rule, new AscmNodeJump(onDefined, OPCODE_JumpOnTrue));


                        if (!ParseExpressionNode(rule->children[2], parser, obj, fobj, true)) {
                            ok = false;
                        }
                        AscmNode* end = new AscmNode();
                        fobj.AddNode(rule, new AscmNodeJump(end, OPCODE_Jump));

                        fobj.AddNode(rule, onDefined);
                        fobj.AddNode(rule, new AscmNodeVariable(tmp->id, OPCODE_EvalLocalVariableCached));
                        fobj.AddNode(rule, end);
                    }


                    return ok;
                }
                else {
                    // push operands
                    bool ok{ true };
                    if (!ParseExpressionNode(rule->children[0], parser, obj, fobj, true)) {
                        ok = false;
                    }
                    if (!ParseExpressionNode(rule->children[2], parser, obj, fobj, true)) {
                        ok = false;
                    }

                    if (op == "|") {
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_Bit_Or));
                    }
                    else if (op == "^") {
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_Bit_Xor));
                    }
                    else if (op == "&") {
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_Bit_And));
                    }
                    else if (op == "!=") {
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_NotEqual));
                    }
                    else if (op == "!==") {
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_SuperNotEqual));
                    }
                    else if (op == "==") {
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_Equal));
                    }
                    else if (op == "===") {
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_SuperEqual));
                    }
                    else if (op == "<") {
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_LessThan));
                    }
                    else if (op == "<=") {
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_LessThanOrEqualTo));
                    }
                    else if (op == "<==") {
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_T10_LowerThanOrSuperEqualTo));
                    }
                    else if (op == ">") {
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_GreaterThan));
                    }
                    else if (op == ">=") {
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_GreaterThanOrEqualTo));
                    }
                    else if (op == ">==") {
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_T10_GreaterThanOrSuperEqualTo));
                    }
                    else if (op == "+") {
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_Plus));
                    }
                    else if (op == "-") {
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_Minus));
                    }
                    else if (op == "*") {
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_Multiply));
                    }
                    else if (op == "/") {
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_Divide));
                    }
                    else if (op == "%") {
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_Modulus));
                    }
                    else if (op == "<<") {
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_ShiftLeft));
                    }
                    else if (op == ">>") {
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_ShiftRight));
                    }
                    else {
                        obj.info.PrintLineMessage(core::logs::LVL_ERROR, rule->children[1], std::format("Unhandled operator: {}", op));
                        ok = false;
                    }
                    if (!expressVal) {
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_DecTop));
                    }
                    return ok;
                }
            }
                                   break;
            case gscParser::RuleIs_expression: {
                if (!ParseExpressionNode(rule->children[0], parser, obj, fobj, expressVal)) {
                    return false;
                }
                bool hasNot = rule->children.size() >= 3 && rule->children[2]->getText() == "not";
                // defs:
                // a is hash|int|etc. -> use ishash(a) functions
                // a is true -> use is_true operator (mw) or super equal (others)
                // a is false -> super equal (others) / mw???
                // a is undefined -> !isdefined(a)

                std::string typeName = rule->children[rule->children.size() - 1]->getText();
                uint64_t typeNameHash = hash::Hash64(typeName.data());

                if (typeNameHash == hash::Hash64("undefined")) {
                    if (hasNot) {
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_IsDefined));
                    }
                    else {
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_IsDefined));
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_BoolNot));
                    }
                    return true;
                }

                if (typeNameHash == hash::Hash64("defined")) {
                    fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_IsDefined));
                    if (hasNot) {
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_BoolNot));
                    }
                    return true;
                }

                if (typeNameHash == hash::Hash64("true")) {
                    if (obj.HasOpCode(OPCODE_IW_IsTrue)) {
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_IW_IsTrue));
                        if (hasNot) {
                            fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_BoolNot));
                        }
                        return true;
                    }

                    fobj.AddNode(rule, obj.BuildAscmNodeData(1));
                    fobj.AddNode(rule, new AscmNodeOpCode(hasNot ? OPCODE_SuperNotEqual : OPCODE_SuperEqual));
                    return true;
                }

                if (typeNameHash == hash::Hash64("false")) {
                    if (!obj.HasOpCode(OPCODE_SuperEqual)) {
                        auto [err, tmp] = fobj.GetSpecialTmpVar();

                        if (err) {
                            obj.info.PrintLineMessage(core::logs::LVL_ERROR, rule->children[1], std::format("Can't create temp variable for is operation: {}", err));
                            return false;
                        }
                        // tmp = ...
                        // isdefined(tmp) && tmp == false
                        fobj.AddNode(rule, new AscmNodeVariable(tmp->id, OPCODE_EvalLocalVariableRefCached));
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_SetVariableField));

                        fobj.AddNode(rule, new AscmNodeVariable(tmp->id, OPCODE_EvalLocalVariableCached));
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_IsDefined));

                        AscmNode* after = new AscmNode();
                        fobj.AddNode(rule, new AscmNodeJump(after, OPCODE_JumpOnFalseExpr));

                        // tmp == false
                        fobj.AddNode(rule, new AscmNodeVariable(tmp->id, OPCODE_EvalLocalVariableCached));
                        fobj.AddNode(rule, obj.BuildAscmNodeData(0));
                        fobj.AddNode(rule, new AscmNodeOpCode(hasNot ? OPCODE_NotEqual : OPCODE_Equal));

                        fobj.AddNode(rule, after);
                        return true;
                    }

                    fobj.AddNode(rule, obj.BuildAscmNodeData(0));
                    fobj.AddNode(rule, new AscmNodeOpCode(hasNot ? OPCODE_SuperNotEqual : OPCODE_SuperEqual));
                    return true;
                }

                auto dtit = obj.vmInfo->dataType.find(typeNameHash);

                if (dtit == obj.vmInfo->dataType.end()) {
                    obj.info.PrintLineMessage(core::logs::LVL_WARNING, rule, std::format("Can't find datatype '{}' for this VM", typeName));
                    return false;
                }

                const char* name = utils::va("is%s", dtit->second);
                obj.AddHash(name);
                uint64_t funcName = obj.vmInfo->HashField(name);
                AscmNodeFunctionCall* asmc = new AscmNodeFunctionCall(OPCODE_CallBuiltinFunction, 0, 1, funcName, obj.currentNamespace, obj.vmInfo);
                obj.AddImport(asmc, obj.currentNamespace, funcName, 1, tool::gsc::T8GSCImportFlags::FUNCTION | tool::gsc::T8GSCImportFlags::GET_CALL);
                fobj.AddNode(rule, asmc);

                // reverse the op if possible
                if (hasNot) {
                    fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_BoolNot));
                }
                return true;
            }
            case gscParser::RuleConst_expr:
            case gscParser::RuleConst_expr_static:
            case gscParser::RuleNumber:
            case gscParser::RuleExpression13:
            case gscParser::RuleExpression14:
            case gscParser::RuleExpression15:
                return ParseExpressionNode(rule->children[rule->children.size() == 3 ? 1 : 0], parser, obj, fobj, expressVal);
            case gscParser::RuleVector_value: {
                if (!expressVal) { // no need to create vector
                    obj.info.PrintLineMessage(core::logs::LVL_WARNING, rule, std::format("Ignored useless value: {}", rule->getText()));
                    return true;
                }
                float x = obj.FloatNumberNodeValue(rule->children[1], false);
                float y = obj.FloatNumberNodeValue(rule->children[3], false);
                float z = obj.FloatNumberNodeValue(rule->children[5], false);

                if (!(std::isnan(x) || std::isnan(y) || std::isnan(z))) {
                    // can be const

                    if (obj.HasOpCode(OPCODE_VectorConstant)) {
                        float common{};
                        // 0 = 0, 1 = -1, 2 = 1
                        if (obj.HasOpCode(OPCODE_VectorScale)) {
                            if (!FindConstVectorCommon(x, y, z, common)) {
                                common = 0;
                            }
                        }
                        byte flag = ToConstVectorFlags(x, y, z);
                        if (flag != INVALID_VECTOR_FLAGS) {
                            // can be expressed using constant

                            if (common != 0) {
                                fobj.AddNode(rule, new AscmNodeData<float>(common, OPCODE_GetFloat));
                            }

                            fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_VectorConstant));
                            fobj.AddNode(rule, new AscmNodeRawData<byte>(flag));

                            if (common != 0) {
                                fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_VectorScale));
                            }
                            return true;
                        }
                    }

                    if (obj.HasOpCode(OPCODE_GetVector)) {
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_GetVector));
                        fobj.AddNode(rule, new AscmNodeRawData<float>(x));
                        fobj.AddNode(rule, new AscmNodeRawData<float>(y));
                        fobj.AddNode(rule, new AscmNodeRawData<float>(z));
                        return true;
                    }
                }

                if (
                    !ParseExpressionNode(rule->children[5], parser, obj, fobj, true),
                    !ParseExpressionNode(rule->children[3], parser, obj, fobj, true),
                    !ParseExpressionNode(rule->children[1], parser, obj, fobj, true)
                    ) {
                    return false;
                }
                fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_Vector));
                return true;
            }
            case gscParser::RuleClass_init: {
                std::string clsName = rule->children[1]->getText();
                if (rule->children.size() > 4) {
                    obj.info.PrintLineMessage(core::logs::LVL_WARNING, rule, "Parameters not supported for class constructor");
                    return false;
                }
                obj.AddHash(clsName);
                fobj.AddNode(rule, fobj.CreateFieldHash(clsName, OPCODE_GetObjectType));
                fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_ClassFunctionCall));
                fobj.AddNode(rule, new AscmNodeRawData<byte>(0)); // params
                fobj.AddNode(rule, fobj.CreateFieldHashRaw("__constructor")); // cls name
                fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_DecTop)); // remove constructor result
                return true;
            }
            case gscParser::RuleArray_def: {
                if (!expressVal) { // no need to create array
                    obj.info.PrintLineMessage(core::logs::LVL_WARNING, rule, std::format("Ignored useless value: {}", rule->getText()));
                    return true;
                }
                fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_CreateArray));

                // 1 = [ (HASHSTR|'#' number) ':'
                bool ok{ true };
                size_t currentKey{};
                for (size_t i = 1; i + 1 < rule->children.size();) {
                    if (rule->children[i + 1]->getText() == ":") {
                        // key ':'

                        ParseTree* expTree = rule->children[i++];

                        i++; // ':'

                        if (!ParseExpressionNode(rule->children[i++], parser, obj, fobj, true)) {
                            ok = false;
                        }

                        if (!ParseExpressionNode(expTree, parser, obj, fobj, true)) {
                            ok = false;
                        }
                    }
                    else {
                        if (!ParseExpressionNode(rule->children[i++], parser, obj, fobj, true)) {
                            ok = false;
                        }
                        // push current key
                        fobj.AddNode(rule->children[i - 1], obj.BuildAscmNodeData(currentKey++));
                    }

                    fobj.AddNode(rule->children[i - 1], new AscmNodeOpCode(OPCODE_AddToArray));

                    i++; // ',' or ']'
                }

                return ok;
            }
            case gscParser::RuleStruct_def: {
                if (!expressVal) { // no need to create struct
                    obj.info.PrintLineMessage(core::logs::LVL_WARNING, rule, std::format("Ignored useless value: {}", rule->getText()));
                    return true;
                }
                fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_CreateStruct));

                bool ok{ true };
                for (size_t i = 1; i < rule->children.size() - 1;) {

                    ParseTree* term = rule->children[i++];

                    i++; // ':'


                    if (!ParseExpressionNode(rule->children[i++], parser, obj, fobj, true)) {
                        ok = false;
                    }

                    TerminalNode* scrName{ obj.GetUniqueNode(term, false) };
                    if (obj.HasOpCode(OPCODE_IW_AddToStruct)) {
                        if (!IS_TERMINAL_TYPE(scrName, gscParser::SCR_HASH)) {
                            obj.info.PrintLineMessage(core::logs::LVL_ERROR, term, std::format("Structure names in this vm should be hashes: {}", term->getText()));
                            ok = false;
                        }
                        else {
                            std::string termStr = term->getText();

                            std::string sub = termStr.substr(1, termStr.size() - 1);
                            fobj.AddNode(rule->children[i - 1], fobj.CreateFieldHash(sub, OPCODE_IW_AddToStruct));
                        }
                    }
                    else {
                        if (!IS_TERMINAL_TYPE(scrName, gscParser::SCR_HASH)) {
                            if (!ParseExpressionNode(term, parser, obj, fobj, true)) {
                                ok = false;
                            }
                            // cast the expression to a canon id
                            fobj.AddNode(term, new AscmNodeOpCode(OPCODE_CastCanon));
                        }
                        else {
                            std::string termStr = term->getText();

                            std::string sub = termStr.substr(1, termStr.size() - 1);

                            // we can't use CreateField hash because the hash is on 64 bits, but 32 in reality
                            obj.AddHash(sub);
                            fobj.AddNode(rule->children[i - 1], new AscmNodeData<uint64_t>(obj.vmInfo->HashField(sub.c_str()), OPCODE_GetHash));
                        }

                        fobj.AddNode(rule->children[i - 1], new AscmNodeOpCode(OPCODE_AddToStruct));
                    }

                    i++; // ',' or '}'
                }
                return ok;
            }
            case gscParser::RuleFunction: {
                FunctionObject* sfobj = ParseFunction(rule, parser, obj, tool::gsc::T8GSCExportFlags::PRIVATE);

                if (!sfobj) {
                    obj.info.PrintLineMessage(core::logs::LVL_ERROR, rule, "Can't parse sub function");
                    return false;
                }

                if (expressVal) {
                    AscmNodeFunctionCall* asmc = new AscmNodeFunctionCall(OPCODE_GetResolveFunction, FCF_GETTER, 0, 0, 0, obj.vmInfo);

                    obj.AddImport(asmc, sfobj->m_name_space, sfobj->m_name, 0, tool::gsc::T8GSCImportFlags::FUNC_METHOD | tool::gsc::T8GSCImportFlags::GET_CALL);
                    fobj.AddNode(rule, asmc);
                }

                return true;
            }
            case gscParser::RuleSet_expression: {
                std::string opVal = rule->children[1]->getText();

                bool ok{ true };
                if (opVal == "=") {
                    if (!ParseExpressionNode(rule->children[2], parser, obj, fobj, true)) {
                        ok = false;
                    }
                    if (!ParseFieldNode(rule->children[0], parser, obj, fobj)) {
                        ok = false;
                    }
                    fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_SetVariableField));
                    if (expressVal) {
                        if (!ParseExpressionNode(rule->children[0], parser, obj, fobj, true)) {
                            ok = false;
                        }
                    }
                    return ok;
                }
                else {
                    if (!ParseExpressionNode(rule->children[0], parser, obj, fobj, true)) {
                        ok = false;
                    }
                    if (!ParseExpressionNode(rule->children[2], parser, obj, fobj, true)) {
                        ok = false;
                    }
                    if (opVal == "+=") {
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_Plus));
                    }
                    else if (opVal == "-=") {
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_Minus));
                    }
                    else if (opVal == "/=") {
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_Divide));
                    }
                    else if (opVal == "*=") {
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_Multiply));
                    }
                    else if (opVal == "%=") {
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_Modulus));
                    }
                    else if (opVal == "&=") {
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_Bit_And));
                    }
                    else if (opVal == "|=") {
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_Bit_Or));
                    }
                    else if (opVal == "^=") {
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_Bit_Xor));
                    }
                    else if (opVal == "<<=") {
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_ShiftLeft));
                    }
                    else if (opVal == ">>=") {
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_ShiftRight));
                    }
                    else if (opVal == "~=") {
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_BoolComplement));
                    }
                    else {
                        obj.info.PrintLineMessage(core::logs::LVL_ERROR, rule->children[1], std::format("Unhandled set operator: {}", opVal));
                        ok = false;
                    }

                    // dogshit way, but it seems to be the same in their compiler...
                    if (!ParseFieldNode(rule->children[0], parser, obj, fobj)) {
                        ok = false;
                    }

                    fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_SetVariableField));
                    if (expressVal) {
                        if (!ParseExpressionNode(rule->children[0], parser, obj, fobj, true)) {
                            ok = false;
                        }
                    }
                    return ok;
                }
                return false;
            }
            case gscParser::RuleFunction_ref: {
                if (!expressVal) {
                    obj.info.PrintLineMessage(core::logs::LVL_WARNING, rule, std::format("Ignored useless value: {}", rule->getText()));
                    return true;
                }
                if (rule->children.size() == 2) {
                    if (IS_IDF(rule->children[1]) ||
                        (IS_RULE_TYPE(rule->children[1], gscParser::RuleLeft_value) && IS_IDF(rule->children[1]->children[0]))
                        ) {
                        if (obj.HasOpCode(OPCODE_T9_GetVarRef)) {
                            // single idf ref
                            // &var ?
                            std::string varName = rule->children[1]->getText();

                            FunctionVar* var = fobj.FindVar(varName);

                            if (var != fobj.VarEnd()) {
                                fobj.AddNode(rule, new AscmNodeVariable(var->id, OPCODE_EvalLocalVariableRefCached));
                                fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_T9_GetVarRef));
                                return true;
                            }
                        }
                    }
                    else if (IS_RULE_TYPE(rule->children[1], gscParser::RuleLeft_value)) {
                        if (!ParseFieldNode(rule->children[1], parser, obj, fobj)) {
                            obj.info.PrintLineMessage(core::logs::LVL_ERROR, rule->children[1], "Can't express field ref");
                            return false;
                        }
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_T9_GetVarRef));
                        return true;
                    }
                }
                if (rule->children.size() == 7) {
                    // @nsp<path>::func
                    std::string nsp = rule->children[1]->getText();
                    std::string path = rule->children[3]->getText();
                    std::string funcName = rule->children[6]->getText();

                    obj.AddHash(nsp);
                    obj.AddHash(path);
                    obj.AddHash(funcName);

                    AscmNodeLazyLink* lazy = new AscmNodeLazyLink(
                        obj.vmInfo->HashPath(path.c_str()),
                        (uint32_t)obj.vmInfo->HashField(nsp.c_str()),
                        (uint32_t)obj.vmInfo->HashField(funcName.c_str())
                    );

                    obj.AddLazy(lazy);

                    fobj.AddNode(rule, lazy);
                    return true;
                }
                // &nsp::func || &func
                uint64_t nsp = obj.currentNamespace;

                byte flags = tool::gsc::T8GSCImportFlags::FUNC_METHOD;

                if (rule->children.size() == 4) {
                    // with nsp
                    std::string nspStr = rule->children[1]->getText();
                    obj.AddHash(nspStr);
                    nsp = obj.vmInfo->HashField(nspStr.c_str());
                }
                else {
                    flags |= tool::gsc::T8GSCImportFlags::GET_CALL;
                }

                assert(rule->children.size());

                std::string funcStr = rule->children[rule->children.size() - 1]->getText();
                obj.AddHash(funcStr);
                uint64_t func = obj.vmInfo->HashField(funcStr.c_str());

                // link by the game, but we write it for test
                AscmNodeFunctionCall* asmc = new AscmNodeFunctionCall(OPCODE_GetResolveFunction, FCF_GETTER, 0, 0, 0, obj.vmInfo);
                obj.AddImport(asmc, nsp, func, 0, flags);
                fobj.AddNode(rule, asmc);

                return true;
            }
            case gscParser::RuleAnim_ref: {
                assert(IS_TERMINAL_TYPE(rule->children[1], gscParser::IDENTIFIER));
                if (rule->children.size() > 2) {
                    assert(IS_TERMINAL_TYPE(rule->children[3], gscParser::IDENTIFIER));
                    std::string tree{ ParseString(dynamic_cast<TerminalNode*>(rule->children[1])) };
                    std::string anim{ ParseString(dynamic_cast<TerminalNode*>(rule->children[3])) };

                    auto* asmc = new AscmNodeData<uint64_t>(0x1234567812345678, OPCODE_IW_GetAnimation);
                    fobj.AddNode(rule, asmc);

                    // add animtree + anim ref
                    obj.animtrees[tree].anims[anim].anim.push_back(asmc);
                }
                else {
                    if (obj.lastAnimTree.empty()) {
                        obj.info.PrintLineMessage(core::logs::LVL_ERROR, exp, "Can't use %anim without having a #using_animtree before the function");
                        return false;
                    }

                    std::string anim{ ParseString(dynamic_cast<TerminalNode*>(rule->children[1])) };
                    auto* asmc = new AscmNodeData<uint64_t>(0x1234567812345678, OPCODE_IW_GetAnimation);
                    fobj.AddNode(rule, asmc);

                    // add anim ref
                    obj.animtrees[obj.lastAnimTree].anims[anim].anim.push_back(asmc);
                }

                return true;
            }
            case gscParser::RuleLeft_value: {
                if (!expressVal) {
                    obj.info.PrintLineMessage(core::logs::LVL_WARNING, exp, std::format("Ignored useless value: {}", rule->getText()));
                    return true;
                }
                if (rule->children.size() == 1) {
                    return ParseExpressionNode(rule->children[0], parser, obj, fobj, expressVal);
                }
                else if (rule->children[0]->getTreeType() == TREE_RULE) {
                    RuleContext* first = dynamic_cast<RuleContext*>(rule->children[0]);

                    std::string second = rule->children[1]->getText();

                    if (second == ".") {
                        // object access
                        if (IS_IDF(rule->children[2])) {
                            if (!ParseExpressionNode(first, parser, obj, fobj, true)) {
                                return false;
                            }
                            std::string fieldText = rule->children[2]->getText();

                            if (fieldText == "size") {
                                fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_SizeOf));
                            }
                            else {
                                // use identifier
                                if (obj.HasOpCode(OPCODE_CastAndEvalFieldVariable)) {
                                    fobj.AddNode(rule, fobj.CreateFieldHash(fieldText, OPCODE_CastAndEvalFieldVariable));
                                }
                                else {
                                    fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_CastFieldObject));
                                    fobj.AddNode(rule, fobj.CreateFieldHash(fieldText, OPCODE_EvalFieldVariable));
                                }
                            }
                        }
                        else {
                            if (!ParseExpressionNode(rule->children[3], parser, obj, fobj, true)) {
                                return false;
                            }
                            fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_CastCanon));

                            if (!ParseExpressionNode(first, parser, obj, fobj, true)) {
                                return false;
                            }
                            fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_CastFieldObject));
                            fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_EvalFieldVariableOnStack));
                        }
                        return true;
                    }
                    else if (second == "?.") {
                        auto [verr, var] = fobj.GetSpecialTmpVar();
                        if (verr) {
                            obj.info.PrintLineMessage(core::logs::LVL_ERROR, exp, std::format("Can't allocate temp variable for ?. operator: {}", verr));
                            return false;
                        }

                        // tmp = left
                        if (!ParseExpressionNode(first, parser, obj, fobj, true)) {
                            return false;
                        }
                        fobj.AddNode(rule, new AscmNodeVariable(var->id, OPCODE_EvalLocalVariableRefCached));
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_SetVariableField));

                        AscmNode* valUndefined = new AscmNode();
                        fobj.AddNode(rule, new AscmNodeVariable(var->id, OPCODE_EvalLocalVariableCached));
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_IsDefined));
                        fobj.AddNode(rule, new AscmNodeJump(valUndefined, OPCODE_JumpOnFalse));

                        // object access
                        bool ok{ true };
                        AscmNode* end = new AscmNode();
                        fobj.AddNode(rule, new AscmNodeVariable(var->id, OPCODE_EvalLocalVariableCached));
                        if (IS_IDF(rule->children[2])) {
                            std::string fieldText = rule->children[2]->getText();

                            if (fieldText == "size") {
                                fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_SizeOf));
                            }
                            else {
                                // use identifier
                                if (obj.HasOpCode(OPCODE_CastAndEvalFieldVariable)) {
                                    fobj.AddNode(rule, fobj.CreateFieldHash(fieldText, OPCODE_CastAndEvalFieldVariable));
                                }
                                else {
                                    fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_CastFieldObject));
                                    fobj.AddNode(rule, fobj.CreateFieldHash(fieldText, OPCODE_EvalFieldVariable));
                                }
                            }
                        }
                        else {
                            if (!ParseExpressionNode(rule->children[3], parser, obj, fobj, true)) {
                                ok = false;
                            }
                            fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_CastCanon));

                            if (!ParseExpressionNode(first, parser, obj, fobj, true)) {
                                ok = false;
                            }
                            fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_CastFieldObject));
                            fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_EvalFieldVariableOnStack));
                        }
                        fobj.AddNode(rule, new AscmNodeJump(end, OPCODE_Jump));

                        fobj.AddNode(rule, valUndefined);
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_GetUndefined));
                        fobj.AddNode(rule, end);

                        return ok;
                    }
                    else if (second == "[") {
                        // array access

                        if (!ParseExpressionNode(rule->children[2], parser, obj, fobj, true)) {
                            return false;
                        }

                        if (!ParseExpressionNode(first, parser, obj, fobj, true)) {
                            return false;
                        }

                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_EvalArray));
                        return true;
                    }
                    else {
                        obj.info.PrintLineMessage(core::logs::LVL_ERROR, exp, std::format("Unknown left value type: {}", second));
                        return false;
                    }
                }
                else {
                    obj.info.PrintLineMessage(core::logs::LVL_ERROR, exp, std::format("Unknown lvalue tree type: {}", exp->getText()));
                    return false;
                }
                obj.info.PrintLineMessage(core::logs::LVL_ERROR, exp, std::format("Unhandled lvalue type: {}", exp->getText()));
                return false;
            }
            }

            obj.info.PrintLineMessage(core::logs::LVL_ERROR, rule, std::format("Unhandled rule: {} ({})", rule->getText(), rule->getRuleIndex()));
            return false;
        }

        if (exp->getTreeType() != TREE_TERMINAL) {
            obj.info.PrintLineMessage(core::logs::LVL_ERROR, exp, std::format("Unknown tree type: {}", exp->getText()));
            return false;
        }

        TerminalNode* term = dynamic_cast<TerminalNode*>(exp);

        size_t len = term->getText().size();

        if (term->getSymbol()->getType() == gscParser::IDENTIFIER) {
            std::string varName = term->getText();

            auto ceit = obj.constexprs.find(hash::Hash64(varName.c_str()));

            if (ceit != obj.constexprs.end()) {
                ParseTree* pt = ceit->second;

                if (!ParseExpressionNode(pt, parser, obj, fobj, expressVal)) {
                    obj.info.PrintLineMessage(core::logs::LVL_ERROR, term, std::format("error when using constexpr {}", varName));
                    return false;
                }
                return true;
            }

            if (!expressVal) {
                obj.info.PrintLineMessage(core::logs::LVL_WARNING, exp, std::format("Ignored useless value: {}", term->getText()));
                return true;
            }

            if (varName == "self") {
                fobj.AddNode(term, new AscmNodeOpCode(OPCODE_GetSelf));
                return true;
            }

            if (obj.HasOpCode(OPCODE_IW_GetPositionRef)) {
                auto itl = fobj.m_jumpLocs.find(varName);

                if (itl != fobj.m_jumpLocs.end()) {
                    if (!itl->second.node) {
                        obj.info.PrintLineMessage(core::logs::LVL_ERROR, exp, std::format("The jump location {} can't be referenced", varName));
                        return false;
                    }

                    fobj.AddNode(term, new AscmNodeJump(itl->second.node, OPCODE_IW_GetPositionRef));
                    return true;
                }
            }

            auto gvarIt = fobj.m_vmInfo->globalvars.find(obj.vmInfo->HashField(varName));

            if (gvarIt != fobj.m_vmInfo->globalvars.end()) {
                GlobalVariableDef& gv = gvarIt->second;

                if (gv.getOpCode) {
                    fobj.AddNode(term, new AscmNodeOpCode(gv.getOpCode));
                    return true;
                }

                if (!obj.gscHandler->HasFlag(tool::gsc::GOHF_GLOBAL)) {
                    obj.info.PrintLineMessage(core::logs::LVL_ERROR, term, std::format("{} is defined as a global, but the vm doesn't support globals", varName));
                    return false;
                }

                GlobalVarObject& decl = obj.globals[gv.name];

                if (!decl.def) {
                    decl.def = &gv;
                }

                AscmNodeGlobalVariable* gvar = new AscmNodeGlobalVariable(&gv, OPCODE_GetGlobal);
                decl.nodes.emplace_back(gvar);
                fobj.AddNode(term, gvar);
                return true;
            }

            FunctionVar* varIt = fobj.FindVar(varName);

            if (varIt == fobj.VarEnd()) {
                obj.info.PrintLineMessage(core::logs::LVL_ERROR, term, std::format("Unknown variable: {}", varName));
                return false;
            }

            fobj.AddNode(term, new AscmNodeVariable(varIt->id, OPCODE_EvalLocalVariableCached));
            return true;
        }

        if (!expressVal) {
            obj.info.PrintLineMessage(core::logs::LVL_WARNING, exp, std::format("Ignored useless value: {}", term->getText()));
            return true;
        }

        switch (term->getSymbol()->getType()) {
        case gscParser::UNDEFINED_VALUE:
            fobj.AddNode(term, new AscmNodeOpCode(OPCODE_GetUndefined));
            return true;
        case gscParser::BOOL_VALUE:
            fobj.AddNode(term, obj.BuildAscmNodeData(term->getText() == "true" ? (fobj.obj.config.obfuscate ? rand() + 1 : 1) : 0));
            return true;
        case gscParser::FLOATVAL:
            fobj.AddNode(term, new AscmNodeData<FLOAT>((FLOAT)std::strtof(term->getText().c_str(), nullptr), OPCODE_GetFloat));
            return true;
        case gscParser::INTEGER10:
            fobj.AddNode(term, obj.BuildAscmNodeData(std::strtoll(term->getText().c_str(), nullptr, 10)));
            return true;
        case gscParser::INTEGER16: {
            bool neg = term->getText()[0] == '-';
            auto val = std::strtoll(term->getText().c_str() + (neg ? 3 : 2), nullptr, 16);
            fobj.AddNode(term, obj.BuildAscmNodeData(neg ? -val : val));
            return true;
        }
        case gscParser::INTEGER8: {
            bool neg = term->getText()[0] == '-';
            auto val = std::strtoll(term->getText().c_str() + (neg ? 2 : 1), nullptr, 8);
            fobj.AddNode(term, obj.BuildAscmNodeData(neg ? -val : val));
            return true;
        }
        case gscParser::INTEGER2: {
            bool neg = term->getText()[0] == '-';
            auto val = std::strtoll(term->getText().c_str() + (neg ? 3 : 2), nullptr, 2);
            fobj.AddNode(term, obj.BuildAscmNodeData(neg ? -val : val));
            return true;
        }
        case gscParser::HASHSTRING: {
            std::string hash = term->getText();
            return AddHashNode(term, hash[0], hash.substr(2, len - 3), fobj, obj);
        }
        case gscParser::ANIMTREE_IDENTIFIER: {
            std::string node{ ParseString(term, 1) };


            AscmNodeOpCode* asmc;
            if (obj.vmInfo->HasFlag(VmFlags::VMF_ANIMTREE_T7)) {
                // t7 doesn't have a GetAnimationTree operator, it uses the 
                asmc = new AscmNodeData<uint32_t>(0x12345678, OPCODE_GetInteger);
            }
            else {
                asmc = new AscmNodeData<byte>(0x12, OPCODE_IW_GetAnimationTree);
            }
            fobj.AddNode(term, asmc);

            // add animtree ref
            obj.animtrees[node].animtree.push_back(asmc);

            return true;
        }
        case gscParser::SCR_HASH: {
            std::string node{ ParseString(term, 1) };

            if (utils::EqualIgnoreCase(node, "animtree")) {
                // use animtree

                if (obj.lastAnimTree.empty()) {
                    obj.info.PrintLineMessage(core::logs::LVL_ERROR, exp, "Can't use #animtree without having a #using_animtree before the function");
                    return false;
                }

                AscmNodeOpCode* asmc;
                if (obj.vmInfo->HasFlag(VmFlags::VMF_ANIMTREE_T7)) {
                    // t7 doesn't have a GetAnimationTree operator, it uses the 
                    asmc = new AscmNodeData<uint32_t>(0x12345678, OPCODE_GetInteger);
                }
                else {
                    asmc = new AscmNodeData<byte>(0x12, OPCODE_IW_GetAnimationTree);
                }
                fobj.AddNode(term, asmc);

                // add animtree ref
                obj.animtrees[obj.lastAnimTree].animtree.push_back(asmc);
            }
            else {
                return AddHashNode(term, 's', node, fobj, obj);
            }

            return true;
        }
        case gscParser::STRING: {
            std::string node = term->getText();
            std::unique_ptr<char[]> newStr = std::make_unique<char[]>(node.length() - 1);
            char* newStrWriter = &newStr[0];

            // format string
            for (size_t i = 1; i < node.length() - 1; i++) {
                if (node[i] != '\\') {
                    *(newStrWriter++) = node[i];
                    continue; // default case
                }

                i++;

                assert(i < node.length() && "bad format, \\ before end");

                switch (node[i]) {
                case 'n':
                    *(newStrWriter++) = '\n';
                    break;
                case 't':
                    *(newStrWriter++) = '\t';
                    break;
                case 'r':
                    *(newStrWriter++) = '\r';
                    break;
                case 'b':
                    *(newStrWriter++) = '\b';
                    break;
                default:
                    *(newStrWriter++) = node[i];
                    break;
                }
            }
            *(newStrWriter++) = 0; // end char

            // link by the game
            AscmNodeData<uint32_t>* asmc = new AscmNodeData<uint32_t>(0x12345678, OPCODE_GetString);
            fobj.AddNode(term, asmc);

            std::string key{ &newStr[0] };

            if (key.length() >= 256) {
                obj.info.PrintLineMessage(core::logs::LVL_ERROR, exp, std::format("String too long: {}", term->getText()));
                return false;
            }

            StringObject& str = obj.RegisterString(key);
            str.nodes.push_back(asmc);
            return true;
        }
        case gscParser::ISTRING: {
            std::string node{ ParseString(term, 1) };

            if (obj.vmInfo->HasFlag(VmFlags::VMF_ISTRING_HASHED)) {
                const char* reshash{ &node[0] };
                uint64_t h{ obj.vmInfo->HashPath(reshash) };
                fobj.AddNode(term, new AscmNodeData<uint64_t>(h, OPCODE_IW_GetLocalizedHash));
                obj.AddHash(reshash);
            }
            else {
                // link by the game
                AscmNodeData<uint32_t>* asmc = new AscmNodeData<uint32_t>(0x12345678, OPCODE_IW_GetIString);
                fobj.AddNode(term, asmc);

                if (node.length() >= 256) {
                    obj.info.PrintLineMessage(core::logs::LVL_ERROR, exp, std::format("IString too long: {}", term->getText()));
                    return false;
                }

                StringObject& str = obj.RegisterString(node);
                str.nodes.push_back(asmc);
            }
            return true;
        }
        }

        obj.info.PrintLineMessage(core::logs::LVL_ERROR, exp, std::format("Unhandled terminal: {}", term->getText()));
        return false;
    }
    
    bool ParseClassDef(RuleContext* func, gscParser& parser, CompileObject& obj) {
        std::string clsName = func->children[1]->getText();

        if (!obj.gscHandler->GetVTableImportFlags()) {
            obj.info.PrintLineMessage(core::logs::LVL_ERROR, func, std::format("This vm doesn't support classes, can't register '{}'", clsName));
            return false;
        }

        obj.AddHash(clsName);
        uint64_t clsNameHash = obj.vmInfo->HashField(clsName);

        ClassCompileContext cctx{ clsNameHash };
        obj.clsCtx = &cctx;
        utils::CloseEnd ce{ [&obj] {obj.clsCtx = nullptr; } };

        bool ok = true;

        size_t idx{ 2 };
        if (func->children[idx]->getText() == ":") {
            do {
                idx++;
                ParseTree* idf{ func->children[idx++] };
                LOG_TRACE("parent: {}", idf->getText());
            } while (func->children[idx]->getText() != "{");
            // todo: parse parent classes
            obj.info.PrintLineMessage(core::logs::LVL_ERROR, func->children[idx], std::format("Parent classes not implemented yet: class {}", clsName));
            ok = false;
        }

        if (func->children[idx]->getText() != "{") {
            obj.info.PrintLineMessage(core::logs::LVL_ERROR, func->children[idx], std::format("Invalid class reading: {}", func->children[idx]->getText()));
            return false;
        }

        idx++;

        while (IS_RULE_TYPE(func->children[idx], gscParser::RuleClass_var)) {
            ParseTree* v{ func->children[idx++] };
            std::string varName = func->children[1]->getText();
            obj.AddHash(varName);
            uint64_t varNameHash = obj.vmInfo->HashField(varName);

            if (cctx.vars.contains(varNameHash)) {
                obj.info.PrintLineMessage(core::logs::LVL_ERROR, func->children[idx], std::format("Class property {} registered twice in {}", varName, clsName));
                ok = false;
                continue;
            }
            if (func->children.size() > 3) {
                cctx.vars[varNameHash] = func->children[3];
            }
            else {
                cctx.vars[varNameHash] = nullptr;
            }
        }

        while (IS_RULE_TYPE(func->children[idx], gscParser::RuleFunction)) {
            ParseTree* f{ func->children[idx++] };

        }


        auto [res, err] = obj.exports.try_emplace(clsNameHash, obj, clsNameHash, obj.currentNamespace, obj.fileNameSpace, obj.vmInfo);


        if (!err) {
            obj.info.PrintLineMessage(core::logs::LVL_ERROR, func, std::format("The export {} was defined twice", clsName));
            return false;
        }


        FunctionObject& exp = res->second;
        exp.m_flags = tool::gsc::CLASS_VTABLE;

        exp.AddNode(func, exp.CreateParamNode());

        /*

.00000000: 000d CheckClearParams
.00000002: 000e PreScriptCall
.00000004: 000f CallBuiltinFunction       params: 0 spawnstruct
.00000010: 06a1 GetZero
.00000012: 0d29 EvalFieldVariableFromGlobalObject classes.cluielemtext
.0000001c: 09b5 SetVariableFieldFromEvalArrayRef

.0000001e: 0bb2 GetResolveFunction        &cluielem::function_7bfd10e6
.00000028: 0253 GetUnsignedInteger        2080182502 (0x7bfd10e6)
.00000030: 0b50 GetZero
.00000032: 05dc EvalGlobalObjectFieldVariable classes.cluielemtext
.0000003c: 0096 EvalArray
.0000003e: 0749 CastFieldObject
.00000040: 0967 EvalFieldVariableRef      __vtable
.00000048: 0439 SetVariableFieldFromEvalArrayRef



        */
        exp.AddNode(func, new AscmNodeOpCode(OPCODE_PreScriptCall));
        uint64_t ss = obj.vmInfo->HashField("spawnstruct");
        AscmNodeFunctionCall* gakNode = new AscmNodeFunctionCall(OPCODE_CallBuiltinFunction, 0, 0, ss, obj.currentNamespace, obj.vmInfo);
        obj.AddImport(gakNode, obj.currentNamespace, ss, 0, tool::gsc::FUNCTION | tool::gsc::GET_CALL);
        exp.AddNode(func, gakNode);
        exp.AddNode(func, new AscmNodeOpCode(OPCODE_GetZero));


        auto gvarIt = exp.m_vmInfo->globalvars.find(obj.vmInfo->HashField("classes"));

        if (gvarIt == exp.m_vmInfo->globalvars.end()) {
            obj.info.PrintLineMessage(core::logs::LVL_ERROR, func, std::format("Can't find classes global variable, does this vm support classes?"));
            return false;
        }

        GlobalVariableDef& gv = gvarIt->second;

        if (obj.HasOpCode(OPCODE_T9_EvalFieldVariableFromGlobalObject)) {
            // T9
            GlobalVarObject& decl = obj.globals[gv.name];

            if (!decl.def) {
                decl.def = &gv;
            }

            AscmNodeGlobalVariable* gvar = new AscmNodeGlobalVariable(&gv, OPCODE_T9_EvalFieldVariableFromGlobalObject);
            decl.nodes.emplace_back(gvar);
            exp.AddNode(func, gvar);
            exp.AddNode(func, exp.CreateFieldHashRaw(clsNameHash));
        }
        else {
            if (gv.getOpCode) { // T7
                exp.AddNode(func, new AscmNodeOpCode(gv.getOpCode));
            }
            else { // T8
                if (!obj.gscHandler->HasFlag(tool::gsc::GOHF_GLOBAL)) {
                    obj.info.PrintLineMessage(core::logs::LVL_ERROR, func, std::format("classes is defined as a global, but the vm doesn't support globals"));
                    return false;
                }

                AscmNodeGlobalVariable* gvar = new AscmNodeGlobalVariable(&gv, OPCODE_GetGlobalObject);
                GlobalVarObject& decl = obj.globals[gv.name];

                if (!decl.def) {
                    decl.def = &gv;
                }
                decl.nodes.emplace_back(gvar);
                exp.AddNode(func, gvar);
            }

            exp.AddNode(func, exp.CreateFieldHash(clsNameHash, OPCODE_EvalFieldVariableRef));
        }

        if (obj.HasOpCode(OPCODE_T9_EvalFieldVariableFromGlobalObject)) {
            exp.AddNode(func, new AscmNodeOpCode(OPCODE_T9_SetVariableFieldFromEvalArrayRef));
        }
        else {
            exp.AddNode(func, new AscmNodeOpCode(OPCODE_EvalArrayRef));
            exp.AddNode(func, new AscmNodeOpCode(OPCODE_SetVariableField));
        }


        exp.AddNode(func->children[func->children.size() - 1], new AscmNodeOpCode(OPCODE_End));

        obj.info.PrintLineMessage(core::logs::LVL_WARNING, func->children[idx], std::format("Class not yet fully implemented: {}", func->children[idx]->getText()));
        return ok;
    }
    
    FunctionObject* ParseFunction(RuleContext* func, gscParser& parser, CompileObject& obj, byte forceFlags) {
        int deltaArrow = (func->children.size() > 2 && func->children[func->children.size() - 2]->getText() == "=>") ? 1 : 0;
        bool hasName = func->children.size() > (4 + deltaArrow) && IS_IDF(func->children[(size_t)(func->children.size() - 5 - deltaArrow)]);

        ParseTree* paramsRule = func->children[(size_t)(func->children.size() - 3 - deltaArrow)];
        ParseTree* blockRule = func->children[(size_t)(func->children.size() - 1)];

        std::string name = hasName ?
            func->children[(size_t)(func->children.size() - (5 + deltaArrow))]->getText()
            : utils::va("$nameless_%llx", (obj.config.obfuscate ? (obj.emptyNameInc += 1 + rand()) : obj.emptyNameInc++));

        obj.AddHash(name);
        uint64_t nameHashed = obj.vmInfo->HashField(name.data());

        auto [res, err] = obj.exports.try_emplace(nameHashed, obj, nameHashed, obj.currentNamespace, obj.fileNameSpace, obj.vmInfo);

        if (!err) {
            obj.info.PrintLineMessage(core::logs::LVL_ERROR, func, std::format("The export {} was defined twice", name));
            return nullptr;
        }

        FunctionObject& exp = res->second;

        exp.m_flags |= forceFlags;

        if (!IS_RULE_TYPE(paramsRule, gscParser::RuleParam_list)) {
            obj.info.PrintLineMessage(core::logs::LVL_ERROR, func, std::format("Bad function {} params declaration {}", name, func->getText()));
            return nullptr;
        }
        if (!IS_RULE_TYPE(blockRule, gscParser::RuleStatement_block)) {
            obj.info.PrintLineMessage(core::logs::LVL_ERROR, func, std::format("Bad function {} block declaration {}", name, func->getText()));
            return nullptr;
        }

        // handle modifiers

        size_t end = (hasName ? 5 : 4) + deltaArrow;

        for (size_t i = 0; i < func->children.size() - end; i++) {
            ParseTree* mod = func->children[i];

            if (IS_RULE_TYPE(mod, gscParser::RuleDetour_info)) {

                if (mod->children.size() != 2) {
                    // function detour
                    std::string dnsp = mod->children[1]->getText();
                    std::string dscript = mod->children[3]->getText();
                    std::string dfunc = mod->children[6]->getText();
                    std::string dscriptp{ obj.GetIncludeName(dscript) };
                    obj.AddHash(dnsp);
                    obj.AddHash(dscriptp);
                    obj.AddHash(dfunc);
                    exp.detour.nsp = obj.vmInfo->HashField(dnsp);
                    exp.detour.func = obj.vmInfo->HashField(dfunc);
                    exp.detour.script = obj.vmInfo->HashPath(dscriptp);
                }
                else {
                    // builtin detour
                    std::string target = mod->children[1]->getText();

                    obj.AddHash(target);
                    exp.detour.nsp = 0;
                    exp.detour.func = obj.vmInfo->HashField(target);
                    exp.detour.script = 0;
                }

                continue;
            }

            if (mod->getTreeType() != TREE_TERMINAL) {
                obj.info.PrintLineMessage(core::logs::LVL_ERROR, mod, std::format("Bad modifier for {}", name));
                return nullptr;
            }

            TerminalNode* term = dynamic_cast<TerminalNode*>(mod);

            std::string txt = term->getText();

            if (txt == "function") {
                continue; // don't care
            }
            if (txt == "private") {
                exp.m_flags |= tool::gsc::T8GSCExportFlags::PRIVATE;
            }
            else if (txt == "autoexec") {
                exp.m_flags |= tool::gsc::T8GSCExportFlags::AUTOEXEC;

                if (IS_RULE_TYPE(func->children[i + 2], gscParser::RuleNumber)) {
                    // use user order
                    exp.autoexecOrder = obj.NumberNodeValue(func->children[i + 2]);
                    i += 3;
                }
                else {
                    // use natural order
                    exp.autoexecOrder = obj.autoexecOrder++;
                }
            }
            else if (txt == "event_handler") {
                if (!obj.gscHandler->HasFlag(tool::gsc::GOHF_SUPPORT_EV_HANDLER)) {
                    obj.info.PrintLineMessage(core::logs::LVL_ERROR, func, "event_handler functions not available for this vm");
                    return nullptr;
                }
                exp.m_flags |= tool::gsc::T8GSCExportFlags::EVENT;
                ParseTree* ev = func->children[i += 2];
                i++; // ']'
                if (ev->getTreeType() != TREE_TERMINAL) {
                    obj.info.PrintLineMessage(core::logs::LVL_ERROR, ev, std::format("Bad event for {}", name));
                    return nullptr;
                }

                std::string evname = static_cast<TerminalNode*>(ev)->getText();

                obj.AddHash(evname);
                exp.m_data_name = obj.vmInfo->HashField(evname.data());
            }
        }

        // handle params

        gscParser::Param_listContext* params = dynamic_cast<gscParser::Param_listContext*>(paramsRule);

        size_t index = 0;
        bool varargDetected{};

        for (ParseTree* child : params->children) {
            if (index++ % 2) {
                continue; // ','
            }

            assert(IS_RULE_TYPE(child, gscParser::RuleParam_val));
            gscParser::Param_valContext* param = dynamic_cast<gscParser::Param_valContext*>(child);

            if (varargDetected) {
                obj.info.PrintLineMessage(core::logs::LVL_ERROR, child, "Can't register param after a vararg");
                return nullptr;
            }

            TerminalNode* idfNode{};
            byte idfFlags{};
            if (!IS_IDF(param->children[0])) {
                if (param->children.size() == 1) {
                    // '...'

                    if (!obj.gscHandler->HasFlag(tool::gsc::GOHF_SUPPORT_VAR_VA)) {
                        obj.info.PrintLineMessage(core::logs::LVL_ERROR, param, "Modifier not available for this vm: vararg...");
                        return nullptr;
                    }
                    exp.m_flags |= tool::gsc::T8GSCExportFlags::VE;

                    std::string paramIdf = "vararg";


                    if (exp.m_params == 256) {
                        obj.info.PrintLineMessage(core::logs::LVL_ERROR, param, std::format("Can't register param '{}': too many params", paramIdf));
                        return nullptr;
                    }

                    exp.m_params++;

                    auto [err, vardef] = exp.RegisterVar(paramIdf, false, tool::gsc::opcode::VARIADIC);
                    if (err) {
                        obj.info.PrintLineMessage(core::logs::LVL_ERROR, param, err);
                        return nullptr;
                    }

                    if (obj.gscHandler->HasFlag(tool::gsc::GOHF_VAR_VA_COUNT)) {
                        std::string paramIdfCount = "varargcount";
                        if (exp.m_params == 256) {
                            obj.info.PrintLineMessage(core::logs::LVL_ERROR, param, std::format("Can't register param '{}': too many params", paramIdfCount));
                            return nullptr;
                        }
                        exp.m_params++;

                        auto [err, vardef] = exp.RegisterVar(paramIdfCount, false);
                        if (err) {
                            obj.info.PrintLineMessage(core::logs::LVL_ERROR, param, err);
                            return nullptr;
                        }
                    }


                    continue;
                }

                // skip modifier
                idfNode = dynamic_cast<TerminalNode*>(param->children[1]);
                std::string modifier = param->children[0]->getText();
                if (modifier == "*") {
                    // ptr (T9)
                    if (!obj.gscHandler->HasFlag(tool::gsc::GOHF_SUPPORT_VAR_PTR)) {
                        obj.info.PrintLineMessage(core::logs::LVL_ERROR, param, std::format("Modifier not available for this vm: {}", modifier));
                        return nullptr;
                    }
                    idfFlags = tool::gsc::opcode::T9_VAR_REF;
                }
                else if (modifier == "&") {
                    // ref (T8)
                    if (!obj.gscHandler->HasFlag(tool::gsc::GOHF_SUPPORT_VAR_REF)) {
                        obj.info.PrintLineMessage(core::logs::LVL_ERROR, param, std::format("Modifier not available for this vm: {}", modifier));
                        return nullptr;
                    }
                    idfFlags = tool::gsc::opcode::ARRAY_REF;
                }
                else {
                    obj.info.PrintLineMessage(core::logs::LVL_ERROR, param, std::format("Modifier not implemented: {}", modifier));
                    return nullptr;
                }
            }
            else {
                idfNode = dynamic_cast<TerminalNode*>(param->children[0]);
            }
            std::string paramIdf = idfNode->getText();

            if (exp.m_params == 256) {
                obj.info.PrintLineMessage(core::logs::LVL_ERROR, idfNode, std::format("Can't register param '{}': too many params", paramIdf));
                return nullptr;
            }

            exp.m_params++;

            auto [err, vardef] = exp.RegisterVar(paramIdf, false, idfFlags);

            if (err) {
                obj.info.PrintLineMessage(core::logs::LVL_ERROR, idfNode, err);
                return nullptr;
            }

            if (param->children.size() >= 3) {
                // default value
                ParseTree* defaultValueExp = param->children[param->children.size() - 1];
                assert(IS_RULE_TYPE(defaultValueExp, gscParser::RuleExpression));

                /*
                    if (!isdefined(param)) {
                        param = defaultValueExp;
                    }
                 */
                if (obj.HasOpCode(OPCODE_EvalLocalVariableDefined)) {
                    exp.AddNode(defaultValueExp, new AscmNodeVariable(vardef->id, OPCODE_EvalLocalVariableDefined));
                }
                else {
                    exp.AddNode(defaultValueExp, new AscmNodeVariable(vardef->id, OPCODE_EvalLocalVariableCached));
                    exp.AddNode(defaultValueExp, new AscmNodeOpCode(OPCODE_IsDefined));
                }
                AscmNode* afterNode = new AscmNode();
                exp.AddNode(defaultValueExp, new AscmNodeJump(afterNode, OPCODE_JumpOnTrue));
                if (!ParseExpressionNode(defaultValueExp, parser, obj, exp, true)) {
                    obj.info.PrintLineMessage(core::logs::LVL_ERROR, defaultValueExp, std::format("Can't create expression node for variable {}", paramIdf));
                    return nullptr;
                }
                exp.AddNode(defaultValueExp, new AscmNodeVariable(vardef->id, OPCODE_EvalLocalVariableRefCached));
                exp.AddNode(defaultValueExp, new AscmNodeOpCode(OPCODE_SetVariableField));
                exp.AddNode(defaultValueExp, afterNode);
            }

        }

        // handle block

        // weirdly, their gsc compiler is converting top level breaks to jump to the end of the function
        AscmNode* endNode = new AscmNodeOpCode(OPCODE_End);
        exp.PushBreakNode(endNode);
        bool badRef{};

        if (!ParseExpressionNode(blockRule, parser, obj, exp, false)) {
            badRef = true;
        }

        exp.PopBreakNode();

        if (obj.devBlockDepth) {
            // add a dev block from start to begin
            AscmNode* djmp = new AscmNodeJump(endNode, OPCODE_DevblockBegin);
            obj.m_devBlocks.push_back(djmp);
            exp.AddNode(exp.m_nodes.begin(), func, djmp);
        }

        exp.AddNode(func, endNode);

        for (auto& [name, loc] : exp.m_jumpLocs) {
            if (loc.defined) {
                continue;
            }

            obj.info.PrintLineMessage(core::logs::LVL_ERROR, loc.def ? loc.def : blockRule, std::format("The location {} was used, but isn't declared", name));

            badRef = true;

            if (loc.node) {
                delete loc.node; // free loc
                loc.node = nullptr;
            }
        }

        exp.AddNode(exp.m_nodes.begin(), func, exp.CreateParamNode());

        if (badRef) {
            obj.info.PrintLineMessage(core::logs::LVL_ERROR, func, std::format("Can't compile function '{}'", hasName ? name : "<no name>"));
            return nullptr;
        }

        return &exp;
    }

    bool ParsePrecache(RuleContext* prec, gscParser& parser, CompileObject& obj) {
        if (!obj.config.precache) {
            obj.info.PrintLineMessage(core::logs::LVL_WARNING, prec, "No precache handler available, but a #preache was declared");
            return true;
        }

        std::string type{ ParseString((TerminalNode*)prec->children[2]) };

        if (type.empty()) {
            obj.info.PrintLineMessage(core::logs::LVL_ERROR, prec->children[2], "Empty precache type");
            return false;
        }
        std::string val{ ParseString((TerminalNode*)prec->children[4]) };

        if (val.empty()) {
            obj.info.PrintLineMessage(core::logs::LVL_ERROR, prec->children[4], "Empty precache value");
            return false;
        }

        (*obj.config.precache)[type].push_back(val);
        return true;
    }

    bool ParseAnimTree(RuleContext* atr, gscParser& parser, CompileObject& obj) {
        std::string tree{ ParseString((TerminalNode*)atr->children[2]) };

        if (tree.empty()) {
            obj.info.PrintLineMessage(core::logs::LVL_ERROR, atr->children[2], "Empty animtree");
            return false;
        }

        // allocate the tree
        obj.animtrees[tree];

        // define it at last for #animtree
        obj.lastAnimTree = tree;

        return true;
    }

    bool ParseInclude(RuleContext* nsp, gscParser& parser, CompileObject& obj) {
        if (nsp->children.size() < 2 || nsp->children[1]->getTreeType() != TREE_TERMINAL) {
            return false; // bad
        }

        std::string txt = nsp->children[1]->getText();

        // add the include/using into the includes
        obj.AddInclude(txt);

        return true;
    }

    bool ParseNamespace(RuleContext* nsp, gscParser& parser, CompileObject& obj) {
        if (nsp->children.size() < 2 || nsp->children[1]->getTreeType() != TREE_TERMINAL) {
            return false; // bad
        }

        std::string txt = nsp->children[1]->getText();

        // set the current namespace to the one specified

        obj.AddHash(txt);
        obj.currentNamespace = obj.vmInfo->HashField(txt.data());
        if (!obj.fileNameSpace && obj.gscHandler->HasFlag(tool::gsc::GscObjHandlerBuildFlags::GOHF_FILENAMESPACE)) {
            obj.fileNameSpace = obj.vmInfo->HashFilePath(txt.data());
        }

        return true;
    }

    bool ParseFileNamespace(RuleContext* nsp, gscParser& parser, CompileObject& obj) {
        if (nsp->children.size() < 2 || nsp->children[1]->getTreeType() != TREE_TERMINAL) {
            return false; // bad
        }

        if (!obj.gscHandler->HasFlag(tool::gsc::GscObjHandlerBuildFlags::GOHF_FILENAMESPACE)) {
            obj.info.PrintLineMessage(core::logs::LVL_WARNING, nsp, "this vm doesn't support file namespace");
            return true;
        }

        std::string txt = nsp->children[1]->getText();

        // set the current file namespace to the one specified

        obj.AddHash(txt);
        obj.fileNameSpace = obj.vmInfo->HashFilePath(txt.data());
        if (!obj.currentNamespace) {
            obj.currentNamespace = obj.vmInfo->HashField(txt.data());
        }

        return true;
    }

    bool ParseConstExpr(RuleContext* exp, gscParser& parser, CompileObject& obj) {
        if (exp->children.size() < 4 || exp->children[3]->getTreeType() != TREE_RULE) {
            return false; // bad
        }

        std::string constexprName = exp->children[1]->getText();
        uint64_t hash = hash::Hash64(constexprName.c_str());

        ParseTree*& expr = obj.constexprs[hash];

        if (expr) {
            obj.info.PrintLineMessage(core::logs::LVL_WARNING, exp, std::format("Redefinition of constexpr {}", constexprName));
        }

        obj.constexprs[hash] = exp->children[3];
        return true;
    }

    bool ParseProg(gscParser::ProgContext* prog, gscParser& parser, CompileObject& obj) {
        if (prog->getTreeType() == TREE_ERROR) {
            obj.info.PrintLineMessage(core::logs::LVL_ERROR, prog, "Bad prog context");
            return false;
        }

        TerminalNode* eof = prog->EOF();

        // find the first namespace (used for multifile inputs)
        for (ParseTree* es : prog->children) {
            if (es == eof) {
                break;
            }
            if (es->getTreeType() == TREE_ERROR) {
                obj.info.PrintLineMessage(core::logs::LVL_ERROR, prog, "Bad export rule type");
                return false;
            }
            if (es->getTreeType() == TREE_TERMINAL) {
                continue; // dev block part
            }

            RuleContext* rule = dynamic_cast<RuleContext*>(es);
            size_t idx = rule->getRuleIndex();

            if (idx == gscParser::RuleNamespace) {
                if (!ParseNamespace(rule, parser, obj)) {
                    return false;
                }
            }
            else if (idx == gscParser::RuleFilenamespace) {
                if (!ParseFileNamespace(rule, parser, obj)) {
                    return false;
                }
            }
        }

        obj.devBlockDepth = 0;
        for (ParseTree* es : prog->children) {
            if (es == eof) {
                break; // done
            }

            if (es->getTreeType() == TREE_TERMINAL) {
                std::string txt{ es->getText() };

                if (txt == "/#") {
                    obj.devBlockDepth++;
                }
                else if (txt == "#/") {
                    if (!obj.devBlockDepth) {
                        obj.info.PrintLineMessage(core::logs::LVL_ERROR, es, "Usage of #/ with no starting /#");
                        return false;
                    }
                    else {
                        obj.devBlockDepth--;
                    }
                }
                else {
                    obj.info.PrintLineMessage(core::logs::LVL_ERROR, es, std::format("Bad export terminal {}", txt));
                    return false;
                }

                continue;
            }

            RuleContext* rule = dynamic_cast<RuleContext*>(es);

            switch (rule->getRuleIndex()) {
            case gscParser::RuleInclude:
                if (!ParseInclude(rule, parser, obj)) {
                    return false;
                }
                break;
            case gscParser::RulePrecache:
                if (!ParsePrecache(rule, parser, obj)) {
                    return false;
                }
                break;
            case gscParser::RuleUsing_animtree:
                if (!ParseAnimTree(rule, parser, obj)) {
                    return false;
                }
                break;
            case gscParser::RuleNamespace:
                if (!ParseNamespace(rule, parser, obj)) {
                    return false;
                }
                break;
            case gscParser::RuleFilenamespace:
                if (!ParseFileNamespace(rule, parser, obj)) {
                    return false;
                }
                break;
            case gscParser::RuleConstexpr:
                if (!ParseConstExpr(rule, parser, obj)) {
                    return false;
                }
                break;
            case gscParser::RuleClass_def:
                if (!ParseClassDef(rule, parser, obj)) {
                    return false;
                }
                break;
            case gscParser::RuleFunction:
                if (!ParseFunction(rule, parser, obj)) {
                    return false;
                }
                break;
            default:
                obj.info.PrintLineMessage(core::logs::LVL_ERROR, es, "Bad export rule");
                return false;
            }
        }

        if (obj.devBlockDepth > 0) {
            obj.info.PrintLineMessage(core::logs::LVL_ERROR, prog, std::format("Missing {} #/", obj.devBlockDepth));
            return false;
        }

        return true;
    }

}