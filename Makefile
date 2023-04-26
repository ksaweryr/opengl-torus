all: main.o vertex.o fragment.o
	g++ -lGL -lGLEW -lglfw vertex.o fragment.o main.o

main.o: main.cpp
	g++ -Wall -Wextra -Wpedantic -std=c++17 -O3 -c -o main.o main.cpp

vertex.o: vertex.glsl
	printf "`cat vertex.glsl`\0" | xxd -i -name _vertex_src > vertex.c
	g++ -c -o vertex.o vertex.c


fragment.o: fragment.glsl
	printf "`cat fragment.glsl`\0" | xxd -i -name _fragment_src > fragment.c
	g++ -c -o fragment.o fragment.c

clean:
	rm vertex.c fragment.c *.o a.out