/**************************
* time: 2015-03-12-22.13
* name:  Hu Yo Can
* Copyright (c) 2015 Kean
* email: huyocan@163.com
****************************/


#include <cstdio>
#include <cstdlib>
#include <QDebug>
#include "lexer.h"
#include "parsetree.h"
#include "parserLL1.h"
#include "tokenmap.h"
#include "common.h"
#include "parsetree.h"


#define MAXPARSERERRORLEN   1024
/**错误信息输出的最大长度**/
#define LL1DEBUG
/**LL1语法分析调试选项**/

#ifdef LL1DEBUG

#define LL1OUTSYMBOLSTACK
/***输出每步符号栈中的内容选项***/
#define LL1CURTERMINALSYMBOL
/***输出每步规约掉的终极符的选项***/
#define LL1OUTPUTPRODUCT
/***输出每次使用的产生式及编号选项***/

#endif // LL1DEBUG
extern ProductExp products[ProductExpressionNum+5];/***所有产生式**/
extern void CreatLL1Table();
static void (*funcs[ProductExpressionNum+5])(void )= {NULL};/**对应于每条产生式的函数指针数组**/
static bool isInitFuncs=false;
static bool getExpResult=false;/*** 进入 RelExp处理标志***/
static bool getExpResult2=false;/**  进入Exp处理标志 **/
static TreeNode * rt=NULL;/**语法树根指针**/
static TreeNode ** tmpPtrPtr;/***语法树节点指针地址临时变量***/
static TreeNode * curTreePtr;/**当前节点指针**/
static TreeNode * savePtr=NULL;
static int expflag=0;/***记录未匹配的左括号的数量***/
static DecKinds  *tmpDecKind=NULL;///temp DecKind
static char  strError[MAXPARSERERRORLEN];/***存储错误信息的临时变量***/
static std::vector<Symbol> symbolStack;/**符号栈**/
static std::vector<TreeNode **> syntaxStack;/**语法树栈**/
static std::vector<TreeNode *> operatorStack;/**操作符栈**/
static std::vector<TreeNode *> numStack;/**操作数栈**/
static std::vector<string> errors;/**保存所有的语法错误信息**/
static std::vector<int> productsSequence;
static Token currentToken;/**当前正在处理的Token**/

/***1,4,5,6,10,13,16,17,18,26,28,29,30,31,35,37,38,39,40,42,43
46,47,49,53,54,55,56,58,60,72,75,77,79,83,86,87,91,99,100,
101,102,103,104,***/

TreeNode * getParserLL1TreeRoot()
{
    return rt;
}

string _getBlankStr_(int n)
{
    string str="";
    for(int i=0;i<n;i++)
        str+=" ";
    return str;
}

bool outputProductsTree(const char fileName[])
{
    int curIndent=0,curPro;
    Token tk;
    initReadOneToken();
    string tmpStr="";
    std::vector<Symbol> symvct;
    std::vector<int> symIndentVct;
    symvct.push_back(Symbol::Program);
    symIndentVct.push_back(curIndent);
    FILE * fn=fopen(fileName,"w");
    if(!fn) return false;

    for(int pseq=0;pseq<productsSequence.size();)
    {
        if(isTerminalSymbol(symvct.back()))
        {
            if(!ReadOneToken(tk))
                printf("something is wrong when get new token.");
            tmpStr=_getBlankStr_(symIndentVct.back())+getTokenStr(tk.type)+" "+tk.name;
            fprintf(fn,"%s\n",tmpStr.c_str());
            symvct.pop_back();
            symIndentVct.pop_back();
        }
        else
        {
            curIndent=symIndentVct.back();
            tmpStr=_getBlankStr_(curIndent)+getTokenStr(symvct.back());
            fprintf(fn,"%s\n",tmpStr.c_str());
            curIndent+=2;

            curPro=productsSequence[pseq++];
            //printf("%d ",curPro);
            symIndentVct.pop_back();
            symvct.pop_back();
            for(int i=(products[curPro].size())-1;i>0;i--)
                if(products[curPro][i]!=Symbol::LAMBDA)
                {
                    symvct.push_back(products[curPro][i]);
                    symIndentVct.push_back(curIndent);
                }
                else
                {
                    tmpStr=_getBlankStr_(curIndent)+"LAMBDA";
                    fprintf(fn,"%s\n",tmpStr.c_str());
                }

        }

    }
    fclose(fn);
    initReadOneToken();
    return true;
}

