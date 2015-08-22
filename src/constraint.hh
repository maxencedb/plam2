#ifndef CONSTRAINT_HH
#   define CONSTRAINT_HH
#   include <string>
#   include <map>

class Constraint
{
    public:
        Constraint();
        Constraint(std::string name, int glp_bnds_type);
        Constraint(std::string name, int glp_bnds_type, double lb, double hb);

        /* Getters setters */
        void set_name(const std::string& name);
        std::string get_name() const;

        void set_bnds_type(int glp_bnds_type);
        int get_bnds_type() const;

        void set_lower_bound(double lower_bound);
        double get_lower_bound() const;

        void set_higher_bound(double higher_bound);
        double get_higher_bound() const;

        const std::map<std::string, double> get_var_coefs();

        /* Other methods */
        void set_var_coef(std::string var_name, double coef);

    private:
        std::string name;
        // upper bound or lower bound, etc ...
        int glp_bnds_type;

        double lb;
        double hb;

        std::map<std::string, double> var_coefs;
};

#endif /* !CONSTRAINT_HH */
