include Makefile.inc

LIBS=-lm
SUBDIRS=devices wavlib channel

all: untitled.mp3

untitled.mp3: untitled.wav
	ffmpeg -i untitled.wav -vn -ar 44100 -ac 2 -ab 192k -f mp3 untitled.mp3

untitled.wav: untitled
	./untitled

untitled: build/main.o subdirs
	cd build && $(CC) -o ../untitled *.o $(LIBS)

subdirs: 
	-for d in $(SUBDIRS); do (cd $$d; $(MAKE) $(MFLAGS)); done

build/main.o: main.c
	$(CC) $(CFLAGS) -o build/main.o main.c

