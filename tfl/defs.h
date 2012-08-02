#ifndef TFL_DEFS_H
#define TFL_DEFS_H

#include <vector>
#include <string>

typedef std::vector<std::string> CsvObject;

enum {
	stop_id = 0,
	stop_name,
	//stop_desc,
	stop_lat,
	stop_lon,
	//stop_zone_id,
	//stop_url
};

struct Coords {
	Coords(double lat_ = 0.0f, double lon_ = 0.0f)
		: lat(lat_), lon(lon_) {}
	double lat;
	double lon;
};

typedef Coords Stop;

struct Vehicle : public Coords{
	Vehicle(std::string route_id_, std::string line_, double lat_ = 0.0f, double lon_ = 0.0f)
		: Coords(lat_, lon_)
		, route_id(route_id_)
		, line(line_) {}

	std::string route_id;
	std::string line;
};

typedef Coords Stop;

enum
{
	/*
	route_id = 0,
	route_agency_id,
	route_short_name,
	route_long_name,
	route_desc,
	route_type,
	route_url,
	route_color,
	route_text_color
	*/
	route_id = 0,
	route_agency_id,
	route_color,
	route_short_name,
	route_long_name,
	route_type,
};

struct Route {
	std::string name;
	std::string desc;
};


// a halt is a planned visit of a vehicle at a stop.
struct Halt {
	std::string trip_id;
	int order;		// which step in the trip is it?

	int arrival;	// timestamp
	int departure;	// timestamp
	std::string stop;
};

struct Trip {
	int earliest;
	int latest;

	std::string id;
};

#endif // TFL_DEFS_H
