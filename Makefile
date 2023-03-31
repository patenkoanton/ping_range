CC = g++
STD = c++11
CFLAGS = -Wall -g3 --std=$(STD)
OTHER_FLAGS = `wx-config --cxxflags` `wx-config --libs` -pthread
SRC_DIR = .
TARGET_EXT = .out

# each target to be listed here
APPS = \
	ping_subnet \
	ping_subnet_gui \

# each target needs to have a similar variable: [target_name]_DEPENDENCIES
ping_subnet_DEPENDENCIES = $(addsuffix .o, \
	main \
	ping \
	subnet \
	socket \
	factory \
	ip_address \
)
ping_subnet_gui_DEPENDENCIES = $(addsuffix .o, \
	gui_app \
	gui_mainframe \
)

# each dependency variable to be listed here
OBJECTS = \
	$(ping_subnet_DEPENDENCIES) \
	$(ping_subnet_gui_DEPENDENCIES) \

.PHONY: all $(APPS) clean install uninstall

all: $(APPS)

$(APPS): %: $(addsuffix $(TARGET_EXT), %)

$(addsuffix $(TARGET_EXT), %):
	@$(MAKE) generic_target \
		DEPENDENCIES=$(addsuffix _DEPENDENCIES, $*) \
		TARGET=$@

generic_target: $($(DEPENDENCIES))
	$(CC) $(CFLAGS) $^ -o $(TARGET) $(OTHER_FLAGS)

%.o: $(SRC_DIR)/%.cpp $(SRC_DIR)/%.h
	$(CC) $(CFLAGS) -c $< -o $@ $(OTHER_FLAGS)

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
