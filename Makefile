CC = g++
STD = c++11
CFLAGS = -Wall -g3 --std=$(STD)
OTHER_FLAGS = `wx-config --cxxflags` `wx-config --libs` -pthread
SRC_DIR = .
TARGET_EXT = out
INSTALL_DIR = /usr/bin
BUILD_DIR = build
TARGETS = $(patsubst $(BUILD_DIR)/%, %, $(shell find $(BUILD_DIR) -name '*.$(TARGET_EXT)' 2>/dev/null))

# IMPORTANT: Each target to be listed here
APPS = \
	ping_subnet \
	ping_subnet_gui \

# IMPORTANT: Each target needs to have a similar variable: [target_name]_DEPS
ping_subnet_DEPS = $(patsubst %, $(BUILD_DIR)/%.o, \
	main \
	output_stream_console \
)
ping_subnet_gui_DEPS = $(patsubst %, $(BUILD_DIR)/%.o, \
	gui_app \
	gui_mainframe \
	output_stream_gui \
)

COMMON_DEPS = $(patsubst %, $(BUILD_DIR)/%.o, \
	ping \
	subnet \
	socket \
	factory \
	ip_address \
	orchestrator \
	output_stream \
)

.PHONY: all $(APPS) clean install uninstall

all: $(APPS)

# generate rules for each [app]
define BUILD_TARGET

# every [app] depends on its [app].out
$(1): $(BUILD_DIR) $(BUILD_DIR)/$(1).$(TARGET_EXT)

# every [app].out depends on its dependencies
$(BUILD_DIR)/$(1).$(TARGET_EXT): $$($(1)_DEPS) $(COMMON_DEPS)
	$(CC) $(CFLAGS) $$^ -o $$@ $(OTHER_FLAGS)
endef
# WARNING: dont add space after BUILD_TARGET's comma otherwise everything will break!
$(foreach i, $(APPS), $(eval $(call BUILD_TARGET,$(i))))

# create a build folder
$(BUILD_DIR):
	mkdir -p $@

# every *.o depends on its *.cpp and *.h
$(BUILD_DIR)/%.o: $(addprefix $(SRC_DIR)/, %.cpp %.h)
	$(CC) $(CFLAGS) -c $< -o $@ $(OTHER_FLAGS)

clean:
	rm -rf $(BUILD_DIR)

# install / uninstall each [target]
install: $(addprefix install_, $(TARGETS))
uninstall: $(addprefix uninstall_, $(TARGETS))

define INSTALL_TARGET
install_$(1): 
	@echo "Installing $(BUILD_DIR)/$(1)..."
	@sudo setcap CAP_NET_RAW=eip $(BUILD_DIR)/$(1)
	@sudo ln -fs $(BUILD_DIR)/$(1) $(INSTALL_DIR)/$(1)

uninstall_$(1):
	@echo "Uninstalling $(1)..."
	@sudo rm -f $(INSTALL_DIR)/$(1)
endef
$(foreach i, $(TARGETS), $(eval $(call INSTALL_TARGET,$(i))))
