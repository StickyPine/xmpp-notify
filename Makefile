CC = gcc
EXE = xmpp-notify

all : $(EXE)

$(EXE) :
	gcc src/$(EXE).c -o $(EXE).exe `pkg-config --cflags --libs libstrophe`
