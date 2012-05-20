vpath %.o %.so ./Obj
PIN="/home/e/pin"

Obj/getProcessFuncCallRelation.so:Obj/getProcessFuncCallRelation.o
    g++  -Wl,--hash-style=sysv -shared -Wl,-Bsymbolic -Wl,--version-script=$(PIN)/source/include/pintool.ver -L$(PIN)/source/tools/Lib/ -L$(PIN)/source/tools/ExtLib/ -L$(PIN)/extras/xed2-ia32/lib -L$(PIN)/ia32/lib -L$(PIN)/ia32/lib-ext  -o Obj/getProcessFuncCallRelation.so Obj/getProcessFuncCallRelation.o -L$(PIN)/source/tools/Lib/ -L$(PIN)/source/tools/ExtLib/ -L$(PIN)/extras/xed2-ia32/lib -L$(PIN)/ia32/lib -L$(PIN)/ia32/lib-ext -lpin  -lxed -ldwarf -lelf -ldl
    
Obj/getProcessFuncCallRelation.o:getProcessFuncCallRelation.cpp
    g++ -c -Wall -Werror -Wno-unknown-pragmas  -O3 -fomit-frame-pointer -DBIGARRAY_MULTIPLIER=1 -DUSING_XED  -fno-strict-aliasing -I$(PIN)/source/tools/Include -I$(PIN)/source/tools/InstLib -I$(PIN)/extras/xed2-ia32/include -I$(PIN)/extras/components/include -I$(PIN)/source/include -I$(PIN)/source/include/gen -fno-stack-protector -DTARGET_IA32 -DHOST_IA32 -DTARGET_LINUX -O3 -fomit-frame-pointer -o Obj/getProcessFuncCallRelation.o getProcessFuncCallRelation.cpp