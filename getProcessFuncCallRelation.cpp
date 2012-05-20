/******************************************************************************

                  版权所有 (C), 2001-2011, 黄长志

 ******************************************************************************
  文 件 名   : proccount.cpp
  版 本 号   : 初稿
  作    者   : @zhi
  生成日期   : 2012年5月16日
  最近修改   :
  功能描述   : 使用pintool插入函数级, 通过记录ESP的栈来完成函数调用关系的记录
              BP 含有一位的等莫名其妙的数值
  函数列表   :
              backTrace
              CallStack.addFunc
              CallStack.CallStack
              CallStack.clearStack
              CallStack.~CallStack
              finish
              funcPackage
              main
              trace
              usage
  修改历史   :
  1.日    期   : 2012年5月16日
    作    者   : @zhi
    修改内容   : 创建文件

******************************************************************************/

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include "pin.H"

#include <stack>
#include <sstream>
#include <ctime>

/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/
const string EXCEPT_SEC = "/lib/";
const string LOG_NAME = "Log/axiba.log";
const unsigned long MAX_LINES = 5;

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
    CallStack(string outFileName);
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

//打开记录文件, 设置输出数据为16进制格式
CallStack::CallStack(string outFileName = LOG_NAME):_outFileName(outFileName)
{
    //open(const char*, std::ios_base::openmode)
    _outFile.open(_outFileName.c_str());
    //! 判断打开成功
    _outFile << hex;

    _outTestFile.open(("test_"+_outFileName).c_str());
    //! 判断打开成功
    _outTestFile << hex;

    _pLogStream= new ostringstream();
    (*_pLogStream) << hex;
    _logLines = 0;
    _logFileNum = 0;
}

//清空函数调用关系栈, 关闭记录文件
CallStack::~CallStack()
{
    clearStack();

    _outFile.close();
    _outTestFile.close();

    (*_pLogStream) << "/*The end*/";
    out2logFiles ();
    delete _pLogStream;
    _pLogStream = NULL;
}

//清空函数调用关系栈, 打印相关信息
void CallStack::clearStack()
{
    while(!_funcs.empty())
    {
        _outFile << "}//" << _funcs.top().funcName << std::endl;
        (*_pLogStream) << "}//" << _funcs.top().funcName << std::endl;

        _funcs.pop();
    }
}

//添加一个函数调用进栈, 通过比较主调当前函数的函数的EBP控制出栈, 输出相关信息
void CallStack::addFunc(const char* funcName, ADDRINT funcAddr,
                        ADDRINT upperFuncBP)
{
    if(0 == upperFuncBP)
    {
        clearStack();
        //return;
    }
    else
    {
        //!clear the BP little than now BP
        while((!_funcs.empty()) && (0!=_funcs.top().upperFuncBP)
                && (_funcs.top().upperFuncBP<=upperFuncBP))
        {
            _outFile << "}//" << _funcs.top().funcName << std::endl;
            (*_pLogStream) << "}//" << _funcs.top().funcName << std::endl;

            _funcs.pop();
        }
    }
    //判断是否到达MAX_LINES行, 超出则分摊到新的文件
    if(MAX_LINES <= _logLines)
    {
        out2logFiles ();

        delete _pLogStream;
        _pLogStream = new ostringstream();
    }

    time_t nowTime = time(NULL);
    string nowTimeStr = ctime(&nowTime);
    //output
    _outFile << funcName << "{//" << funcAddr << ", " << nowTimeStr
             << std::endl;
    (*_pLogStream) << funcName << "{//" << funcAddr << ", " << nowTimeStr
                   << std::endl;
    ++ _logLines;

    _funcs.push(FuncItem(funcName, funcAddr, upperFuncBP));
}

void CallStack::out2logFiles()
{
    ostringstream osstream;
    osstream << _outFileName << "." << _logFileNum;

    ofstream splitFile;
    splitFile.open(osstream.str().c_str());
    //输出到上传日志文件, 以--结尾的表示程序未终止
    splitFile << (*_pLogStream).str();
    splitFile.close();

    ++ _logFileNum;
    _logLines = 0;
}

/*----------------------------------------------*
 * 全局变量                                     *
 *----------------------------------------------*/
CallStack g_callStack;
bool g_backTraceFlg = true;

/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/

/*****************************************************************************
 函 数 名  : funcPackage
 功能描述  : 添加函数调用到函数调用栈的接口
 输入参数  : const char* funcName
             ADDRINT funcAddr
             ADDRINT upperFuncBP
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年5月16日
    作    者   : @zhi
    修改内容   : 新生成函数

*****************************************************************************/
void funcPackage(const char* funcName, ADDRINT funcAddr, ADDRINT upperFuncBP)
{
    //std::cout << "name:" << funcName << ", addr:" << funcAddr
    //  << ", BP:" << funcBP << std::endl;
    g_callStack.addFunc(funcName, funcAddr, upperFuncBP);
}

