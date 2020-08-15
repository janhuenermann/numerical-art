#include <vector>
#include <algorithm>

#include <opencv2/opencv.hpp>
#include <Eigen/Dense>
#include <chrono>
#include <random>


#include <algo/b_spline.hpp>
#include <algo/noise.hpp>
#include <algo/generative.hpp>

using namespace std;
using namespace cv;
using namespace Eigen;

Mat frame(Size(960, 960), CV_32FC3);
#define WINDOW_1 "w1"
typedef Point3_<float> Pixel;


void draw_points(double *& pts, int n, CairoCtxPtr ctx, mt19937_64 &eng, double c)
{
    Cairo::RefPtr<Cairo::ImageSurface> surf = Cairo::RefPtr<Cairo::ImageSurface>::cast_static(ctx->get_group_target());

    normal_distribution noise(0.0, 0.25);
    ctx->save();
    const double w = (double)surf->get_width(), h = (double)surf->get_height();

    ctx->set_line_width(2);
    ctx->set_line_cap(Cairo::LineCap::LINE_CAP_ROUND);
    for (int i = 0; i < n; i++)
    {
        double x = pts[2*i+0], y = pts[2*i+1];
        double cd = sqrt((x-w/2)*(x-w/2) + (y-h/2)*(y-h/2));

        if (cd > w/2)
            continue;

        double cc = max(min(c + noise(eng), 1.0), 0.0);
        
        ctx->set_source_rgba(0.0,cc,0.0, 0.33);
        ctx->move_to(x,y);
        ctx->close_path();
        ctx->stroke();
    }
    ctx->restore();
}

void draw(CairoCtxPtr ctx, mt19937_64& eng)
{
    Cairo::RefPtr<Cairo::ImageSurface> surf = Cairo::RefPtr<Cairo::ImageSurface>::cast_static(ctx->get_group_target());
    const int w = surf->get_width(), h = surf->get_height();
    int gw = w / 20, gh = h / 20;

    int noise_scale = 600;
    int size[3] = { w/noise_scale, h/noise_scale, 3 };
    noise::perlin<3, mt19937_64> pn(size, (double)noise_scale, eng);

    // init 2d grid
    const int n = gw * gh;
    double *grid = new double[2 * n];

    for (int y = 0; y < gh; y++)
    {
        for (int x = 0; x < gw; x++)
        {
            grid[2 * (y * gw + x) + 0] = (double)x * (double)(w / gw);
            grid[2 * (y * gw + x) + 1] = (double)y * (double)(h / gh);
        }
    }

    int iter = 200;
    for (int k = 0; k < iter; ++k)
    {
        draw_points(grid, n, ctx, eng, 0.4 + (double)k / (double)iter * 0.4);
        
        for (int i = 0; i < n; ++i)
        {
            double x = grid[2 * i + 0], y = grid[2 * i + 1];
            double dx = pn.noise(x,y, 0.0), dy = pn.noise(x,y, 200.0);
            double norm = sqrt(dx*dx + dy*dy);
            grid[2 * i + 0] += dx / norm * 4.0;
            grid[2 * i + 1] += dy / norm * 4.0;
        }
    }
    

    delete [] grid;
}

int main(int argc, char **argv)
{
    const int w = 2800, h = 2800;

    int k = 0;
    mt19937_64::result_type seed = time(0);

    generative::collection(16, w, h, [&](CairoCtxPtr ctx) {
        mt19937_64 eng(seed + (++k));

        ctx->save(); // save the state of the context
        ctx->set_source_rgb(0.0, 0.0, 0.0);
        ctx->paint(); // fill image with the color
        ctx->restore(); // color is back to black now

        draw(ctx, eng);
    }, "artworks/flows");

    destroyAllWindows();

    return 0;
}
