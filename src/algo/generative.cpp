#include <algorithm>
#include <numeric>

#include <algo/generative.hpp>

using namespace std;
using namespace cv;
using namespace generative;

namespace generative
{

int window_counter = 0;

string create_window()
{
    string window_name = "window_" + to_string(window_counter++);
    namedWindow(window_name, 1);

    return window_name;
}

void series(int w, int h, function<void (Mat&)> logic, double delay, int num, int scale)
{
    string win = create_window();
    Mat image(Size(w * scale, h * scale), CV_8UC3);
    resizeWindow(win, Size(w, h));
    
    while (num != 0)
    {
        image.setTo(Scalar(0,0,0));
        logic(image);
        imshow(win, image);
        waitKey(delay);
        num--;
    }
}

void single(int w, int h, function<void (Mat&)> logic, int scale)
{
    string win = create_window();
    Mat image(Size(w * scale, h * scale), CV_8UC4);
    resizeWindow(win, Size(w, h));
    image.setTo(Scalar(0.f,0.f,0.f));
    logic(image);
    imshow(win, image);
    waitKey(0);
}

void single(int w, int h, function<void (CairoCtxPtr)> logic, int scale, string save_path)
{
    single(w, h, [&] (Mat &image) {
        Cairo::RefPtr<Cairo::ImageSurface> surface =
            Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, w*scale, h*scale);
        CairoCtxPtr cr = Cairo::Context::create(surface);
        logic(cr);
        memcpy(image.data, surface->get_data(), 4 * surface->get_width() * surface->get_height());

        if (save_path.length() > 0)
        {
            imwrite(save_path, image);
        }
    }, scale);
}

void collection(int num, int w, int h, function<void (CairoCtxPtr)> logic, string folder) 
{
#pragma omp parallel
#pragma omp for
    for (int k = 0; k < num; k++)
    {
        Cairo::RefPtr<Cairo::ImageSurface> surface =
            Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, w, h);

        CairoCtxPtr cr = Cairo::Context::create(surface);
        logic(cr);

        Mat image(Size(w, h), CV_8UC4);
        memcpy(image.data, surface->get_data(), 4 * surface->get_width() * surface->get_height());

        Mat resized;
        resize(image, resized, Size(w/2, h/2));

        char num[5];
        snprintf (num, 5, "%04d", k);
        imwrite(folder + "/seq_" + string(num) + ".png", resized);

        cout << "Wrote image " << setw(4) << setfill('0') << num << endl;
    }
}


};