#include <iostream>
#include <glpk.h>
#include "problem.hh"

Problem::Problem()
    : lp(glp_create_prob())
    , smcp()
    , iocp()
    , c0(0.0)
    , obj_coefs()
    , row_index()
    , col_index()
    , values()
    , variables()
    , constraints()
    , var_indexes()
{
}

Problem::~Problem()
{
    glp_delete_prob(lp);
}

void
Problem::set_name(std::string str)
{
    glp_set_prob_name(lp, str.c_str());
}

std::string
Problem::get_name()
{
    return std::string(glp_get_prob_name(lp));
}

void
Problem::set_maximize()
{
    glp_set_obj_dir(lp, GLP_MAX);
}

void
Problem::set_minimize()
{
    glp_set_obj_dir(lp, GLP_MIN);
}

bool
Problem::is_maximize()
{
    return glp_get_obj_dir(lp) == GLP_MAX;
}

void
Problem::set_c0_shift(double c0)
{
    this->c0 = c0;
}

double
Problem::get_c0_shift()
{
    return this->c0;
}

bool
Problem::add_var_coef(std::string name, double coef)
{
    if (variables.find(name) == variables.end())
        return false;

    obj_coefs[name] = coef;

    return true;
}

bool
Problem::add_variable(const Variable& v)
{
   if (variables.find(v.get_name()) != variables.end())
       return false;
   else
   {
       variables[v.get_name()] = v;
       return true;
   }
}

bool
Problem::add_constraint(const Constraint& c)
{
    if (constraints.find(c.get_name()) != constraints.end())
        return false;
    else
    {
        constraints[c.get_name()] = c;
        return true;
    }
}

double
Problem::get_var_value(std::string name)
{
    return glp_mip_col_val(lp, var_indexes[name]);
}

double
Problem::get_obj_value()
{
    return glp_mip_col_val(lp, 0);
}

void
Problem::process_variables()
{
    glp_add_cols(lp, variables.size());

    int index = 1;
    for (auto couple : variables)
    {
        var_indexes[couple.second.get_name()] = index;

        glp_set_col_name(lp, index, couple.second.get_name().c_str());
        glp_set_col_kind(lp, index, couple.second.get_variable_kind());

        int tmp_choice = GLP_FR;
        Variable& tmp_var = couple.second;

        if (tmp_var.has_lower_bound())
        {
            if (tmp_var.has_higher_bound())
            {
                if (tmp_var.get_lower_bound() == tmp_var.get_higher_bound())
                    tmp_choice = GLP_FX;
                else
                    tmp_choice = GLP_DB;
            }
            else
                tmp_choice = GLP_LO;
        }
        else
        {
            if (tmp_var.has_higher_bound())
                tmp_choice = GLP_UP;
            else
                tmp_choice = GLP_FR;
        }

        glp_set_col_bnds(lp, index, tmp_choice, tmp_var.get_lower_bound(), tmp_var.get_higher_bound());

        ++index;
    }
}

void
Problem::process_constraints()
{
    glp_add_rows(lp, constraints.size());

    // empty 0 because [0] is not used
    row_index.push_back(0);
    col_index.push_back(0);
    values.push_back(0.0);

    int index = 1;
    for (auto couple : constraints)
    {
        Constraint& tmp_cons = couple.second;

        // Name
        glp_set_row_name(lp, index, tmp_cons.get_name().c_str());

        // Bounds
        glp_set_row_bnds(lp, index, tmp_cons.get_bnds_type(),
                tmp_cons.get_lower_bound(), tmp_cons.get_higher_bound());

        // Constraint matrix
        for (auto coef_couple : tmp_cons.get_var_coefs())
        {
            const std::string& var_name = coef_couple.first;
            double var_coef = coef_couple.second; 

            // Find the corresponding variable in var map.
            auto tmp_var = variables.find(var_name);

            if (tmp_var == variables.end())
            {
                // cannot find required variable
                std::cerr << "Constraint: \"" << tmp_cons.get_name()
                          << "\" requires variable: \"" << var_name
                          << "\" but no such variable is present in the problem\n";
                continue;
            }
            else
            {
                // found required variable
                int tmp_var_index = var_indexes[var_name];
                row_index.push_back(index);
                col_index.push_back(tmp_var_index);
                values.push_back(var_coef);
            }
        }
        // switch to a new constraint index
        ++index;
    }

    // Load constraint matrix
    glp_load_matrix(lp, row_index.size() - 1, row_index.data(),
                        col_index.data(), values.data());
}

void
Problem::process_objective()
{
    // Set c0 constant problem shift
    glp_set_obj_coef(lp, 0, this->c0);

    // Set other coefs
    //  Checking if variables exist has already been done in var adding
    for (auto couple : obj_coefs)
        glp_set_obj_coef(lp, var_indexes[couple.first], couple.second);
}

void
Problem::launch_computing()
{
    /* solve in real domain */
    // Shut trivial messages up
    glp_init_smcp(&smcp);
    smcp.msg_lev = GLP_MSG_ERR;

    // launch simplex and abort on error
    if (glp_simplex(lp, &smcp) != 0)
    {
        std::cerr << "Some errors occured when computing simplex on real domain\n";
        return;
    }

    /* Solve on integer domain */
    // Shut trivial messages up
    glp_init_iocp(&iocp);
    iocp.msg_lev = GLP_MSG_ERR;

    // launch branch and cut
    glp_intopt(lp, &iocp);
}

void
Problem::display_solution(std::ostream& output)
{
    output << "Z = " << glp_mip_obj_val(lp) << "\n\n";

    for (auto couple : var_indexes)
        output << couple.first << ": " << glp_mip_col_val(lp, couple.second) << "\n";
}

void
Problem::solve()
{
    process_variables();
    process_constraints();
    process_objective();
    launch_computing();
}

