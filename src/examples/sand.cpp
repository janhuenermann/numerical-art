#include <vector>
#include <algorithm>
#include <ostream>

#include <opencv2/opencv.hpp>
#include <Eigen/Dense>
#include <chrono>
#include <algorithm>

#include <random>

#include <cairomm/context.h>
#include <cairomm/surface.h>

#include <algo/b_spline.hpp>
#include <algo/noise.hpp>
#include <algo/generative.hpp>

using namespace std;
using namespace cv;
using namespace Eigen;

void draw_point_series(CairoCtxPtr ctx, Vector2d origin, Vector2d dst, mt19937 &eng)
{
    normal_distribution noise(0.0, 0.14);
    const double c = 0.4;
    ctx->save();
    
    ctx->set_line_width(2);
    ctx->set_line_cap(Cairo::LineCap::LINE_CAP_ROUND);

    const Vector2d d = dst-origin;
    const double len = d.norm();
    const double offset = 3.5;
    const Vector2d dd = d/len * offset;
    Vector2d p = origin;

    double walked = 0;
    while (walked < len)
    {
        const double cc = c + noise(eng);
        ctx->set_source_rgba(cc,cc,cc, 0.4);
        ctx->move_to(p.x(), p.y());
        ctx->close_path();
        ctx->stroke();

        p += dd;
        walked += offset;
    }
    
    ctx->restore();
}

void draw(CairoCtxPtr ctx, noise::perlin3 &pn)
{
    mt19937::result_type seed = time(0);
    mt19937 eng(seed);

    int w = 1600, h = 1600;
    Vector2d origin, dst, delta;
    Vector2d size(w, h);

    uniform_real_distribution image_pt(0.0, 1.0);
    uniform_real_distribution delta_vec(-1.0, 1.0);
    uniform_real_distribution prob(0.0, 1.0);

    uniform_int_distribution type_dist(0, 2);

    int n = 0, type, m = 0;
    const int edge = 100;

    for (; m < 21; )
    {
        while (origin.x() < edge || dst.x() < edge || origin.x() > w - edge || dst.x() > w - edge 
            || origin.y() < edge || dst.y() < edge || origin.y() > h - edge || dst.y() > h - edge)
        {
            origin = Vector2d(image_pt(eng), image_pt(eng)).cwiseProduct(size);
            dst = Vector2d(image_pt(eng), image_pt(eng)).cwiseProduct(size);

            do
            {
                delta = Vector2d(delta_vec(eng), delta_vec(eng));
            } while (delta.norm() > 1.0);

            delta = delta / delta.norm() * 3.0;
            n = 0;
            ++m;
        }

        // draw sparse lines
        draw_point_series(ctx, origin, dst, eng);

//        const Vector2d pos = (origin + dst) / 2;
//        const double P = pn.noise(pos.x(), pos.y(), 0.0) / M_SQRT2 + 0.5;
        if (prob(eng) < 0.01)
            type = type_dist(eng);

        if (type == 0)
        {
            origin += delta;
        }
        else if (type == 1)
        {
            dst += delta;
        }
        else 
        {
            origin += delta;
            dst += delta;
        }
    }
}

int main(int argc, char **argv)
{
    int noise_size[5] = { 10, 10, 10 };
    noise::perlin3 pn(noise_size, 100.0, 123 << 5);

    generative::collection(128, 1600, 1600, [&](CairoCtxPtr ctx) {
        ctx->save(); // save the state of the context
        ctx->set_source_rgb(1.0, 1.0, 1.0);
        ctx->paint(); // fill image with the color
        ctx->restore(); // color is back to black now
        draw(ctx, pn);
    }, "artworks/sand2");

    destroyAllWindows();

    return 0;
}
