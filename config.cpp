#include "config.h"

using namespace std;

Config::Config() {
}

Config::Config(string file,
		int x0, int y0, double lat0, double lon0,
		int x1, int y1, double lat1, double lon1,
		int train_size, int bus_size)
	: 
		m_file(file),
		m_x0(x0), m_y0(y0), m_x1(x1), m_y1(y1),
		m_lat0(lat0), m_lon0(lon0), m_lat1(lat1), m_lon1(lon1),
		m_train(train_size), m_bus(bus_size)
{
}

const string&
Config::file() const {
	return m_file;
}

pair<int,int>
Config::project(double lat, double lon) const {
	return make_pair(
			m_x0 + ((lon - m_lon0) / (m_lon1 - m_lon0)) * (m_x1 - m_x0),
			m_y0 + ((lat - m_lat0) / (m_lat1 - m_lat0)) * (m_y1 - m_y0));
}

int
Config::train() const {
	return m_train;
}

int
Config::bus() const {
	return m_bus;
}
