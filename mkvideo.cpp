#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

#include <sys/types.h>

#include "image.h"
#include "config.h"
#include "worker.h"
#include "tfl/gtfs.h"

using namespace std;

static void
usage(const char *argv0) {
	cerr << "Usage: " << argv0 << "(city|london) OUTPUT-DIR PATH-TO-GTFS-DIRS..." << endl;
	_exit(1);
}

int
main(int argc, char *argv[]) {

	if(argc < 4)
		usage(argv[0]);

	Config cfgLondon("london.png",
			152,75,51.655306,-0.4949,		// a point at the top left
			1629,1008,51.389566,0.182819,	// a point at the bottom right
			5,								// train size
			1);								// bus size

	Config cfgCity("city.png",
			545,410,51.53742,-0.192668,		// a point at the top left
			1493,846,51.474828,0.024526,	// a point at the bottom right
			10,								// train size
			3);								// bus size

	Config cfg;
	string zoom = argv[1];
	if(zoom == "city") {
		cfg = cfgCity;
	} else if(zoom == "london") {
		cfg = cfgLondon;
	} else {
		usage(argv[0]);
	}

	string outdir = argv[2];

	// find GTFS files
	cout << "Loading GTFS data..." << endl;
	vector<string> directories;
	for(int i = 3; i < argc; ++i) {
		Gtfs::findFiles(argv[i], directories);
	}

	Gtfs g(START_TIME);
	for(string s : directories) {
		g.loadDirectory(s);
	}

	cout << "Rendering frames..." << endl;
	Image img(cfg.file(), IMAGE_RATIO);

	// draw frames
	for(int second = START_TIME; second < DURATION + START_TIME;) {

		Worker* workers[THREAD_COUNT];
		for(int i = 0; i < THREAD_COUNT; ++i) {
			// generate image name
			stringstream ss;
			ss << outdir + "/frame-" << setw(5) << std::setfill('0') << ((second - START_TIME) / IMAGE_FREQ) << ".png";

			// render in a worker thread
			workers[i] = new Worker(g, img, cfg, ss.str(), IMAGE_RATIO, second);
			workers[i]->start();
			second += IMAGE_FREQ;
		}

		// join all threads
		for(Worker *w : workers) {
			w->join();
			delete w;
		}
	}

	return 0;
}
