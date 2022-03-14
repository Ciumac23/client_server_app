build:
	gcc server.c -lm -o server
	gcc subscriber.c -o sub

clean:
	rm -r *.out
