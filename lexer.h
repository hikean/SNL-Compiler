#ifndef LEXER_H_INCLUDED
#define LEXER_H_INCLUDED


//#define NoCharC
/**不处理字符常量，取消注释则不会处理字符常量的，影响包括词法、语法LL1,递归下降默认不支持**/
#define SingleLineComment

#define ShowNewTree
/***使用 # 的单行注释用法 **/

struct Token;
/****词法分析器
sourceFileName 源代码文件名
返回值 True 成功/False失败 ***/
bool lexer(const char sourceFileName[]);


/***    输出Token到屏幕或文件,默认输出到屏幕***/
void outputTokens(bool toScreen=true,const char  tokenFileName[]="tokenList.txt");
/***   输出词法分析的错误信息到文件或屏幕默认输出到屏幕  ***/
void outputLexerErrors(bool toScreen=true,const char errorFileName[]="lexerError.txt");
/****读取一个Token ****/
bool ReadOneToken(Token & tk);

void initReadOneToken();
#include<vector>
std::vector<Token> & getTokenList();
/***getTokenList  获取词法分析器的结果,结果存在vector中，需要确认vector是否为空***/

#endif // LEXER_H_INCLUDED
