SRCS=$(wildcard *.cpp)

chip8:
	g++ -g -o chip8 $(SRCS) -lglut -lGLU -lGL

clean:
	$(RM) chip8

re: clean chip8

.PHONY: clean re
