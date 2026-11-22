app: ./src/main.cpp
	g++ -std=c++23 -I./include/ -lSDL3 -lSDL3_ttf -lSDL3_image ./src/main.cpp -o ./build/main

clean:
	rm ./build/*
