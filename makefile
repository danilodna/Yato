CC = gcc
CFLAGS = -lm -g -Wall -O2 -pthread
OBJDIR = obj

objects = $(addprefix $(OBJDIR)/, server.o truco.o utils.o safe.o)
server = $(addprefix $(OBJDIR)/, server.o)
client = $(addprefix $(OBJDIR)/, client.o)
truco = $(addprefix $(OBJDIR)/, truco.o)
utils = $(addprefix $(OBJDIR)/, utils.o)
safe = $(addprefix $(OBJDIR)/, safe.o)

TARGET = server client

all: $(TARGET)

server: $(server)
	$(CC) -o $@ $(objects) $(CFLAGS)

client: $(client)
	$(CC) -o $@ $(client) $(utils) $(safe) $(CFLAGS)

$(server): src/server.c $(truco)
	$(CC) -c src/server.c -o $@ $(CFLAGS)

$(client): src/client.c $(utils)
	$(CC) -c src/client.c -o $@ $(CFLAGS)

$(truco): src/truco.c include/truco.h $(utils)
	$(CC) -c src/truco.c -o $@ $(CFLAGS)

$(utils): src/utils.c include/utils.h $(safe)
	$(CC) -c src/utils.c -o $@ $(CFLAGS)

$(safe): src/safe.c include/safe.h
	$(CC) -c src/safe.c -o $@ $(CFLAGS)

clean:
	rm -f obj/*.o server client *~
