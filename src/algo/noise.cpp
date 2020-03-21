#include <algo/noise.hpp>
#include <cmath>

using namespace std;
using namespace noise;

template<unsigned int Dims, class RNG, typename FloatingPoint>
basePerlin<Dims, RNG, FloatingPoint>::basePerlin(const int size[Dims], FloatingPoint resolution, uint32_t seed, RNG engine) : 
    engine_(engine), 
    resolution_(resolution)
{
    numel_ = 1;
    for (int k = 0; k < Dims; ++k)
    {
        numel_ *= size[k];
    }

    for (int j = 0, n = 1; j < Dims; ++j, n *= 2)
    {
        for (int k = 0; k < n; ++k)
        {
            offsets_[k][j] = 0.0; // nearest point
            offsets_[n+k] = offsets_[k];
            offsets_[n+k][j] = 1.0; // further point
        }
    }

    grid_ = new VectorNd[numel_];
    memcpy(size_, size, Dims * sizeof(int));
    reseed(seed);
}

template<unsigned int Dims, class RNG, typename FloatingPoint>
void basePerlin<Dims, RNG, FloatingPoint>::reseed(uint32_t seed)
{
    FloatingPoint norm;

    engine_.seed(seed);
    uniform_real_distribution dist(-1.0, 1.0);

    for (int k = 0; k < numel_; ++k)
    {
        do 
        {
            for (int j = 0; j < Dims; ++j)
            {
                grid_[k][j] = dist(engine_);
            }

            norm = grid_[k].norm();
        }
        while (norm > 1.0 || norm < 1e-4);

        grid_[k] = grid_[k] / norm;
    }
}

template<unsigned int Dims, class RNG, typename FloatingPoint>
FloatingPoint basePerlin<Dims, RNG, FloatingPoint>::noise(const VectorNd v)
{
    Matrix<FloatingPoint, Corners, 1> _dots;
    VectorNd v0 = v / resolution_;
    VectorNd p0 = floor(v0.array());
    VectorNd p;

    for (int i = 0; i < Corners; ++i)
    {
        p = p0 + offsets_[i];
        _dots[i] = gradient(p).dot(v0 - p);
    }

    // interpolate
    const auto w = (v0 - p0).array();
    const auto ww = w*w*w*(w*(w*6.0 - 15.0) + 10.0);

    for (int j = Dims - 1, n = Corners >> 1; j >= 0; --j, n = n >> 1)
    {
        for (int k = 0; k < n; ++k)
        {
            _dots[k] += ww[j] * (_dots[n+k] - _dots[k]); // eq. to a0 + ww * (a1-a0)
        }
    }

    return _dots[0];
}

namespace noise 
{
#define template_perlin(d)     \
    template class basePerlin<d>;\
    template class perlin<d>;

    template_perlin(2);
    template_perlin(3);
    template_perlin(4);
    template_perlin(5);


}