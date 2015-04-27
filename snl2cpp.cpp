/**************************
* time: 2015-04-15-22.37
* name: K:\C_CPP\Compiler\snl2cpp.cpp
* Copyright (c) 2015 Kean
* email: huyocan@163.com
****************************/
#include "common.h"
#include "lexer.h"
#include "compile.h"
#include <stdio.h>
#include<iostream>
using namespace std;

#define MAXFUNCLAYER 100
static  std::map<int,string>  snlMapCpp;
static bool isSetMap=false;
static std::vector<Token> cppTokens;
static std::vector<Token> snlTokens;
static int index=0;


void typeState()//////////
{
    index++;///name
    while(snlTokens[index].type!=TokenType::VAR&&
          snlTokens[index].type!=TokenType::PROCEDURE&&
          snlTokens[index].type!=TokenType::BEGIN)
    {
         cppTokens.push_back(Token(snlTokens[index].lineNum,"typedef ",TokenType::ID));
         if(snlTokens[index+2].type==TokenType::INTEGER||snlTokens[index+2].type==TokenType::CHAR)
         {
             cppTokens.push_back(snlTokens[index+2]);
             cppTokens.push_back(snlTokens[index]);
             cppTokens.push_back(snlTokens[index+3]);
             index+=3;
         }
         else if(snlTokens[index+2].type==TokenType::ARRAY)//ARRAY LMIDPAREN Low UNDERANGE Top RMIDPAREN OF BaseType
         {
             cppTokens.push_back(snlTokens[index+9]);///typename
             cppTokens.push_back(snlTokens[index]);/// newtype
             cppTokens.push_back(snlTokens[index+3]);///[
             cppTokens.push_back(snlTokens[index+6]);///intc
             cppTokens.push_back(snlTokens[index+7]);///]
             cppTokens.push_back(snlTokens[index+10]);///;
             index+=10;
         }
         else if(snlTokens[index+2].type==TokenType::RECORD)//RECORD FieldDecList END   FieldDecList -> BaseType IdList SEMI FieldDecMore
         {
             int tmp=index;
            cppTokens.push_back(snlTokens[index+2]);///record
            cppTokens.push_back(snlTokens[index]);/// name
            cppTokens.push_back(Token(snlTokens[index+2].lineNum,"{\n ",TokenType::ID));
            index+=3;///jmp =
            while(snlTokens[index].type!=TokenType::END)
            {
                cppTokens.push_back(snlTokens[index++]);
            }
            cppTokens.push_back(snlTokens[index]);///end
            cppTokens.back().name="}";
            cppTokens.back().type=TokenType::ID;
            cppTokens.push_back(snlTokens[tmp]);/// name
            cppTokens.push_back(snlTokens[++index]);///;
         }
         index++;
    }
    index--;
}


void procedureState()
{
    if(snlTokens[index].type==TokenType::PROGRAM)
    {
        cppTokens.push_back(Token(snlTokens[index].lineNum," void program(){\n ",TokenType::ID));
        index=1;
        return;
    }
    index++;
    string tmpnm="auto "+snlTokens[index].name+"=[&]";
    cppTokens.push_back(Token(snlTokens[index].lineNum,tmpnm.c_str(),TokenType::ID));
    bool hasRef=snlTokens[index+2].type==TokenType::VAR;
    int curType=index+2;
    if(hasRef) curType++;
    ++index;//jump name
    while(snlTokens[index].type!=TokenType::RPAREN)
    {
        cppTokens.push_back(snlTokens[index]);
        if(snlTokens[index].type==TokenType::SEMI)
        {
            cppTokens[cppTokens.size()-1].type=TokenType::COMMA;
            hasRef=snlTokens[index+1].type==TokenType::VAR;
            curType=index+1;
            if(hasRef) curType++;
        }
        else if(snlTokens[index].type==TokenType::VAR)
        {
            ++index;
            hasRef=true;
            curType=index;
            cppTokens.push_back(snlTokens[index]);
            cppTokens.push_back(Token(snlTokens[index].lineNum," & ",TokenType::ID));
        }
        if(snlTokens[index].type==TokenType::COMMA)
        {
            cppTokens.push_back(snlTokens[curType]);
            if(hasRef)
                cppTokens.push_back(Token(snlTokens[index].lineNum," & ",TokenType::ID));
        }
        index++;
    }
    cppTokens.push_back(snlTokens[index]);///)
    ++index;///;
     cppTokens.push_back(Token(snlTokens[index].lineNum," ->void\n{\n",TokenType::ID));
}