void pushProduct(int num)/**将产生式右部入栈**/
{
    productsSequence.push_back(num);
    for(int i=int(products[num].size())-1; i>0; i--)
        if(products[num][i]!=Symbol::LAMBDA)///lambda 不用入符号栈
            symbolStack.push_back(products[num][i]);
#ifdef LL1OUTPUTPRODUCT    /**输出调试信息**/
    printf(": %-3d ",num);
    for(Symbol tmp:products[num])
        printf("%-8s ",getTokenStr(tmp).c_str());
    puts("");
#endif // LL1OUTPUTPRODUCT
#ifdef LL1OUTSYMBOLSTACK    /**输出调试信息**/
    puts("Rest Symbol in Stack is:");
    for(Symbol tmp:symbolStack)
        printf("%-8s ",getTokenStr(tmp).c_str());
    puts("");
#endif // LL1OUTSYMBOLSTACK
}

/**插入错误信息**/
void insertError(string er)
{
    errors.push_back(er);
}

/***输出语法错误信息***/
void outputParserLL1Errors(bool toScrean,const char errorFileName[])
{
    if(toScrean)
    {
        for(string tmp:errors)
            printf("%s\n",tmp.c_str());
    }
    else
    {
        FILE * fn=fopen(errorFileName,"w");
        for(string tmp:errors)/**遍历Vector并输出**/
            fprintf(fn,tmp.c_str());
        fclose(fn);
    }

}

/** 初始化处理每一条产生式的函数数组 **/
void initParserLL1Funcs();
bool chooseFunc(int num)/**根据产生式编号选择执行那条产生式对应的函数**/
{
    printf("choosing func %d\n",num);
    if(num<1||num>ProductExpressionNum) return false;
    pushProduct(num);
    (*funcs[num])();
    return true;
}

void deleteTree(TreeNode * trnd)///删除上次创建的语法树
{
    if(trnd==NULL) return ;
    for(int i=0; i<MAXSON; i++)
        deleteTree(trnd->son[i]);
    deleteTree(trnd->sibling);
    delete trnd;
}

void deleteParserLL1Tree()/***删除语法树***/
{
    deleteTree(rt);
    rt=NULL;
}

bool parserLL1()/***语法分析主函数,返回真假值表示是否出现了语法错误***/
{
    /***init***/
    if(!isInitFuncs)
    {
        initParserLL1Funcs();
        isInitFuncs=true;
    }
    deleteParserLL1Tree();
    expflag=0;
    getExpResult2=false;
    getExpResult=true;
    savePtr=NULL;
    errors.clear();
    CreatLL1Table();
    symbolStack.clear();
    operatorStack.clear();
    numStack.clear();
    syntaxStack.clear();
    productsSequence.clear();
    symbolStack.push_back(Symbol::Program);
    rt=new TreeNode(0,NodeKinds::ProK);///根节点
    for(int i=2; i>=0; i--)
        syntaxStack.push_back(&(rt->son[i]));
    /***init***/
    ReadOneToken(currentToken);
    while(!symbolStack.empty())
    {
        if(isTerminalSymbol(symbolStack.back()))///终极符
        {
            if(currentToken.type==symbolStack.back())
            {
                symbolStack.pop_back();
#ifdef LL1CURTERMINALSYMBOL  /**输出调试信息**/
                printf(">>>>>>  Pop Terminal Symbol %s\n",currentToken.name.c_str());
#endif // LL1CURTERMINALSYMBOL
                ReadOneToken(currentToken);
            }
            else
            {
                sprintf(strError,"syntax error : unexpected tokentype= '%s' name= %s found at line %d,\n\
                        expected Token type is %s.\n",
                        getTokenStr(currentToken.type).c_str(),currentToken.name.c_str(),
                        currentToken.lineNum,getTokenStr(symbolStack.back()).c_str());
                insertError(strError);
#ifdef LL1OUTSYMBOLSTACK   /**输出调试信息**/
                puts("Rest Symbol in Stack is:");
                for(Symbol tmp:symbolStack)
                    printf("%-8s ",getTokenStr(tmp).c_str());
                puts("");
#endif // LL1OUTSYMBOLSTACK
                return false;
            }
        }
        else///非终极符
        {
            int pnum=LL1Table[int(symbolStack.back())][int(currentToken.type)];///取得对应的产生式
            printf("LL1 List %s %s \n",getTokenStr(symbolStack.back()).c_str(),getTokenStr(currentToken.type).c_str());
            Symbol tsym=symbolStack.back();
            symbolStack.pop_back();
            if(!chooseFunc(pnum))///没有对应的产生式
            {
                 printf("ok shit???????????");
                sprintf(strError,"syntax error : unexpected tokentype= '%s' name= %s found at line %d.\n  maybe missed one of these: %s",
                                        getTokenStr(currentToken.type).c_str(),currentToken.name.c_str(),
                                        currentToken.lineNum,getPredictStr(tsym).c_str());
                insertError(strError);
                return false;
            }
        }
    }
    if(currentToken.type!=TokenType::DOT)/***匹配结束符 . ***/
    {
        sprintf(strError,"syntax error : unexpected token found at line %d,Expected Dot\n",currentToken.lineNum);
        insertError(strError);
        return false;
    }
    else if(!ReadOneToken(currentToken))/***有结束符 . 则读入下一个TOKEN EOF***/
    {
        sprintf(strError,"syntax error : Token ends Before Code or File end\n");
        insertError(strError);
        return false;
    }
    if(currentToken.type==TokenType::ENDFILE)
        return true;
    else
    {
        sprintf(strError,"syntax error : unexpected token '%s' found at line %d,Expected EOF\n"
                ,getTokenStr(currentToken.type).c_str(),currentToken.lineNum);
        insertError(strError);
        return false;
    }
    return errors.size()==0;
}

