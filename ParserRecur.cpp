#include "parsetree.h"
#include "lexer.h"
#include "ParserRecur.h"
#include <cstdio>
#include <cstdlib>
#include "tokenmap.h"

#define TreeNode(nkk) TreeNode(linenum,nkk)
#define defaulttoken \
    {\
        ReadOneToken(token);\
        syntaxError("unexpected token is here!");\
    }

static int linenum = 0;
static Token token;
static FILE * listing;
static string temp_name;
static bool Error = false;

TreeNode * program();
TreeNode * varDec();
void match (TokenType tkt);
void syntaxError(char * message);
TreeNode * programHead();
TreeNode * declarePart();
TreeNode * programBody();
TreeNode * procDec();
TreeNode * typeDec();
TreeNode * typeDeclaration();
TreeNode * typeDecList();
void typeDef(TreeNode * t);
TreeNode * fieldDecList();
bool belong(const int & n);
void baseType(TreeNode * t);
void strutureType(TreeNode * t);
void arrayType(TreeNode * t);
void idMore(TreeNode * t);
void recType(TreeNode * t);
void idList(TreeNode * t);
TreeNode * fieldDecMore();
TreeNode * varDeclaration();
TreeNode * varDecList();
void varIdList(TreeNode * t);
void varIdMore(TreeNode * t);
TreeNode * procDeclaration();
TreeNode * varDecMore();
void paramList(TreeNode * t);
TreeNode * paramDecList();
TreeNode * param();
void formList(TreeNode * t);
void fidMore(TreeNode * t);
TreeNode * procDecPart();
TreeNode * procBody();
TreeNode * stmList();
TreeNode * stmMore();
TreeNode * stm();
TreeNode * assCall();
TreeNode * assignmentRest();
TreeNode * exp();
TreeNode * loopStm();
TreeNode * inputStm();
TreeNode * outputStm();
TreeNode * returnStm();
TreeNode * callStmRest();
TreeNode * actParamList();
TreeNode * actParamMore();
TreeNode * term();
TreeNode * simple_exp();
TreeNode * factor();
void variMore(TreeNode * t);
TreeNode * variable();
TreeNode * fieldvar();
void fieldvarMore(TreeNode * t );
TreeNode * conditionalStm();
/**
**/
void printnowtoken()
{
    cout<<token.name<<endl<<token.lineNum<<endl;
}
inline bool belong(const int & n)
{
    return Predict[n].count(token.type);
}

void baseType(TreeNode * t)
{
    if(Error) return;
    if(belong(15))
    {
        match(INTEGER);
        t->kind.dec = IntegerK;
    }
    else if(belong(16))
    {
        match(CHAR);
        t->kind.dec = CharK;
    }
    else defaulttoken;
}

TreeNode * fieldDecMore()
{
    if(Error) return NULL;
    TreeNode * t = NULL;
	if(belong(25)) ;
	else if(belong(26))
        t=fieldDecList();
    else defaulttoken;
	return t;
}

void varIdMore(TreeNode * t)
{
    if(Error) return;
	if(belong(37));
	else if(belong(38))
    {
        match(COMMA);
		varIdList(t);
    }
	else defaulttoken;
}

void varIdList(TreeNode * t)
{
    if(Error) return;
	if (token.type==ID)
	{
        t->name.push_back(token.name);
        match(ID);
	}
	else
	{
		syntaxError("a varid is expected here!");
		ReadOneToken(token);
	}
	varIdMore(t);
}

TreeNode * varDecMore()
{
    if(Error) return NULL;
	TreeNode * t =NULL;
	if(belong(34)) ;
	else if(belong(35))
		t = varDecList();
	else defaulttoken;
	return t;
}

void idMore(TreeNode * t)
{
    if(Error) return;
	if(belong(28)) ;
	else if(belong(29))
    {
        match(COMMA);
        idList(t);
    }
	else defaulttoken;
}

TreeNode * varDecList()
{
    if(Error) return NULL;
	TreeNode * t = new TreeNode(DecK);
	TreeNode * p = NULL;
	if ( t != NULL )
	{
		t->lineno = token.lineNum;
		typeDef(t);
	    varIdList(t);
	    match(SEMI);
        p = varDecMore();
	    t->sibling = p;
	}
	return t;
}


