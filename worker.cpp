#include "worker.h"

#include "tfl/gtfs.h"
#include "config.h"

#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>

#define BUS_COLOR	"<BUS>"

using namespace std;

Worker::Worker(Gtfs &g, Image &city, Config &cfg, string name, double ratio, int second)
	: m_g(g)
	, m_img(city.width(), city.height(), cfg.time_w(), cfg.time_h(), cfg.time_padding(), ratio)
	, m_city(city)
	, m_ratio(ratio)
	, m_second(second)
	, m_cfg(cfg)
	, m_name(name)
{
}

static struct Palette
palette(int color) {
	struct Palette p;
	p.center = color;
	p.border = 0x40000000 | color;
	return p;
}


static struct Palette
palette(int color, int center) {
	struct Palette p;
	p.center = color;
	p.border = center;
	return p;
}

void
Worker::initColorMap() {
	m_colorMap.insert(make_pair(BUS_COLOR, palette(m_img.color(0xbe, 0x00, 0x02)))); // bus color
	/*
	m_colorMap.insert(make_pair("DLR", m_img.color(0x00, 0xaf, 0xad)));
	m_colorMap.insert(make_pair("Bakerloo", m_img.color(0x89, 0x4E, 0x24)));
	m_colorMap.insert(make_pair("Central", m_img.color(0xDC, 0x24, 0x1F)));
	m_colorMap.insert(make_pair("Circle", m_img.color(0xFF, 0xCE, 0x00)));
	m_colorMap.insert(make_pair("District", m_img.color(0x00, 0x72, 0x29)));
	m_colorMap.insert(make_pair("Hammersmith & City", m_img.color(0xD7, 0x99, 0xAF)));
	m_colorMap.insert(make_pair("Jubilee", m_img.color(0x86, 0x8F, 0x98)));
	m_colorMap.insert(make_pair("Metropolitan", m_img.color(0x75, 0x10, 0x56)));
	m_colorMap.insert(make_pair("Northern", m_img.color(0x00, 0x00, 0x00)));
	m_colorMap.insert(make_pair("Piccadilly", m_img.color(0x00, 0x19, 0xA8)));
	m_colorMap.insert(make_pair("Victoria", m_img.color(0x00, 0xA0, 0xE2)));
	m_colorMap.insert(make_pair("Waterloo & City", m_img.color(0x76, 0xD0, 0xBD)));
	m_colorMap.insert(make_pair("Tramlink 1", m_img.color(0x66, 0xCC, 0x00)));
	m_colorMap.insert(make_pair("Tramlink 3", m_img.color(0x66, 0xCC, 0x00)));
	m_colorMap.insert(make_pair("DLR", m_img.color(0x00, 0xAF, 0xAD)));
	*/
	m_colorMap.insert(make_pair("bakerloo", palette(m_img.color(0x89, 0x4E, 0x24))));
	m_colorMap.insert(make_pair("central", palette(m_img.color(0xDC, 0x24, 0x1F))));
	m_colorMap.insert(make_pair("circle", palette(m_img.color(0xFF, 0xCE, 0x00), m_img.color(0x00, 0x00, 0x00))));
	m_colorMap.insert(make_pair("district", palette(m_img.color(0x00, 0x72, 0x29))));
	m_colorMap.insert(make_pair("dlr", palette(m_img.color(0x00, 0xAF, 0xAD))));
	m_colorMap.insert(make_pair("emirates-air-line", palette(m_img.color(0xE5, 0x19, 0x37))));
	m_colorMap.insert(make_pair("hammersmith-city", palette(m_img.color(0xD7, 0x99, 0xAF), m_img.color(0x00, 0x00, 0x00))));
	m_colorMap.insert(make_pair("jubilee", palette(m_img.color(0x6A, 0x72, 0x78), m_img.color(0x00, 0x00, 0x00))));
	m_colorMap.insert(make_pair("london-overground", palette(m_img.color(0xE8, 0x6A, 0x10))));
	m_colorMap.insert(make_pair("metropolitan", palette(m_img.color(0x75, 0x10, 0x56))));
	m_colorMap.insert(make_pair("northern", palette(m_img.color(0x00, 0x00, 0x00))));
	m_colorMap.insert(make_pair("piccadilly", palette(m_img.color(0x00, 0x19, 0xA8))));
	m_colorMap.insert(make_pair("rb1", palette(m_img.color(0x2D, 0x30, 0x39))));
	m_colorMap.insert(make_pair("rb2", palette(m_img.color(0x00, 0x72, 0xBC))));
	m_colorMap.insert(make_pair("rb4", palette(m_img.color(0x61, 0xC2, 0x9D), m_img.color(0x00, 0x00, 0x00))));
	m_colorMap.insert(make_pair("rb5", palette(m_img.color(0xBA, 0x68, 0x30))));
	m_colorMap.insert(make_pair("rb6", palette(m_img.color(0xDF, 0x64, 0xB0), m_img.color(0x00, 0x00, 0x00))));
	m_colorMap.insert(make_pair("tfl-rail", palette(m_img.color(0x00, 0x19, 0xA8))));
	m_colorMap.insert(make_pair("victoria", palette(m_img.color(0x00, 0xA0, 0xE2))));
	m_colorMap.insert(make_pair("waterloo-city", palette(m_img.color(0x76, 0xD0, 0xBD), m_img.color(0x00, 0x00, 0x00))));
	m_colorMap.insert(make_pair("woolwich-ferry", palette(m_img.color(0xF7, 0x93, 0x1D), m_img.color(0x00, 0x00, 0x00))));
	//| London Overground=E86A10
	//| Chelsea-Hackney=8CC63E

}

static void*
worker_start(void *self) {

	Worker *w = reinterpret_cast<Worker*>(self);
	w->_entry_point();
	return 0;
}

void
Worker::start() {
	pthread_create(&m_self, 0, worker_start, reinterpret_cast<void*>(this));
}

void
Worker::join() {
	pthread_join(m_self, 0);
}

void
Worker::_entry_point() {

	// init color map
	initColorMap();

	// draw city
	m_img.blend(m_city);

	// find vehicles
	vector<Vehicle> points = m_g.getTrainsAt(m_second);

	// draw vehicles
	int num_trains = 0, num_buses = 0, num_boats = 0;
	for(Vehicle c : points) {
		pair<int,int> px = m_cfg.project(c.lat, c.lon);

		auto it = m_colorMap.find(c.route_id);

		if(it == m_colorMap.end()) { // bus
			if (m_img.drawBus(px.first, px.second, m_colorMap[BUS_COLOR], m_cfg.bus() * m_ratio))
				num_buses++;
		} else if (c.route_id.rfind("rb", 0) == 0) { // river bus
			if (m_img.drawBoat(px.first, px.second, it->second, m_cfg.train() * m_ratio))
				num_boats++;
		} else { // train
			if (m_img.drawTrain(px.first, px.second, it->second, m_cfg.train() * m_ratio))
				num_trains++;
		}
	}

	cout << "Drew " << num_trains << " trains, " << num_buses << " buses, " << num_boats << " boats at "
		<< setw(2) << std::setfill('0') << ((m_second / 3600)%24) << ":"
		<< setw(2) << std::setfill('0') << ((m_second - 3600 * (m_second / 3600)) / 60) << ":"
		<< setw(2) << std::setfill('0') << m_second % 60
		<< " -- file name: " << m_name
		<< endl;

	m_img.drawTime(m_second % SECONDS_PER_DAY);

	// save PNG.
	m_img.save(m_name);
}