#define PopBackSyntaxStack  syntaxStack.pop_back();

#define SyntaxPopBack \
    tmpPtrPtr = syntaxStack.back();\
    PopBackSyntaxStack
#define SyntaxPopBackAndSet \
    SyntaxPopBack;\
    (*tmpPtrPtr)=curTreePtr;
#define NewTreeNode(K) new TreeNode(currentToken.lineNum,NodeKinds::K);
#define NewTreeNode2(K,K2) new TreeNode(currentToken.lineNum,NodeKinds::K,int(K2));

#define NewSyntaxPopBackAndSet(K) \
    curTreePtr = new TreeNode(currentToken.lineNum,NodeKinds::K);\
    SyntaxPopBackAndSet;
#define NewSyntaxPopBackAndSet2(K,K2) \
    curTreePtr = new TreeNode(currentToken.lineNum,NodeKinds::K,int(K2));\
    SyntaxPopBackAndSet;

#define SiblingPushBack   syntaxStack.push_back(&(curTreePtr->sibling));
#define SonPushBack(N)     syntaxStack.push_back(&(curTreePtr->son[N]));
#define SonsPushBack    \
    SonPushBack(2);\
    SonPushBack(1);\
    SonPushBack(0);
#define NamePush   curTreePtr->name.push_back(currentToken.name);


void process1() {}///Program -> ProgramHead DeclarePart ProgramBody

void process2()///ProgramHead -> PROGRAM ProgramName
{
    NewSyntaxPopBackAndSet(PheadK);
}


void process3()/// ProgramName -> ID
{
    NamePush
}
void process4() {} ///DeclarePart -> TypeDec VarDec ProcDec
void process5() {} ///TypeDec  -> LAMBDA
void process6() {} ///TypeDec -> TypeDeclaration

void process7()/// TypeDec -> TYPE TypeDecList
{
    NewSyntaxPopBackAndSet(TypeK)
            SiblingPushBack
            SonPushBack(0)
}

void process8()/// TypeDecList -> TypeId EQ TypeName SEMI TypeDecMore
{
    NewSyntaxPopBackAndSet(DecK)
            SiblingPushBack
}

void process9()/// TypeDecMore -> LAMBDA
{
    PopBackSyntaxStack
}

void process10() {}/// TypeDecMore -> TypeDecList

void process11()/// TypeId -> ID
{
    NamePush
}

void process12()/// TypeName -> BaseType  /**************/
{
    tmpDecKind = &(curTreePtr->kind.dec);
}

