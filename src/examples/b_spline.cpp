#include <vector>
#include <algorithm>

#include <opencv2/opencv.hpp>
#include <Eigen/Dense>
#include <chrono>

#include <algo/b_spline.hpp>

using namespace std;
using namespace cv;
using namespace Eigen;

const string WINDOW_1 = "w1";
const int SplineOrder = 4;

Mat3f frame(1080, 1920);
const double scale = (double)max(frame.cols, frame.rows);
vector<Vector2d> points;

BSpline<SplineOrder, 2> generate_spline()
{
    vector<double> knots;
    vector<Vector2d> cp;

    int j;
    double t = 0.0;

    for (j = 0; j < points.size(); ++j, t += 1.0)
    {
        knots.push_back(t);
        cp.push_back(points[j]);
    }

    return BSpline<SplineOrder, 2>::create(knots, cp);
}

void draw(Vector2d pt, const BSpline<SplineOrder, 2>& sp)
{
    circle(frame, Point((int)(pt.x() * scale), (int)(pt.y() * scale)), 5.0, Scalar(0.0f, 0.0f, 1.0f), FILLED);
}

void invoke(Vector2i pt)
{
    points.push_back(pt.cast<double>() / scale);
    frame.setTo(Scalar(0.0f,0.0f, 0.0f)); // clear frame

    if (points.size() >= 2)
    {
        BSpline<SplineOrder, 2> sp = generate_spline();
        sp.walk(0.01, &draw);
    }
    
    for (int k = 0; k < points.size(); ++k)
    {
        circle(frame, Point((int)(points[k].x() * scale), (int)(points[k].y() * scale)), 5.0, Scalar(0.0f, 1.0f, 0.0f), FILLED);
    }

    imshow(WINDOW_1, frame);
}

// needed for debounce
Vector2i last_click_point;
chrono::time_point<std::chrono::steady_clock> last_click_time;

void click_callback(int event, int x, int y, int flags, void* userdata)
{
    if (flags != (EVENT_FLAG_CTRLKEY | EVENT_FLAG_LBUTTON))
    {
        return ;
    }

    Vector2i click_point(x, y);
    std::chrono::duration<double> diff = std::chrono::steady_clock::now() - last_click_time;

    // Debounce
    if (last_click_point == click_point || diff.count() < 0.05)
    {
        return ;
    }

    cout << "Left mouse button is clicked while pressing CTRL key - position (" << x << ", " << y << ")" << endl;

    invoke(click_point);

    last_click_time = std::chrono::steady_clock::now();
    last_click_point = click_point;
}

int main(int argc, char **argv)
{
    namedWindow(WINDOW_1, 1);
    setMouseCallback(WINDOW_1, click_callback, NULL);

    cout << "Click on a spot with the left mouse button and hold down CTRL to add a point to the spline." << endl;

    imshow(WINDOW_1, frame);
    waitKey(0);

    return 0;
}
