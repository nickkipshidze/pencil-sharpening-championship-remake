CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++23 -I./include/
LDLIBS = -lSDL3 -lSDL3_image -lSDL3_ttf

TARGET = main
SRC = $(wildcard ./src/*.cpp)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o ./build/$(TARGET) $(LDLIBS)

run: $(TARGET)
	./build/$(TARGET)

dist:
	mkdir -p ./build/lib
	cp -r ./assets ./build/assets

	$(CXX) -Ofast $(CXXFLAGS) $(SRC) -o ./build/PencilSharpening $(LDLIBS) -Wl,-rpath,'$$ORIGIN/lib'

	@for lib in $$(ldd ./build/PencilSharpening | awk '/=>/ {print $$3}' | grep '^/'); do \
		cp -L "$$lib" ./build/lib/; \
	done

clean:
	rm ./build/*

.PHONY: run clean