void process13() {} ///TypeName -> StructureType

void process14() /// TypeName -> ID
{
    curTreePtr->kind.dec=DecKinds::IdK;
    curTreePtr->attr.type_name=currentToken.name;
}

void process15() /// BaseType -> INTEGER
{
    (*tmpDecKind)=DecKinds::IntegerK;
}

void process16()/// BaseType -> CHAR
{
    (*tmpDecKind) =CharK;
}

void process17() {}/// StructureType -> ArrayType
void process18() {}/// StructureType -> RecType

void process19()/// ArrayType -> ARRAY LMIDPAREN Low UNDERANGE Top RMIDPAREN OF BaseType
{
    (*curTreePtr).kind.dec=DecKinds::ArrayK;
    tmpDecKind = &(curTreePtr->attr.ArrayAttr.memtype);
}

void process20()/// Low -> INTC
{
    curTreePtr->attr.ArrayAttr.low=atoi(currentToken.name.c_str());
}

void process21()/// Top -> INTC
{
    curTreePtr->attr.ArrayAttr.up=atoi(currentToken.name.c_str());
}

void process22()/// RecType -> RECORD FieldDecList END
{
    curTreePtr->kind.dec=DecKinds::RecordK;
    savePtr = curTreePtr;
    SonPushBack(0)
}

void process23()/// FieldDecList -> BaseType IdList SEMI FieldDecMore
{
    NewSyntaxPopBackAndSet(DecK)
            tmpDecKind=&(curTreePtr->kind.dec);
    SiblingPushBack
}

void process24()/// FieldDecList -> ArrayType IdList SEMI FieldDecMore
{
    NewSyntaxPopBackAndSet(DecK)
            SiblingPushBack
}

void process25()/// FieldDecMore -> LAMBDA
{
    syntaxStack.pop_back();
    curTreePtr=savePtr;
}

void process26() {}/// FieldDecMore -> FieldDecList

void process27()/// IdList -> ID IdMore
{
    NamePush
}

void process28()/// IdMore -> LAMBDA
{
}

void process29() {}/// IdMore -> COMMA IdList
void process30() {}/// VarDec -> LAMBDA
void process31() {}/// VarDec -> VarDeclaration

void process32()/// VarDeclaration -> VAR VarDecList
{
    NewSyntaxPopBackAndSet(VarK);
    SiblingPushBack
            SonPushBack(0)
}

void process33()/// VarDecList -> TypeName VarIdList SEMI VarDecMore
{
    NewSyntaxPopBackAndSet(DecK)
            SiblingPushBack
}

void process34() /// VarDecMore -> LAMBDA     ///
{
    PopBackSyntaxStack
}

void process35()/// VarDecMore -> VarDecList
{
}

void process36()/// VarIdList -> ID VarIdMore
{
    NamePush
}

void process37() {}/// VarIdMore -> LAMBDA
void process38() {}/// VarIdMore -> COMMA VarIdList
void process39() {}/// ProcDec -> LAMBDA
void process40() {}/// ProcDec -> ProcDeclaration

void process41()/// ProcDeclaration -> PROCEDURE ProcName LPAREN ParamList RPAREN SEMI ProcDecPart ProcBody ProcDecMore
{
    NewSyntaxPopBackAndSet(ProcDecK);/*****/
    SiblingPushBack
            SonsPushBack

}

void process42() {}/// ProcDecMore -> LAMBDA
void process43() {}/// ProcDecMore -> ProcDeclaration

void process44() /// ProcName -> ID
{
    NamePush
}

void process45() /// ParamList -> LAMBDA
{
    PopBackSyntaxStack
}

void process46() {}/// ParamList -> ParamDecList
void process47() {}/// ParamDecList -> Param ParamMore

void process48()/// ParamMore -> LAMBDA
{
    PopBackSyntaxStack
}

void process49() {}/// ParamMore SEMI ParamDecList

void process50()/// Param -> TypeName FormList
{
    NewSyntaxPopBackAndSet(DecK)
            curTreePtr->attr.ProcAttr.param=ParamKinds::ValparamType;
    SiblingPushBack
}