TreeNode * varDeclaration()
{
    if(Error) return NULL;
	match(VAR);
	TreeNode * t = varDecList();
    if(t==NULL)
		syntaxError("a var declaration is expected!");
	return t;
}

TreeNode * varDec()
{
    if(Error) return NULL;
	TreeNode * t = NULL;
	if(belong(30));
	else if(belong(31))
		t = varDeclaration();
	else defaulttoken;
	return t;
}

void idList(TreeNode * t)
{
    if(Error) return;
	if (token.type==ID)
	{
        t->name.push_back(token.name);
		match(ID);
	}
	idMore(t);
}

TreeNode * procDecPart()
{
    if(Error) return NULL;
     return declarePart();
}

TreeNode * fieldDecList()
{
    if(Error) return NULL;
	TreeNode * t = new TreeNode(DecK);
	TreeNode * p = NULL;
	if (t!=NULL)
	{
	    t->lineno = token.lineNum;
		if(belong(23))
        {
           baseType(t);
	       idList(t);
	       match(SEMI);
	       p = fieldDecMore();
        }
	    else if(belong(24))
        {
           arrayType(t);
	       idList(t);
	       match(SEMI);
	       p = fieldDecMore();
        }
        else defaulttoken;
	    t->sibling = p;
	}
	return t;
}

void recType(TreeNode * t)
{
    if(Error) return;
    t->kind.dec = RecordK;
    TreeNode * p = NULL;
	match(RECORD);
    p = fieldDecList();
	if(p!=NULL)
		t->son[0] = p;
	else
		syntaxError("a record body is requested!");
    match(END);
}

void arrayType(TreeNode * t)
{
    if(Error) return;
     match(ARRAY);
	 match(LMIDPAREN);
     if (token.type== INTC)
		 t->attr.ArrayAttr.low = atoi(token.name.c_str());
	 match(INTC);
	 match(UNDERANGE);
	 if (token.type==INTC)
		 t->attr.ArrayAttr.up = atoi(token.name.c_str());
	 match(INTC);
	 match(RMIDPAREN);
	 match(OF);
     baseType(t);
     t->attr.ArrayAttr.memtype = t->kind.dec;
	 t->kind.dec = ArrayK;
}


void strutureType(TreeNode * t)
{
    if(Error) return;
	if(belong(17))
		arrayType(t);
    else if(belong(18))
        recType(t);
    else defaulttoken;
}

void typeDef(TreeNode * t)
{
    if(Error) return;
    if(belong(12))
        baseType(t);
    else if(belong(13))
        strutureType(t);
    else if(belong(14))
    {
        t->kind.dec = IdK;
        t->attr.type_name = token.name;
        match(ID);
    }
    else defaulttoken;
}

TreeNode * typeDecMore()
{
    if(Error) return NULL;
    TreeNode * t = NULL;
    if(belong(10))
        t = typeDecList();
    else if(!belong(9)) defaulttoken;
    return t;
}

TreeNode * typeDecList()
{
    if(Error) return NULL;
    TreeNode * t = new TreeNode(DecK);
	if (t != NULL)
	{
		t->lineno = token.lineNum;
		if (token.type==ID)
            t->name.push_back(token.name);//typeId()
        match(ID);
	    match(EQ);
	    typeDef(t);
	    match(SEMI);
        t->sibling = typeDecMore();
	}
    return t;
}

TreeNode * typeDeclaration()
{
    if(Error) return NULL;
    match(TYPE);
	TreeNode * t = typeDecList();
	if (t==NULL)
        syntaxError("A type declaration is expected!");
	return t;
}

TreeNode * typeDec()
{
    if(Error) return NULL;
    TreeNode * t = NULL;
    if(belong(6))
        t = typeDeclaration();
    else if(!belong(5)) defaulttoken;
	return t;
}

TreeNode * paramMore()
{
    if(Error) return NULL;
	TreeNode * t = NULL;
    if(belong(48));
	else if(belong(49))
    {
        match(SEMI);
		t = paramDecList();
		if (t==NULL)
			syntaxError("A param declaration is required!");
    }
    else defaulttoken;
	return t;
}

void fidMore(TreeNode * t)
{
    if(Error) return;
	if(belong(53));
	else if(belong(54))
    {
        match(COMMA);
		formList(t);
    }
	else defaulttoken;
}

