
all: build

build:

format-all:
	find . -wholename './app/*.cpp' -o -wholename './app/*.hpp' | xargs clang-format -i
	clang-format -i ./main.cpp
