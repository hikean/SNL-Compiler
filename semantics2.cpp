#include "semantics.h"
#include <sstream>

static vector<symbnode> allparam;
static vector<int> Offsets;///每层当前偏移
static vector<Lsymbtable> scope;
static vector<string> errorlist;
static int getLevel();
static intIR intir;
static charIR charir;
static boolIR boolir;

FILE * listing;
FILE * cppfile;

int getLevel() {return scope.size()-1;}

void  createTable()
{
    Lsymbtable t;
    scope.push_back(t);
    Offsets.push_back(0);
}

void  deleteTable()
{
    printSymbTable(getLevel());
    scope.pop_back();
    Offsets.pop_back();
}

symbnode * findSymb(const int & lev,const string & sname)
{
    for(int i=0;i<scope[lev].size();++i)
        if(scope[lev][i].idname == sname)
                        return &scope[lev][i];
    return NULL;
}

symbnode * findSymbAll(const string & sname)
{
    symbnode * sn = NULL;

    for(int lev = scope.size()-1;lev>=0;--lev)
        if(sn = findSymb(lev,sname))
            return sn;
    return NULL;
}

void addSymble(const symbnode & st)
{
    scope[getLevel()].push_back(st);
    int cur = scope[getLevel()].size()-1;
}

void  analyzeProK(TreeNode * rt)
{
    if(rt == NULL) {semanticsError("No Program!",rt->lineno);return;}
    listing = fopen("Semout.txt","w");
    createTable();
    analyzePheadK(rt->son[0]);
    analyzeDeclareK(rt->son[1]);
    fprintf(cppfile,"int main()\n{\n");
    analyzeStmLK(rt->son[2]->son[0]);
    fprintf(cppfile,"\nsystem(\"pause\");\nreturn 0;\n}\n");
    deleteTable();
    fclose(listing);
}

string int2str(int n)
{
    stringstream stream;
    string st;
    stream<<n;
    stream>>st;
    return st;
}

inline void  semanticsError(const string & s,const int & linenumber)
{
    errorlist.push_back("line "+int2str(linenumber)+": "+s+"\n -_-||\n");
}

void  analyzePheadK(TreeNode * rt)
{
    if(rt == NULL) {semanticsError("No program!",rt->lineno);return;}
}

void  analyzeDeclareK(TreeNode * rt)
{
    if(rt == NULL) return;
    switch(rt->nodekind)
    {
        case TypeK:     analyzeTypedecK(rt->son[0]);break;
        case VarK:      analyzeVardecK(rt->son[0]);break;
        case ProcDecK:  analyzeProcdecK(rt);break;
    }
    analyzeDeclareK(rt->sibling);
}

typeIR *  menType2TypePtr(const DecKinds & dk)
{
    if(dk == CharK) return &charir;
    else return &intir;
}

int  getTypeSize(TreeNode * rt)
{
    if(rt->kind.dec == ArrayK)
        return rt->attr.ArrayAttr.up - rt->attr.ArrayAttr.low + 1;
    else
        return 1;
}

typeIR * getTypePtr(TreeNode * rt)///working
{
    switch(rt->kind.dec)
    {
    case IntegerK:
    case CharK:
        return menType2TypePtr(rt->kind.dec);

    case ArrayK:
        {
            const int s = rt->attr.ArrayAttr.up - rt->attr.ArrayAttr.low + 1;
            if(s<=0) semanticsError("Array index error!",rt->lineno);
            return new arrayIR(s,menType2TypePtr(rt->attr.ArrayAttr.memtype));
        }

    case IdK:
        {
            symbnode * ss = findSymbAll(rt->attr.type_name);
            if(ss != NULL)
                return ss->attrIR.typePtr;
            else {semanticsError("Undeclared type!",rt->lineno);return NULL;}
        }

    case RecordK:
        {
            recordIR * rst = new recordIR(0);
            for(TreeNode * p = rt->son[0];p != NULL;p = p->sibling)
                for(int j = 0; j<p->name.size();++j)
                {
                    for(int i=0;i< rst->rdlist.size();++i)
                    if(rst->rdlist[i].idname == p->name[j])
                        semanticsError("Member redeclared in the record",rt->lineno);

                    rdnode rn;
                    rn.idname = p->name[j];
                    rn.offset = rst->size;
                    rn.unitTypePtr = getTypePtr(p);
                    rst->size += getTypeSize(p);
                    rst->rdlist.push_back(rn);
                }
            return rst;
        }
    }
}

