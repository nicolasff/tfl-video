#include "gtfs.h"
#include "../config.h"

#include <iostream>
#include <set>
#include <dirent.h>
#include <glob.h>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

Gtfs::Gtfs(int crossover)
	: m_crossover(crossover)
{
}

void
Gtfs::loadDirectory(string dir) {

	loadRoutes(findFile(dir + "/routes.*"));
	loadTrips(findFile(dir + "/trips.*"));
	loadStops(findFile(dir + "/stops.*"));
	loadStopTimes(findFile(dir + "/stop*times.*"));

	m_dirs.push_back(dir);
}

std::string
Gtfs::findFile(std::string pattern) {
	glob_t glob_result;
	::memset(&glob_result, 0, sizeof(glob_result));
	int return_value = glob(pattern.c_str(), GLOB_TILDE, NULL, &glob_result);
    if(return_value != 0) {
        globfree(&glob_result);
		cerr << "No file matching " << pattern << endl;
		return "";
	}

	// number of matches: glob_result.gl_pathc
	return glob_result.gl_pathv[0];
}

void
Gtfs::loadRoutes(std::string filename) {

	cout << "Loading routes from " << filename << endl;
	CsvReader csv(filename);
	int idx_route_id = csv.keyIndex("route_id"),
		idx_short_name = csv.keyIndex("route_short_name"),
		idx_long_name = csv.keyIndex("route_long_name");

	int debug = 0;
	while(!csv.eof()) {
		CsvObject line = csv.get();
		if(line.size() != csv.dimensions())
			continue;

		// decode route
		string id = line[idx_route_id];
		Route route;
		route.name = line[idx_short_name];
		route.desc = line[idx_long_name];
		m_routes.insert(make_pair(id, route));

		if (debug++ < 5) {
			cout << "Loaded route: id=[" << id << "], name=[" << route.name << "], desc=[" << route.desc << "]" << endl;
		}
	}
	cout << "Loaded " << m_routes.size() << " routes" << endl;
}

void
Gtfs::loadTrips(std::string filename) {

	cout << "Loading trips from " << filename << endl;
	CsvReader csv(filename);
	int idx_route_id = csv.keyIndex("route_id"),
		idx_trip_id = csv.keyIndex("trip_id");

	int debug = 0;
	while(!csv.eof()) {
		CsvObject line = csv.get();
		if(line.size() != csv.dimensions())
			continue;

		// decode trip
		string route_id = line[idx_route_id];
		string trip_id = line[idx_trip_id];
		m_route_by_trip.insert(make_pair(trip_id, route_id));
		if (debug++ < 5) {
			cout << "Loaded trip: trip_id=[" << trip_id << "], route_id=[" << route_id << "]" << endl;
		}
	}
	cout << "Loaded " << m_route_by_trip.size() << " trips" << endl;
}

void
Gtfs::loadStops(string filename) {

	cout << "Loading stops from " << filename << endl;
	CsvReader csv(filename);
	int idx_stop_id = csv.keyIndex("stop_id"),
		idx_stop_lat = csv.keyIndex("stop_lat"),
		idx_stop_lon = csv.keyIndex("stop_lon");

	while(!csv.eof()) {
		CsvObject line = csv.get();
		if(line.size() != csv.dimensions())
			continue;

		// decode stop
		string id = line[idx_stop_id];
		Stop stop;
		stop.lat = atof(line[idx_stop_lat].c_str());
		stop.lon = atof(line[idx_stop_lon].c_str());
		m_stops.insert(make_pair(id, stop));
	}
	cout << "Loaded " << m_stops.size() << " stops" << endl;
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
	string expected[] = {"/stops.*", "/routes.*", "/trips.*"};
	for(string s : expected) {
		string pattern = src + s;
		if(findFile(pattern) == "") {
			cerr << "Not including [" << src << "] since we couldn't find [" << s << "]" << endl;
			return;
		}
	}

	// found everything.
	directories.push_back(src);
}


void
Gtfs::loadStopTimes(string filename) {

	cout << "Loading stop times from " << filename << endl;
	CsvReader csv(filename);
	int idx_trip_id = csv.keyIndex("trip_id"),
		idx_stop_id = csv.keyIndex("stop_id"),
		idx_arrival_time = csv.keyIndex("arrival_time"),
		idx_departure_time = csv.keyIndex("departure_time"),
		idx_order = csv.keyIndex("stop_sequence");

	Halt last;
	last.order = -1;

	int loaded = 0;
	while(!csv.eof()) {
		CsvObject line = csv.get();
		if(line.size() != csv.dimensions())
			continue;

		Halt h;

		h.trip_id = line[idx_trip_id];
		h.stop = line[idx_stop_id];
		h.arrival = seconds(line[idx_arrival_time]);
		h.departure = seconds(line[idx_departure_time]);

		// watch out for cross-over
		if(h.arrival < h.departure)
			h.arrival += SECONDS_PER_DAY;

		h.order = atoi(line[idx_order].c_str());

		loaded++;
		if (loaded % 100000 == 0) {
			cout << "Loaded " << loaded << " stop times" << endl;
		}
		m_halts.push_back(h);
	}
	cout << "Loaded " << m_halts.size() << " stop times" << endl;
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
	return Vehicle(route_id, r.name, sfrom.lat + ratio * (sto.lat - sfrom.lat),
			 sfrom.lon + ratio * (sto.lon - sfrom.lon));
}

