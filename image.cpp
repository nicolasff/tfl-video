#include "image.h"

#include <iostream>
#include <sstream>
#include <iomanip>

#include <gdfontg.h>

using namespace std;

#define FONT_PATH "/Library/Fonts/SF-Pro.ttf"

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
Image::Image(int w, int h, int time_w, int time_h, int time_padding, double ratio)
	: m_w(w * ratio)
	, m_h(h * ratio)
	, m_time_w(time_w)
	, m_time_h(time_h)
	, m_time_padding(time_padding)
	, m_ratio(ratio)
{
	m_im = gdImageCreateTrueColor(m_w,m_h);
	initColors();
}

void
Image::initColors() {
	colors.black = gdImageColorAllocateAlpha(m_im, 0x00,0x00,0x00, 0);
	colors.time_bg = gdImageColorAllocateAlpha(m_im, 0x00,0x00,0x00, 0x20);
	colors.white = gdImageColorAllocateAlpha(m_im, 0xff,0xff,0xff, 0);
	gdImageAlphaBlending(m_im, 0);
}

int
Image::color(int r, int g, int b) {
	return gdImageColorAllocateAlpha(m_im, r, g, b, 0);
}

int
Image::color(int r, int g, int b, int a) {
	return gdImageColorAllocateAlpha(m_im, r, g, b, a);
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

bool
Image::drawTrain(int x, int y, Palette &palette, int size) {

	x = m_ratio * x;
	y = m_ratio * y;

	if(x < 0 || y < 0 || x > m_w || y > m_h)
		return false;

	gdImageFilledEllipse(m_im, x, y, size+1, size+1, palette.border);
	gdImageFilledEllipse(m_im, x, y, size-1, size-1, palette.center);
	return true;
}

bool
Image::drawBus(int x, int y, Palette &palette, int size) {

	const int size_rem = size % 2;
	const int half_width = size / 2;
	x = m_ratio * x - half_width;
	y = m_ratio * y - half_width;

	if(x < 0 || y < 0 || x > m_w || y > m_h)
		return false;

	gdImageFilledRectangle(m_im, x, y, x + size + size_rem, y + size + size_rem, palette.center);
	return true;
}

bool
Image::drawBoat(int x, int y, Palette &palette, int size) {

	x = m_ratio * x;
	y = m_ratio * y;
	const int half_size = size/2;

	if(x < 0 || y < 0 || x > m_w || y > m_h)
		return false;

	gdPoint points[] = {  // diamond shape 🔸
		{x, y - half_size},	// top
		{x + half_size, y},	// right
		{x, y + half_size},	// bottom
		{x - half_size, y}	// left
	};
	gdImageFilledPolygon(m_im, &points[0], 4, palette.center);
	gdImagePolygon(m_im, &points[0], 4, palette.border);
	return true;
}
/*
#define TIME_WIDTH	522
#define TIME_HEIGHT	96
#define PADDING		20
*/

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

	// gdImageFilledRectangle(m_im, 0, 0, PADDING * 2 + TIME_WIDTH, PADDING * 2 + TIME_HEIGHT, colors.time_bg);

	// gdImageString(m_im, gdFontGiant, 3, 3,
	// 		(unsigned char*)time.c_str(), colors.white);

int brect[8] = {
	0,		// bottom left X
	m_time_h,		// bottom left Y
	m_time_w,	// bottom right X
	m_time_h,		// bottom right Y
	m_time_w,	// top right X
	0,		// top right Y
	0,		// top left X
	0,		// top left Y
};
	char *err = gdImageStringFT(m_im, brect, colors.time_bg, FONT_PATH, m_time_h, 0.0,
	m_time_padding,
	m_time_h + m_time_padding,
	(char *)time.c_str()); 
	if(err) {
		cerr << "Error: " << err << endl;
	}
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
