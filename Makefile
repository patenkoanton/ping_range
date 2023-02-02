CC = g++
STD = c++11
CFLAGS = -g3 --std=$(STD)
TARGET = ping_subnet
BINARY_DIR = /usr/bin
BINARY_PATH = $(BINARY_DIR)/$(TARGET)
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
	@echo "\nInstalling to $(BINARY_DIR)..."
	@sudo cp $(TARGET) $(BINARY_PATH)
	@echo "Setting CAP_NET_RAW capability for $(TARGET) executable..."
	@sudo setcap cap_net_raw=eip $(TARGET) cap_net_raw=eip $(BINARY_PATH)

clean:
	@rm -f $(OBJ) $(TARGET)

uninstall:
	@sudo rm -f $(BINARY_PATH)
	@echo "Uninstall complete..."
