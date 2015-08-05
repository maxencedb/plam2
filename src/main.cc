#include <iostream>

#include "parser.hh"

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "I need exactly one filename\n";
        return 1;
    }

    Parser p(argv[1]);
    p.load();
    p.dump(std::cout);

    return 0;
}
