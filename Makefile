CC = g++
STD = c++11
CFLAGS = -Wall -g3 --std=$(STD) -pthread
CFLAGS += -I$(SRC_DIR)/ping_subnet_gui -I$(SRC_DIR)/ping_subnet -I$(SRC_DIR)/common
OTHER_FLAGS = `wx-config --cxxflags` `wx-config --libs`

SRC_DIR = src
BUILD_DIR = build
BIN_DIR = $(BUILD_DIR)/bin
INSTALL_DIR = /usr/bin

# IMPORTANT: Each target to be listed here
PING_SUBNET = ping_subnet
PING_SUBNET_GUI = ping_subnet_gui
TARGETS = \
	$(PING_SUBNET) \
	$(PING_SUBNET_GUI) \

# IMPORTANT: Each target needs to have a similar variable: [target_name]_DEPS
$(PING_SUBNET)_DEPS = $(patsubst %, $(PING_SUBNET)/%.o, \
	main \
)

$(PING_SUBNET_GUI)_DEPS = $(patsubst %, $(PING_SUBNET_GUI)/%.o, \
	gui_app \
	gui_mainframe \
	output_stream_gui \
)

COMMON_DEPS = $(patsubst %, common/%.o, \
	ping \
	subnet \
	socket \
	factory \
	ip_address \
	orchestrator \
	output_stream \
)

.PHONY: all clean install uninstall $(TARGETS)

all: $(TARGETS)

# Generate target-specific rules.
define BUILD_TARGET

$(1): $(BIN_DIR)/$(1)

$(BIN_DIR)/$(1): $$(addprefix $(BUILD_DIR)/, $$($(1)_DEPS) $(COMMON_DEPS))
	@mkdir -p $$(dir $$@)
	$(CC) $(CFLAGS) $$^ -o $$@ $(OTHER_FLAGS)

endef
# WARNING: dont add space after BUILD_TARGET's comma otherwise everything will break!
$(foreach TARGET, $(TARGETS), $(eval $(call BUILD_TARGET,$(TARGET))))

# every *.o depends on its *.cpp and *.h
$(BUILD_DIR)/%.o: $(addprefix $(SRC_DIR)/, %.cpp %.h)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@ $(OTHER_FLAGS)

clean:
	rm -rf $(BUILD_DIR)

# Install / uninstall.
CURRENT_TARGETS = $(addprefix $(BIN_DIR)/, $(shell ls $(BIN_DIR) 2>/dev/null))
install: $(addprefix install_, $(CURRENT_TARGETS))
uninstall: $(addprefix uninstall_, $(CURRENT_TARGETS))

define INSTALL_TARGET
install_$(1):
	@echo "Installing $(1)..."
	@sudo setcap CAP_NET_RAW=eip $(1)
	@sudo ln -f $(1) $(INSTALL_DIR)/$(notdir $(1))

uninstall_$(1):
	@echo "Uninstalling $(1)..."
	@sudo rm -f $(INSTALL_DIR)/$(notdir $(1))
endef
$(foreach TARGET, $(CURRENT_TARGETS), $(eval $(call INSTALL_TARGET,$(TARGET))))
