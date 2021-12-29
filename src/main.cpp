#include "Engine.hpp"

#include <iostream>
#include <stdexcept>


int main(int, char **)
{
    Engine engine;
    engine.init();

    try
    {
        engine.run();
    }
    catch(const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
    
    return 0;
}
