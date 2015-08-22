#include <iostream>
#include <sstream>
#include <glpk.h>

#include "tools.hh"
#include "parser.hh"
#include "problem.hh"
#include "plam.hh"

void plam(const Parser& p, std::ostream& output)
{
    Problem prob;

    prob.set_name("Warehouse mumbo-jumbo");
    prob.set_maximize();

    /* Variables */

    // Add all warehouse activation variables
    for (int i = 0; i < p.nr_sites; ++i)
    {
        std::stringstream ss;
        ss << "b" << i + 1;
        prob.add_variable(Variable(ss.str(), GLP_BV));
    }

    // Add all secondary distance variables
    for (int i = 0; i < p.nr_sites; ++i)
        for (int j = i + 1; j < p.nr_sites; ++j)
        {
            std::stringstream ss;
            ss << "w(" << i + 1 << ", " << j + 1 << ")";
            prob.add_variable(Variable(ss.str(), GLP_BV));
        }

    /* Constraints */

    // Boolean sum
    {
        Constraint tmp("The sum of the booleans must be == M"
                     , GLP_FX
                     , p.nr_warhouses
                     , p.nr_warhouses);

        for (int i = 0; i < p.nr_sites; ++i)
        {
            std::stringstream ss;
            ss << "b" << i + 1;
            tmp.set_var_coef(ss.str(), 1.0);
        }
        prob.add_constraint(tmp);
    }

    // Couple boolean constraints
    {
        for (int i = 0; i < p.nr_sites; ++i)
            for (int j = i + 1; j < p.nr_sites; ++j)
            {
                std::stringstream ss;
                ss << "w(" << i + 1 << ", " << j + 1 << ")";
                std::stringstream ssA;
                ssA << "b" << i + 1;
                std::stringstream ssB;
                ssB << "b" << j + 1;

                Constraint tmpA("Operator constraint A(" + ssA.str() + ") for: " + ss.str()
                             , GLP_UP
                             , 0.0
                             , 0.0);
                Constraint tmpB("Operator constraint B(" + ssB.str() + ") for: " + ss.str()
                             , GLP_UP
                             , 0.0
                             , 0.0);

                tmpA.set_var_coef(ss.str(), 1.0);
                tmpB.set_var_coef(ss.str(), 1.0);

                tmpA.set_var_coef(ssA.str(), -1.0);
                tmpB.set_var_coef(ssB.str(), -1.0);

                prob.add_constraint(tmpA);
                prob.add_constraint(tmpB);
            }
    }

    // Distance boolean constraints
    {
        for (int i = 0; i < p.nr_sites; ++i)
            for (int j = i + 1; j < p.nr_sites; ++j)
            {
                std::stringstream ss;
                ss << "w(" << i + 1 << ", " << j + 1 << ")";

                Constraint tmp("Distance for: " + ss.str()
                             , GLP_UP
                             , 2500.0
                             , 2500.0);

                tmp.set_var_coef(ss.str(), dist_sq(p.positions[i], p.positions[j]));

                prob.add_constraint(tmp);
            }
    }

    /* Objective */
    double incentive = p.max_capacity * 2;

    // c0 shift
    prob.set_c0_shift(-(p.nr_warhouses * (p.nr_warhouses - 1) / 2) * incentive);

    // capacities
    for (int i = 0; i < p.nr_sites; ++i)
    {
        std::stringstream ss;
        ss << "b" << i + 1;

        prob.add_var_coef(ss.str(), p.capacities[i]);
    }

    // incentives
    for (int i = 0; i < p.nr_sites; ++i)
        for (int j = i + 1; j < p.nr_sites; ++j)
        {
            std::stringstream ss;
            ss << "w(" << i + 1 << ", " << j + 1 << ")";

            prob.add_var_coef(ss.str(), incentive);
        }

    /* Do magic */
    prob.solve();
    prob.display_solution(output);
}
