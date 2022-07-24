CC = g++
CFLAGS = -g --std=c++11
TARGET = ping_subnet
MODULES = \
	main \
	ping \
	subnet \
	socket \
	factory \

OBJ = $(MODULES:=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.cpp %.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
