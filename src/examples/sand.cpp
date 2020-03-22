#include <vector>
#include <algorithm>
#include <ostream>

#include <opencv2/opencv.hpp>
#include <Eigen/Dense>
#include <chrono>
#include <algorithm>
#include <math.h>

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
    const double offset = 4.0;
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

Vector2d sample_delta(mt19937 &eng)
{
    uniform_real_distribution ang_dist(0.0, 2.0 * M_PI);
    double ang = ang_dist(eng);
    return 4.0 * Vector2d(cos(ang), sin(ang));
}

Vector2d sample_new_delta(mt19937 &eng, Vector2d old_delta, Vector2d ab)
{
    double ab_ang = atan2(ab.y(), ab.x());

    // only allow new delta vector that points to the same side of origin->dst vector as old delta
    uniform_real_distribution ang_dist(0.0, M_PI);
    double ang = ang_dist(eng);

    Vector3d v1(old_delta.x(), old_delta.y(), 0);
    Vector3d v2(ab.x(), ab.y(), 0);
    Vector3d v3 = v2.cross(v1); // cross product tells us how angle is oriented

    ang = ang * copysign(1.0, v3.z());

    return 4.0 * Vector2d(cos(ab_ang + ang), sin(ab_ang + ang));
}

double get_distance_to_border(Vector2d p, Vector2d size)
{
    Vector2d d = size - p;
    return min(min(p.x(), p.y()), min(d.x(), d.y()));
}

void draw(CairoCtxPtr ctx, mt19937& eng)
{
    Cairo::RefPtr<Cairo::ImageSurface> surf = Cairo::RefPtr<Cairo::ImageSurface>::cast_static(ctx->get_group_target());
    
    const int w = surf->get_width(), h = surf->get_height();
    Vector2d size((double)w, (double)h);
    Vector2d origin(0,0), dst(0,0), delta(0,0);

    uniform_real_distribution image_pt(0.0, 1.0);
    uniform_real_distribution prob(0.0, 1.0);
    binomial_distribution count_dist(20, 0.5);

    uniform_int_distribution type_dist(0, 2);

    const double edge = 100.0;
    int n = 0, type, m = 0, M = count_dist(eng);

    while (1)
    {
        // if we reach an edge, make new shape
        while (get_distance_to_border(origin, size) <= edge || get_distance_to_border(dst, size) <= edge)
        {
            do
            {

                origin = Vector2d(image_pt(eng), image_pt(eng)).cwiseProduct(size);
                dst = Vector2d(image_pt(eng), image_pt(eng)).cwiseProduct(size);
                delta = sample_delta(eng);

            } while (get_distance_to_border(origin + 20.0 * delta, size) < edge
                  || get_distance_to_border(dst + 20.0 * delta, size) < edge);
            
            n = 0;
        }

        if (n == 0)
        {
            m++;
        }

        // at max 5 shapes
        if (m > M)
        {
            return ;
        }

        // 1% chance to switch delta, new delta cannot go into existing lines
        if (n > 0 && prob(eng) < 0.01)
        {
            delta = sample_new_delta(eng, delta, dst-origin);
        }

        // draw sparse lines
        draw_point_series(ctx, origin, dst, eng);

        // 1% chance to switch type of shape
        if (n == 0 || prob(eng) < 0.01)
        {
            type = type_dist(eng);
        }

        switch (type)
        {
            case 0:
                origin += delta;
                break ;
            case 1:
                dst += delta;
                break;
            default:
            case 2:
                origin += delta;
                dst += delta;
                break ;
        }

        ++n;
    }
}

int main(int argc, char **argv)
{
    const int w = 1600, h = 1600;

    int k = 0;
    mt19937::result_type seed = time(0);

    generative::collection(128, w, h, [&](CairoCtxPtr ctx) {
        mt19937 eng(seed + (++k));

        ctx->save(); // save the state of the context
        ctx->set_source_rgb(1.0, 1.0, 1.0);
        ctx->paint(); // fill image with the color
        ctx->restore(); // color is back to black now

        draw(ctx, eng);
    }, "artworks/sand2");

    destroyAllWindows();

    return 0;
}
