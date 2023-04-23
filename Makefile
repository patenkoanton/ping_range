CC = g++
STD = c++11
CFLAGS = -Wall -g3 --std=$(STD) -pthread
CFLAGS_INCLUDE = -I$(SRC_DIR)/ping_subnet_gui -I$(SRC_DIR)/ping_subnet -I$(SRC_DIR)/common
OTHER_FLAGS = `wx-config --cxxflags` `wx-config --libs`
SRC_DIR = src
TARGET_EXT = out
INSTALL_DIR = /usr/bin
BUILD_DIR = build

# IMPORTANT: Each target to be listed here
PING_SUBNET_APP = ping_subnet
PING_SUBNET_GUI_APP = ping_subnet_gui
APPS = \
	$(PING_SUBNET_APP) \
	$(PING_SUBNET_GUI_APP) \

# IMPORTANT: Each target needs to have a similar variable: [target_name]_DEPS
$(PING_SUBNET_APP)_DEPS = $(BUILD_DIR)/$(PING_SUBNET_APP)/
$(PING_SUBNET_APP)_DEPS += $(patsubst %, $(BUILD_DIR)/$(PING_SUBNET_APP)/%.o, \
	main \
)

$(PING_SUBNET_GUI_APP)_DEPS = $(BUILD_DIR)/$(PING_SUBNET_GUI_APP)/
$(PING_SUBNET_GUI_APP)_DEPS += $(patsubst %, $(BUILD_DIR)/$(PING_SUBNET_GUI_APP)/%.o, \
	gui_app \
	gui_mainframe \
	output_stream_gui \
)

COMMON_DEPS = $(BUILD_DIR)/common/
COMMON_DEPS += $(patsubst %, $(BUILD_DIR)/common/%.o, \
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

# Generate app-specific rules.
# Every app depends on its target.out
define BUILD_APP
FULL_TARGET_NAME = $(BUILD_DIR)/$(1)/$(1).$(TARGET_EXT)
$(1): $$(FULL_TARGET_NAME)
$$(FULL_TARGET_NAME): $$($(1)_DEPS) $(COMMON_DEPS)
	$(CC) $(CFLAGS) $(CFLAGS_INCLUDE) $$(filter %.o, $$^) -o $$@ $(OTHER_FLAGS)
endef
# WARNING: dont add space after BUILD_APP's comma otherwise everything will break!
$(foreach APP, $(APPS), $(eval $(call BUILD_APP,$(APP))))

# every *.o depends on its *.cpp and *.h
$(BUILD_DIR)/%.o: $(addprefix $(SRC_DIR)/, %.cpp %.h)
	$(CC) $(CFLAGS) $(CFLAGS_INCLUDE) -c $< -o $@ $(OTHER_FLAGS)

$(BUILD_DIR)/%/:
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR)

# Install / uninstall.
TARGETS = $(shell find $(BUILD_DIR) -name '*.$(TARGET_EXT)' 2>/dev/null)
install: $(addprefix install_, $(TARGETS))
uninstall: $(addprefix uninstall_, $(TARGETS))

define INSTALL_TARGET
install_$(1):
	@echo "Installing $(1)..."
	@sudo setcap CAP_NET_RAW=eip $(1)
	@sudo ln -fs $(1) $(INSTALL_DIR)/$(notdir $(1))

uninstall_$(1):
	@echo "Uninstalling $(1)..."
	@sudo rm -f $(INSTALL_DIR)/$(notdir $(1))
endef
$(foreach TARGET, $(TARGETS), $(eval $(call INSTALL_TARGET,$(TARGET))))
