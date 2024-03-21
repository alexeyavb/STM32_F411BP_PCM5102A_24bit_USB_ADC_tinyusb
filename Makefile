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
#####################################
# ST Cube MX Audio
INC += \
lib/STM32_Audio/Addons/SRC236/Inc \
lib/STM32_Audio/Addons/Common/Inc \
lib/STM32_Audio/Addons/SRC441/Inc \

LIBDIR += \
-Llib/STM32_Audio/Addons/SRC236/Lib/ \
-Llib/STM32_Audio/Addons/SRC441/Lib \

LIBS += \
-lSRC236_CM4_GCC \
-lSRC441_CM4_GCC \

#-lSRC236_32b_CM4_GCC \
-lSRC441_32b_CM4_GCC \

#####################################
#####################################
# libresample
# INC += \
  lib/libresample/include/ \
  lib/libresample/src/ \

# LIBDIR += \
-Llib/libresample/ \

# LIBRESAMPLE_SRC = $(wildcard lib/libresample/src/*.c)
# LIBS += \
# -lresample \
#####################################


# board specfic source
EXAMPLE_SOURCE += $(wildcard src/*.c)
# Example source
EXAMPLE_SOURCE += $(wildcard board/src/*.c)
EXAMPLE_SOURCE += $(wildcard drivers/src/*.c)
EXAMPLE_SOURCE += $(LIBRESAMPLE_SRC)

SRC_C += $(addprefix $(CURRENT_PATH)/, $(EXAMPLE_SOURCE))

include ../../build_system/make/rules.mk
