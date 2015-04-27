#ifndef SEM_H
#define SEM_H

/***semantics.h的简化版**/
struct TreeNode;
bool analyze(TreeNode * rt);
void outputErrorlist(bool toscreen = 1,const char fileName[]="semanticsError.txt");

#endif // SEM_H