/*****************************************************************************
 函 数 名  : traceBack
 功能描述  : 回溯已经调用的函数栈,解析当前函数调用的信息
 输入参数  : const char* cfuncName
             ADDRINT funcAddr
             ADDRINT funcBP
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年5月16日
    作    者   : @zhi
    修改内容   : 新生成函数

*****************************************************************************/
VOID traceBack( ADDRINT funcCurSP, ADDRINT funcUpperBP )
{
    if(g_backTraceFlg)
    {
#if 1
        string funcName;
        std::stack<FuncItem>tmpFuncs;
        ADDRINT tmpAddr;
        ADDRINT funcAddr;
        //待运行函数入栈,后面的插入直接是从该函数开始
        //tmpFuncs.push(FuncItem(funcName, funcAddr, funcBP-1));
        tmpAddr = *((ADDRINT *)funcCurSP);
        funcName = RTN_FindNameByAddress(tmpAddr);
        //获取不到函数名
        if("" == funcName)
        {
            funcName  = "[unknown]";
        }
        PIN_LockClient();
        funcAddr = RTN_Address(RTN_FindByAddress(tmpAddr));
        PIN_UnlockClient();

        while(0 != funcUpperBP)
        {
            tmpFuncs.push(FuncItem(funcName, funcAddr, funcUpperBP));

            tmpAddr = *((ADDRINT *)funcUpperBP + 1);
            funcName = RTN_FindNameByAddress(tmpAddr);
            //获取不到函数名
            if("" == funcName)
            {
                funcName  = "[unknown]";
            }
            PIN_LockClient();
            funcAddr = RTN_Address(RTN_FindByAddress(tmpAddr));
            PIN_UnlockClient();

            funcUpperBP = *(ADDRINT*) funcUpperBP;
        }

#endif

#if 1
        tmpAddr =0;
        while(!tmpFuncs.empty())
        {
            funcPackage(tmpFuncs.top().funcName.c_str(), tmpFuncs.top().
                        funcAddr, tmpAddr);

            tmpAddr = tmpFuncs.top().upperFuncBP;
            tmpFuncs.pop();
        }
#endif
        g_backTraceFlg = false;
    }
}

/*****************************************************************************
 函 数 名  : rtn
 功能描述  : Pin calls this function every time a new rtn is executed
 输入参数  : RTN rtn
             VOID * v
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年5月16日
    作    者   : @zhi
    修改内容   : 新生成函数

*****************************************************************************/
VOID rtn(RTN rtn, VOID * v)
{
    //干掉系统的动态连接库, 即IMG为/lib/*
    if(EXCEPT_SEC == IMG_Name(SEC_Img(RTN_Sec(rtn)))
            .substr(0, EXCEPT_SEC.size()))
    {
        return;
    }

    RTN_Open(rtn);

    if ( g_backTraceFlg )
    {
        //第一次进入,回溯已调用的函数(附加到进程模式)
        //, 参数为待执行函数的函数名,函数起始地址,函数EBP(函数初始未压栈)
        RTN_InsertCall(rtn, IPOINT_BEFORE,
                       (AFUNPTR)traceBack,
                       //IARG_PTR, RTN_Name(rtn).c_str(),
                       //IARG_ADDRINT, RTN_Address(rtn),
                       IARG_REG_VALUE, REG_STACK_PTR,
                       IARG_REG_VALUE, REG_EBP, IARG_END);
    }
    RTN_InsertCall(rtn, IPOINT_BEFORE,
                   (AFUNPTR)funcPackage,
                   IARG_PTR, RTN_Name(rtn).c_str(),
                   IARG_ADDRINT, RTN_Address(rtn),
                   IARG_REG_VALUE, REG_EBP,
                   //IARG_REG_VALUE, REG_STACK_PTR, IARG_END);
                   IARG_END);
    RTN_Close(rtn);
}

/*****************************************************************************
 函 数 名  : finish
 功能描述  : pintool进程终止之前所调用函数; 此处清空函数调用栈
 输入参数  : INT32 code
             VOID * v
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年5月16日
    作    者   : @zhi
    修改内容   : 新生成函数

*****************************************************************************/
VOID finish(INT32 code, VOID * v)
{
    g_callStack.~CallStack();
}

/*****************************************************************************
 函 数 名  : usage
 功能描述  : 打印错误信息
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年5月16日
    作    者   : @zhi
    修改内容   : 新生成函数

*****************************************************************************/
INT32 usage()
{
    cerr << "init pin tools failed.." << std::endl;
    return -1;
}

/*****************************************************************************
 函 数 名  : main
 功能描述  : main函数; 初始化pintool, 插入RTN级调用
 输入参数  : int argc
             char **argv
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年5月16日
    作    者   : @zhi
    修改内容   : 新生成函数

*****************************************************************************/
int main(int argc, char **argv)
{
    //initial pin tool
    if (PIN_Init(argc, argv))
    {
        return usage();
    }
    //initial pin symbols table
    PIN_InitSymbols();

    //rtn level
    RTN_AddInstrumentFunction(rtn, 0);

    //call when finish
    PIN_AddFiniFunction(finish, 0);

    //start pin program
    PIN_StartProgram();

    return 0;
}
