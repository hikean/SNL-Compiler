/**************************
* time: 2015-03-15-13.04
* name:  name)
* Copyright (c) 2015 Kean
* email: huyocan@163.com
****************************/
#include <cstdio>
#include "common.h"
#include "parsetree.h"
#include "tokenmap.h"
#include "lexer.h"

#ifndef ShowNewTree
#define OutTreeInsertBlank
#endif
/**采用松散形式输出语法树**/

#define MaxNodeKindsValue   int(NodeKinds::ExpK)
#define MaxDecKindsValue    int(DecKinds::IdK)
#define MaxStmtKindsValue   int(StmtKinds::ReturnK)
#define MaxExpKindsValue    int(ExpKinds::VariK)
#define MaxParamKindsValue  int(ParamKinds::VarparamType)
#define MaxVarKindsValue    int(VarKinds::FieldMembV)
#define MaxExpTypeValue     int(ExpType::Boolean)

#define _ParamK     PheadK
#define _ExpType    TypeK
///                              ParamK   ExpType
string  strNodeKinds[]= {"ProK","PheadK","TypeK","VarK","ProcDecK","StmLK","DecK","StmtK","ExpK"};
string strDecKinds[]= {"ArrayK","CharK","IntegerK","RecordK","IdK"} ;
string strStmtKinds[]= {"IfK","WhileK","AssignK","ReadK","WriteK","CallK","ReturnK"};
string strExpKinds[]= {"OpK","ConstK","VariK"} ;
string strParamKinds[]= {"ValparamType","VarparamType"} ;///
string strVarKinds[]= {"IdV","ArrayMembV","FieldMembV"} ;
string strExpType[]= {"Void","Integer","Boolean"} ;///




string getKindStr(NodeKinds nks,int onk=-1) /**取得对应的Kind的字符串表示**/
{
    if(int(nks)<0 ||int(nks)>MaxNodeKindsValue) return string("");
    if(onk<0)
        return strNodeKinds[nks];
    switch (nks)
    {
    case DecK:
        if(onk <= MaxDecKindsValue)
            return strDecKinds[onk];
        break;
    case StmtK:
        if(onk <= MaxStmtKindsValue)
            return strStmtKinds[onk];
        break;
    case ExpK:
        if(onk<=MaxExpKindsValue)
            return strExpKinds[onk];
        break;
    case VarK:
        if(onk <= MaxVarKindsValue )
            return strVarKinds[onk];
        break;
    case _ParamK :
        if(onk <=MaxParamKindsValue)
            return strParamKinds[onk];
        break;
    case _ExpType:
        if(onk <= MaxExpTypeValue)
            return strExpType[onk];
        break;
    default:
        return string("  ---Error");
    }
    return string(" ");
}

static  vector<string> outTree;/**保存要输出的语法树字符串信息，一个字符串为一行**/
#define IndentSeg   4   /***缩进的空格个数***/
static int indent=0;///记录语法树输出时的缩进
string getBlankStr(int n) /**得到对应长度的空白字符串和|___来表示缩进**/
{
    string t="";
    for(int i=0; i<n-IndentSeg; i++)
        t=t+" ";
#ifndef ShowNewTree
    if(n)
        t+="|___";/// │
#else
    if(n)
        t+="    ";
#endif
    return t;
}


void inserNodeInfo(const string str)/**插入一个要输出的语法树节点**/
{
    outTree.push_back(str);

#ifdef OutTreeInsertBlank  /* If The Blank Is Short,There Would Be A Bug. */
    outTree.push_back(string("                                                                        \n"));
#endif // OutTreeInsertBlank
}

void beautifyOutTree()
{
    for(int i=int(outTree.size())-1; i; i--)
    {
        int tag=-1;
        for(int j=0; j<outTree[i].size(); j+=IndentSeg)
            if(outTree[i][j]=='|' && outTree[i-1][j]==' ')
            {
                tag=j;
                break;
            }
        if(tag!=-1)
        {
            for(int j=i-1; j; j--)
                if(outTree[j][tag]==' ')
                    outTree[j][tag]='|';
                else break;
        }
    }
}

