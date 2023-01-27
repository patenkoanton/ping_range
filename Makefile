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

$(TARGET): SHELL:=/bin/bash
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@
	@echo -e "\nSetting cap_net_raw capability for ping_subnet executable..."
	@sudo setcap cap_net_raw=eip ~/documents/c_projects/ping_subnet/ping_subnet

%.o: %.cpp %.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
