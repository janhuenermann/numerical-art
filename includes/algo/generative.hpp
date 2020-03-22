#ifndef GENERATE_HPP
#define GENERATE_HPP

#include <functional>
#include <opencv2/opencv.hpp>
#include <cairomm/context.h>
#include <cairomm/surface.h>

using namespace cv;
using namespace std;

typedef Cairo::RefPtr<Cairo::Context> CairoCtxPtr;

namespace generative
{
    void series(int w, int h, function<void (Mat&)> logic, double delay, int num = -1, int scale = 2);
    void single(int w, int h, function<void (Mat&)> logic, int scale = 2);
    void single(int w, int h, function<void (CairoCtxPtr)> logic, int scale = 2, string save_path = "");
    void collection(int num, int w, int h, function<void (CairoCtxPtr)> logic, string folder);
}


#endif