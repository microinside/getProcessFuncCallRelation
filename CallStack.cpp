#include <ctime>
#include "CallStack.h"

const unsigned long MAX_LINES = 5;

//打开记录文件, 设置输出数据为16进制格式
CallStack::CallStack(string outFileName):_outFileName(outFileName)
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

