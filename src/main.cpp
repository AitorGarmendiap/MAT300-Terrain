#include "app.hpp"

#include <iostream>
#include <stdexcept>

int main(int argc, char** argv)
{
    mat300_terrain::App app;

    try
    {
        app.Run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}