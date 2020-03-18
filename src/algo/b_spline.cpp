#include <iostream>
#include <unistd.h>

#include <algo/constants.hpp>
#include <algo/b_spline.hpp>

using namespace std;

template<int Order, int Dims>
typename BSpline<Order, Dims>::VectorNd BSpline<Order, Dims>::interpolate(double tau) const
{
    const int p = Order;
    const int n = knots.size();
    int R = n - 2 - p, L = p, k;

    if (tau < knots[0] || tau > knots[n-1])
    {
        throw std::invalid_argument("tau out of bounds: " + to_string(tau));
    }

    // find knot index using binary search
    while (L < R)
    {
        k = (R+L) / 2;

        if (knots[k] > tau)
        {
            L = k + 1;
        }
        else if (knots[k+1] < tau)
        {
            R = k - 1;
        }
        else
        {
            R = k;
        }
    }

    return interpolate(tau, L);
}

template<int Order, int Dims>
typename BSpline<Order, Dims>::VectorNd BSpline<Order, Dims>::interpolate(double tau, int k) const
{
    if (k < 0 || k >= knots.size())
    {
        throw std::invalid_argument("k out of bounds");
    }

    if (tau < knots[k] || tau > knots[k+1])
    {
        throw std::invalid_argument("tau out of bounds: " + to_string(tau));
    }

    const int p = Order;
    VectorNd d[p+1];
    double alpha;

    // De Boor's algorithm
    for (int j = 0; j <= p; ++j)
    {
        d[j] = control_points[j+k+1 - p];
    }

    for (int r = 1; r <= p; ++r)
    {
        for (int j = p; j >= r; --j)
        {
            alpha = (tau - knots[j+k - p]) / (knots[j+k - r + 1] - knots[j+k - p]);
            d[j] = (1.0 - alpha) * d[j-1] + alpha * d[j];
        }
    }

    return d[p];
}

template<int Order, int Dims>
void BSpline<Order, Dims>::walk(double delta, void (*fn)(VectorNd pt, const BSpline &)) const
{
    const int p = Order;
    int k = Order;
    int m = knots.size() - p - 1;

    if (m < 0)
    {
        return ;
    }

    double tau = knots[k];

    while (k < m)
    {
        VectorNd pt = interpolate(tau, k);
        fn(pt, *this);

        tau += delta;
        if (tau >= knots[k+1]) ++k;
    }
}

template<int Order, int Dims>
BSpline<Order, Dims> BSpline<Order, Dims>::create(vector<double> knots, vector<VectorNd> control_points)
{
    assert(knots.size() >= 2);

    const int p = Order;
    const int n = knots.size();

    // pad
    for (int j = 0; j < p; ++j)
    {
        knots.insert(knots.end(), knots[n-1+j]);
        control_points.insert(control_points.end(), control_points[n-1+j]);

        knots.insert(knots.begin(), knots[j]);
        control_points.insert(control_points.begin(), control_points[j]);
    }

    BSpline<Order, Dims> sp;
    sp.knots = knots;
    sp.control_points = control_points;

    return sp; 
}

template class BSpline<1, 2>;
template class BSpline<2, 2>;
template class BSpline<3, 2>;
template class BSpline<4, 2>;
template class BSpline<6, 2>;
template class BSpline<8, 2>;