#include "pin.H"
#include "getProcessFuncCallRelation.h"
#include "getProcessFuncCallRelation.cpp"

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
