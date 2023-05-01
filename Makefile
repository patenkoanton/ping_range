CPP = g++
STD = c++14
CPPFLAGS = -Wall -g3 --std=$(STD) -pthread
CPPFLAGS += -Isrc/common
INSTALL_DIR = /usr/bin

# IMPORTANT: Each target to be listed here
PING_SUBNET = ping_subnet
PING_SUBNET_GUI = ping_subnet_gui
TARGETS = \
	$(PING_SUBNET) \
	$(PING_SUBNET_GUI) \

# IMPORTANT: Each target needs to have a similar variable: [target_name]_DEPS
$(PING_SUBNET)_DEPS = $(filter %.o, $(patsubst src/%.cpp, build/$(PING_SUBNET)/%.o, $(wildcard src/$(PING_SUBNET)/* src/common/*)))
$(PING_SUBNET)_CPPFLAGS = -Isrc/$(PING_SUBNET)

$(PING_SUBNET_GUI)_DEPS = $(filter %.o, $(patsubst src/%.cpp, build/$(PING_SUBNET_GUI)/%.o, $(wildcard src/$(PING_SUBNET_GUI)/* src/common/*)))
$(PING_SUBNET_GUI)_CPPFLAGS = `wx-config --cxxflags` `wx-config --libs`
$(PING_SUBNET_GUI)_CPPFLAGS += -Isrc/$(PING_SUBNET_GUI)

.PHONY: all clean install uninstall

all: $(TARGETS)

# Generate target-specific rules.
define BUILD_TARGET

$(1): build/$(1)/bin/$(1)

build/$(1)/bin/$(1): $$($(1)_DEPS)
	@echo "\n------Finishing build for [$(1)]------"
	@mkdir -p $$(dir $$@)
	$(CPP) $$^ -o $$@ $(CPPFLAGS) $$($(1)_CPPFLAGS)

build/$(1)/%.o: $$(addprefix src/, %.cpp %.h)
	@echo "\n------Building $$(notdir $$<) for [$(1)]------"
	@mkdir -p $$(dir $$@)
	$(CPP) -c $$< -o $$@ $(CPPFLAGS) $$($(1)_CPPFLAGS)

endef
# WARNING: dont add space after BUILD_TARGET's comma otherwise everything will break!
$(foreach TARGET, $(TARGETS), $(eval $(call BUILD_TARGET,$(TARGET))))

clean:
	rm -rf build

# Install / uninstall.
BINARIES = $(foreach TARGET, $(TARGETS), $(wildcard build/$(TARGET)/bin/*))
install: $(addprefix install_, $(BINARIES))
uninstall: $(addprefix uninstall_, $(BINARIES))

define INSTALL_BINARY
install_$(1):
	@echo "Installing $(notdir $(1))..."
	@sudo setcap CAP_NET_RAW=eip $(1)
	@sudo ln -f $(1) $(INSTALL_DIR)/$(notdir $(1))

uninstall_$(1):
	@echo "Uninstalling $(1)..."
	@sudo rm -f $(INSTALL_DIR)/$(notdir $(1))
endef
$(foreach BINARY, $(BINARIES), $(eval $(call INSTALL_BINARY,$(BINARY))))
