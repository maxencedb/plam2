#ifndef TOOLS_HH
#   define TOOLS_HH
#   include <utility>

/* Distance */
inline
double dist_sq(std::pair<int, int> a, std::pair<int, int> b)
{
    double sic = ((a.first - b.first) * (a.first - b.first) +
                 (a.second - b.second) * (a.second - b.second));

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
inline
int ab_to_ind(int n, int a, int b)
{
    return (n - 1) * n / 2 - (n - a) * (n - a - 1) / 2 - n + b - 1;
}

#endif /* !TOOLS_HH */
