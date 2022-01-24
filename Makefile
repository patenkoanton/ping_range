CC = g++
CFLAGS = -g --std=c++11
TARGET = ping_range

$(TARGET): main.o ping_range.o address_range.o icmp_socket.o
	$(CC) $(CFLAGS) $^ -o $@

main.o: main.cpp
	$(CC) $(CFLAGS) $^ -c

ping_range.o: ping_range.cpp ping_range.h
	$(CC) $(CFLAGS) $^ -c

address_range.o: address_range.cpp address_range.h
	$(CC) $(CFLAGS) $^ -c

icmp_socket.o: icmp_socket.cpp icmp_socket.h
	$(CC) $(CFLAGS) $^ -c

clean:
	rm -f ping_range.o ping_range.h.gch main.o address_range.o address_range.h.gch icmp_socket.o icmp_socket.h.gch $(TARGET)