void process51()/// Param -> VAR TypeName FormList
{
    NewSyntaxPopBackAndSet(DecK);
    curTreePtr->attr.ProcAttr.param=ParamKinds::VarparamType;
    SiblingPushBack
}

void process52()/// FormList -> ID FidMore
{
    NamePush
}

void process53() {}/// FidMore -> LAMBDA
void process54() {}/// FidMore COMMA -> FormList
void process55() {}/// ProcDecPart -> DeclarePart
void process56() {}/// ProcBody -> ProgramBody

void process57()/// ProgramBody -> BEGIN StmList END
{
    PopBackSyntaxStack
            NewSyntaxPopBackAndSet(StmLK);
    SonPushBack(0)
}

void process58() {}/// StmList -> Stm StmMore

void process59()/// StmMore -> LAMBDA
{
    PopBackSyntaxStack
}
void process60() {}/// StmMore -> SEMI StmList

void process61()/// Stm -> ConditionalStm
{
    NewSyntaxPopBackAndSet2(StmtK,IfK)
            SiblingPushBack
}

void process62()/// Stm -> LoopStm
{
    NewSyntaxPopBackAndSet2(StmtK,WhileK)
            SiblingPushBack
}

void process63()/// Stm -> InputStm
{
    NewSyntaxPopBackAndSet2(StmtK,ReadK)
            SiblingPushBack
}

void process64()/// Stm -> OutputStm
{
    NewSyntaxPopBackAndSet2(StmtK,WriteK)
            SiblingPushBack
}

void process65()/// Stm -> ReturnStm
{
    NewSyntaxPopBackAndSet2(StmtK,ReturnK)
            SiblingPushBack
}

void process66()/// Stm -> ID AssCall
{
    curTreePtr = NewTreeNode2(StmtK,AssignK);
    TreeNode * t1= NewTreeNode2(ExpK,VariK);
    t1->name.push_back(currentToken.name);
    curTreePtr->son[0]=t1;
    SyntaxPopBackAndSet
            SiblingPushBack
}

void process67()/// AssCall -> AssignmentRest
{
    curTreePtr->kind.stmt=AssignK;
}

void process68()/// AssCall -> CallStmRest
{
    curTreePtr->son[0]->attr.ExpAttr.varkind=IdV;
    curTreePtr->kind.stmt=CallK;
}

void process69()/// AssignmentRest -> VariMore ASSIGN  Exp
{
    SonPushBack(1);
    curTreePtr=curTreePtr->son[0];
    TreeNode *t=NewTreeNode2(ExpK,OpK);
    t->attr.ExpAttr.op=END;
    operatorStack.push_back(t);/******************************/
}

void process70()/// ConditionalStm -> IF RelExp THEN StmList ELSE StmList FI
{
    SonsPushBack
}

void process71()/// LoopStm -> WHILE RelExp DO StmList ENDWH
{
    SonPushBack(1)
            SonPushBack(0)
}

void process72() {}/// InputStm -> READ LPAREN InVar RPAREN

void process73()/// InVar -> ID
{
    NamePush
}

void process74()/// OutputStm -> WRITE LPAREN Exp RPAREN
{
    SonPushBack(0)
            TreeNode *t = NewTreeNode2(ExpK,OpK);
    t->attr.ExpAttr.op=TokenType::END;
    operatorStack.push_back(t);
}

void process75() {}/// ReturnStm -> RETURN

void process76()/// CallStmRest -> LPAREN ActParamList RPAREN
{
    SonPushBack(1)
}

void process77()/// ActParamList -> LAMBDA
{
    SyntaxPopBack;
}

void process78()/// ActParamList -> Exp ActParamMore
{
    TreeNode *t = NewTreeNode2(ExpK,OpK);
    t->attr.ExpAttr.op=TokenType::END;
    operatorStack.push_back(t);
}

void process79() {}/// ActParamMore -> LAMBDA

void process80()/// ActParamMore -> COMMA ActParamList
{
    SiblingPushBack
}

int getPriority(TokenType op)
{
    int pri=0;
    switch(op)
    {
    case TokenType::END:
        pri=-1;
        break;
    case TokenType::LPAREN:
        pri=0;
        break;
    case TokenType::LT:
    case TokenType::EQ:
        pri=1;
        break;
    case TokenType::PLUS:
    case TokenType::MINUS:
        pri=2;
        break;
    case TokenType::TIMES:
    case TokenType::OVER:
        pri=3;
        break;
    default:
    {
        sprintf(strError,"no any operator like this : %d\n",int(op));
        insertError(strError);
        pri=-1;
        break;
    }
    }
    return pri;
}

