vpath %.o %.so ./Obj
Obj/getProcessFuncCallRelation.so:Obj/getProcessFuncCallRelation.o
    g++  -Wl,--hash-style=sysv -shared -Wl,-Bsymbolic -Wl,--version-script=/home/e/pin/source/include/pintool.ver -L/home/e/pin/source/tools/Lib/ -L/home/e/pin/source/tools/ExtLib/ -L/home/e/pin/extras/xed2-ia32/lib -L/home/e/pin/ia32/lib -L/home/e/pin/ia32/lib-ext  -o Obj/getProcessFuncCallRelation.so Obj/getProcessFuncCallRelation.o -L/home/e/pin/source/tools/Lib/ -L/home/e/pin/source/tools/ExtLib/ -L/home/e/pin/extras/xed2-ia32/lib -L/home/e/pin/ia32/lib -L/home/e/pin/ia32/lib-ext -lpin  -lxed -ldwarf -lelf -ldl
    
Obj/getProcessFuncCallRelation.o:getProcessFuncCallRelation.cpp
    g++ -c -Wall -Werror -Wno-unknown-pragmas  -O3 -fomit-frame-pointer -DBIGARRAY_MULTIPLIER=1 -DUSING_XED  -fno-strict-aliasing -I/home/e/pin/source/tools/Include -I/home/e/pin/source/tools/InstLib -I/home/e/pin/extras/xed2-ia32/include -I/home/e/pin/extras/components/include -I/home/e/pin/source/include -I/home/e/pin/source/include/gen -fno-stack-protector -DTARGET_IA32 -DHOST_IA32 -DTARGET_LINUX -O3 -fomit-frame-pointer -o Obj/getProcessFuncCallRelation.o getProcessFuncCallRelation.cpp
