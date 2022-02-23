CC = gcc
EXE = xmpp-notify

all : $(EXE)

$(EXE) :
	gcc src/$(EXE).c -o $(EXE) `pkg-config --cflags --libs libstrophe`
