all: build

build:
	g++ -pthread tema1.cpp -o tema1 -Wall -Werror

clean:
	rm tema1
