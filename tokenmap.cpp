#include"tokenmap.h"
#include<map>

static std::map<string,int> tokenMap;
static std::map<TokenType,string> tokenStrMap;
static bool setTokenMap=false,setTokenStrMap=false;
TokenType getIDtype(string str)
{
    if(!setTokenMap)
    {
        tokenMap["program"]=int(TokenType::PROGRAM);
        tokenMap["type"]=int(TokenType::TYPE);
        tokenMap["var"]=int(TokenType::VAR);
        tokenMap["procedure"]=int(TokenType::PROCEDURE);
        tokenMap["begin"]=int(TokenType::BEGIN);///
        tokenMap["end"]=int(TokenType::END);
        tokenMap["array"]=int(TokenType::ARRAY);
        tokenMap["of"]=int(TokenType::OF);
        tokenMap["record"]=int(TokenType::RECORD);
        tokenMap["if"]=int(TokenType::IF);///
        tokenMap["else"]=int(TokenType::ELSE);
        tokenMap["then"]=int(TokenType::THEN);
        tokenMap["fi"]=int(TokenType::FI);
        tokenMap["while"]=int(TokenType::WHILE);
        tokenMap["do"]=int(TokenType::DO);///
        tokenMap["endwh"]=int(TokenType::ENDWH);
        tokenMap["read"]=int(TokenType::READ);
        tokenMap["write"]=int(TokenType::WRITE);
        tokenMap["return"]=int(TokenType::RETURN);
        tokenMap["integer"]=int(TokenType::INTEGER);///
        tokenMap["char"]=int(TokenType::CHAR);
        setTokenMap=true;
    }
    if(tokenMap[str]) return TokenType(tokenMap[str]);
    return TokenType::ID;
}
string getTokenStr(TokenType tkty)
{
    if(!setTokenStrMap)
    {
#if 0
        tokenStrMap[TokenType::ENDFILE]=string("EOF");
        tokenStrMap[TokenType::ERROR]=string("ERROR");
        tokenStrMap[TokenType::PROGRAM]=string("Reserved Word:");
        tokenStrMap[TokenType::PROCEDURE]=string("Reserved Word:");
        tokenStrMap[TokenType::TYPE]=string("Reserved Word:");
        tokenStrMap[TokenType::VAR]=string("Reserved Word:");
        tokenStrMap[TokenType::IF]=string("Reserved Word:");
        tokenStrMap[TokenType::THEN]=string("Reserved Word:");
        tokenStrMap[TokenType::ELSE]=string("Reserved Word:");
        tokenStrMap[TokenType::FI]=string("Reserved Word:");
        tokenStrMap[TokenType::WHILE]=string("Reserved Word:");
        tokenStrMap[TokenType::DO]=string("Reserved Word:");
        tokenStrMap[TokenType::ENDWH]=string("Reserved Word:");
        tokenStrMap[TokenType::BEGIN]=string("Reserved Word:");
        tokenStrMap[TokenType::END]=string("Reserved Word:");
        tokenStrMap[TokenType::READ]=string("Reserved Word:");
        tokenStrMap[TokenType::WRITE]=string("Reserved Word:");
        tokenStrMap[TokenType::ARRAY]=string("Reserved Word:");
        tokenStrMap[TokenType::OF]=string("Reserved Word:");
        tokenStrMap[TokenType::RECORD]=string("Reserved Word:");
        tokenStrMap[TokenType::RETURN]=string("Reserved Word:");
        tokenStrMap[TokenType::INTEGER]=string("Reserved Word:");
        tokenStrMap[TokenType::CHAR]=string("Reserved Word:");
        tokenStrMap[TokenType::ID]=string("id name=");
        tokenStrMap[TokenType::INTC]=string("const int value =");
        tokenStrMap[TokenType::CHARC]=string("const char value =");
        tokenStrMap[TokenType::ASSIGN]=string("ASSIGN");
        tokenStrMap[TokenType::EQ]=string("EQ");
        tokenStrMap[TokenType::LT]=string("LT");
        tokenStrMap[TokenType::PLUS]=string("PLUS");
        tokenStrMap[TokenType::MINUS]=string("MINUS");
        tokenStrMap[TokenType::TIMES]=string("TIMES");
        tokenStrMap[TokenType::OVER]=string("OVER");
        tokenStrMap[TokenType::LPAREN]=string("LPAREN");
        tokenStrMap[TokenType::RPAREN]=string("RPAREN");
        tokenStrMap[TokenType::DOT]=string("DOT");
        tokenStrMap[TokenType::COLON]=string("COLON");
        tokenStrMap[TokenType::SEMI]=string("SEMI");
        tokenStrMap[TokenType::COMMA]=string("COMMA");
        tokenStrMap[TokenType::LMIDPAREN]=string("LMIDPAREN");
        tokenStrMap[TokenType::RMIDPAREN]=string("RMIDPAREN");
        tokenStrMap[TokenType::UNDERANGE]=string("UNDERANGE");
#else
        tokenStrMap[TokenType::ENDFILE]=string("ENDFILE");
        tokenStrMap[TokenType::ERROR]=string("ERROR");
        tokenStrMap[TokenType::PROGRAM]=string("PROGRAM");
        tokenStrMap[TokenType::PROCEDURE]=string("PROCEDURE");
        tokenStrMap[TokenType::TYPE]=string("TYPE");
        tokenStrMap[TokenType::VAR]=string("VAR");
        tokenStrMap[TokenType::IF]=string("IF");
        tokenStrMap[TokenType::THEN]=string("THEN");
        tokenStrMap[TokenType::ELSE]=string("ELSE");
        tokenStrMap[TokenType::FI]=string("FI");
        tokenStrMap[TokenType::WHILE]=string("WHILE");
        tokenStrMap[TokenType::DO]=string("DO");
        tokenStrMap[TokenType::ENDWH]=string("ENDWH");
        tokenStrMap[TokenType::BEGIN]=string("BEGIN");
        tokenStrMap[TokenType::END]=string("END");
        tokenStrMap[TokenType::READ]=string("READ");
        tokenStrMap[TokenType::WRITE]=string("WRITE");
        tokenStrMap[TokenType::ARRAY]=string("ARRAY");
        tokenStrMap[TokenType::OF]=string("OF");
        tokenStrMap[TokenType::RECORD]=string("RECORD");
        tokenStrMap[TokenType::RETURN]=string("RETURN");
        tokenStrMap[TokenType::INTEGER]=string("INTEGER");
        tokenStrMap[TokenType::CHAR]=string("CHAR");
        tokenStrMap[TokenType::ID]=string("ID");
        tokenStrMap[TokenType::INTC]=string("INTC");
        tokenStrMap[TokenType::CHARC]=string("CHARC");
        tokenStrMap[TokenType::ASSIGN]=string("ASSIGN");
        tokenStrMap[TokenType::EQ]=string("EQ");
        tokenStrMap[TokenType::LT]=string("LT");
        tokenStrMap[TokenType::PLUS]=string("PLUS");
        tokenStrMap[TokenType::MINUS]=string("MINUS");
        tokenStrMap[TokenType::TIMES]=string("TIMES");
        tokenStrMap[TokenType::OVER]=string("OVER");
        tokenStrMap[TokenType::LPAREN]=string("LPAREN");
        tokenStrMap[TokenType::RPAREN]=string("RPAREN");
        tokenStrMap[TokenType::DOT]=string("DOT");
        tokenStrMap[TokenType::COLON]=string("COLON");
        tokenStrMap[TokenType::SEMI]=string("SEMI");
        tokenStrMap[TokenType::COMMA]=string("COMMA");
        tokenStrMap[TokenType::LMIDPAREN]=string("LMIDPAREN");
        tokenStrMap[TokenType::RMIDPAREN]=string("RMIDPAREN");
        tokenStrMap[TokenType::UNDERANGE]=string("UNDERANGE");
        tokenStrMap[TokenType::LAMBDA]=string("LAMBDA");
        tokenStrMap[TokenType::Program]=string("Program");
        tokenStrMap[TokenType::ProgramHead]=string("ProgramHead");
        tokenStrMap[TokenType::ProgramName]=string("ProgramName");
        tokenStrMap[TokenType::DeclarePart]=string("DeclarePart");
        tokenStrMap[TokenType::TypeDec]=string("TypeDec");
        tokenStrMap[TokenType::TypeDeclaration]=string("TypeDeclaration");
        tokenStrMap[TokenType::TypeDecList]=string("TypeDecList");
        tokenStrMap[TokenType::TypeDecMore]=string("TypeDecMore");
        tokenStrMap[TokenType::TypeId]=string("TypeId");
        tokenStrMap[TokenType::TypeName]=string("TypeName");
        tokenStrMap[TokenType::BaseType]=string("BaseType");
        tokenStrMap[TokenType::StructureType]=string("StructureType");
        tokenStrMap[TokenType::ArrayType]=string("ArrayType");
        tokenStrMap[TokenType::Low]=string("Low");
        tokenStrMap[TokenType::Top]=string("Top");
        tokenStrMap[TokenType::RecType]=string("RecType");
        tokenStrMap[TokenType::FieldDecList]=string("FieldDecList");
        tokenStrMap[TokenType::FieldDecMore]=string("FieldDecMore");
        tokenStrMap[TokenType::IdList]=string("IdList");
        tokenStrMap[TokenType::IdMore]=string("IdMore");
        tokenStrMap[TokenType::VarDec]=string("VarDec");
        tokenStrMap[TokenType::VarDeclaration]=string("VarDeclaration");
        tokenStrMap[TokenType::VarDecList]=string("VarDecList");
        tokenStrMap[TokenType::VarDecMore]=string("VarDecMore");
        tokenStrMap[TokenType::VarIdList]=string("VarIdList");
        tokenStrMap[TokenType::VarIdMore]=string("VarIdMore");
        tokenStrMap[TokenType::ProcDec]=string("ProcDec");
        tokenStrMap[TokenType::ProcDeclaration]=string("ProcDeclaration");
        tokenStrMap[TokenType::ProcDecMore]=string("ProcDecMore");
        tokenStrMap[TokenType::ProcName]=string("ProcName");
        tokenStrMap[TokenType::ParamList]=string("ParamList");
        tokenStrMap[TokenType::ParamDecList]=string("ParamDecList");
        tokenStrMap[TokenType::ParamMore]=string("ParamMore");
        tokenStrMap[TokenType::Param]=string("Param");
        tokenStrMap[TokenType::FormList]=string("FormList");
        tokenStrMap[TokenType::FidMore]=string("FidMore");
        tokenStrMap[TokenType::ProcDecPart]=string("ProcDecPart");
        tokenStrMap[TokenType::ProcBody]=string("ProcBody");
        tokenStrMap[TokenType::ProgramBody]=string("ProgramBody");
        tokenStrMap[TokenType::StmList]=string("StmList");
        tokenStrMap[TokenType::StmMore]=string("StmMore");
        tokenStrMap[TokenType::Stm]=string("Stm");
        tokenStrMap[TokenType::AssCall]=string("AssCall");
        tokenStrMap[TokenType::AssignmentRest]=string("AssignmentRest");
        tokenStrMap[TokenType::ConditionalStm]=string("ConditionalStm");
        tokenStrMap[TokenType::StmL]=string("StmL");
        tokenStrMap[TokenType::LoopStm]=string("LoopStm");
        tokenStrMap[TokenType::InputStm]=string("InputStm");
        tokenStrMap[TokenType::InVar]=string("InVar");
        tokenStrMap[TokenType::OutputStm]=string("OutputStm");
        tokenStrMap[TokenType::ReturnStm]=string("ReturnStm");
        tokenStrMap[TokenType::CallStmRest]=string("CallStmRest");
        tokenStrMap[TokenType::ActParamList]=string("ActParamList");
        tokenStrMap[TokenType::ActParamMore]=string("ActParamMore");
        tokenStrMap[TokenType::RelExp]=string("RelExp");
        tokenStrMap[TokenType::OtherRelE]=string("OtherRelE");
        tokenStrMap[TokenType::Exp]=string("Exp");
        tokenStrMap[TokenType::OtherTerm]=string("OtherTerm");
        tokenStrMap[TokenType::Term]=string("Term");
        tokenStrMap[TokenType::OtherFactor]=string("OtherFactor");
        tokenStrMap[TokenType::Factor]=string("Factor");
        tokenStrMap[TokenType::Variable]=string("Variable");
        tokenStrMap[TokenType::VariMore]=string("VariMore");
        tokenStrMap[TokenType::FieldVar]=string("FieldVar");
        tokenStrMap[TokenType::FieldVarMore]=string("FieldVarMore");
        tokenStrMap[TokenType::CmpOp]=string("CmpOp");
        tokenStrMap[TokenType::AddOp]=string("AddOp");
        tokenStrMap[TokenType::MultOp]=string("MultOp");

#endif
        setTokenStrMap=true;
    }

    if(tokenStrMap[tkty].size())
        return tokenStrMap[tkty];
    return string("unknown TokenType");
}
