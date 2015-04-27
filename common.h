#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED
#include<string>
#include<iostream>
#include<set>
#include<vector>
#include<map>
#include"lexer.h"
using std::string;
#define NotTerminalSymbolStartValue int(TokenType::Program)
#define MaxSymbolValue int(TokenType::MultOp)
#define isTerminalSymbol(x) (int(x)< NotTerminalSymbolStartValue)
#ifdef NoCharC
#define ProductExpressionNum    104
#else
#define ProductExpressionNum    105
#endif


 enum TokenType
{
    /* 单词符号 */
    ENDFILE=0,ERROR,
    /* 保留字 */
    PROGRAM,	PROCEDURE,	TYPE,	VAR,		IF,
    THEN,		ELSE,		FI,		WHILE,		DO,
    ENDWH,		BEGIN,		END,	READ,		WRITE,
    ARRAY,		OF,			RECORD,	RETURN,

    INTEGER,	CHAR,
    /* 多字符单词符号 */
    ID,			INTC,		CHARC,
    /*特殊符号 */
    ASSIGN/** := **/,		EQ /** = **/,			LT/** < **/,		PLUS/** + **/,		MINUS/** - **/,
    TIMES/** * **/,		OVER/** / **/,		LPAREN/**(**/,	RPAREN/**)**/,		DOT/**.**/,
    COLON/**:**/,		SEMI/**;**/,		COMMA/**,**/,	LMIDPAREN/**[**/,	RMIDPAREN/**]**/,
    UNDERANGE/**..**/,
    /*** newly added **/
    LAMBDA,  ///is terminal Symbol ?
    /*** 非终极符  ***/
    Program,        ProgramHead,	    ProgramName,	DeclarePart,
    TypeDec,        TypeDeclaration/****/,	TypeDecList,	TypeDecMore,
    TypeId,         TypeName/****/,			BaseType,	    StructureType,
    ArrayType,      Low,	            Top,            RecType,
    FieldDecList,   FieldDecMore,	    IdList,	        IdMore,
    VarDec,         VarDeclaration/****/, 	VarDecList,		VarDecMore,
    VarIdList,      VarIdMore,		    ProcDec,		ProcDeclaration,
    ProcDecMore,    ProcName,		    ParamList,		ParamDecList,
    ParamMore,      Param,		        FormList,		FidMore,
    ProcDecPart,    ProcBody,	    	ProgramBody,	StmList,
    StmMore,        Stm,				AssCall,		AssignmentRest,
    ConditionalStm, StmL,			    LoopStm,		InputStm,
    InVar,          OutputStm,		    ReturnStm,		CallStmRest,
    ActParamList,   ActParamMore,		RelExp,			OtherRelE,
    Exp,			OtherTerm,		    Term,           OtherFactor,
    Factor,         Variable,			VariMore,		FieldVar,
    FieldVarMore,   CmpOp,			    AddOp,          MultOp,
    /*** newly added ***/
    //TypeDecPart,        TypeDef,        VarDecPart,
} ;//TokenType;
typedef TokenType Symbol;


typedef struct Token
{
    int  lineNum;//行号
    string name;//名称
    TokenType type;//标识符类型
    Token(int ln=0):lineNum(ln),type(TokenType::ERROR) {}
    Token(int ln,const char ch[],TokenType tp):lineNum(ln),name(ch),type(tp) {}
} Token;

typedef std::vector<Symbol> ProductExp;
extern ProductExp products[ProductExpressionNum+5];
extern int LL1Table[MaxSymbolValue+5][MaxSymbolValue+5];
extern std::set<TokenType> Predict[ProductExpressionNum+5];

void initProducts();
void InitPredict();
string getPredictStr(Symbol sm);


#endif // COMMON_H_INCLUDED