void decvar2file(TreeNode * rt,bool isparam = 0)
{
    string sep = isparam? ",":";\n";
    string isvar = (isparam&&rt->attr.ProcAttr.param == VarparamType)? "& ":"";
    switch(rt->kind.dec)
    {
        case IntegerK:
            for(int i=0;i<rt->name.size();++i)
                fprintf(cppfile,"int %s",(isvar+"_"+rt->name[i]+sep).c_str());
            break;
        case CharK:
            for(int i=0;i<rt->name.size();++i)
                fprintf(cppfile,"char %s",(isvar+"_"+rt->name[i]+sep).c_str());
            break;
        case ArrayK:
            for(int i=0;i<rt->name.size();++i)
                if(rt->attr.ArrayAttr.memtype==IntegerK)
                    fprintf(cppfile,"int _%s[%d]%s",rt->name[i].c_str(),rt->attr.ArrayAttr.up + 1,sep.c_str());
                else
                    fprintf(cppfile,"char _%s[%d]%s",rt->name[i].c_str(),rt->attr.ArrayAttr.up + 1,sep.c_str());
            break;
        case IdK:
            for(int i=0;i<rt->name.size();++i)
            fprintf(cppfile,"_%s",(rt->attr.type_name+" "+isvar+"_"+rt->name[i]+sep).c_str()); break;
        case RecordK:
            fprintf(cppfile,"struct _%s\n{\n",rt->name[0].c_str());
            for(TreeNode * p = rt->son[0];p != NULL;p = p->sibling) decvar2file(p);
            fprintf(cppfile,"}_%s",rt->name[0].c_str());
            for(int i=1;i<rt->name.size();++i)
                    fprintf(cppfile,",_%s",rt->name[i].c_str());
            fprintf(cppfile,";\n");
    }
}

void  analyzeTypedecK(TreeNode * rt)
{
    if(rt == NULL) return;

    symbnode * sn = findSymb(getLevel(),rt->name[0]);
    if(sn == NULL)
    {
        symbnode st;
        st.idname = rt->name[0];
        st.attrIR.kind = typeKind;
        st.attrIR.typePtr = getTypePtr(rt);
        addSymble(st);
    }
    else
        semanticsError("Type redeclared!",rt->lineno);

    fprintf(cppfile,"typedef ");
    decvar2file(rt);
    analyzeTypedecK(rt->sibling);
}

void printTy(const TypeKinds & tk)
{
    switch(tk)
    {
        case intTy:     fprintf(listing,"intTy  ");     break;
        case charTy:    fprintf(listing,"charTy  ");    break;
        case arrayTy:   fprintf(listing,"arrayTy  ");   break;
        case recordTy:  fprintf(listing,"recordTy  ");  break;
        case boolTy:    fprintf(listing,"boolTy  ");    break;
    }
}

void  printSymb(const symbnode & st)
{
    fprintf(listing,"%s:    ",st.idname.c_str());

    switch(st.attrIR.kind)
    {
    case typeKind:
        fprintf(listing,"typekind  ");
        if(st.attrIR.typePtr) printTy(st.attrIR.typePtr->kind);
        fprintf(listing,"\n");
        break;
    case varKind:
        fprintf(listing,"varkind ");
        if(st.attrIR.typePtr) printTy(st.attrIR.typePtr->kind);
        fprintf(listing,"  Level = %2d Offset= %d ",st.attrIR.level,st.attrIR.off);
        fprintf(listing,"%s\n",st.attrIR.access==dir? "dir":"indir");
        break;
    case procKind:
        fprintf(listing,"prockind ");
        fprintf(listing,"  Level = %2d  Noff= %d\n",st.attrIR.level,st.attrIR.off);
        break;
    }
}

void  printSymbTable(const int & lev)
{
    if(scope[lev].size())
    {
        fprintf(listing,"\n ------------  Level: %d \n",lev);
        for(int i=0;i<scope[lev].size();++i)
            printSymb(scope[lev][i]);
        fprintf(listing," --------------\n");
    }
}

