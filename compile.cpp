/**************************
* time: 2015-04-16-09.06
* name: K:\C_CPP\Compiler\compile.cpp
* Copyright (c) 2015 Kean
* email: huyocan@163.com
****************************/

#include<windows.h>
#include<string>
#include<iostream>
using namespace std;
using std::string;
void compileCpp(const char cppFileName[],const char outExeFileName[])
{
    const string cmd="mingw32-g++ -m32 -std=c++11 ";
    string tmp=cmd+" -o "+string(outExeFileName)+" "+string(cppFileName);
    tmp=tmp+" 2>compileError.txt";
    cout<<"compiling "<<tmp<<endl;
    system(tmp.c_str());

}
void runCppExe(const char exeFileName[])
{
    const string  cmd="C:/x64x86/CodeBlocks/cb_console_runner.exe  ";
    string tmp=cmd+string(exeFileName);
    cout<<"running "<<tmp<<endl;
    system(tmp.c_str());
    //  "K:\C_CPP\Compiler\bin\Debug\Compiler.exe"  (in K:\C_CPP\Compiler\.)
}

void astyleBeatify(const char fileName[])
{
    string cmd="astyle.exe --style=ansi -xd "+string(fileName)+" "+string(fileName);
    system(cmd.c_str());
}