string inline getIntStr(int n)  /****将整数转为字符串*****/
{
    char t[20];
    sprintf(t,"%d",n);
    return string(t);
}

static bool _outParam=false;

void _outputOneNode(TreeNode * rt)/*****输出语法树的一个节点*****/
{
    if(rt==NULL) return ;
    string tmpStr=getBlankStr(indent);
    switch(rt->nodekind)
    {
    case ProK :
    case TypeK:

    case StmLK:
        tmpStr+=getKindStr(rt->nodekind);
        break;
    case VarK :
        tmpStr+=getKindStr(rt->nodekind)+" ";
        break;
    case PheadK:
    case ProcDecK:
        tmpStr+=getKindStr(rt->nodekind)+" ";
        for(string t:rt->name)
            tmpStr=tmpStr + t+" ";
        break;
    case StmtK:
        tmpStr=tmpStr+"StmtK "+getKindStr(StmtK,rt->kind.stmt);
        break;
    case ExpK:
        tmpStr=tmpStr+"ExpK ";//+getKindStr(_ExpType,rt->attr.ExpAttr.type)+" ";

        for(string t:rt->name)
            tmpStr=tmpStr + t+" ";
        if(rt->kind.exp==ExpKinds::OpK)
        {
            tmpStr+=getTokenStr(rt->attr.ExpAttr.op)+" ";
        }
        else if(rt->kind.exp==ExpKinds::ConstK)
        {
            tmpStr+=getIntStr(rt->attr.ExpAttr.val)+" ";
        }
        else  if(rt->kind.exp==ExpKinds::VariK)
        {
           ;// tmpStr+=getKindStr(NodeKinds::VarK,rt->attr.ExpAttr.varkind)+" ";//IdV
        }
        tmpStr+=getKindStr(NodeKinds::ExpK,rt->kind.exp)+" ";
        break;
    case DecK:
        tmpStr=tmpStr+getKindStr(rt->nodekind)+" ";

        tmpStr = tmpStr + getKindStr(DecK,rt->kind.dec)+" "+rt->attr.type_name+ ": ";
        for(string t:rt->name)
            tmpStr=tmpStr + t+" ";
        /*****/
        if(_outParam)
            tmpStr+=getKindStr(_ParamK,rt->attr.ProcAttr.param)+" ";
        break;
    default:
        break;
    }
    inserNodeInfo(tmpStr+"\n");
}


void _outputParserTree(TreeNode * rt)/****递归输出整个语法树****/
{
    if(rt==NULL) return;
    _outputOneNode(rt);
    indent+=IndentSeg;
    for(int i=0; i<MAXSON; i++)
        if(rt->son[i])
        {
            _outParam=rt->nodekind==NodeKinds::ProcDecK;
            _outputParserTree(rt->son[i]);
        }
    indent-=IndentSeg;
    _outputParserTree(rt->sibling);
}

void outputParserTree(TreeNode * rt)/**** 输出语法树到屏幕的驱动函数 *****/
{
    indent=0;
     _outParam=false;
    outTree.clear();
    _outputParserTree(rt);
#ifndef ShowNewTree
    beautifyOutTree();
#endif
    for(string t:outTree)
        printf("%s",t.c_str());
}

void outputParserTree(TreeNode *rt,const char fileName[])/**** 输出语法树到文件的驱动函数 *****/
{
    indent=0;
    _outParam=false;
    outTree.clear();
    _outputParserTree(rt);
#ifndef ShowNewTree
    beautifyOutTree();
#endif
    FILE *fn=fopen(fileName,"w");
    for(string t:outTree)
        fprintf(fn,"%s",t.c_str());
     fclose(fn);
}