void formList(TreeNode * t)
{
    if(Error) return;
	if (token.type==ID)
	{
		t->name.push_back(token.name);
		match(ID);
	}
	fidMore(t);
}

TreeNode * param()
{
    if(Error) return NULL;
	TreeNode * t = new TreeNode(DecK);
	if (t!=NULL)
	{
		t->lineno = token.lineNum;

	     if(belong(50))
         {
             t->attr.ProcAttr.param = ValparamType;
			 typeDef(t);
	    	 formList(t);
         }
		 else if(belong(51))
		 {
		     match(VAR);
			 t->attr.ProcAttr.param = VarparamType;
			 typeDef(t);
			 formList(t);
		 }
         else defaulttoken;
	}
	return t;
}

TreeNode * paramDecList()
{
    if(Error) return NULL;
	TreeNode * t = param();
	TreeNode * p = paramMore();
    t->sibling = p;
	return t;
}

void paramList(TreeNode * t)
{
    if(Error) return;
	TreeNode * p = NULL;
	if(belong(45)) ;
	else if(belong(46))
    {
        p = paramDecList();
		t->son[0] = p;
    }
	else defaulttoken;
}

TreeNode * procBody()
{
    if(Error) return NULL;
	TreeNode * t = programBody();
	if (t==NULL)
		syntaxError("A program body is requested!");
	return t;
}

TreeNode * procDeclaration()
{
    if(Error) return NULL;
	TreeNode * t = new TreeNode(ProcDecK);

	match(PROCEDURE);
	if (t!=NULL)
	{
		t->lineno = token.lineNum;
		if (token.type==ID)
		{
			t->name.push_back(token.name);
			match(ID);
		}
		match(LPAREN);
		paramList(t);
		match(RPAREN);
        match(SEMI);
		t->son[1] = procDecPart();
		t->son[2] = procBody();
		t->sibling = procDec();
	}
	return t;
}

TreeNode * procDec()
{
    if(Error) return NULL;
    TreeNode * t = NULL;
	if(belong(39));
	else if(belong(40))
        t = procDeclaration();
	else defaulttoken;
	return t;
}

TreeNode * programHead()
{
    if(Error) return NULL;
    TreeNode * t = new TreeNode(PheadK);
	match(PROGRAM);
    if((t!=NULL)&&(token.type==ID))
	{
		t->lineno = 0;
		t->name.push_back(token.name);
	}
    match(ID);
    return t;
}

#define CreateDec(nkind,pointer) \
    tp = pointer##Dec();\
    TreeNode * pointer = NULL;\
    if(tp!=NULL)\
        {\
         pointer = new TreeNode(nkind);\
         pointer->son[0] = tp;\
         pointer->lineno = 0;\
        }
TreeNode * declarePart()
{
    if(Error) return NULL;
     /*类型*/
     TreeNode * tp;

     CreateDec(TypeK,type);
	 /*变量*/
     CreateDec(VarK,var);
	 /*函数*/
     TreeNode * proc = procDec();//CreateDec(ProcDecK,proc);

	 if(var != NULL) var->sibling = proc;
	 else var = proc;
	 if(type != NULL) type->sibling = var;
	 else type = var;
	 return type;
}
#undef CreateDec(nkind,pointer)

TreeNode * stmMore()
{
    if(Error) return NULL;
	TreeNode * t = NULL;
	if(belong(59));
	else if(belong(60))
    {
        match(SEMI);
		t = stmList();
    }
	else defaulttoken;
	return t;
}

TreeNode * returnStm()
{
    if(Error) return NULL;
	TreeNode * t = new TreeNode(StmtK);//newStmtNode(ReturnK);
	t->kind.stmt = ReturnK;
	match(RETURN);
	if(t!=NULL)
		t->lineno = token.lineNum;
	return t;
}

TreeNode * conditionalStm()
{
    if(Error) return NULL;
	TreeNode * t = new TreeNode(StmtK);//newStmtNode(IfK);
	t->kind.stmt = IfK;
	match(IF);
	if(t!=NULL)
	{
		t->lineno = token.lineNum;
		t->son[0] = exp();
	}
	match(THEN);
	if(t!=NULL)  t->son[1] = stmList();
	if(token.type==ELSE)
	{
		match(ELSE);
		if(t!=NULL)
			t->son[2] = stmList();
	}
	match(FI);
	return t;
}
TreeNode * loopStm()
{
    if(Error) return NULL;
	TreeNode * t = new TreeNode(StmtK);//newStmtNode(WhileK);
	t->kind.stmt = WhileK;
	match(WHILE);
	if (t!=NULL)
	{
		t->lineno = token.lineNum;
		t->son[0] = exp();
	    match(DO);
	    t->son[1] = stmList();
	    match(ENDWH);
	}
	return t;
}

