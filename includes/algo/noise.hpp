#ifndef NOISE_HPP
#define NOISE_HPP

#include <random>
#include <Eigen/Core>

using namespace std;
using namespace Eigen;

namespace noise
{

    template<unsigned int Dims, class RNG = mt19937_64, typename FloatingPoint = float>
    class basePerlin
    {
    public:
        static const constexpr int Corners = 1 << Dims;

        typedef Matrix<FloatingPoint, Dynamic, 1> VectorXd;
        typedef Matrix<FloatingPoint, Dims, 1> VectorNd;
        typedef Matrix<int, Dims, 1> VectorNi;

        static_assert(Dims > 1);

        basePerlin(int size[Dims], FloatingPoint resolution, RNG engine = RNG(), uint32_t seed = default_random_engine::default_seed) : 
            engine_(engine), resolution_(resolution)
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

        FloatingPoint noise(VectorNd v);
        void reseed(uint32_t seed);

        inline VectorNd gradient(VectorNd p)
        {
            int k = (int)p[0] % size_[0];

            for (int j = 1; j < Dims; ++j)
            {
                k = k * size_[j] + ((int)p[j] % size_[j]);
            }

            return grid_[k];
        }

    protected:
        int numel_;
        int size_[Dims];
        VectorNd *grid_;
        VectorNd offsets_[Corners];
        FloatingPoint resolution_;
        RNG engine_;

        // used during noise generation
    };

    template<unsigned int Dims, class RNG = mt19937_64, typename FloatingPoint = float>
    class perlin : public basePerlin<Dims, RNG, FloatingPoint>
    {};

    template<class RNG, typename FloatingPoint>
    class perlin<2, RNG, FloatingPoint> : public basePerlin<2, RNG, FloatingPoint>
    {
    public:
        using VectorNd = typename basePerlin<2, RNG, FloatingPoint>::VectorNd;

        using basePerlin<2, RNG, FloatingPoint>::basePerlin;
        using basePerlin<2, RNG, FloatingPoint>::noise;

        FloatingPoint noise(FloatingPoint x, FloatingPoint y)
        {
            return noise(VectorNd(x, y));
        }
    };

    template<class RNG, typename FloatingPoint>
    class perlin<3, RNG, FloatingPoint> : public basePerlin<3, RNG, FloatingPoint>
    {
    public:
        using VectorNd = typename basePerlin<3, RNG, FloatingPoint>::VectorNd;

        using basePerlin<3, RNG, FloatingPoint>::basePerlin;
        using basePerlin<3, RNG, FloatingPoint>::noise;

        FloatingPoint noise(FloatingPoint x, FloatingPoint y, FloatingPoint z)
        {
            return noise(VectorNd(x, y, z));
        }
    };

    template<class RNG, typename FloatingPoint>
    class perlin<4, RNG, FloatingPoint> : public basePerlin<4, RNG, FloatingPoint>
    {
    public:
        using VectorNd = typename basePerlin<4, RNG, FloatingPoint>::VectorNd;

        using basePerlin<4, RNG, FloatingPoint>::basePerlin;
        using basePerlin<4, RNG, FloatingPoint>::noise;

        FloatingPoint noise(FloatingPoint x, FloatingPoint y, FloatingPoint z, FloatingPoint w)
        {
            return noise(VectorNd(x, y, z, w));
        }
    };


};


#endif