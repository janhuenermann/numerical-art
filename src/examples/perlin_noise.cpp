#include <vector>
#include <algorithm>

#include <opencv2/opencv.hpp>
#include <Eigen/Dense>
#include <chrono>

#include <algo/noise.hpp>

using namespace std;
using namespace cv;
using namespace Eigen;

Mat frame(Size(960, 960), CV_32FC3);
#define WINDOW_1 "w1"
typedef Point3_<float> Pixel;

void draw(noise::perlin<3>& pn, double z)
{
    frame.setTo(Scalar(0.0f,0.0f, 0.0f)); // clear frame

    const double n0_max = 0.6;
    const double n0_min = -0.6;

#define normalise(n0) (n0 - n0_min) / (n0_max - n0_min)

    auto t1 = std::chrono::high_resolution_clock::now();

    frame.forEach<Pixel>([&] (Pixel &px, const int *position) -> void {
        float n0 = (float)normalise(pn.noise((float)position[0]+0.5, (float)position[1]+0.5, z));
        px.x = n0;
        px.y = n0;
        px.z = n0;
    });

    auto t2 = std::chrono::high_resolution_clock::now();
    auto duration = (double)std::chrono::duration_cast<std::chrono::nanoseconds>( t2 - t1 ).count() / (double)(frame.rows * frame.cols);

    cout << duration << "ns" << endl;
}

int main(int argc, char **argv)
{
    namedWindow(WINDOW_1, 1);

    int size[3] = { 100, 100, 100 };
    noise::perlin<3> pn(size, 20.0);

    for (int k = 0; k < 1000; ++k)
    {
        double z = (double)k / 1000.0 * 400.0;
        draw(pn, z);

        imshow(WINDOW_1, frame);
        waitKey(1);
    }

    waitKey(0);

    return 0;
}
