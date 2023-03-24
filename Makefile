all: build run

build:
	gcc -m32 main.c -o demo_client
	gcc -m32 mainS.c -o demo_server -lpthread

run:
	./demo_server 8080
	./demo_client localhost 8080 example
	@echo "\n"

clean:
	rm demo_client