CC = g++
CFLAGS = -g --std=c++11
TARGET = ping_subnet
MODULES = \
	main \
	ping_subnet \
	address_range \
	icmp_socket \

OBJ = $(MODULES:=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.cpp %.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
