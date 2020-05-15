INCS=-I/usr/local/Cellar/glew/2.1.0_1/include \
-I/usr/local/Cellar/glfw/3.3.2/include \
-I/usr/local/Cellar/freeimage/3.18.0/include \
-I/usr/local/Cellar/glm/0.9.9.8/include

LIBS=-L/usr/local/Cellar/glew/2.1.0_1/lib -lglfw \
-L/usr/local/Cellar/glfw/3.3.2/lib -lGLEW \
-L/usr/local/Cellar/freeimage/3.18.0/lib -lfreeimage \
-framework GLUT -framework OpenGL -framework Cocoa

all: main

main: main.o common.o
	g++ $(LIBS) main.o common.o -o main
	rm -f *.o

main.o: main.cpp
	g++ -c $(INCS) main.cpp -o main.o

common.o: common.cpp
	g++ -c $(INCS) common.cpp -o common.o

.PHONY: clean

clean:
	rm -vf main
