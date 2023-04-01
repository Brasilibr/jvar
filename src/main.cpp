#include "../lib/jvar.h"
#include <iostream>

int main( int argc, char *argv[ ] )
{
    jvar test123 = jo{
        "outroObjeto" << jo {"teste" << jv "abc"},
        "varbool" << jv true,
        "varStr" << jv "batatinha",
        "varNumber" << jv 1,

    };

    std::cout << test123 << std::endl;
    return 0;
}