TreeNode * inputStm()
{
    if(Error) return NULL;
	TreeNode * t = new TreeNode(StmtK);//newStmtNode(ReadK);
	t->kind.stmt = ReadK;
	match(READ);
	match(LPAREN);
	if((t!=NULL)&&(token.type==ID))
	{
		t->lineno = token.lineNum;
		t->name.push_back(token.name);
	}
	match(ID);
	match(RPAREN);
	return t;
}

TreeNode * outputStm()
{
    if(Error) return NULL;
	TreeNode * t = new TreeNode(StmtK);//newStmtNode(WriteK);
	t->kind.stmt = WriteK;
	match(WRITE);
	match(LPAREN);
	if(t!=NULL)
	{
		t->lineno = token.lineNum;
		t->son[0] = exp();
	}
	match(RPAREN);
	return t;
}

TreeNode * actParamMore()
{
    if(Error) return NULL;
	TreeNode * t = NULL;
	if(belong(79));
	else if(belong(80))
    {
        match(COMMA);
		t = actParamList();
    }
	else defaulttoken;
	return t;
}

TreeNode * actParamList()
{
    if(Error) return NULL;
	TreeNode * t = NULL;

	if(belong(77));
	else if(belong(78))
	{
        t =exp();
		if(t!=NULL)
			t->sibling = actParamMore();
	}
	else defaulttoken;
	return t;
}

TreeNode * callStmRest()
{
    if(Error) return NULL;
	TreeNode * t=new TreeNode(StmtK);//newStmtNode(CallK);
	t->kind.stmt = CallK;
	match(LPAREN);
	/*函数调用时，其子节点指向实参*/
	if(t!=NULL)
	{
		t->lineno = token.lineNum;
		/*函数名的结点也用表达式类型结点*/
		TreeNode * son0 = new TreeNode(ExpK);//newExpNode(VariK);
		son0->kind.exp = VariK;
		if(son0!=NULL)
		{
			son0->lineno = token.lineNum;
			son0->name.push_back(temp_name);
			t->son[0] = son0;
		}
		t->son[1] = actParamList();
	}
	match(RPAREN);
	return t;
}

void fieldvarMore(TreeNode * t )
{
    if(Error) return;
	if(belong(97)) ;
	else if(belong(98))//case LMIDPAREN:
    {
        match(LMIDPAREN);
		/*用来以后求出其表达式的值，送入用于数组下标计算*/
		t->son[0] = exp();
		t->son[0]->attr.ExpAttr.varkind = ArrayMembV;
		match(RMIDPAREN);
    }
	else defaulttoken;
}

TreeNode * fieldvar()
{
    if(Error) return NULL;
	/*注意，可否将此处的IdEK改为一个新的标识，用来记录记录类型的域*/
	TreeNode * t = new TreeNode(ExpK);//newExpNode(VariK);
	t->kind.exp = VariK;
	if ((t!=NULL) && (token.type==ID))
	  {
		  t->lineno = token.lineNum;
		  t->name.push_back(token.name);
	  }
	match(ID);
	fieldvarMore(t);
	return t;
}

void variMore(TreeNode * t)
{
    if(Error) return;
	if(belong(93));
	else if(belong(94))
    {
        match(LMIDPAREN);
		/*用来以后求出其表达式的值，送入用于数组下标计算*/
		t->son[0] = exp();
		t->attr.ExpAttr.varkind = ArrayMembV;
		/*此表达式为数组成员变量类型*/
		t->son[0]->attr.ExpAttr.varkind = IdV;
		match(RMIDPAREN);
    }
	else if(belong(95))
    {
        match(DOT);
		/*第一个儿子指向域成员变量结点*/
		t->son[0] = fieldvar();
		t->attr.ExpAttr.varkind = FieldMembV;
		t->son[0]->attr.ExpAttr.varkind = IdV;
    }
	else defaulttoken;
}

