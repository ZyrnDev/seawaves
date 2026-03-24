CC:=clang
CFLAGS:=-pedantic -Wall -Wextra -Werror -Wno-gnu-zero-variadic-macro-arguments -pedantic -ftrapv -fsanitize=address -fno-omit-frame-pointer -std=c99 -I/usr/local/include
# -DLOG_LEVEL=LOG_INFO
LDFLAGS:=-L/usr/local/lib
LDLIBS:=-lraylib -lm

FILE:=output.wav

APP_NAME=seawaves
EXECUTABLE=bin/$(APP_NAME)

SRC = src
SOURCES=$(wildcard $(SRC)/*.c $(SRC)/**/*.c)
#HEADERS=$(wildcard $(SRC)/*.h $(SRC)/**/*.h)
OBJECTS=$(SOURCES:.c=.o)

$(EXECUTABLE): $(OBJECTS)
	@mkdir -p bin
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

all: $(EXECUTABLE)

play: test.wav
	#ffplay -autoexit -nodisp $<
	cvlc --play-and-exit $<

clean:
	rm -f $(EXECUTABLE) $(OBJECTS) *.wav *.mp4

run: ./$(EXECUTABLE)
	./$(EXECUTABLE) test.wav

%.wav: all
	./$(EXECUTABLE) --destination $@ generate

%.mp4: %.wav
	ffmpeg -f lavfi -i color=c=black:s=1920x1080:r=5 -i $< -c:a aac -b:a 128k -shortest -max_interleave_delta 200M -fflags +shortest $@

.PHONY: all clean run play
