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

vector<Vector2d> spline_points;

void draw_spline(Vector2d pt, const BSpline2<SplineOrder>& sp)
{
    spline_points.push_back(pt);
}

void draw_spline(Mat &image, BSpline2<SplineOrder> &sp, int opac)
{
    spline_points.clear();
    sp.walk(2e-2, &draw_spline);

    vector<Point> cv_points;
    cv_points.resize(spline_points.size());

    transform(spline_points.begin(), spline_points.end(), cv_points.begin(), [](Vector2d &v) -> Point {
        return Point(v.x(), v.y());
    });

    polylines(image, cv_points, true, Scalar(opac, opac, opac), 1, LINE_AA);
}

void draw_z(Mat &image, double z, double w, noise::perlin5& pn)
{
    Vector2d size((double)image.cols, (double)image.rows);
    Vector2d center = size / 2;

    double radius = size.y() / 3;

    vector<double> knots;
    vector<Vector2d> points;

    int num_knots = 64;

    for (int k = 0; k < num_knots; ++k)
    {
        double rad = (double)k / (double)num_knots * 2 * M_PI;
        Vector2d p0 = center + Vector2d(cos(rad) * radius, sin(rad) * radius);

        Vector5d p1;
        p1 << p0.x(), p0.y(), 18.0 * z, 30 * cos(w), 30 * sin(w);

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
    draw_spline(image, spline, (int)(255.0f * clamp(1.0f - ((float)z - 1.0f) / 5.0f, 0.0f, 1.0f)));
}

void draw(Mat &image, noise::perlin5 &pn, double w)
{
    for (double z = 1.0; z < 8.0;)
    {
        draw_z(image, z, w, pn);
        z *= 1.1;
    }
}

int main(int argc, char **argv)
{
    int fourcc = VideoWriter::fourcc('m', 'p', '4', 'v');
    VideoWriter video("output.mp4", fourcc, 30, Size(960*2,640*2));

    int noise_size[5] = { 100, 100, 100, 2, 2 };
    noise::perlin5 pn(noise_size, 60.0, 123 << 5);
    double w = 0.0;

    generative::series(960, 640, [&pn, &w, &video](Mat& image) {
        draw(image, pn, w);
        w += M_PI / 60;
        video.write(image);
    }, 1, 120);

    video.release();
    destroyAllWindows();

    return 0;
}
