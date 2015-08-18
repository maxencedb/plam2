#include <iostream>
#include <sstream>
#include <glpk.h>

#include "parser.hh"

void plam(const Parser& p, std::ostream& output);

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "I need exactly one filename\n";
        return 1;
    }

    Parser p(argv[1]);
    p.load();
    p.dump(std::cout);
    plam(p, std::cout);

    return 0;
}

/* Distance */
static inline
double dist_sq(std::pair<int, int> a, std::pair<int, int> b)
{
    double sic = ((a.first - b.first) * (a.first - b.first) +
                 (a.second - b.second) * (a.second - b.second));

    //std::cout << "DEBUG: distance calculated: " << sic << "\n";

    return sic;
}

/* Returns index from 2D in the upper triangular couple matrix to 1D index
 * n is the size of the square matrix
 *
 *    a b ->  1  2  3  4  5
 *    v
 *    1       x  1  2  3  4
 *    2       x  x  5  6  7
 *    3       x  x  x  8  9
 *    4       x  x  x  x  10
 *    5       x  x  x  x  x
 *
 * */
static inline
int ab_to_ind(int n, int a, int b)
{
    return (n - 1) * n / 2 - (n - a) * (n - a - 1) / 2 - n + b - 1;
}

void plam(const Parser& p, std::ostream& output)
{
    // the actual linear programming problem
    glp_prob *lp;
    // control parameters of real problem
    glp_smcp smcp;
    // control parameters of integer problem
    glp_iocp iocp;

    // Stringstream for debug info
    std::stringstream ss;

    /* Creation of the contraint coefficient matrix */
    // Number of site-couples
    int couples = (p.nr_sites - 1) * p.nr_sites / 2;
    // max size of matrix
    //  rows: number of constraints (1 for boolean addition,
    //        then for each couple: 2 to enforce boolean "AND"
    //                              1 to enforce "less than 50")
    //  columns: number of variables (1 boolean for each site and
    //                                1 boolean for each couple of sites)
    int matrix_sz = (1 + couples * 3) * (p.nr_sites + couples);

    // matrix contents
    // the +1 is necessary because for some reason GLPK indexes starting from
    // 1. It does so in the examples, at least.
    int* row_index = (int*)calloc(sizeof(int), (matrix_sz + 1));
    int* col_index = (int*)calloc(sizeof(int), (matrix_sz + 1));
    double* values = (double*)calloc(sizeof(float), (matrix_sz + 1));

    // create the problem structure
    lp = glp_create_prob();

    /* Set the problem properties */
    // name
    glp_set_prob_name(lp, "Warehose capacity maximization");
    // direction of optimization (MIN or MAX). We want maximization of capacity
    glp_set_obj_dir(lp, GLP_MAX);
    
    /* Set constraint equations properties and values */
    // set number of constraints, one for number of booleans and 2 among N
    glp_add_rows(lp, 1 + couples * 3);
    // set numper of variables
    glp_add_cols(lp, p.nr_sites + couples);

    //std::cout << "DEBUG: Adding " << 1 + couples * 3 << " rows\n";
    //std::cout << "DEBUG: Adding " << p.nr_sites + couples << " columns\n";

    /* Set variable kinds */
    for (int i = 0; i < p.nr_sites; ++i)
    {
        glp_set_col_kind(lp, i + 1, GLP_BV);

        // clear
        ss.str(std::string());
        ss.clear();
        ss << "b" << i + 1;
    //std::cout << "DEBUG: Variable n#" << i + 1 << " is called: " << ss.str() << std::endl;

        glp_set_col_name(lp, i + 1, ss.str().c_str());
    }

    int count = 0;
    for (int i = 0; i < p.nr_sites; ++i)
        for (int j = i + 1; j < p.nr_sites; ++j)
        {
            glp_set_col_kind(lp, p.nr_sites + count + 1, GLP_BV);

            // clear
            ss.str(std::string());
            ss.clear();
            ss << "w(" << i + 1 << ", " << j + 1 << ")";
    //std::cout << "Variable n#" << p.nr_sites + count + 1 << " is called: " << ss.str() << std::endl;

            glp_set_col_name(lp, p.nr_sites + count + 1, ss.str().c_str());
            ++count;
        }

    /* Counters */
    int constraint_id = 1;
    int matrix_square_id = 1;

    /* For the boolean sum */
        /* set constraint properties */

    // clear
    ss.str(std::string());
    ss.clear();
    // debug text
    ss << "There are exactly M = " << p.nr_warhouses << " activate sites";

    //std::cout << "DEBUG: " << ss.str() << std::endl;

    glp_set_row_name(lp, constraint_id, ss.str().c_str());
    glp_set_row_bnds(lp, constraint_id, GLP_FX, p.nr_warhouses, p.nr_warhouses);
    ++constraint_id;
        /* set constraint matrix */
    for (int i = 0; i < p.nr_sites; ++i)
    {
        row_index[matrix_square_id] = 1;
        col_index[matrix_square_id] = i + 1;
        values[matrix_square_id] = 1.0;
        ++matrix_square_id;
    }

    /* For couple-boolean constraints */
    for (int i = 0; i < p.nr_sites; ++i)
        for (int j = i + 1; j < p.nr_sites; ++j)
        {
            // clear
            ss.str(std::string());
            ss.clear();
            // debug text
            ss << "Boolean couple w(" << i + 1 << ", " << j + 1 << ") must be lower than b" << i + 1;
    //std::cout << "DEBUG: " << ss.str() << std::endl;

            /* set constraint properties (A) */
            glp_set_row_name(lp, constraint_id, ss.str().c_str());
            glp_set_row_bnds(lp, constraint_id, GLP_UP, 0.0, 0.0);

            /* set constraint matrix (A)*/
            row_index[matrix_square_id] = constraint_id;
            col_index[matrix_square_id] = ab_to_ind(p.nr_sites, i + 1, j + 1) + p.nr_sites + 1;
            values[matrix_square_id] = 1.0;
            ++matrix_square_id;

            row_index[matrix_square_id] = constraint_id;
            col_index[matrix_square_id] = i + 1;
            values[matrix_square_id] = -1.0;
            ++matrix_square_id;

            ++constraint_id;

            // clear
            ss.str(std::string());
            ss.clear();
            // debug text
            ss << "Boolean couple w(" << i + 1 << ", " << j + 1 << ") must be lower than b" << j + 1;
    //std::cout << "DEBUG: " << ss.str() << std::endl;

            /* set constraint properties (B) */
            glp_set_row_name(lp, constraint_id, ss.str().c_str());
            glp_set_row_bnds(lp, constraint_id, GLP_UP, 0.0, 0.0);

            /* set constraint matrix (B) */
            row_index[matrix_square_id] = constraint_id;
            col_index[matrix_square_id] = ab_to_ind(p.nr_sites, i + 1, j + 1) + p.nr_sites + 1;
            values[matrix_square_id] = 1.0;
            ++matrix_square_id;

            row_index[matrix_square_id] = constraint_id;
            col_index[matrix_square_id] = j + 1;
            values[matrix_square_id] = -1.0;
            ++matrix_square_id;

            ++constraint_id;
        }

    /* For the distances */
    for (int i = 0; i < p.nr_sites; ++i)
        for (int j = i + 1; j < p.nr_sites; ++j)
        {
            // clear
            ss.str(std::string());
            ss.clear();
            // debug text
            ss << "Squared distance between sites " << i + 1 << " and " << j + 1 << " must be <= 2500";
    //std::cout << "DEBUG: " << ss.str() << std::endl;

            /* set constraint properties */
            glp_set_row_name(lp, constraint_id, ss.str().c_str());
            glp_set_row_bnds(lp, constraint_id, GLP_UP, 2500.0, 2500.0);

            /* set constraint matrix */
            row_index[matrix_square_id] = constraint_id;
            col_index[matrix_square_id] = ab_to_ind(p.nr_sites, i + 1, j + 1) + p.nr_sites + 1;
            values[matrix_square_id] = dist_sq(p.positions[i], p.positions[j]);
            ++matrix_square_id;

            ++constraint_id;
        }
    /* Set objective function parameters */

    // a "big M"
    double incentive = p.max_capacity * 2;

        // set warehouse capacities as boolean coefficients
    for (int i = 0; i < p.nr_sites; ++i)
        glp_set_obj_coef(lp, i + 1, p.capacities[i]);
        // set a "big M" as couple boolean coefficients to force booleans to their max value
    for (int i = 0; i < couples; ++i)
        glp_set_obj_coef(lp, p.nr_sites + 1 + i, incentive);

        // set objective shift to negative "the number of activated couples" which should
        // be 2 among M so M * (M - 1) / 2 multiplied by the incentive, i.e.: the "big M" value
        // This is necessary to reajust the objective function after applying non-zero coefficients
        // to the objective function
    glp_set_obj_coef(lp, 0, -(p.nr_warhouses * (p.nr_warhouses - 1) / 2) * incentive);


    /* Do the magic */
     // load matrix
    glp_load_matrix(lp, matrix_square_id - 1, row_index, col_index, values);
    // solve on R
    glp_init_smcp(&smcp);
    smcp.msg_lev = GLP_MSG_ERR;

    if (glp_simplex(lp, &smcp) != 0)
        std::cout << "Some errors occured\n";
    // solve on N
    glp_init_iocp(&iocp);
    iocp.msg_lev = GLP_MSG_ERR;

    glp_intopt(lp, &iocp);

    /* Print all the output */
    //glp_print_sol(lp, "out.txt");
    //glp_print_mip(lp, "mip.txt");
    //glp_write_prob(lp, 0, "prob.txt");

    /* Manual output */
    std::cerr << "\nInteger solution of the problem\n";

    // objective function
    std::cerr << "Z = " << glp_mip_obj_val(lp) << "\n";

    // boolean variables
    for (int i = 0; i < p.nr_sites; ++i)
        std::cerr << "b" << i + 1 << " = " << glp_mip_col_val(lp, i + 1) << "\n";

    count = 0;
    for (int i = 0; i < p.nr_sites; ++i)
        for (int j = i + 1; j < p.nr_sites; ++j)
        {
            std::cerr << "w(" << i + 1 << ", " << j + 1 << ") = "
                      << glp_mip_col_val(lp, p.nr_sites + count + 1) << "\n";
            ++count;
        }

    /* Requested output */
    for (int i = 0; i < p.nr_sites; ++i)
        if (glp_mip_col_val(lp, i + 1) > 0.5)
            output << i + 1 << " ";
    output << "\n";

    /* Cleanup */
    glp_delete_prob(lp);
    free(row_index);
    free(col_index);
    free(values);
}
