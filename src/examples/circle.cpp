#include <vector>
#include <algorithm>
#include <ostream>

#include <opencv2/opencv.hpp>
#include <Eigen/Dense>
#include <chrono>

#include <algo/b_spline.hpp>
#include <algo/noise.hpp>

using namespace std;
using namespace cv;
using namespace Eigen;

const int SplineOrder = 4;

Mat frame(Size(1400, 1400), CV_32FC3);
#define WINDOW_1 "w1"

vector<Vector2d> spline_points;
ofstream output("tikz");

void draw_spline(Vector2d pt, const BSpline2<SplineOrder>& sp)
{
    spline_points.push_back(pt);
}

void draw_spline(BSpline2<SplineOrder> &sp, float c, float thickness, float opacity = 1.0f)
{
    spline_points.clear();
    sp.walk(2e-3, &draw_spline);

    // for (const Point &p : spline_points)
    //     circle(frame, p, 1, Scalar(c,c,c), FILLED);

    // polylines(frame, spline_points, true, Scalar(c, c, c), thickness, LINE_AA);
    auto& output_ = output << std::fixed << std::setprecision(3);

    output_ << "\\draw [black!" << (int)(opacity*100.0f) << ", line width = " << thickness << "] ";

    for (int j = 0; j < spline_points.size(); j++)
    {
        const Vector2d& p = spline_points[j];
        output_ << "(" << (p.x() - (double)frame.cols/2) << ", " << (p.y() - (double)frame.rows/2) << ") -- ";
    }

    output_ << "(" << (spline_points[0].x() - (double)frame.cols/2) << ", " << (spline_points[0].y() - (double)frame.rows/2) << ")";
    output_ << "; \n" << endl;
}

void draw_z(double z, double z_offset, noise::perlin3& pn, float c)
{
    Vector2d size((double)frame.cols, (double)frame.rows);
    Vector2d center = size / 2;

    double radius = 500.0;

    vector<double> knots;
    vector<Vector2d> points;

    int num_knots = 30;

    for (int k = 0; k < num_knots; ++k)
    {
        double rad = (double)k / (double)num_knots * 2 * M_PI;
        Vector2d p0 = center + Vector2d(cos(rad) * radius, sin(rad) * radius);

        Vector3d p1 = Vector3d(p0.x(), p0.y(), z_offset + z * 10.0);

        double r = (radius + radius * 1.2 * pn(p1)) / z;
        Vector2d p = center + Vector2d(cos(rad) * r, sin(rad) * r);
        // p += Vector2d(, pn_y(p)) * radius;

        knots.push_back(rad);
        points.push_back(p);
    }

    for (int k = 0; k <= 2 * SplineOrder; ++k)
    {
        knots.push_back(2 * M_PI + knots[k]);
        points.push_back(points[k]);
    }

    BSpline2<SplineOrder> spline = BSpline2<SplineOrder>::create(knots, points, false, false);
    draw_spline(spline, c, 0.5f, 1.0f);
}

void draw(noise::perlin3 &pn, double z_offset)
{

    frame.setTo(Scalar(1.0f,1.0f,1.0f)); // clear frame

    for (double z = 1.0, c = 1.0; z < 9.0; c *= 0.9)
    {
        draw_z(z, z_offset, pn, 0.0f);
        z += z * 0.1;
    }
}

int main(int argc, char **argv)
{
    int noise_size[3] = { 100, 100, 100 };
    noise::perlin3 pn(noise_size, 60.0, 123 << 2);

    namedWindow(WINDOW_1, 1);

    draw(pn, 0.0);
    imshow(WINDOW_1, frame);
    waitKey(0);

    // for (int k = 0; k < 1000; ++k)
    // {
    //     draw(pn, (double)k * 0.5);
    //     imshow(WINDOW_1, frame);
    //     waitKey(1);
    // }
    

    return 0;
}
