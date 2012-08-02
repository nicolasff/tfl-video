#ifndef IMAGE_H
#define IMAGE_H

#include <string>
#include <gd.h>


struct Palette {
	int center;
	int border;
};

class Image {
public:

	Image(std::string filename, double ratio = 1.0f);
	Image(int w, int h, int time_w, int time_h, int time_padding, double ratio = 1.0f);
	~Image();
	void save(std::string file);

	int color(int r, int g, int b);
	int color(int r, int g, int b, int a);

	bool drawTrain(int x, int y, Palette &palette, int size);
	bool drawBus(int x, int y, Palette &palette, int size);
	bool drawBoat(int x, int y, Palette &palette, int size);
	void drawTime(int t);

	void blend(Image &layer);

	int width() const;
	int height() const;

private:
	void initColors();

private:
	int m_w;
	int m_h;
	int m_time_w;
	int m_time_h;
	int m_time_padding;
	double m_ratio;

	gdImagePtr m_im;

	struct {
		int transparent;
		int black;
		int time_bg;
		int white;
		int pink;
		int blue;
		int brown;
		int green;
		int road;
	} colors;

};

#endif // IMAGE_H
