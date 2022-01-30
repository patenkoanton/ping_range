CC = g++
CFLAGS = -g --std=c++11
TARGET = ping_subnet

$(TARGET): main.o ping_subnet.o address_range.o icmp_socket.o
	$(CC) $(CFLAGS) $^ -o $@

main.o: main.cpp
	$(CC) $(CFLAGS) $^ -c

ping_subnet.o: ping_subnet.cpp ping_subnet.h
	$(CC) $(CFLAGS) $^ -c

address_range.o: address_range.cpp address_range.h
	$(CC) $(CFLAGS) $^ -c

icmp_socket.o: icmp_socket.cpp icmp_socket.h
	$(CC) $(CFLAGS) $^ -c

clean:
	rm -f ping_subnet.o ping_subnet.h.gch main.o address_range.o address_range.h.gch icmp_socket.o icmp_socket.h.gch $(TARGET)