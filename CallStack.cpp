/******************************************************************************

                  版权所有 (C), 2001-2011, 志长有限公司

 ******************************************************************************
  文 件 名   : CallStack.cpp
  版 本 号   : 初稿
  作    者   : @zhi
  生成日期   : 2012年5月21日
  最近修改   :
  功能描述   : CallStack函数定义文件
  函数列表   :
              CallStack.addFunc
              CallStack.CallStack
              CallStack.clearStack
              CallStack.out2logFiles
              CallStack.~CallStack
  修改历史   :
  1.日    期   : 2012年5月21日
    作    者   : @zhi
    修改内容   : 创建文件

******************************************************************************/

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
#include <ctime>
#include "CallStack.h"

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
 * 常量定义                                     *
 *----------------------------------------------*/
const unsigned long MAX_LINES = 5;

/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/

/*****************************************************************************
 函 数 名  : CallStack.CallStack
 功能描述  : 打开记录文件, 设置数据输出为16进制
 输入参数  : string outFileName
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年5月21日
    作    者   : @zhi
    修改内容   : 新生成函数

*****************************************************************************/
CallStack::CallStack(string outFileName):_outFileName(outFileName)
{
    _outFile.open(_outFileName.c_str());
    //判断打开失败
    if ( _outFile.fail())
    {
        std::cerr << "file:" << __FILE__<< ", line:" << __LINE__
                << ", open log file failed" << std::endl;
        return;
    }
    _outFile << hex;

    _outTestFile.open((_outFileName+".test").c_str());
    if ( _outTestFile.fail())
    {
        std::cerr << "file:" << __FILE__<< ", line:" << __LINE__
                << ", open test log file failed" << std::endl;
        return;
    }
    _outTestFile << hex;

    _pLogStream= new ostringstream();
    if ( NULL == _pLogStream )
    {
        std::cerr << "file:" << __FILE__<< ", line:" << __LINE__
                << ", new string stream failed" << std::endl;
        return;
    }
    (*_pLogStream) << hex;
    _logLines = 0;
    _logFileNum = 0;
}

/*****************************************************************************
 函 数 名  : CallStack.~CallStack
 功能描述  : 清空函数调用关系栈, 关闭记录文件
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年4月21日
    作    者   : @zhi
    修改内容   : 新生成函数

*****************************************************************************/
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

/*****************************************************************************
 函 数 名  : CallStack.clearStack
 功能描述  : 清空函数调用关系栈, 打印相关信息
 输入参数  : 无
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年4月21日
    作    者   : @zhi
    修改内容   : 新生成函数

*****************************************************************************/
void CallStack::clearStack()
{
    while(!_funcs.empty())
    {
        _outFile << "}//" << _funcs.top().funcName << std::endl;
        (*_pLogStream) << "}//" << _funcs.top().funcName << std::endl;

        _funcs.pop();
    }
}

/*****************************************************************************
 函 数 名  : CallStack.addFunc
 功能描述  : 添加一个函数调用入栈, 通过比较调用当前函数的函数的EBP控制栈内数
             据
 输入参数  : const char* funcName
             ADDRINT funcAddr
             ADDRINT upperFuncBP
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年4月21日
    作    者   : @zhi
    修改内容   : 新生成函数

*****************************************************************************/
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
        if ( NULL == _pLogStream )
        {
            std::cerr << "file:" << __FILE__<< ", line:" << __LINE__
                    << ", new string stream failed" << std::endl;
            return;
        }
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
/*****************************************************************************
 函 数 名  : CallStack.out2logFiles
 功能描述  : 分拆日志到临时待上传子文件
 输入参数  : 无
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年4月21日
    作    者   : @zhi
    修改内容   : 新生成函数

*****************************************************************************/
void CallStack::out2logFiles()
{
    ostringstream osstream;
    osstream << _outFileName << "." << _logFileNum;

    ofstream splitFile;
    splitFile.open(osstream.str().c_str());
    if ( splitFile.fail())
    {
        std::cerr << "file:" << __FILE__<< ", line:" << __LINE__
                << ", open split log file failed" << std::endl;
        return;
    }
    //输出到上传日志文件, 以--结尾的表示程序未终止
    splitFile << (*_pLogStream).str();
    splitFile.close();

    ++ _logFileNum;
    _logLines = 0;
}

/*
static CallStack* CallStackFactory::pCallStack = NULL;
static CallStack& CallStackFactory::getCallStack()
{
	pCallStack = new CallStack();
	return *pCallStack;
}
static void CallStackFactory::delCallStack()
{
	if(NULL != pCallStack)
	{
		delete pCallStack;
		pCallStack = NULL;
	}
}
*/

