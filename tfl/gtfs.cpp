#include "gtfs.h"
#include "../config.h"

#include <iostream>
#include <set>
#include <dirent.h>

using namespace std;

Gtfs::Gtfs(int crossover)
	: m_crossover(crossover)
{
}

void
Gtfs::loadDirectory(string dir) {

	loadRoutes(dir + "/routes.txt");
	loadTrips(dir + "/trips.txt");
	loadStops(dir + "/stops.txt");
	loadStopTimes(dir + "/stop_times.txt");

	m_dirs.push_back(dir);
}

void
Gtfs::loadRoutes(std::string filename) {

	CsvReader csv(filename);

	while(!csv.eof()) {
		CsvObject line = csv.get();
		if(line.size() != csv.dimensions())
			continue;

		// decode route
		string id = line[route_id];
		Route route;
		route.name = line[route_short_name];
		route.desc = line[route_desc];
		m_routes.insert(make_pair(id, route));
	}
}

void
Gtfs::loadTrips(std::string filename) {

	CsvReader csv(filename);

	while(!csv.eof()) {
		CsvObject line = csv.get();
		if(line.size() != csv.dimensions())
			continue;

		// decode trip
		string route_id = line[0];
		string trip_id = line[2];
		m_route_by_trip.insert(make_pair(trip_id, route_id));
	}
}

void
Gtfs::loadStops(string filename) {

	CsvReader csv(filename);

	while(!csv.eof()) {
		CsvObject line = csv.get();
		if(line.size() != csv.dimensions())
			continue;

		// decode stop
		string id = line[stop_id];
		Stop stop;
		stop.lat = atof(line[stop_lat].c_str());
		stop.lon = atof(line[stop_lon].c_str());
		m_stops.insert(make_pair(id, stop));
	}
}

vector<Vehicle>
Gtfs::getTrainsAt(int t) {

	vector<Vehicle> ret;

	Halt last;
	last.departure = 0;

	for(Halt h : m_halts) {

		int second = t;

		if(h.trip_id == last.trip_id && h.order == last.order + 1) { // have 2 positions

			if(second >= last.departure && second <= h.arrival
					&& h.arrival - last.departure < 3600) {	// avoid stuck vehicles

				Vehicle c = interpolate(last, h, second);
				ret.push_back(c);
			}

		}
		last = h;
	}

	return ret;
}

void
Gtfs::findFiles(string src, vector<string> &directories) {

	set<string> files;

	// read files
	DIR *d = opendir(src.c_str());
	struct dirent *f;
	while ((f = readdir(d))) {
		if(f->d_name[0] == '.') continue;

		if(f->d_type == DT_DIR) {
			findFiles((src + "/") + f->d_name, directories);
		} else if(f->d_type == DT_REG) {
			files.insert(f->d_name);
		}
	}
	closedir(d);

	string expected[] = {"stops.txt", "routes.txt", "trips.txt"};
	for(string s : expected) {
		if(files.find(s) == files.end())
			return;
	}

	// found everything.
	directories.push_back(src);
}


void
Gtfs::loadStopTimes(string filename) {

	CsvReader csv(filename);

	Halt last;
	last.order = -1;

	while(!csv.eof()) {
		CsvObject line = csv.get();
		if(line.size() != csv.dimensions())
			continue;

		Halt h;

		h.trip_id = line[0];
		h.stop = line[3];
		h.arrival = seconds(line[1]);
		h.departure = seconds(line[2]);

		// watch out for cross-over
		if(h.arrival < h.departure)
			h.arrival += SECONDS_PER_DAY;

		h.order = atoi(line[4].c_str());

		m_halts.push_back(h);
	}
}

int
Gtfs::seconds(std::string &stime) const {
	if(stime.size() != 8)
		return -1;

	const char *p = stime.c_str();

	int h = atoi(p);
	int m = atoi(p + 3);
	int s = atoi(p + 6);

	int t = h * 3600 + m * 60 + s;
	if(t < m_crossover)
		t += SECONDS_PER_DAY;

	return t;
}

Vehicle
Gtfs::interpolate(Halt &from, Halt &to, int second) {

	// get stops
	Stop sfrom = m_stops[from.stop];
	Stop sto = m_stops[to.stop];

	// create coords
	Coords cfrom(sfrom.lat, sfrom.lon);
	Coords cto(sto.lat, sto.lon);

	// scaling ratio
	double ratio = (second - from.departure);
	ratio /= (to.arrival - from.departure);

	// find line
	string route_id = m_route_by_trip[from.trip_id];
	Route r = m_routes[route_id];

	// compute coords
	return Vehicle(r.name, sfrom.lat + ratio * (sto.lat - sfrom.lat),
			 sfrom.lon + ratio * (sto.lon - sfrom.lon));
}

