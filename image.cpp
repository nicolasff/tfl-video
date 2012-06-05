#include "image.h"

#include <iostream>
#include <sstream>
#include <iomanip>

#include <gdfontg.h>

using namespace std;

Image::Image(string filename, double ratio)
	: m_ratio(ratio)
{

	FILE *f = fopen(filename.c_str(), "r");
	gdImagePtr src = gdImageCreateFromPng(f);
	fclose(f);

	int w = src->sx, h = src->sy;
	m_w = m_ratio * w; 
	m_h = m_ratio * h; 

	// possible resize
	if(ratio != 1.0f) {
		m_im = gdImageCreateTrueColor(m_w,m_h);
		gdImageCopyResampled(m_im, src, 0, 0, 0, 0, m_w, m_h, w, h);
		gdImageDestroy(src);
	} else {
		m_im = src;
	}

	initColors();
}
Image::Image(int w, int h, double ratio)
	: m_w(w * ratio)
	, m_h(h * ratio)
	, m_ratio(ratio)
{
	m_im = gdImageCreateTrueColor(m_w,m_h);
	initColors();
}

void
Image::initColors() {
	colors.black = gdImageColorAllocateAlpha(m_im, 0x00,0x00,0x00, 0);
	colors.white = gdImageColorAllocateAlpha(m_im, 0xff,0xff,0xff, 0);
	gdImageAlphaBlending(m_im, 0);
}

int
Image::color(int r, int g, int b) {
	return gdImageColorAllocateAlpha(m_im, r, g, b, 0);
}

Image::~Image() {
	if(m_im)
		gdImageDestroy(m_im);
}

void
Image::save(string file) {
	gdImageSaveAlpha(m_im, 1);
	FILE *pngout = fopen(file.c_str(), "wb");
	gdImagePng(m_im, pngout);
	fclose(pngout);
}

void
Image::drawTrain(int x, int y, int color, int size) {

	x = m_ratio * x;
	y = m_ratio * y;

	if(x < 0 || y < 0)
		return;
	
	gdImageFilledArc(m_im, x, y, size, size, 0, 360, color, gdArc);
}

void
Image::drawBus(int x, int y, int color, int size) {

	x = m_ratio * x;
	y = m_ratio * y;

	if(x < 0 || y < 0)
		return;

	gdImageFilledRectangle(m_im, x, y, x + size, y + size, color);
}

void
Image::drawTime(int t) {


	int h = t / 3600;
	int m = (t - 3600 * h) / 60;
	int s = t % 60;

	stringstream ss;
	ss
		<< setw(2) << std::setfill('0') << h << ":"
		<< setw(2) << std::setfill('0') << m << ":"
		<< setw(2) << std::setfill('0') << s;

	string time = ss.str();

	gdImageFilledRectangle(m_im, 0, 0, 78, 16, colors.black);

	gdImageString(m_im, gdFontGiant, 3, 0,
			(unsigned char*)time.c_str(), colors.white);
}


void
Image::blend(Image &layer) {

	gdImageCopyResampled(m_im, layer.m_im,
			0, 0, 0, 0,
			m_w, m_h, m_w, m_h);
}

int
Image::width() const {
	return m_w / m_ratio;
}

int
Image::height() const {
	return m_h / m_ratio;
}
