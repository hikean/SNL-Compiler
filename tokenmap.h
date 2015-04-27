#ifndef TOKENMAP_H_INCLUDED
#define TOKENMAP_H_INCLUDED
#include"common.h"

/***根据标志符判断是否是保留字并返回其TokenType***/
TokenType getIDtype(string str);


/***返回TokenType的字符串表示***/
string getTokenStr(TokenType tkty);

#endif // TOKENMAP_H_INCLUDED
