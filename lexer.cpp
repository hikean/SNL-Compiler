#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include"common.h"

/***
isdigit() isalnum() isalpha() isspace()
***/
#include"common.h"
#include"lexer.h"
#include"tokenmap.h"

#define MAXIDLEN  1024
#define MAXERRORLEN 1024


typedef enum State/***自动机状态***/
{
    Start,InID,InNum,Done,InAssign,
    InComment,InRange,InChar,SError,
    SFileEnd,Finished,
} State;

static int lineCnt;/**当前代码行号,即源代码中文本文件的行号，用\n区分**/
static int tmpLn;///记录行号的临时变量
static FILE *sourceText=NULL;///源代码文件
static State status;///自动机状态
static char currentChar;///当前正在处理的字符
static std::vector<Token> tokens;///使用vector存储所有的Token
static std::vector<string> errors;
static bool singleChar[256]= {0}; ///单个分隔字符
static bool isSetSigleChar=false;///是否初始化了sigleChar[]标志
static char tmpChar;///临时变量用来记录预读取的字符
static char tmpStrError[MAXERRORLEN];///储存错误信息的字符数组

static size_t tkPtr=0;//Token指针
bool ReadOneToken(Token & tk) { /**顺序读取一个token**/
    //printf("%s\n",tokens[tkPtr].name.c_str());
    if(tkPtr<tokens.size())
    {
        tk=tokens[tkPtr++];
        return true;
    }
    tkPtr=0;///go to first
    return false;
}

std::vector<Token> & getTokenList()
{
    return tokens;
}

void initReadOneToken()
{
    tkPtr=0;
}
inline char readOneChar()///从输入流中读取一个字符
{
    tmpChar=getc(sourceText);// uha,I'm tmpChar ,I'm Just Here.Be Careful!
    if(tmpChar=='\n')
        lineCnt++;//This is the only place where lineCnt Would Be Modified.
    return tmpChar;
}


#define RECSTART  \
    char ch[MAXIDLEN];\
    int n=0;\
    Token tk(lineCnt);
#define RECONE \
    {\
        ch[n++]=currentChar;\
        if( n==MAXIDLEN)\
        {\
            sprintf(tmpStrError,"ID or NUM has been too long  ,at line: %d\n",tmpLn);\
            status=State::SError;\
            return Token(lineCnt,"ID_OR_NUM_IS_TOO_LONG",TokenType::ERROR);\
        }\
        currentChar=readOneChar();\
    }

Token recNum()///识别十进制整数
{
    RECSTART
    while(isdigit(currentChar))
        RECONE
        ch[n]='\0';
    tk.name=string(ch);
    tk.type=TokenType::INTC;
    status=State::Done;
    if(isalpha(currentChar))
    {
        status=State::SError;
        sprintf(tmpStrError,"Expected punctuation or blank after '%c',at line %d\n",ch[n-1],lineCnt);
    }
    return tk;
}
Token recID()///识别标识符和保留字
{
    RECSTART
    while(isalnum(currentChar))
        RECONE
        ch[n]='\0';
    tk.name=string(ch);
    tk.type=getIDtype(tk.name);//是否是保留字？
    status=State::Done;
    return tk;
}

void recComment()///识别处理注释
{
    tmpLn=lineCnt;
    while(currentChar!='}')
    {
        currentChar=readOneChar();
        if(currentChar==EOF)
        {
            status=State::SError;
            sprintf(tmpStrError,"{ excepted } ,at line :%d",tmpLn);
            return;
        }
    }
    currentChar=readOneChar();
    status=State::Done;
}
#ifdef SingleLineComment
void recLineComment()
{
    while(currentChar!='\n'&&currentChar!=EOF)
        currentChar=readOneChar();
    if(currentChar==EOF)
    {
        status=State::SError;
        sprintf(tmpStrError,"excepted  ENTER ,at line :%d",lineCnt);
        return;
    }
    currentChar=readOneChar();
    status=State::Done;
}
#endif
/***打印所有Token到文件或者屏幕,默认输出到文件****/
void outputTokens(bool toScreen,const char  tokenFileName[])
{
    if(toScreen)///输出到屏幕
    {
        for(size_t i=0; i<tokens.size(); i++)
            printf("%-5d  %-18s %s \n",tokens[i].lineNum,getTokenStr(tokens[i].type).c_str(),tokens[i].name.c_str());
    }
    else///输出到文件
    {
        FILE * fn=fopen(tokenFileName,"w");
        for(size_t i=0; i<tokens.size(); i++)
        {
           fprintf(fn,"%-5d  %-18s %s \n",tokens[i].lineNum,getTokenStr(tokens[i].type).c_str(),tokens[i].name.c_str());
           // fprintf(fn,"%-5d  %-18d %s \n",tokens[i].lineNum,tokens[i].type,tokens[i].name.c_str());
        }
        fclose(fn);
    }
}
/**** 输出错误信息到文件或屏幕 ****/
void outputLexerErrors(bool toScreen,const char errorFileName[])///输出错误信息到文件
{
    if(toScreen)///输出到屏幕
    {
        for(size_t i=0; i<errors.size(); i++)
            puts(errors[i].c_str());
    }
    else///输出到文件
    {
        FILE * fn=fopen(errorFileName,"w");
        for(size_t i=0; i<errors.size(); i++)
            fputs(errors[i].c_str(),fn);
        fclose(fn);
    }
}