TreeNode * variable()
{
    if(Error) return NULL;
	TreeNode * t = new TreeNode(ExpK);//newExpNode(VariK);
    t->kind.exp = VariK;
	if ((t!=NULL) && (token.type==ID))
	{
		t->lineno = token.lineNum;
		t->name.push_back(token.name);
	}
	match(ID);
	variMore(t);
	return t;
}

TreeNode * factor()
{
    if(Error) return NULL;
  TreeNode * t = NULL;
  if(belong(90))
    {
	  /* 创建新的ConstK表达式类型语法树节点,赋值给t */
      t = new TreeNode(ExpK);//newExpNode(ConstK);
      t->kind.exp = ConstK;

      if ((t!=NULL) && (token.type==INTC))
	  {
		  t->lineno = token.lineNum;
		  t->attr.ExpAttr.val = atoi(token.name.c_str());
	  }
	  /* 当前单词token与数字单词NUM匹配 */
      match(INTC);
    }
    else if(belong(91))
	  t = variable();
    else if(belong(89))
    {
      match(LPAREN);
      t = exp();
      match(RPAREN);
    }
    else defaulttoken;
  return t;
}
TreeNode * term()
{
    if(Error) return NULL;
  /* 调用因子处理函数factor(),函数返回语法树节点指针给t */
  TreeNode * t = factor();
  /* 当前单词token为乘法运算符单词TIMES或OVER */
  while ((token.type==TIMES)||(token.type==OVER))
  {
    /* 创建新的OpK表达式类型语法树节点,新节点指针赋给p */
	TreeNode * p = new TreeNode(ExpK);//newExpNode(OpK);
	p->kind.exp = OpK;
	/* 新语法树节点p创建成功,初始化第一个子节点成员child[0]为t	*
	 * 将当前单词token赋值给语法树节点p的运算符成员attr.op		*/
    if (p!=NULL)
	{
      p->lineno= token.lineNum;
	  p->son[0] = t;
      p->attr.ExpAttr.op = token.type;
      t = p;
	}
	/* 当前单词token与指定乘法运算符单词(为TIMES或OVER)匹配 */
    match(token.type);
	/* 调用因子处理函数factor(),函数返回语法树节点指针赋给p第二个子节点成员child[1] */
    p->son[1] = factor();
  }
  /* 函数返回表达式类型语法树节点t */
  return t;
}

TreeNode * simple_exp()
{
    if(Error) return NULL;
  /* 调用元处理函数term(),函数返回语法树节点指针给t */
  TreeNode * t = term();
  /* 当前单词token为加法运算符单词PLUS或MINUS */
  while ((token.type==PLUS)||(token.type==MINUS))
  {
    /* 创建新OpK表达式类型语法树节点，新语法树节点指针赋给p */
	TreeNode * p = new TreeNode(ExpK);//newExpNode(OpK);
    p->kind.exp = OpK;
 	/* 语法树节点p创建成功,初始化p第一子节点成员child[0]	*
	 * 返回语法树节点指针给p的运算符成员attr.op				*/
    if (p!=NULL)
	{
      p->lineno = token.lineNum;
	  p->son[0] = t;
      p->attr.ExpAttr.op = token.type;
	  /* 将函数返回值t赋成语法树节点p */
      t = p;
	  /* 当前单词token与指定加法运算单词(为PLUS或MINUS)匹配 */
      match(token.type);
	  /* 调用元处理函数term(),函数返回语法树节点指针给t的第二子节点成员child[1] */
      t->son[1] = term();
    }
  }
  /* 函数返回表达式类型语法树节点t */
  return t;
}

TreeNode * exp()
{
    if(Error) return NULL;
  /* 调用简单表达式处理函数simple_exp(),返回语法树节点指针给t */
  TreeNode * t = simple_exp();
  /* 当前单词token为逻辑运算单词LT或者EQ */
  if ((token.type==LT)||(token.type==EQ))
  {
    /* 创建新的OpK类型语法树节点，新语法树节点指针赋给p */
    TreeNode * p = new TreeNode(ExpK);//newExpNode(OpK);
    p->kind.exp = OpK;
	/* 新语法树节点p创建成功,初始化p第一个子节点成员child[0]
	 * 并将当前单词token(为EQ或者LT)赋给语法树节点p的运算符成员attr.op*/
    if (p!=NULL)
	{
      p->lineno = token.lineNum;
	  p->son[0] = t;
      p->attr.ExpAttr.op = token.type;
	  /* 将新的表达式类型语法树节点p作为函数返回值t */
      t = p;
    }
	/* 当前单词token与指定逻辑运算符单词(为EQ或者LT)匹配 */
    match(token.type);
	/* 语法树节点t非空,调用简单表达式处理函数simple_exp()	*
	 * 函数返回语法树节点指针给t的第二子节点成员child[1]	*/
      t->son[1] = simple_exp();
  }
  /* 函数返回表达式类型语法树节点t */
  return t;
}

