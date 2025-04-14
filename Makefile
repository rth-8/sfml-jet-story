build:
	c++ main.cpp -o js.exe -std=c++20 -O3 -s -IC:/SFML-3.0.0/include -LC:/SFML-3.0.0/lib -lsfml-graphics -lsfml-window -lsfml-system

run:
	js.exe

clean:
	del js.exe
