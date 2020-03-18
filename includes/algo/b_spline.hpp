#ifndef BASIS_SPLINE
#define BASIS_SPLINE

#include <vector>
#include <Eigen/Core>

// 18-03-2020
// created by Jan Hünermann

using namespace std;
using namespace Eigen;

template<int Order, int Dims>
struct BSpline
{
    typedef Matrix<double, Dims, 1> VectorNd;

    vector<double> knots;
    vector<VectorNd> control_points;

    VectorNd interpolate(double tau) const;
    VectorNd interpolate(double tau, int k) const;

    void walk(double delta, void (*fn)(VectorNd pt, const BSpline &)) const;

    static BSpline create(vector<double> knots, vector<VectorNd> control_points);

};

#endif