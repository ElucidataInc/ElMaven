#ifndef SPECTRALHEATMAP_H
#define SPECTRALHEATMAP_H
#define cimg_display 0

#include "mzSample.h"
#include "CImg.h"

class mzSample; 

class SpectralHeatmap 
{
    private:
        int _margin;
        int _width;
        int _height;
        float maxX;
        float maxY;
        void _outerSquare(cimg_library::CImg<unsigned char> &image);
        void _plotAxis(mzSample* sample, cimg_library::CImg<unsigned char> &image);
        void _labels(cimg_library::CImg<unsigned char> &image);
        void _plotDataPoints(mzSample* sample, 
                            cimg_library::CImg<unsigned char> &image);
        float toX(float x);
        float toY(float y);


    public:
        void processSpectralHeatMapImage(mzSample* sample, string path);

};

#endif