typeIR * getExpType(TreeNode * rt,AccessKinds * ac)
{
    switch(rt->kind.exp)
    {
        case ConstK:
            if(ac != NULL) (*ac) = indir;
            fprintf(cppfile,"%d",rt->attr.ExpAttr.val);
            return &intir;

        case VariK:
            {
                if(ac != NULL) (*ac) = dir;
                symbnode * sn = findSymbAll(rt->name[0]);
                if(sn != NULL)
                {
                    switch(rt->attr.ExpAttr.varkind)
                    {
                        case ArrayMembV:
                            {
                                fprintf(cppfile,"_%s[",rt->name[0].c_str());
                                if(getExpType(rt->son[0]) != &intir)
                                semanticsError("Wrong array index type！",rt->lineno);
                                fprintf(cppfile,"]");
                                arrayIR * tmp = (arrayIR *)sn->attrIR.typePtr;
                                return tmp->elemTyPtr;
                            }

                        case IdV:
                            fprintf(cppfile,"_%s",rt->name[0].c_str());
                            return sn->attrIR.typePtr;

                        case FieldMembV:
                            {
                                int pos = -1;
                                recordIR * rptr =(recordIR *)sn->attrIR.typePtr;

                                for(int i = 0;i< rptr->rdlist.size();++i)
                                        if(rt->son[0]->name[0] == rptr->rdlist[i].idname)
                                         pos = i;
                                fprintf(cppfile,"_%s._%s",rt->name[0].c_str(),rt->son[0]->name[0].c_str());

                                if(pos != -1)
                                    return rptr->rdlist[pos].unitTypePtr;
                                else
                                    {semanticsError("Undeclared record member!",rt->lineno);return NULL;}
                            }
                            break;
                    }
                }
                else {semanticsError("Undeclared variable!",rt->lineno);return NULL;}
            }

        case OpK:
            if(ac != NULL) (*ac) = indir;
            fprintf(cppfile,"(");
            typeIR * gext0 = getExpType(rt->son[0]);
            switch(rt->attr.ExpAttr.op)
            {
                case LT: fprintf(cppfile,"<"); break;
                case EQ: fprintf(cppfile,"=="); break;
                case PLUS: fprintf(cppfile,"+"); break;
                case MINUS: fprintf(cppfile,"-"); break;
                case TIMES: fprintf(cppfile,"*"); break;
                case OVER: fprintf(cppfile,"/"); break;
                //case COMMA: fprintf(cppfile,"<<"); break;
            }
            typeIR * gext1 = getExpType(rt->son[1]);
            fprintf(cppfile,")");
            if(gext0 == gext1)
            {
                if(rt->attr.ExpAttr.op == LT || rt->attr.ExpAttr.op == EQ)
                        return &boolir;
                else
                        return gext0;
            }
            else {semanticsError("Types not match between Left part and Right part of the expression!",rt->lineno);return NULL;}
    }
}

void  StmLKIf(TreeNode * rt)
{
    fprintf(cppfile,"if(");
    if(getExpType(rt->son[0]) == &boolir)
    {
        fprintf(cppfile,")\n{\n");
        analyzeStmLK(rt->son[1]);
        fprintf(cppfile,"}\nelse\n{\n");
        analyzeStmLK(rt->son[2]);
        fprintf(cppfile,"}\n");
    }
    else semanticsError("'If' conditional expression must be 'bool'!",rt->lineno);
}

void  StmLKWhile(TreeNode * rt)
{
    fprintf(cppfile,"while(");
    if(getExpType(rt->son[0]) == &boolir)
        {
            fprintf(cppfile,")\n{\n");
            analyzeStmLK(rt->son[1]);
            fprintf(cppfile,"}\n");
        }
    else semanticsError("'While' conditional expression must be 'bool'!",rt->lineno);
}

void  StmLKWrite(TreeNode * rt)
{
    fprintf(cppfile,"cout<<");
    getExpType(rt->son[0]);
    fprintf(cppfile,"<<endl;\n");
}

void  StmLKRead(TreeNode * rt)
{
    symbnode * sn = findSymbAll(rt->name[0]);
    if(sn == NULL || sn->attrIR.kind != varKind)
        semanticsError("Undeclared variable!",rt->lineno);
    else fprintf(cppfile,"cin>>_%s;\n",sn->idname.c_str());
    arrayIR * air = (arrayIR *)(sn->attrIR.typePtr);
    if(sn->attrIR.typePtr->kind == arrayTy && air->elemTyPtr==&intir)
        semanticsError("Cannot read Integer Array!",rt->lineno);
}

void  StmLKReturn(TreeNode * rt)
{
    if(getLevel() == 0) semanticsError("Cannot return here!",rt->lineno);
    fprintf(cppfile,"return;\n");
}

void  StmLKCall(TreeNode * rt)
{
    symbnode * sn = findSymbAll(rt->son[0]->name[0]);
    fprintf(cppfile,"_%s(",rt->son[0]->name[0].c_str());

    if(sn == NULL || sn->attrIR.kind != procKind)
        {semanticsError("Undeclared procedure",rt->lineno);return;}

    int i;  TreeNode * p;
    for(i=0,p = rt->son[1];p != NULL && i<sn->attrIR.param.size();p = p->sibling,++i)
    {
        symbnode entry = allparam[sn->attrIR.param[i]];
        if(entry.attrIR.access == dir)
        {
            if(getExpType(p) != entry.attrIR.typePtr) break;
        }
        else
        {
            AccessKinds tac;
            if(getExpType(p,&tac) != entry.attrIR.typePtr || tac == indir)
                break;
        }
        fprintf(cppfile,",");
    }
    if(i!=sn->attrIR.param.size() || p!=NULL) semanticsError("Param error!",rt->lineno);
    if(i) fseek(cppfile,-1,SEEK_CUR);
    fprintf(cppfile,");\n");
}

