#ifndef IMAGE_H
#define IMAGE_H

#include <string>
#include <gd.h>

class Image {
public:

	Image(std::string filename, double ratio = 1.0f);
	Image(int w, int h, double ratio = 1.0f);
	~Image();
	void save(std::string file);

	int color(int r, int g, int b);

	void drawTrain(int x, int y, int color, int size);
	void drawBus(int x, int y, int color, int size);
	void drawTime(int t);

	void blend(Image &layer);

	int width() const;
	int height() const;

private:
	void initColors();

private:
	int m_w;
	int m_h;
	double m_ratio;

	gdImagePtr m_im;

	struct {
		int transparent;
		int black;
		int white;
		int pink;
		int blue;
		int brown;
		int green;
		int road;
	} colors;

};

#endif // IMAGE_H