TreeNode * assignmentRest()
{
    if(Error) return NULL;
	TreeNode * t = new TreeNode(StmtK);
	t->kind.stmt = AssignK;
	/* 赋值语句节点的第一个儿子节点记录赋值语句的左侧变量名，*
	/* 第二个儿子结点记录赋值语句的右侧表达式*/
	if(t!=NULL)
    {
		t->lineno = token.lineNum;
		/*处理第一个儿子结点，为变量表达式类型节点*/
		TreeNode * son1 = new TreeNode(ExpK);//newExpNode(VariK);
		son1->kind.exp = VariK;
		if(son1!=NULL)
		{
			son1->lineno = token.lineNum;
			son1->name.push_back(temp_name);
			variMore(son1);
			t->son[0] = son1;
		}
		/*赋值号匹配*/
		match(ASSIGN);
		/*处理第二个儿子节点*/
		t->son[1] = exp();
	}
	return t;
}

TreeNode * assCall()
{
    if(Error) return NULL;
	TreeNode * t = NULL;
	if(belong(67))
        t = assignmentRest();
	else if(belong(68))
		t = callStmRest();
	else defaulttoken;
	return t;
}

TreeNode * stm()
{
    if(Error) return NULL;
	TreeNode * t = NULL;
	if(belong(61))
		t = conditionalStm();
    else if(belong(62))
		t = loopStm();
    else if(belong(63))
		t = inputStm();
    else if(belong(64))
		t = outputStm();
    else if(belong(65))
		t = returnStm();
    else if(belong(66))
    {
        temp_name = token.name;
		match(ID);
        t = assCall();
    }
	else defaulttoken;
	return t;
}

TreeNode * stmList()
{
    if(Error) return NULL;
	TreeNode * t = stm();
	//cout<<token.name<<endl;
	if(t!=NULL)
        t->sibling = stmMore();
	return t;
}

TreeNode * programBody()
{
    if(Error) return NULL;
	TreeNode * t = new TreeNode(StmLK);//newStmlNode();
    match(BEGIN);
	if(t!=NULL)
	{
		t->lineno = 0;
		t->son[0] = stmList();
	}
    match(END);
    return t;
}

TreeNode * program()
{
    if(Error) return NULL;
	TreeNode * t=programHead();
	TreeNode * q=declarePart();
	TreeNode * s=programBody();

	TreeNode * root = new TreeNode(ProK);
    if(root!=NULL)
	{
		root->lineno = 0;
		if(t!=NULL) root->son[0] = t;
        else syntaxError("A program head is expected!");
	    if(q!=NULL) root->son[1] = q;
	    if(s!=NULL) root->son[2] = s;
        else syntaxError("A program body is expected!");
	}
	match(DOT);
	return root;
}

void syntaxError(char * message)
{
	fprintf(listing,"\n>>> error :   ");
    fprintf(listing,"Syntax error at line %d: %s\n",token.lineNum,message);
	Error = true;
}
void match(TokenType tkt)
{
  if (token.type == tkt) ReadOneToken(token);
  else
  {
	  syntaxError("not match error ");
	  fprintf(listing,"'%s'\n",token.name.c_str());

      printf("\nSyntax Error At Line:%d ,You Can Get The Reason In the Output File.\n",token.lineNum);
	  fprintf(listing,"\nexpected: %s\nbut now is:%s\n",getTokenStr(tkt).c_str(),getTokenStr(token.type).c_str());
	  ReadOneToken(token);
  }
}

TreeNode * parserecur()
{
    listing = fopen("out.txt","w");
    TreeNode * t=NULL;
    Error = false;
    ReadOneToken(token);
    t = program();
    if (token.type!=ENDFILE) syntaxError("Code ends before file\n");
    if (Error)
    {
        t=NULL;
    }
    fclose(listing);
    return t;
}
