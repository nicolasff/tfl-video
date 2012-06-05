#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <map>

// global settings
#define IMAGE_RATIO	1.0f
#define SECONDS_PER_DAY	86400
#define START_TIME	(4 * 3600 + 30 * 60)
#define DURATION	(24 * 3600)
#define IMAGE_FREQ	10
#define THREAD_COUNT	4

// Image configuration
class Config {
public:
	Config();
	Config(std::string file,
			int x0, int y0, double lat0, double lon0,
			int x1, int y1, double lat1, double lon1,
			int train_size, int bus_size);

	const std::string& file() const;
	std::pair<int,int> project(double lat, double lon) const;

	int train() const;
	int bus() const;

private:
	std::string m_file;

	int m_x0, m_y0, m_x1, m_y1;
	double m_lat0, m_lon0, m_lat1, m_lon1;

	int m_train, m_bus;
};

#endif // CONFIG_H
