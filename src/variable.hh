#ifndef VARIABLE_HH
#   define VARIABLE_HH
#   include <string>

class Variable
{
    public:
        // Default ctor needed for std::map
        Variable();
        Variable(std::string name, int glp_variable_kind);
        Variable(std::string name, int glp_variable_kind, double lb, double hb);
        
        // Getters setters
        void set_name(const std::string& name);
        std::string get_name() const;

        void set_variable_kind(int glp_variable_kind);
        int get_variable_kind() const;

        void set_lower_bound(double lower_bound);
        bool has_lower_bound() const;
        double get_lower_bound() const;

        void set_higher_bound(double higher_bound);
        bool has_higher_bound() const;
        double get_higher_bound() const;

    private:
        // Name of the variable (should be unique)
        std::string name; 
        // Type of variable (Real, Boolean, Integer)
        int glp_variable_kind;

        // there is a lower bound
        bool has_lb;
        // there is a higher bound
        bool has_hb;

        // lower bound
        double lb;
        // higher bound
        double hb;
};

#endif /* !VARIABLE_HH */
