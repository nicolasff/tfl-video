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
	, m_img(city.width(), city.height(), ratio)
	, m_city(city)
	, m_ratio(ratio)
	, m_second(second)
	, m_cfg(cfg)
	, m_name(name)
{
}

void
Worker::initColorMap() {
	m_colorMap.insert(make_pair(BUS_COLOR, m_img.color(0xbe, 0x00, 0x02))); // bus color
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
	cout << "Looking for trains at second=" << m_second << ": "
		<< points.size() << " points." << endl;

	// draw vehicles
	for(Vehicle c : points) {
		pair<int,int> px = m_cfg.project(c.lat, c.lon);

		auto it = m_colorMap.find(c.line);

		if(it == m_colorMap.end()) { // bus
			m_img.drawBus(px.first, px.second, m_colorMap[BUS_COLOR], m_cfg.bus() * m_ratio);
		} else { // train
			m_img.drawTrain(px.first, px.second, it->second, m_cfg.train() * m_ratio);
		}
	}

	m_img.drawTime(m_second % SECONDS_PER_DAY);

	// save PNG.
	m_img.save(m_name);
}
