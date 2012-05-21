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
#include <iostream>
#include <string>
#include <stack>

#include "pin.H"
#include "CallStack.h"
#include "CallStack.cpp"//g++默认的链接不会去找同名.cpp, 需手动加入； 手动加入了，发现编译命令同时存在-c和-o， 不能同时指定多个源文件
#include "getProcessFuncCallRelation.h"
/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/
const string EXCEPT_SEC = "/lib/";

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
    std::cerr << "init pin tools failed.." << std::endl;
    return -1;
}
