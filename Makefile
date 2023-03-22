all: build run

build:
	gcc -m32 main.c -o demo_client

run:
	./demo_client localhost 8080 example
	@echo "\n"

clean:
	rm demo_client