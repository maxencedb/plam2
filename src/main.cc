#include <iostream>

#include "parser.hh"
#include "plam.hh"

int main(int argc, char* argv[])
{
    // Check arguments
    if (argc != 2)
    {
        std::cerr << "I need exactly one filename\n";
        return 1;
    }

    // Load file
    Parser p(argv[1]);
    p.load();

    // Launch computing and display
    plam(p, std::cout);

    return 0;
}


