prgm = client_GAIA
second_prgm = base64
make: $(prgm).cpp
	g++ -o exe $(prgm).cpp $(second_prgm).cpp -lpthread -lrt `pkg-config --cflags --libs opencv4` -lsfml-graphics -lsfml-window -lsfml-system -Wall


