#ifndef PARSERLL1_H_INCLUDED
#define PARSERLL1_H_INCLUDED

///**输出Parser LL1 错误信息到文件或屏幕，默认输出到屏幕**/

struct TreeNode;

void outputParserLL1Errors(bool toScrean=true,const char errorFileName[]="parserLL1Errors.txt");
///****LL1语法分析主函数,返回真假值表示是否出现了语法错误***/
bool parserLL1();
void deleteParserLL1Tree();///**删除LL1语法树**/
void deleteTree(TreeNode * rt);///***递归删除语法树**/
TreeNode * getParserLL1TreeRoot();
void outputParserTree(TreeNode* rt);
void outputParserTree(TreeNode *rt,const char fileName[]);
bool outputProductsTree(const char fileName[]);
#endif // PARSERLL1_H_INCLUDED
