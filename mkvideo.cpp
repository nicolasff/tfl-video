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
	cerr << "Usage: " << argv0 << "(city|london|mini|mini2) OUTPUT-DIR PATH-TO-GTFS-DIRS..." << endl
        << "GTFS: see Dropbox/code/lovene.st/data/gtfs/" << endl;
	exit(1);
}

int
main(int argc, char *argv[]) {

	if(argc < 4)
		usage(argv[0]);

//	Config cfgLondon("london.png",
//			152,75,51.655306,-0.4949,		// a point at the top left
//			1629,1008,51.389566,0.182819,	// a point at the bottom right
//			5,								// train size
//			1);								// bus size

	Config cfgLondon("london-4k.png",
			158, 179, 51.594034827261325, -0.3823943437059084,
			3600, 1976, 51.42302529394492, 0.14465225016537356,
			16,		// train size
			4,		// bus size
			10,		// boat size
			522, 96, 20);	// font size

//	Config cfgCity("city.png",
//			545,410,51.53742,-0.192668,		// a point at the top left
//			1493,846,51.474828,0.024526,	// a point at the bottom right
//			10,								// train size
//			3);								// bus size

	Config cfgCity("city-4k.png",
			116,63,51.56009070364965, -0.2519833947621261,		// a point at the top left
			3797,2095,51.46396632585791, 0.02786072728859159,	// a point at the bottom right
			16,		// train size
			4,		// bus size
			10,		// boat size
			522, 96, 20);	// font size
			

	Config cfgMini("mini.png",
			58, 58, 51.53330535593542, -0.1729884692457905,		// a point at the top left
			1201, 663, 51.47618741789619, 0.0006056783837631854,	// a point at the bottom right
			8,		// train size
			2,		// bus size
			7,		// boat size
			261, 48, 10);	// font size

	Config cfgMini2("mini2.png",
			27, 33, 51.5398710752789, -0.18728407878070755,		// a point at the top left
			1252, 681, 51.481526597488156, -0.010110741486055274,	// a point at the bottom right
			8,		// train size
			2,		// bus size
			7,		// boat size
			261, 48, 10);	// font size

	Config cfg;
	string zoom = argv[1];
	if(zoom == "city") {
		cfg = cfgCity;
	} else if(zoom == "london") {
		cfg = cfgLondon;
	} else if(zoom == "mini") {
		cfg = cfgMini;
	} else if(zoom == "mini2") {
		cfg = cfgMini2;
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
        cout << "Loading directory " << s << endl;
		g.loadDirectory(s);
	}

	cout << "Rendering frames..." << endl;
	Image img(cfg.file(), IMAGE_RATIO);

	long start_time_sec = time(nullptr);
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

		long now_sec = time(nullptr);
		double percent_done = ((double)(second-START_TIME))/((double)DURATION);
		long total_time_sec = ((now_sec - start_time_sec) / percent_done);
		long remaining_sec = (total_time_sec - (now_sec - start_time_sec));

		std::stringstream ss;
		if (remaining_sec >= 3600) {
			ss << (remaining_sec/3600) << " hour";
			ss << (remaining_sec/3600 > 1 ? "s " : " ");
			remaining_sec %= 3600;
		}
		if (remaining_sec >= 60) {
			ss << (remaining_sec/60) << " minute";
			ss << (remaining_sec/60 > 1 ? "s " : " ");
			remaining_sec %= 60;
		}
		if (remaining_sec >= 0) {
			ss << remaining_sec << " second";
			ss << (remaining_sec > 1 ? "s " : "");
		}

		cout << std::setprecision(2) << 100.0 * percent_done
			<< "% done in " << (now_sec - start_time_sec) << " seconds. ETA: "
			<< ss.str()
			<< endl;
	}

	return 0;
}
