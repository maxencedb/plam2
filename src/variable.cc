#include "variable.hh"

Variable::Variable()
    : name("-default ctor for Variable (probably an error)-")
    , glp_variable_kind(0)
    , has_lb(false)
    , has_hb(false)
    , lb(0.0)
    , hb(0.0)
{
}

Variable::Variable(std::string name, int glp_variable_kind)
    : name(name)
    , glp_variable_kind(glp_variable_kind)
    , has_lb(false)
    , has_hb(false)
    , lb(0.0)
    , hb(0.0)
{
}

Variable::Variable(std::string name, int glp_variable_kind,
                    double lb, double hb)
    : name(name)
    , glp_variable_kind(glp_variable_kind)
    , has_lb(true)
    , has_hb(true)
    , lb(lb)
    , hb(hb)
{
}

// Getters setters
void
Variable::set_name(const std::string& name)
{
    this->name = std::string(name);
}

std::string
Variable::get_name() const
{
    return name;
}

void
Variable::set_variable_kind(int glp_variable_kind)
{
    this->glp_variable_kind = glp_variable_kind;
}

int
Variable::get_variable_kind() const
{
    return glp_variable_kind;
}

void
Variable::set_lower_bound(double lower_bound)
{
    has_lb = true;
    this->lb = lower_bound;
}

bool
Variable::has_lower_bound() const
{
    return has_lb;
}

double
Variable::get_lower_bound() const
{
    return this->lb;
}

void
Variable::set_higher_bound(double higher_bound)
{
    has_hb = true;
    this->lb = higher_bound;
}

bool
Variable::has_higher_bound() const
{
    return has_hb;
}

double
Variable::get_higher_bound() const
{
    return this->hb;
}
