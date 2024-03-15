include ../../build_system/make/make.mk

INC += \
  inc \
  $(TOP)/hw \
# board specfic includes
INC += \
  board/inc \
#driver specific src
INC += \
drivers/inc \


# board specfic source
EXAMPLE_SOURCE += $(wildcard src/*.c)
# Example source
EXAMPLE_SOURCE += $(wildcard board/src/*.c)
EXAMPLE_SOURCE += $(wildcard drivers/src/*.c)

SRC_C += $(addprefix $(CURRENT_PATH)/, $(EXAMPLE_SOURCE))

include ../../build_system/make/rules.mk
