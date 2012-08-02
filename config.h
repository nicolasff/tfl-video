#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <map>

// global settings
#define IMAGE_RATIO	1.0f
#define SECONDS_PER_DAY	86400
#define START_TIME	(4 * 3600 + 30 * 60)
#define DURATION	(24 * 3600)
#define IMAGE_FREQ	4
#define THREAD_COUNT	16

// Image configuration
class Config {
public:
	Config();
	Config(std::string file,
			int x0, int y0, double lat0, double lon0,
			int x1, int y1, double lat1, double lon1,
			int train_size, int bus_size, int boat_size,
			int time_w, int time_h, int time_padding);

	const std::string& file() const;
	std::pair<int,int> project(double lat, double lon) const;

	int train() const;
	int bus() const;
	int boat() const;

	int time_w() const;
	int time_h() const;
	int time_padding() const;

private:
	std::string m_file;

	int m_x0, m_y0, m_x1, m_y1;
	double m_lat0, m_lon0, m_lat1, m_lon1;

	int m_train, m_bus, m_boat;
	int m_time_w, m_time_h, m_time_padding;
};

#endif // CONFIG_H