void  StmLKAssign(TreeNode * rt)
{
    AccessKinds ac0,ac1;
    typeIR * gext1 = getExpType(rt->son[0],&ac0);
    fprintf(cppfile,"=");
    typeIR * gext2 = getExpType(rt->son[1],&ac1);
    fprintf(cppfile,";\n");
    if(gext1 != gext2 || ac0 ==indir) semanticsError("Assigning types not match",rt->lineno);
}

void  analyzeStmLK(TreeNode * rt)
{
    if(rt == NULL) return;

    switch(rt->kind.stmt)
    {
        case IfK:       StmLKIf(rt);        break;
        case WhileK:    StmLKWhile(rt);     break;
        case AssignK:   StmLKAssign(rt);    break;
        case ReadK:     StmLKRead(rt);      break;
        case WriteK:    StmLKWrite(rt);     break;
        case CallK:     StmLKCall(rt);      break;
        case ReturnK:   StmLKReturn(rt);    break;
    }

    analyzeStmLK(rt->sibling);
}

void  analyzeVardecK(TreeNode * rt,symbnode * stp,bool isparam)
{
    if(rt==NULL) return;
    decvar2file(rt,isparam);
    for(int i=0;i< rt->name.size();++i)
    {
        symbnode * sn = findSymb(getLevel(),rt->name[i]);
        if(sn == NULL)
        {
            symbnode st;
            st.idname = rt->name[i];
            st.attrIR.kind =varKind;
            st.attrIR.level = getLevel();
            st.attrIR.access = dir;
            st.attrIR.off = Offsets[getLevel()];
            st.attrIR.typePtr = getTypePtr(rt);
            Offsets[getLevel()] += st.attrIR.typePtr->size;
            if(isparam)
                st.attrIR.access = (AccessKinds)rt->attr.ProcAttr.param;
            addSymble(st);
            if(isparam)
            {
                stp->attrIR.param.push_back(allparam.size());
                allparam.push_back(st);
            }
        }
        else semanticsError("Variable redeclared!",rt->lineno);
    }
    analyzeVardecK(rt->sibling,stp,isparam);
}

void  analyzeParam(TreeNode * rt,symbnode * stp)
{
    if(rt==NULL) return;
    analyzeVardecK(rt,stp,1);
}

string getParamTypes(TreeNode * rt)
{
    string paramtypes="";
        for(TreeNode * p = rt;p!=NULL;p = p->sibling)
        {
            string isvar = p->attr.ProcAttr.param == VarparamType? "&":"";
            for(int i=0;i<p->name.size();++i)
            switch(p->kind.dec)
            {
                case IntegerK:  paramtypes += "int"+isvar+",";                  break;
                case CharK:     paramtypes += "char"+isvar+",";                 break;
                case IdK:       paramtypes += "_"+p->attr.type_name+isvar+",";  break;
            }
        }
        if(paramtypes.size()>0) paramtypes =paramtypes.substr(0,paramtypes.size()-1);
    return paramtypes;
}

void analyzeProcdecK(TreeNode * rt)///过程语义分析
{
    if(rt == NULL) return;

    symbnode * sn = findSymb(getLevel(),rt->name[0]);
    if(sn == NULL)
    {
        symbnode st;
        st.idname = rt->name[0];
        st.attrIR.kind = procKind;
        st.attrIR.typePtr = NULL;
        st.attrIR.level = getLevel();
        st.attrIR.off = Offsets[getLevel()];

        createTable();

        fprintf(cppfile,"function<void(%s)> _%s=[&](",getParamTypes(rt->son[0]).c_str(),rt->name[0].c_str());
        analyzeParam(rt->son[0],&st);
        if(rt->son[0]) fseek(cppfile,-1,SEEK_CUR);
        fprintf(cppfile,")->void\n");

        scope[getLevel()-1].push_back(st);

        fprintf(cppfile,"{\n");
        analyzeDeclareK(rt->son[1]);
        analyzeStmLK(rt->son[2]->son[0]);
        fprintf(cppfile,"};\n");

        deleteTable();
    }
    else
        semanticsError("Proccess redeclared!",rt->lineno);
}

bool analyze(TreeNode * rt)
{
    errorlist.clear();
    cppfile = fopen("outcpp.cpp","w");
    fprintf(cppfile,"#include<algorithm>\n#include<cstdlib>\n#include<iostream>\nusing namespace std;\n\n");
    analyzeProK(rt);
    fclose(cppfile);
    return errorlist.size() == 0;
}

void outputErrorlist(bool toscreen,const char fileName[])
{
    FILE * errfile = toscreen? stdout:fopen(fileName,"w");

    if(errorlist.size() == 0)
        fprintf(errfile,"No semantic error!!!\n");

    for(auto tmp:errorlist)
            fprintf(errfile,"%s\n",tmp.c_str());
    fclose(errfile);
}
