
# Flags de raylib (rutas de include y librerias) via pkg-config
RAYLIB = $(shell pkg-config --cflags --libs raylib)

all: compile_all

compile_all:
	g++ -std=c++17 -o proyecto main.cpp -lcurl $(RAYLIB)
