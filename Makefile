# Set this to the name of your program
TARGET = test_prinfo

# Edit this variable to point to all
# of your sources files (make sure
# to put a complete relative path)
#SOURCES = mysrc1.c \
          mysrc2.c

SOURCES = sys_test_prinfo.c
# -----------------------------------------------
# 
# Don't touch things below here unless
# you know what you're doing :-)
# 
OBJECTS = $(SOURCES:%.c=%.o)
INCLUDE = -I.
CFLAGS = -g -O2 -Wall $(INCLUDE) -static
LDFLAGS = -static
CC = arm-none-linux-gnueabi-gcc
LD = arm-none-linux-gnueabi-gcc

default: $(SOURCES) $(TARGET)

$(TARGET): $(OBJECTS)
	@echo [Arm-LD] $@
	@$(LD) $(LDFLAGS) $(OBJECTS) -o $@

%.c.o: %.c
	@echo [Arm-CC] $<...
	@$(CC) -c $(CFLAGS) $< -o $@

clean: .PHONY
	@echo [CLEAN]
	@rm -f $(OBJECTS) $(TARGET)

.PHONY:
