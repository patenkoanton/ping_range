CC = g++
CFLAGS = -g3 --std=c++11
TARGET = ping_subnet
MODULES = \
	main \
	ping \
	subnet \
	socket \
	factory \
	ip_address \

OBJ = $(MODULES:=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.cpp %.h
	$(CC) $(CFLAGS) -c $< -o $@

install: $(TARGET)
	@echo -e "\nSetting CAP_NET_RAW capability for ping_subnet executable..."
	@sudo setcap cap_net_raw=eip ping_subnet

clean:
	rm -f $(OBJ) $(TARGET)
