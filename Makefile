.SUFFIXES:

BUILD_DIR ?= ./build

werrors += -Wall -Wextra -Werror
werrors += -Wformat=2
werrors += -Wno-null-pointer-arithmetic
cflags += -Os
cflags += -static
cflags += $(werrors)

init = $(BUILD_DIR)/init

init: # build the init binary
init: $(init)
.PHONY: init

$(init): init.c
		@mkdir -p $(BUILD_DIR)
		@rm -f $(init)
		@gcc $(cflags) -o $@ $<
		@strip -s $@


.PHONY: clean
clean: # clean the build directory
		@rm -rf $(BUILD_DIR)