void snl2cpp(const char cppFileName[])
{
    snlTokens=getTokenList();
    if(!isSetMap)
    {
        isSetMap=true;
        snlMapCpp[TokenType::IF]=" if(";
        snlMapCpp[TokenType::THEN]=")\n{\n";
        snlMapCpp[TokenType::ELSE]=";\n}\nelse\n{\n";
        snlMapCpp[TokenType::FI]=";}\n";
        snlMapCpp[TokenType::WHILE]=" while(";
        snlMapCpp[TokenType::DO]=")\n{\n";
        snlMapCpp[TokenType::ENDWH]=";\n}\n";
        snlMapCpp[TokenType::BEGIN]="\n";
        snlMapCpp[TokenType::END]=";\n};\n";
        snlMapCpp[TokenType::RECORD]="struct ";
        snlMapCpp[TokenType::RETURN]="return";
        snlMapCpp[TokenType::INTEGER]="int ";
        snlMapCpp[TokenType::CHAR]="char ";
        snlMapCpp[TokenType::ASSIGN]="=";
        snlMapCpp[TokenType::EQ]="==";
        snlMapCpp[TokenType::LT]="<";
        snlMapCpp[TokenType::PLUS]="+";
        snlMapCpp[TokenType::MINUS]="-";
        snlMapCpp[TokenType::TIMES]="*";
        snlMapCpp[TokenType::OVER]="/";
        snlMapCpp[TokenType::LPAREN]="(";
        snlMapCpp[TokenType::RPAREN]=")";
        snlMapCpp[TokenType::DOT]=".";
        snlMapCpp[TokenType::COLON]=":";
        snlMapCpp[TokenType::SEMI]=";\n ";
        snlMapCpp[TokenType::COMMA]=",";
        snlMapCpp[TokenType::LMIDPAREN]="[";
        snlMapCpp[TokenType::RMIDPAREN]="]";
        snlMapCpp[TokenType::UNDERANGE]=" ";
        snlMapCpp[TokenType::VAR]=" ";
        snlMapCpp[TokenType::TYPE]=" ";
    }
    cppTokens.clear();
    string tmpstem ="#include<iostream>\nusing namespace std;\n"\
                    "void write(int a) {cout<<a<<endl;}\n"\
                    "void read(int &a){ cin>>a;}\n";

    cppTokens.push_back(Token(0,tmpstem.c_str(),TokenType::ID));
    index=0;
    while(index< snlTokens.size())
    {
        if(snlTokens[index].type==TokenType::TYPE)
            typeState();
        else if(snlTokens[index].type==TokenType::BEGIN)///
        {
            cppTokens.push_back(snlTokens[index++]);///begin
            std::map<string,string> name2type;
            while(snlTokens[index].type!=TokenType::END)
            {
                cppTokens.push_back(snlTokens[index++]);
            }
            cppTokens.push_back(snlTokens[index]);///end
           /// cppTokens.push_back(Token(snlTokens[index].lineNum,";\n",TokenType::ID));
        }
        else if(snlTokens[index].type==TokenType::PROCEDURE||
                snlTokens[index].type==TokenType::PROGRAM)
            procedureState();
        else
            cppTokens.push_back(snlTokens[index]);
         //cout<<" alll: "<<snlTokens[index].name<<endl;
        index++;
    }
    cppTokens.pop_back();
    cppTokens.pop_back();
    cppTokens.push_back(Token(snlTokens[index].lineNum,"int main()\n{\nprogram();\n;return 0;\n}\n",TokenType::ID));
    FILE *outCpp=fopen(cppFileName,"w");
    for(int i=0;i<cppTokens.size();i++)
    {
        if(snlMapCpp[cppTokens[i].type]!="")
            cppTokens[i].name=snlMapCpp[cppTokens[i].type];
        if(i+1!=cppTokens.size() &&
            cppTokens[i].type==cppTokens[i+1].type &&
            cppTokens[i+1].type==TokenType::ID)
        {
            fprintf(outCpp,"%s ",cppTokens[i].name.c_str());
            cout<<cppTokens[i].name<<" ";
        }
        else
        {
            fprintf(outCpp,"%s",cppTokens[i].name.c_str());
            cout<<cppTokens[i].name;
        }
    }
    fclose(outCpp);
    astyleBeatify(cppFileName);

    FILE *inCpp=fopen(cppFileName,"r");
    char tch[5000];
    while(fgets(tch,200,inCpp)!=NULL)
        printf("%s",tch);
    fclose(inCpp);
}
