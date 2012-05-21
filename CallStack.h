/******************************************************************************

                  版权所有 (C), 2001-2011, 志长有限公司

 ******************************************************************************
  文 件 名   : CallStack.h
  版 本 号   : 初稿
  作    者   : @zhi
  生成日期   : 2012年4月21日
  最近修改   :
  功能描述   : CallStack类的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2012年4月21日
    作    者   : @zhi
    修改内容   : 创建文件

******************************************************************************/

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
#include <fstream>
#include <iostream>
#include <string>
#include "pin.H"

#include <stack>
#include <sstream>

/*----------------------------------------------*
 * 外部变量说明                                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 外部函数原型说明                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 全局变量                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/
#ifndef __CALLSTACK_H__
#define __CALLSTACK_H__

using namespace std;
/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/
const string LOG_NAME = "/home/e/getProcessFuncCallRelation/Log/axiba.log";

/*****************************************************************************
 类 名  : FuncItem
 功能描述  : 函数记录类
 public函数  : FuncItem

 修改历史      :
  1.日    期   : 2012年4月21日
    作    者   : @zhi
    修改内容   : 新生成类

*****************************************************************************/
class FuncItem
{
public:
    string funcName;
    ADDRINT funcAddr;
    ADDRINT upperFuncBP;

    FuncItem(string name, ADDRINT addr, ADDRINT upperBP)
        :funcName(name), funcAddr(addr), upperFuncBP(upperBP) {}
};

/*****************************************************************************
 类 名  : CallStack
 功能描述  : 记录函数调用关系堆栈类
 public函数  :  CallStack
                ~CallStack
                addFunc
                clearStack
                getOutFile
                getTestFile

 修改历史      :
  1.日    期   : 2012年4月21日
    作    者   : @zhi
    修改内容   : 新生成类

*****************************************************************************/
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
