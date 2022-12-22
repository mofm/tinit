.DEFAULT_GOAL:=init

.SUFFIXES:

BUILD_DIR ?= ./build

werrors += -Wall -Wextra -Werror
werrors += -Wformat=2
werrors += -Wno-null-pointer-arithmetic
cflags += -Os
cflags += -static
cflags += $(werrors)

init = $(BUILD_DIR)/init

##@ Build
init: ## build the init binary
init: $(init)
.PHONY: init

$(init): src/init.c
		@mkdir -p $(BUILD_DIR)
		@rm -f $(init)
		@gcc $(cflags) -o $@ $<
		@strip -s $@

##@ Clean
.PHONY: clean
clean: ## clean the build directory
		@rm -rf $(BUILD_DIR)

##@ Help
.PHONY: help
help: ## help
		@echo "Usage: make [target]"
		@echo
		@echo "Targets:"
		@fgrep -h "##" $(MAKEFILE_LIST) | fgrep -v fgrep | sed -e 's/\\$$//' | sed -e 's/##//'
