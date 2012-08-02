OUT=mkvideo
OBJS=mkvideo.o image.o config.o worker.o $(TFL_OBJS)
TFL_OBJS=tfl/csv_reader.o tfl/gtfs.o
CXXFLAGS=-std=c++17 -O3 -Wall -Wextra
#CXXFLAGS=-std=c++0x -O0 -ggdb -Wall -Wextra
LDFLAGS=-lm -lgd -pthread

all: $(OUT) Makefile

$(OUT): $(OBJS) Makefile
	$(CXX) -o $(OUT) $(OBJS) $(LDFLAGS)

%.o: %.cpp %.h Makefile config.h
	$(CXX) -c $(CXXFLAGS) -o $@ $<

%.o: %.cpp Makefile config.h
	$(CXX) -c $(CXXFLAGS) -o $@ $<

clean:
	rm -f $(OBJS) $(OUT)

video:
	# ffmpeg -r 12 -i frame-%5d.png -r 12 -sameq -y /tmp/foo.avi
	# ffmpeg -r 60 -i frame-%5d.png -r 12 -qscale:v 0 -threads 16  -vcodec libx264 -y /tmp/london-4k.avi
	# ffmpeg -r 60 -i frame-%5d.png  -i /tmp/15*.wav -acodec aac -filter_complex:a 'afade=t=in:st=0:d=5,afade=t=out:st=357:d=3' -qscale:v 0 -threads 16 -vframes 21600 -t 360 -filter:v 'fade=in:st=0:d=3,fade=out:st=357:d=3' -f mp4 -vcodec libx264 -pix_fmt yuv420p -y /tmp/city-4k-new.avi
