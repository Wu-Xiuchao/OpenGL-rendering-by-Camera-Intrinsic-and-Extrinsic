test:main.cpp
	g++ -std=c++11 -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo -framework CoreFoundation -o main.out main.cpp -lGLEW -lglfw3 -lSOIL
clean:
	rm main
