# make rule primaria con dummy target ‘all’--> non crea alcun file all ma fa un complete build
# che dipende dai target client e server scritti sotto
all: client server other


server: server.o 
	gcc –Wall server.o –o server

client: client.o
	gcc –Wall client.o –o client

other: client.o
	gcc –Wall client.o –o other

server.o: server.c
	gcc -Wall -c server.c -o server.o

client.o: client.c
	gcc -Wall -c client.c -o client.o

clean:
	rm -f *.o client server other

run: all
		gnome-terminal --tab --title="Server Terminal" --command="bash -c './server 4242; exec bash'" \
                   --tab --title="Client Terminal" --command="bash -c './client 6000; exec bash'" \
				   				   --tab --title="Other Terminal" --command="bash -c './other 6100; exec bash'"