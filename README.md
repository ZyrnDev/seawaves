# Seawaves

**Seawaves** is a C project for processing `.wav` audio files. It provides tools to read, analyze, and manipulate WAV files from the command line.

## Features

- Reads and parses WAV file headers and audio data
- Prints WAV file information (sample rate, bit depth, channels, etc.)
- Displays sample data for inspection

## Building

To build the project, ensure you have a C compiler (e.g., `gcc`) installed. Then run:

```sh
make
```

This will produce the executable in the `bin/` directory.

## Usage

```sh
./bin/seawaves <path_to_wav_file>
```

Example:

```sh
./bin/seawaves test.wav
```

The program will print information about the WAV file and display sample data.

## Cleaning Up

To remove build artifacts:

```sh
make clean
```

## Requirements

- C99-compatible compiler
- Standard C libraries (`stdio.h`, `stdlib.h`, `math.h`)

## License

TBD: I will add the most permissive license I can once I work out if any dependencies I might use could restrict the license I can offer.