/**********************表达式部分************/
void process81()/// RelExp -> Exp OtherRelE
{
    TreeNode *t =NewTreeNode2(ExpK,OpK);
    t->attr.ExpAttr.op=TokenType::END;
    operatorStack.push_back(t);

    getExpResult=false;
}

void process82() /// OtherRelE -> CmpOp Exp
{
    TreeNode *tt =NewTreeNode2(ExpK,OpK);
    tt->attr.ExpAttr.op=currentToken.type;

    TokenType sTop = operatorStack.back()->attr.ExpAttr.op;
    while(getPriority(sTop) >=getPriority(currentToken.type))
    {
        TreeNode * t = operatorStack.back();
        operatorStack.pop_back();
        TreeNode *Rnum=numStack.back();
        numStack.pop_back();
        TreeNode * Lnum=numStack.back();
        numStack.pop_back();
        t->son[0]=Lnum;
        t->son[1]=Rnum;
        numStack.push_back(t);
        sTop=operatorStack.back()->attr.ExpAttr.op;
    }
    operatorStack.push_back(tt);
    getExpResult = true;
}

void process83() {}/// Exp -> Term OtherTerm

void process84()/// OtherTerm -> LAMBDA
{
    if((currentToken.type==TokenType::RPAREN)&&(expflag!=0))
    {
        while(operatorStack.back()->attr.ExpAttr.op!=TokenType::LPAREN)
        {
            TreeNode * t = operatorStack.back();
            operatorStack.pop_back();
            TreeNode *Rnum=numStack.back();
            numStack.pop_back();
            TreeNode * Lnum=numStack.back();
            numStack.pop_back();
            t->son[0]=Lnum;
            t->son[1]=Rnum;
            numStack.push_back(t);
        }
        operatorStack.pop_back();
        expflag--;
    }
    else
    {
        if(getExpResult||getExpResult2)
        {
            while(operatorStack.back()->attr.ExpAttr.op!=TokenType::END)
            {
                TreeNode * t = operatorStack.back();
                operatorStack.pop_back();
                TreeNode *Rnum=numStack.back();
                numStack.pop_back();
                TreeNode * Lnum=numStack.back();
                numStack.pop_back();
                t->son[0]=Lnum;
                t->son[1]=Rnum;
                numStack.push_back(t);
            }
            operatorStack.pop_back();
            curTreePtr=numStack.back();
            numStack.pop_back();
            tmpPtrPtr = syntaxStack.back();
            syntaxStack.pop_back();
            (*tmpPtrPtr)=curTreePtr;
            if(getExpResult2=true)
                getExpResult2=false;
        }
    }

}

void process85()/// OtherTerm -> AddOp Exp
{
    TreeNode *tt =NewTreeNode2(ExpK,OpK);
    tt->attr.ExpAttr.op=currentToken.type;

    TokenType sTop = operatorStack.back()->attr.ExpAttr.op;
    while(getPriority(sTop) >=getPriority(currentToken.type))
    {
        TreeNode * t = operatorStack.back();
        operatorStack.pop_back();
        TreeNode * Rnum=numStack.back();
        numStack.pop_back();
        TreeNode * Lnum=numStack.back();
        numStack.pop_back();
        t->son[0]=Lnum;
        t->son[1]=Rnum;
        numStack.push_back(t);
        sTop=operatorStack.back()->attr.ExpAttr.op;
    }
    operatorStack.push_back(tt);
}

void process86() {}/// Term -> Factor OtherFactor
void process87() {}/// OtherFactor -> LAMBDA


