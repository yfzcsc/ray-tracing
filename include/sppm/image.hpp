#ifndef IMAGE_H
#define IMAGE_H

#include <vecmath.h>

#include <algorithm>
#include <cassert>

// Simple image class
class Image {
   public:
    Image(int w, int h, int flag = 0) {
        width = w;
        height = h;
        isRd = flag;
        if(flag) rdImg = new unsigned char[width * height * 3];
        else data = new Vector3f[width * height];
    }

    ~Image() { delete[] data; }

    int Width() const { return width; }

    int Height() const { return height; }

    Vector3f GetPixel(int x, int y) const {
        assert(x >= 0 && x < width);
        assert(y >= 0 && y < height);
        int A = (y * width + x) * 3;
        return isRd ? Vector3f(rdImg[A] / 255., rdImg[A + 1] / 255., rdImg[A + 2] / 255.) : data[y * width + x];
    }

    Vector3f GetPixel(double x, double y) const {
        x *= width - 1, y *= height - 1;
        int xx = abs(x), yy = abs(y);
        return GetPixel(xx, yy) * (xx + 1 - x) * (yy + 1 - y) + GetPixel(std::min(xx + 1, width - 1), yy) * (x - xx) * (yy + 1 - y) + GetPixel(xx, std::min(yy + 1, height - 1)) * (xx + 1 - x) * (y - yy) + GetPixel(std::min(xx + 1, width - 1), std::min(yy + 1, height - 1)) * (x - xx) * (y - yy);
    }

    void SetAllPixels(const Vector3f &color) {
        for (int i = 0; i < width * height; ++i) {
            data[i] = color;
        }
    }

    void SetPixel(int x, int y, const Vector3f &color) {
        assert(x >= 0 && x < width);
        assert(y >= 0 && y < height);
        data[y * width + x] = color;
    }

    void SetPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b) {
        assert(x >= 0 && x < width);
        assert(y >= 0 && y < height);
        int A = (y * width + x) * 3;
        rdImg[A] = r;
        rdImg[A + 1] = g;
        rdImg[A + 2] = b;
    }

    static Image *LoadPPM(const char *filename);

    void SavePPM(const char *filename) const;

    static Image *LoadTGA(const char *filename);

    void SaveTGA(const char *filename) const;

    int SaveBMP(const char *filename);

    void SaveImage(const char *filename);

   private:
    int width;
    int height;
    Vector3f *data;
    unsigned char *rdImg;
    int isRd;
};

#endif  // IMAGE_H
