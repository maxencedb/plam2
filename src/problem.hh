#ifndef PROBLEM_HH
#   define PROBLEM_HH
#   include <vector>
#   include <map>
#   include <glpk.h>
#   include <string>
#   include "variable.hh"
#   include "constraint.hh"

class Problem
{
    public:
        /* Functions */
        Problem();
        ~Problem();

        /* Getters setters */
        void set_name(std::string str);
        std::string get_name();

        void set_maximize();
        void set_minimize();
        bool is_maximize();

        void set_c0_shift(double c0);
        double get_c0_shift();
        bool add_var_coef(std::string name, double coef);

        /* Methods */
        bool add_variable(const Variable& v);
        bool add_constraint(const Constraint& c);
        void solve();
        void display_solution(std::ostream& output);

    private:
        // The actual problem
        glp_prob *lp;
        // The normal control parameters
        glp_smcp smcp;
        // The integer control parameters
        glp_iocp iocp;

        // Shift variable c0
        double c0;
        // coefs for objective function
        std::map<std::string, double> obj_coefs;

        // Vector for contraint matrix
        std::vector<int> row_index;
        std::vector<int> col_index;
        std::vector<double> values;

        // Map for variables
        std::map<std::string, Variable> variables;
        // Vector of constraints
        std::map<std::string, Constraint> constraints;

        // Tmp containers for refs
        std::map<std::string, int> var_indexes;

        /* Private methods */
        void process_variables();
        void process_constraints();
        void process_objective();
        void launch_computing();
};

#endif /* !PROBLEM_HH */