void process88()/// OtherFactor -> MultOp Term
{
    TreeNode *tt =NewTreeNode2(ExpK,OpK);
    tt->attr.ExpAttr.op=currentToken.type;

    TokenType sTop = operatorStack.back()->attr.ExpAttr.op;
    while(getPriority(sTop) >=getPriority(currentToken.type))
    {
        TreeNode * t = operatorStack.back();
        operatorStack.pop_back();
        TreeNode *Rnum=numStack.back();
        numStack.pop_back();
        TreeNode * Lnum=numStack.back();
        numStack.pop_back();
        t->son[0]=Lnum;
        t->son[1]=Rnum;
        numStack.push_back(t);
        sTop=operatorStack.back()->attr.ExpAttr.op;
    }
    operatorStack.push_back(tt);
}

void process89()/// Factor -> LPAREN Exp RPAREN
{
    TreeNode *t = NewTreeNode2(ExpK,OpK);
    t->attr.ExpAttr.op=currentToken.type;
    operatorStack.push_back(t);
    expflag++;
}

void process90()/// Factor -> INTC
{
    TreeNode *t=NewTreeNode2(ExpK,ConstK);
    t->attr.ExpAttr.val=atoi(currentToken.name.c_str());

    numStack.push_back(t);
}

void process91() {}/// Factor -> Variable

void process92()/// Variable -> ID VariMore
{
    curTreePtr=NewTreeNode2(ExpK,VariK);
    NamePush
            numStack.push_back(curTreePtr);
}

void process93()/// VariMore -> LAMBDA
{
    curTreePtr->attr.ExpAttr.varkind=IdV;
}

void process94()/// VariMore -> LMIDPAREN Exp RMIDPAREN
{
    curTreePtr->attr.ExpAttr.varkind=ArrayMembV;
    SonPushBack(0)
            TreeNode *t = NewTreeNode2(ExpK,OpK);
    t->attr.ExpAttr.op=END;
    operatorStack.push_back(t);

    getExpResult2=true;
}

void process95()/// VariMore -> DOT  FieldVar
{
    curTreePtr->attr.ExpAttr.varkind=FieldMembV;
    SonPushBack(0)
}

void process96()/// FieldVar -> ID FieldVarMore
{
    curTreePtr=NewTreeNode2(ExpK,VariK);
    NamePush
            SyntaxPopBackAndSet/*******Bingo ?*******/
}

void process97()/// FieldVarMore -> LAMBDA
{
    curTreePtr->attr.ExpAttr.varkind=IdV;
}

void process98()/// FieldVarMore -> LMIDPAREN Exp RMIDPAREN
{
    process94();
}

void process99() {}/// CmpOp -> LT
void process100() {}/// CmpOp -> EQ
void process101() {}/// AddOp -> PLUS
void process102() {}/// AddOp -> MINUS
void process103() {}/// MultOp -> TIMES
void process104() {}/// MultOp -> OVER
#ifndef NoCharC
void process105()/// Factor -> CHARC
{
    TreeNode *t=NewTreeNode2(ExpK,ConstK);
    t->attr.ExpAttr.val=int(currentToken.name.c_str()[0]);//atoi(currentToken.name.c_str());
    numStack.push_back(t);
}
#endif

#define HAHAHA

#ifdef HAHAHA
#define SETFUNC(n) funcs[n]=process##n
#define SETFUNC1(n) \
    SETFUNC(n##1) ;\
    SETFUNC(n##2) ;\
    SETFUNC(n##3) ;\
    SETFUNC(n##4) ;\
    SETFUNC(n##5) ;\
    SETFUNC(n##6) ;\
    SETFUNC(n##7) ;\
    SETFUNC(n##8) ;\
    SETFUNC(n##9) ;\
    SETFUNC(n##0) ;
#endif // HAHAHA

void initParserLL1Funcs()
{
#ifdef HAHAHA
    SETFUNC1(1);
    SETFUNC1(2);
    SETFUNC1(3);
    SETFUNC1(4);
    SETFUNC1(5);
    SETFUNC1(6);
    SETFUNC1(7);
    SETFUNC1(8);
    SETFUNC1(9);
    SETFUNC(100);
    SETFUNC(101);
    SETFUNC(102);
    SETFUNC(103);
    SETFUNC(104);
#ifndef NoCharC
    SETFUNC(105);
#endif
    SETFUNC(1);
    SETFUNC(2);
    SETFUNC(3);
    SETFUNC(4);
    SETFUNC(5);
    SETFUNC(6);
    SETFUNC(7);
    SETFUNC(8);
    SETFUNC(9);
#endif // HAHAHA
}
