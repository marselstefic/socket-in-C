all: build run

build:
	gcc -m32 mainS.c -o demo_server -lpthread

run:
	./demo_server 8080
	@echo "\n"

clean:
	rm demo_client