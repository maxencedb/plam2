#ifndef PARSER_H
# define PARSER_H

# include <string>
# include <vector>
# include <fstream>

class Parser
{
    public:
        Parser(std::string path);

        /* Actually load the variables from the file */
        bool load();
        /* Display the contents of the parser in an orderly fashion */
        void dump(std::ostream& o) const;

        /* variables gathered from doc */
        int nr_sites;
        int nr_warhouses;
        int max_capacity;
        std::vector<int> capacities;
        std::vector<std::pair<int, int>> positions;

    private:
        std::string path;
        std::ifstream ifstream;
};

#endif /* !PARSER_H */
