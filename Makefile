CC = g++
STD = c++11
CFLAGS = -Wall -g3 --std=$(STD) -pthread
SRC_DIR = .
TARGET_EXT = .out

# each target to be listed here
APPS = \
	ping_subnet \

# each target needs to have a similar variable: [target_name]_DEPENDENCIES
ping_subnet_DEPENDENCIES = $(addsuffix .o, \
	main \
	ping \
	subnet \
	socket \
	factory \
	ip_address \
)

# each dependency variable to be listed here
OBJECTS = \
	$(ping_subnet_DEPENDENCIES) \

.PHONY: all $(APPS) clean install uninstall

all: $(APPS)

$(APPS): %: $(addsuffix $(TARGET_EXT), %)

$(addsuffix $(TARGET_EXT), %):
	@$(MAKE) \
		DEPENDENCIES=$(addsuffix _DEPENDENCIES, $*) \
		TARGET=$@ \
		generic_target

generic_target: $($(DEPENDENCIES))
	$(CC) $(CFLAGS) $^ -o $(TARGET)

%.o: $(SRC_DIR)/%.cpp $(SRC_DIR)/%.h
	$(CC) $(CFLAGS) -c $< -o $@

TARGETS = $(addsuffix $(TARGET_EXT), $(APPS))
clean:
	rm -f $(TARGETS)
	rm -f $(OBJECTS)


# TODO: uncomment and fix

# # install / uninstall
# INSTALL_DIR = /usr/bin

# install:
# 	@echo "\nInstalling to $(INSTALL_DIR)..."
# 	@sudo cp $(TARGET) $(INSTALL_PATH)
# 	@echo "Setting CAP_NET_RAW capability for $(TARGET) executable..."
# 	@sudo setcap cap_net_raw=eip $(TARGET) cap_net_raw=eip $(INSTALL_PATH)

# uninstall:
# 	@sudo rm -f $(INSTALL_PATH)
# 	@echo "Uninstall complete..."
