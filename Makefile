CC = g++
STD = c++11
CFLAGS = -Wall -g3 --std=$(STD) -pthread
TARGET = ping_subnet
INSTALL_DIR = /usr/bin
INSTALL_PATH = $(INSTALL_DIR)/$(TARGET)
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

install:
	@echo "\nInstalling to $(INSTALL_DIR)..."
	@sudo cp $(TARGET) $(INSTALL_PATH)
	@echo "Setting CAP_NET_RAW capability for $(TARGET) executable..."
	@sudo setcap cap_net_raw=eip $(TARGET) cap_net_raw=eip $(INSTALL_PATH)

clean:
	@rm -f $(OBJ) $(TARGET)

uninstall:
	@sudo rm -f $(INSTALL_PATH)
	@echo "Uninstall complete..."
