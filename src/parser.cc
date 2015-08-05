#include "parser.hh"

//DEBUG
#include <iostream>

Parser::Parser(std::string path)
    : nr_sites(-1)
    , nr_warhouses(-1)
    , capacities()
    , positions()
    , path(path)
    , ifstream(path.c_str())
{
}

bool Parser::load()
{

    // Read the number warehouses
    if (!(ifstream >> nr_sites >> nr_warhouses))
    {
        ifstream.close();
        return false;
    }

    std::cout << "DEBUG parser.cc 25: sites :" << nr_sites << " ware: " << nr_warhouses << "\n";

    // Prepare vectors in case of big malloc ahead
    capacities.reserve(nr_sites);
    positions.reserve(nr_sites);

    // readline loop
    int id = 0;
    int x = 0;
    int y = 0;
    int c = 0;

    // read all lines
    while (ifstream >> id >> x >> y >> c)
    {
        capacities.push_back(c);
        positions.push_back(std::make_pair(x, y));
    }

    // Reading done, close
    ifstream.close();

    return true;
}

void Parser::dump(std::ostream& o) const
{
    o << "Number of warehouses needed: " << nr_warhouses << "\n";
    o << "Number of sites:             " << nr_sites << "\n";
    o << "\nAvailable sites:\n";

    for (int i = 0; i < nr_sites; ++i)
    {
        o << " #" << i+1 << ": (" << positions[i].first << ", "
          << positions[i].second << ") capacity: " << capacities[i] << "\n";
    }
}

