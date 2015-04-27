#ifndef PARSETREE_H_INCLUDED
#define PARSETREE_H_INCLUDED

#include "common.h"
#include <vector>
#include <string>
#include <cstring>
using namespace std;

struct symbnode;

const int MAXSON = 3;

typedef enum {ProK,PheadK,TypeK,VarK,ProcDecK,StmLK,DecK,StmtK,ExpK} NodeKinds;

typedef enum {ArrayK,CharK,IntegerK,RecordK,IdK} DecKinds;

typedef enum {IfK,WhileK,AssignK,ReadK,WriteK,CallK,ReturnK} StmtKinds;

typedef enum {OpK,ConstK,VariK} ExpKinds;

typedef enum {ValparamType,VarparamType} ParamKinds;

typedef enum {IdV,ArrayMembV,FieldMembV} VarKinds;

typedef enum {Void,Integer,Boolean} ExpType;

struct symbtable;

struct SArrayAttr
{
    int low;/*数组下界*/
    int up; /*数组上界*/
    DecKinds memtype; /*数组成员类型*/
};
struct SProcAttr
{
    ParamKinds param; /*函数参类型*/
};
struct SExpAttr
{
    TokenType op;
    int val;
    VarKinds varkind;
    ExpType type;
};
struct SAttr
{
    SArrayAttr ArrayAttr;/*记录数组类型的属性*/

    SProcAttr ProcAttr;

    SExpAttr ExpAttr;
    string type_name;/*记录类型名*/
};

struct TreeNode
{
    TreeNode * son [MAXSON]; /*儿子结点*/
    TreeNode * sibling; /*兄弟结点*/

    int lineno; /*源程序行号*/
    NodeKinds nodekind; /*结点类型，取值有：ProK,PeadK,TypeK,VarK,ProcDecK,StmLK,DecK,StmtK,ExpK*/

    union
    {
        DecKinds dec;
        StmtKinds stmt;
        ExpKinds exp;
    } kind;

    vector<string> name; /*结点中标志符的名字*/

    SAttr attr; /*记录语法树结点的其他属性*/

    TreeNode(const int & linenum,const NodeKinds nks,int cks = -1)
    {
        /* 初始化新语法树节点t各子节点son[i]为NULL */
        memset(son,NULL,sizeof(son));
        for(int i=0; i<MAXSON; i++)
            son[i]=NULL;
        /* 初始化新语法树节点t兄弟节点sibling为NULL */
        sibling = NULL;
        /* 指定新语法树节点t成员:结点类型nodekind*/
        nodekind = nks;
        /* 指定新语法树节点t成员:源代码行号lineno为全局变量linenum */
        lineno = linenum;

        if(cks != -1)
        {
            if(nks == StmtK)
                kind.stmt = (StmtKinds)cks;
            else if(nks == ExpK)
            {
                kind.exp = (ExpKinds)cks;
                attr.ExpAttr.varkind = IdV;
                /* 指定新语法树节点t成员: 类型检查类型type为Void */
                attr.ExpAttr.type = Void;
            }
        }
    }
};


void outputParserTree(TreeNode* rt);

#endif // PARSETREE_H_INCLUDED
