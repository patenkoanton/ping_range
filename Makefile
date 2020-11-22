CC = g++
CFLAGS = -g
TARGET = ping_range.exe

$(TARGET): main.o ping_range.o address_range.o
	$(CC) $(CFLAGS) $^ -o $@

main.o: main.cpp
	$(CC) $(CFLAGS) $^ -c

ping_range.o: ping_range.cpp ping_range.h
	$(CC) $(CFLAGS) $^ -c

address_range.o: address_range.cpp address_range.h
	$(CC) $(CFLAGS) $^ -c

clean:
	del ping_range.o ping_range.h.gch main.o address_range.o address_range.h.gch $(TARGET)