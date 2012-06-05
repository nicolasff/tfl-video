#ifndef TFL_GTFS_H
#define TFL_GTFS_H

#include <string>
#include <vector>
#include <map>

#include "defs.h"
#include "csv_reader.h"

class Gtfs {
public:

	Gtfs(int crossover);
	void loadDirectory(std::string dir);
	std::vector<Vehicle> getTrainsAt(int second);

	static void findFiles(std::string src, std::vector<std::string> &directories);

private:
	void loadRoutes(std::string filename);
	void loadTrips(std::string filename);
	void loadStops(std::string filename);
	void loadStopTimes(std::string filename);

	int seconds(std::string &stime) const;

	Vehicle interpolate(Halt &from, Halt &to, int second);

private:
	int m_crossover;

	std::map<std::string,Stop> m_stops; // stops, indexed by id.
	std::map<std::string,Route> m_routes; // routes, indexed by id.
	std::map<std::string,std::string> m_route_by_trip; // routes ID, indexed by trip id.

	std::vector<std::string> m_dirs; // GTFS directories

	std::vector<Halt> m_halts;
};

#endif // TFL_GTFS_H
