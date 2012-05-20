#include <fstream>
#include <iostream>
#include <string>
#include "pin.H"

#include <stack>
#include <sstream>

#ifndef __CALLSTACK_H__
#define __CALLSTACK_H__


const string LOG_NAME = "Log/axiba.log";

/*------------------------------------------------------------*
 *函数记录类--------------------------------------------------*
 *三个成员: 函数名, 函数起始地址, 本次调用本函数的函数的EBP---*
 *------------------------------------------------------------*/
class FuncItem
{
public:
    string funcName;
    ADDRINT funcAddr;
    ADDRINT upperFuncBP;

    FuncItem(string name, ADDRINT addr, ADDRINT upperBP)
        :funcName(name), funcAddr(addr), upperFuncBP(upperBP) {}
};
/*
 *记录函数调用关系堆栈, 控制相应日志输出类
 *通过比较当前函数主调的EBP, 判断前面函数终止情况
*/
class CallStack
{
private:
    std::stack<FuncItem>_funcs;
    string _outFileName;
    ofstream _outFile;
    ofstream  _outTestFile;

    unsigned long _logLines;
    unsigned long _logFileNum;
    ostringstream* _pLogStream;

    void out2logFiles();

public:
    //默认参数写在.h
    CallStack(string outFileName=LOG_NAME);
    ~CallStack();
    //添加一个函数调用进栈
    void addFunc(const char* funcName, ADDRINT funcAddr, ADDRINT upperFuncBP);
    void clearStack();

    //方便调试本程序的两个ofstream引用
    //标准日志输出文件
    ofstream& getOutFile()
    {
        return _outFile;
    }
    //后者是专门为测试的文件
    ofstream& getTestFile()
    {
        return _outTestFile;
    }
};

#endif /* __CALLSTACK_H__ */
