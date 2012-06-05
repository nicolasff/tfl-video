OUT=mkvideo
OBJS=mkvideo.o image.o config.o worker.o $(TFL_OBJS)
TFL_OBJS=tfl/csv_reader.o tfl/gtfs.o
CXXFLAGS=-std=c++0x -O3 -Wall -Wextra
LDFLAGS=-lm -lgd -pthread

all: $(OUT) Makefile

$(OUT): $(OBJS) Makefile
	$(CXX) -o $(OUT) $(OBJS) $(LDFLAGS)

%.o: %.cpp %.h Makefile
	$(CXX) -c $(CXXFLAGS) -o $@ $<

%.o: %.cpp Makefile
	$(CXX) -c $(CXXFLAGS) -o $@ $<

clean:
	rm -f $(OBJS) $(OUT)

video:
	# ffmpeg -r 12 -i frame-%5d.png -r 12 -sameq -y /tmp/foo.avi
