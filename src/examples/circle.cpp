#include <vector>
#include <algorithm>
#include <ostream>

#include <opencv2/opencv.hpp>
#include <Eigen/Dense>
#include <chrono>
#include <algorithm>

#include <algo/b_spline.hpp>
#include <algo/noise.hpp>
#include <algo/generative.hpp>

using namespace std;
using namespace cv;
using namespace Eigen;

const int SplineOrder = 4;
typedef Matrix<double, 5, 1> Vector5d;

void draw_spline(Vector2d pt, const BSpline2<SplineOrder>& sp, void *payl)
{
    vector<Vector2d> *target = reinterpret_cast<vector<Vector2d> *>(payl);
    target->push_back(pt);
}

void draw_spline(CairoCtxPtr ctx, BSpline2<SplineOrder> &sp, double opac)
{
    vector<Vector2d> spline_points;
    sp.walk(1e-3, &draw_spline, reinterpret_cast<void *>(&spline_points));

    Vector2d& start = spline_points[0];

    ctx->save();
    ctx->set_line_width(6);
    ctx->set_source_rgba(1,1,1, opac);
    ctx->move_to(start.x(), start.y());

    for_each(spline_points.begin()+1, spline_points.end(), [&ctx](Vector2d &v) {
        ctx->line_to(v.x(), v.y());
    });

    ctx->close_path();
    ctx->stroke();
    ctx->restore();
}

void draw_z(CairoCtxPtr ctx, double z, double w, noise::perlin5& pn)
{
    Cairo::RefPtr<Cairo::ImageSurface> surf = Cairo::RefPtr<Cairo::ImageSurface>::cast_static(ctx->get_group_target());
    Vector2d size((double)surf->get_width(), (double)surf->get_height());
    Vector2d center = size / 2;

    double radius = size.y() / 3;

    vector<double> knots;
    vector<Vector2d> points;

    int num_knots = 32;

    for (int k = 0; k < num_knots; ++k)
    {
        double rad = (double)k / (double)num_knots * 2 * M_PI;
        Vector2d p0 = center + Vector2d(cos(rad) * radius, sin(rad) * radius);

        Vector5d p1;
        p1 << p0.x(), p0.y(), 10.0 * z, 30 * cos(w), 30 * sin(w);

        double r = (radius + radius * 1.2 * pn(p1)) / z;
        Vector2d p = center + Vector2d(cos(rad) * r, sin(rad) * r);

        knots.push_back(rad);
        points.push_back(p);
    }

    for (int k = 0; k <= 2 * SplineOrder; ++k)
    {
        knots.push_back(2 * M_PI + knots[k]);
        points.push_back(points[k]);
    }

    BSpline2<SplineOrder> spline = BSpline2<SplineOrder>::create(knots, points, false, false);
    draw_spline(ctx, spline, clamp(1.0f - ((float)z - 1.0f) / 5.0f, 0.0f, 1.0f));
}

void draw(CairoCtxPtr ctx, noise::perlin5 &pn, double w)
{
    for (double z = 1.0; z < 8.0;)
    {
        draw_z(ctx, z, w, pn);
        z *= 1.1;
    }
}

int main(int argc, char **argv)
{
    // int fourcc = VideoWriter::fourcc('m', 'p', '4', 'v');
    // VideoWriter video("output.mp4", fourcc, 30, Size(960*2,640*2));

    const int w = 2000, h = 2000;    

    int k = 0;
    mt19937_64::result_type seed = time(0);

    generative::collection(100, w, h, [&k, &seed](CairoCtxPtr ctx) {
        mt19937_64 eng(seed + (++k));

        int noise_size[5] = { 50, 50, 50, 2, 2 };
        noise::perlin5 pn(noise_size, 48.0, eng);

        // fill white
        ctx->save(); // save the state of the context
        ctx->set_source_rgb(0.0/255.0, 0.0/255.0, 0.0/255.0);
        ctx->paint(); // fill image with the color
        ctx->restore(); // color is back to black now

        draw(ctx, pn, 0.0);
        // video.write(image);
    }, "artworks/01/series");

    destroyAllWindows();

    return 0;
}
