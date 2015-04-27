#ifndef SEMANTICS_H_INCLUDED
#define SEMANTICS_H_INCLUDED

#include <vector>
#include <string>
#include <cstdio>
#include "parsetree.h"
using namespace std;

/*******************************类型的内部表示begin**********************************/
typedef enum {arrayTy,charTy,intTy,recordTy,boolTy} TypeKinds;

struct typeIR /**类型的内部表示**/
{
    int size; ///类型所占空间大小
    TypeKinds kind; ///类型
};

struct intIR:public typeIR ///int内部表示
{
    intIR() {size = 1;kind = intTy;}
};

struct boolIR:public typeIR ///bool内部表示
{
    boolIR() {size = 1;kind = boolTy;}
};

struct charIR:public typeIR ///char内部表示
{
    charIR() {size = 1;kind = charTy;}
};

struct arrayIR:public typeIR ///数组内部表示
{
    int low;
    typeIR * elemTyPtr; ///指向数组元素的类型内部表示
    arrayIR(const int & s,typeIR * eptr,int L) {size = s;kind = arrayTy;elemTyPtr = eptr;low=L;}
};

struct rdnode   ///域节点
{
    string idname; ///记录域中的标识符名
    typeIR * unitTypePtr;///指向域类型的内部表示
    int offset; ///表示当前标识符在记录中的内部偏移
};
typedef vector<rdnode> RecordDomain;///记录域列表 用vector实现
struct recordIR:public typeIR  ///记录类型的内部表示
{
    RecordDomain rdlist;
    recordIR(const int & s) {size = s;kind = recordTy;}
};
/*******************************类型的内部表示end************************************/


/*******************************标识符的内部表示begin**********************************/
typedef enum {typeKind,varKind,procKind} IdKinds; ///标识符类型

typedef enum {dir,indir} AccessKinds; ///直接还是间接变量

struct symbnode;
typedef vector<int> ParamTable ;///形参信息表，用vector实现

struct idIR    /**标识符的内部表示**/
{
    typeIR * typePtr; ///指向标识符的类型内部表示
    IdKinds kind; ///标识符的类型

        ///varIR
        AccessKinds access; ///直接还是间接变量
        int level; ///标识符层数
        int off; ///当前偏移量


        ///procIR
        ParamTable param; ///形参信息表，用vector实现
        ///int level;

};

/*******************************标识符的内部表示end**********************************/

/****符号表****/
struct symbnode ///符号节点
{
    string idname; ///标识符名
    idIR attrIR; ///标识符属性
};
typedef vector<symbnode> Lsymbtable;

inline void semanticsError(const string & s,const int & linenumber);
symbnode * findSymb(const int & lev,const string & snamen);
symbnode * findSymbAll(const string & sname);
void analyzeProK(TreeNode * rt); ///分析根节点
void analyzePheadK(TreeNode *  rt);///分析头节点
void analyzeDeclareK(TreeNode * rt); ///分析声明标志节点
void analyzeStmLK(TreeNode * rt); ///分析语句节点
void analyzeTypedecK(TreeNode * rt); ///分析类型声明具体节点
void analyzeVardecK(TreeNode * rt,symbnode * stp = NULL,bool isparam = 0);///分析变量声明具体节点
void analyzeProcdecK(TreeNode * rt);///分析过程声明具体节点
void createTable();///往scope栈添加一行符号表
void addSymble(const symbnode & st);///给当前层添加符号(改动)
void printSymbTable(const int & lev); ///输出某层符号表
void printSymb(const symbnode & st);///输出单个符号
void printSymbType();///输出scope栈所有符号表
typeIR * menType2TypePtr(const DecKinds & dk);///将数组成员类型转换为字符串
int getTypeSize(TreeNode * rt);
typeIR * getTypePtr(TreeNode * rt);///根据树声明节点创建类型内部表示并返回内部表示指针
void deleteTable();
void analyzeParam(TreeNode * rt,symbnode * stp);
void StmLKWhile(TreeNode * rt);
void StmLKIf(TreeNode * rt);
void StmLKWrite(TreeNode * rt);
void StmLKRead(TreeNode * rt);
typeIR * getExpType(TreeNode * rt,AccessKinds * ac = NULL,typeIR * recmemptr = NULL);
void StmLKReturn(TreeNode * rt);
void StmLKCall(TreeNode * rt);
void StmLKAssign(TreeNode * rt);
/*************/

bool analyze(TreeNode * rt);
void outputErrorlist(bool toscreen = 1,const char fileName[]="semanticsError.txt");

#endif // SEMANTICS_H_INCLUDED
