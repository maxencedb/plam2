#include <iostream>

#include "parser.hh"
#include "plam.hh"

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "I need exactly one filename\n";
        return 1;
    }

    Parser p(argv[1]);
    p.load();
    p.dump(std::cout);
    plam(p, std::cout);

    return 0;
}


