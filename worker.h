#ifndef WORKER_H
#define WORKER_H

#include "image.h"

#include <map>
#include <string>
#include <pthread.h>

class Gtfs;
class Config;

class Worker {
public:
	Worker(Gtfs &g, Image &city, Config &m_cfg, std::string name,
			double ratio, int second);
	void start();
	void join();
	void _entry_point();

private:
	void initColorMap();

private:
	Gtfs &m_g;
	Image m_img, &m_city;

	std::map<std::string,int> m_colorMap;

	double m_ratio;
	int m_second;
	Config &m_cfg;
	std::string m_name;

	pthread_t m_self;
};

#endif // WORKER_H
