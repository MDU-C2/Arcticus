prgm = server
second_prgm = base64
make: $(prgm).cpp
	g++ -o exe $(prgm).cpp $(second_prgm).cpp -lwiringPi -pthread `pkg-config --cflags --libs opencv4` -Wall