/****词法分析器
sourceFileName 源代码文件名
返回值 True 成功/False失败***/
bool lexer(const char sourceFileName[])
{   /**init**/
    if(!isSetSigleChar)
    {
        singleChar['+']=singleChar['-']=singleChar['*']=singleChar['/']=true;
        singleChar['(']=singleChar[')']=singleChar[';']=singleChar['[']=true;
        singleChar[']']=singleChar['=']=singleChar['<']=singleChar['\n']=true;
        singleChar['\t']=singleChar[' ']=singleChar['\r']=true;//=singleChar[EOF]
        singleChar['\f']=singleChar['\v']=true;
        isSetSigleChar=true;
    }
    sourceText = fopen(sourceFileName,"r");
    if(!sourceText)//文件打开失败
    {
        errors.push_back(string("File Open Failed.Sure You've Given Me A Right File Name?\n"));
        return false;
    }
    tmpStrError[0]='\0';
    lineCnt=1;
    tkPtr=0;
    tokens.clear();
    errors.clear();
    currentChar=readOneChar();
    status=State::Start;
    /**init**/
    while(status!=State::SFileEnd)
    {
        status=State::Start;
        if(isdigit(currentChar))
            tokens.push_back(recNum());
        else if(isalpha(currentChar))
            tokens.push_back(recID());
        else if(currentChar=='{')
            recComment();

        else if(currentChar==EOF|| isspace(currentChar)||singleChar[ int(currentChar)])/****** maybe bugs here, while using unicode ****/
        {
            switch (currentChar)
            {
            case '+':
                tokens.push_back(Token(lineCnt,"+",TokenType::PLUS));
                break;
            case '-':
                tokens.push_back(Token(lineCnt,"-",TokenType::MINUS));
                break;
            case '*':
                tokens.push_back(Token(lineCnt,"*",TokenType::TIMES));
                break;
            case '/':
                tokens.push_back(Token(lineCnt,"/",TokenType::OVER));
                break;
            case '(':
                tokens.push_back(Token(lineCnt,"(",TokenType::LPAREN));
                break;
            case ')':
                tokens.push_back(Token(lineCnt,")",TokenType::RPAREN));
                break;
            case ';':
                tokens.push_back(Token(lineCnt,";",TokenType::SEMI));
                break;
            case '[':
                tokens.push_back(Token(lineCnt,"[",TokenType::LMIDPAREN));
                break;
            case ']':
                tokens.push_back(Token(lineCnt,"]",TokenType::RMIDPAREN));
                break;
            case '=':
                tokens.push_back(Token(lineCnt,"=",TokenType::EQ));
                break;
            case '<':
                tokens.push_back(Token(lineCnt,"<",TokenType::LT));
                break;
            case EOF:
                tokens.push_back(Token(lineCnt,"EOF",TokenType::ENDFILE));
                break;
            default : /***other blank char***/
                break;
            }
            if(currentChar!=EOF)
            {
                currentChar=readOneChar();
                status=State::Done;
            }
            else status=State::SFileEnd;
        }
        else if(currentChar=='.')
        {
            tmpChar=readOneChar();
            // tmpChar is Really A Bug,If You Don't Know Where It Comes Up.
            if(tmpChar!='.')/***sth need to do***/
            {
                tokens.push_back(Token(lineCnt,".",TokenType::DOT));
                status=State::Done;
            }
            else
            {
                tokens.push_back(Token(lineCnt,"..",TokenType::UNDERANGE));
                tmpChar=readOneChar();
                status=State::Done;
            }
            currentChar=tmpChar;
        }
        else if (currentChar==':')
        {
            tmpLn=lineCnt;
            tmpChar=readOneChar();
            if(tmpChar=='=')
            {
                tokens.push_back(Token(lineCnt,":=",TokenType::ASSIGN));
                currentChar=readOneChar();
                status=State::Done;
            }
            else
            {
                status=State::SError;
                sprintf(tmpStrError,"Unexpected  char after ':',should be = ,line: %d\n",tmpLn);
                currentChar=tmpChar;
            }
        }
        else if(currentChar==',')
        {
            tmpLn=lineCnt;
            tmpChar=readOneChar();
            if(isalnum(tmpChar)||isspace(tmpChar))
            {
                status=State::Done;
                tokens.push_back(Token(tmpLn,",",TokenType::COMMA));
                currentChar=tmpChar;
                status=State::Done;
            }
            else
            {
                status=State::SError;
                sprintf(tmpStrError,"Unexpected  char after ',' at line: %d\n",tmpLn);
                currentChar=tmpChar;
            }
        }
#ifndef NoCharC /**charC*/
        else if(currentChar=='\'')
        {
            char tch=readOneChar();
            currentChar=readOneChar();
            if(currentChar!='\'')
            {
                status=State::SError;
                sprintf(tmpStrError,"Unexpected char '%c',at line %d\n",currentChar,lineCnt);
                currentChar=readOneChar();
            }
            else
            {
                char tstr[]={tch,'\0'};
                tokens.push_back(Token(lineCnt,tstr,TokenType::CHARC));
                currentChar=readOneChar();
                status=State::Done;
            }
        }
#endif
#ifdef SingleLineComment /**# 单行注释*/
        else if(currentChar=='#')
        {
            recLineComment();
        }
#endif
        else //if(currentChar!='\n'&&currentChar!='\t'&&currentChar!=' ')
        {
            status=State::SError;
            sprintf(tmpStrError,"Unexpected char '%c',at line %d\n",currentChar,lineCnt);
            currentChar=readOneChar();
        }
        switch (status)
        {
        case SError:
            errors.push_back(string(tmpStrError));
            break;
        default:
            break;
        }
    }
    fclose(sourceText);///关闭源文件
    return errors.size()==0;
}
