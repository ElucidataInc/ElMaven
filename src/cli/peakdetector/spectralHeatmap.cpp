#include <iostream>

#include <QString>

#include "standardincludes.h"
#include "spectralHeatmap.h"
#include "Scan.h"

unsigned char black[] = {0, 0, 0};
unsigned char white[] = {255, 255, 255};
unsigned char red[] = {255, 51, 51};

using namespace cimg_library;

void SpectralHeatmap::processSpectralHeatMapImage(mzSample* sample, string path)
{
    _margin = 60;
    _width = 800;
    _height = 800;

    cimg_library::CImg<unsigned char> image(800, 800, 1, 3);
    
    // Fill with white
    cimg_forXY(image,x,y) {
        image(x,y,0,0) = 255;
        image(x,y,0,1) = 255;
        image(x,y,0,2) = 255;
    }

    _outerSquare(image);
    _plotAxis(sample, image);
    _labels(image);
    _plotDataPoints(sample, image);

   image.save_pnm(path.c_str());
}


void SpectralHeatmap::_outerSquare(cimg_library::CImg<unsigned char> &image) {
    // Creating Outer Square.
    image.draw_line(_margin, _margin, _margin, _width - _margin, black); // left line
    image.draw_line(_margin, _width - _margin, _width - _margin, _width - _margin, black);    // bottom line
    image.draw_line(_width - _margin, _width - _margin, _width - _margin, _margin, black); // right line
    image.draw_line(_margin, _margin, _width - _margin, _margin, black); // top line

}

void SpectralHeatmap::_plotAxis(mzSample* sample, cimg_library::CImg<unsigned char> &image)
{
    // change Axis Maxima Minima. 
    
    float min = 0;
    maxX = sample->maxMz;
    maxY = sample->maxRt;


    int nticks = 20;
    int x0 = _margin;
    int x1 = _width - _margin;

    int y0 = _height - _margin;
    int y1 = _margin;

    int Y0 = _height - _margin;
    int X0 = _margin + _margin;

    float range = (maxX - min);
    float b = range / nticks;
    float ix = (x1 - x0) / nticks;
    float iy = (y1 - y0) / nticks;
    int fontHeight = 15;

    // X-axis ticks    
    for (int i = 1; i < nticks; i++)
        image.draw_line(x0 + ix * i, Y0 - 5, x0 + ix * i, Y0, black);
    for (int i = 1; i < nticks; ++i) {
        auto value = QString::number(min + b * i, 'f', 0).toStdString();
        int x = x0 + ix * i - (fontHeight / 2);
        int y = Y0 + 10;
        image.draw_text(x, y, value.c_str(), black, white, 1, fontHeight);
    }

    range = (maxY - min);
    b = range / nticks;
    // Y-axis ticks
    for (int i = 1; i < nticks; i++)
        image.draw_line(_margin, y0 + iy * i, _margin + 5, y0 + iy * i, black);
    for (int i = 1; i < nticks; ++i) {
        auto value = QString::number(min + b * i, 'f', 0).toStdString();
        int x = x0 - 30;
        int y = y0 + iy * i - (fontHeight / 2);
        image.draw_text(x, y, value.c_str(), black, white, 1, fontHeight);
    }
}

void SpectralHeatmap::_labels(cimg_library::CImg<unsigned char> &image)
{
    // Drawing M/Z
    int x = (_width / 2);
    int y = _height - 20;
    int fontHeight = 15;
    image.draw_text(x, y, "m/z", black, white, 1, fontHeight);

    // Drawing RT
    x = 0;
    y = _height / 2;
    image.draw_text(x, y, "RT", black, white, 1, fontHeight);
}

void SpectralHeatmap::_plotDataPoints(mzSample* sample, cimg_library::CImg<unsigned char> &image) 
{
    auto maxSampleIntensity = sample->maxIntensity;
    auto minSampleIntensity = sample->minIntensity;
    
    auto scanCount = sample->scanCount();
    for (int yIndex = 0; yIndex < scanCount; ++yIndex) {
        
        auto scan = sample->getScan(yIndex);
        if (scan->mslevel > 1)
            continue;

        auto intensityVector = scan->intensity;
        auto mzVector = scan->mz;
        auto scanRt = scan->rt;
        
        for (int xIndex = 0; xIndex < mzVector.size(); ++xIndex) {
            auto x = mzVector[xIndex];
            auto y = scanRt;

            auto intensity = intensityVector[xIndex];
            
            auto z = intensity / maxSampleIntensity;
            
            image.draw_point(toX(x), toY(y), red, z);
        }
    }

}

float SpectralHeatmap::toX(float x) {
    float minX = 0;
	if (minX == maxX || x < minX || x > maxX)
		return 0;
	return _margin + ((x - minX) / (maxX - minX) * (_width - (2 * _margin)));
}

float SpectralHeatmap::toY(float y) {
    float minY = 0;
	if (minY == maxY || y < minY || y > maxY)
		return 0;
	return (_height
			- ((y - minY) / (maxY - minY) * (_height - (2 * _margin)))) - _margin;
}
