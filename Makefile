CC = g++
CXXFLAGS = -Wall -Wextra -O2 -g -std=c++11

obj = main.o

opengl: $(obj)
	$(CC) $^ -o $@ -lglut -lGLEW -lGL -pthread

%.o: %.c
	$(CC)  -o $@ -c $<

clean:
	rm -f *.o

mrproper: clean
	rm -rf opengl

