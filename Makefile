CFLAGS:=-pedantic -W -Wextra -std=c99
LDFLAGS:=-lm

FILE:=output.wav

APP_NAME=seawaves
EXECUTABLE=bin/$(APP_NAME)

SRC = src
SOURCES=$(wildcard $(SRC)/*.c $(SRC)/**/*.c)
#HEADERS=$(wildcard $(SRC)/*.h $(SRC)/**/*.h)
OBJECTS=$(SOURCES:.c=.o)

$(EXECUTABLE): $(OBJECTS)
	@mkdir -p bin
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

all: $(EXECUTABLE)

clean:
	rm -f $(EXECUTABLE) $(OBJECTS) #*.wav *.mp4

run: ./$(EXECUTABLE)
	./$(EXECUTABLE) test.wav

%.wav: all
	./$(EXECUTABLE) $@

%.mp4: %.wav
	ffmpeg -f lavfi -i color=c=black:s=1920x1080:r=5 -i $< -c:a aac -b:a 128k -shortest -max_interleave_delta 200M -fflags +shortest $@

.PHONY: all clean run
