#include <iostream>
#include <jal/Error.h>

#define STRINGIFY_TOKEN_VALUE(x) #x
#define STRINGIFY(x) STRINGIFY_TOKEN_VALUE(x)

struct TestOutput
{
    std::string name;
    bool pass;
};
typedef TestOutput (*Test)();

//normal test
#define chkTst(healthy, idx, fnc) \
if(healthy) \
{ \
    healthy &= fnc(); \
    if(healthy) { idx++; } \
}
//make sure it throws an exception
#define chkExcp(healthy, idx, fnc) \
if(healthy) \
{ \
    bool thrown{false}; \
    try { fnc(); } \
    catch(...) { thrown=true; }\
    healthy=thrown; \
    if(healthy) { idx++; } \
}

int main(int argc, char *argv[]){
   std::cout << "Hello World!" << std::endl;
   return 0;
}
