prgm = client
second_prgm = base64
make: $(prgm).cpp
	g++ -o exe $(prgm).cpp $(second_prgm).cpp -lpthread -lX11 -lrt `pkg-config --cflags --libs opencv` -lsfml-graphics -lsfml-window -lsfml-system -Wall


