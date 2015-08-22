#include <string>
#include "constraint.hh"

Constraint::Constraint()
    : name("-default Constraint ctor(probably an error)-")
    , glp_bnds_type(0)
    , lb(0.0)
    , hb(0.0)
    , var_coefs()
{
}

Constraint::Constraint(std::string name, int glp_bnds_type)
    : name(name)
    , glp_bnds_type(glp_bnds_type)
    , lb(0.0)
    , hb(0.0)
    , var_coefs()
{
}

Constraint::Constraint(std::string name, int glp_bnds_type, double lb, double hb)
    : name(name)
    , glp_bnds_type(glp_bnds_type)
    , lb(lb)
    , hb(hb)
    , var_coefs()
{
}

/* Getters setters */
void
Constraint::set_name(const std::string& name)
{
    this->name = name;
}

std::string
Constraint::get_name() const
{
    return name;
}

void
Constraint::set_bnds_type(int glp_bnds_type)
{
    this->glp_bnds_type = glp_bnds_type;
}

int
Constraint::get_bnds_type() const
{
    return glp_bnds_type;
}

void
Constraint::set_lower_bound(double lower_bound)
{
    this->lb = lower_bound;
}

double
Constraint::get_lower_bound() const
{
    return lb;
}

void
Constraint::set_higher_bound(double higher_bound)
{
    this->hb = higher_bound;
}

double
Constraint::get_higher_bound() const
{
    return hb;
}

void
Constraint::set_var_coef(std::string var_name, double coef)
{
    var_coefs[var_name] = coef;
}

const std::map<std::string, double>
Constraint::get_var_coefs()
{
    return var_coefs;
}
