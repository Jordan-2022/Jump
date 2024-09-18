################################################################################
# These are variables for the GBA toolchain build
# You can add others if you wish to
# ***** YOUR NAME HERE *****
################################################################################

# TA-TODO: Put your application name here.
# This should be a just a name i.e MyFirstGBAProgram
# No SPACES AFTER THE NAME.
PROGNAME = App

# TA-TODO: Add the C files you want compiled here (replace extension with .o)
# Here you must put a list of all of the object files
# that will be compiled into your program. For example
# if you have main.c and myLib.c then in the following
# line you would put main.o and myLib.o
OFILES = gba.o font.o main.o images/garbage.o images/mountaint.o images/mountainm.o images/mountainb.o images/left.o images/right.o images/leftjump.o images/rightjump.o images/squish.o images/leftcharge.o images/rightcharge.o images/mm.o screens.o images/difficulty.o images/end.o

################################################################################
# These are various settings used to make the GBA toolchain work
# DO NOT EDIT BELOW.
################################################################################

.PHONY: all
all: CFLAGS += $(CDEBUG) -I../shared
all: LDFLAGS += $(LDDEBUG)
all: $(PROGNAME).gba
	@echo "[FINISH] Created $(PROGNAME).gba"

include /opt/cs2110-tools/GBAVariables.mak

LDFLAGS += --specs=nosys.specs

# Adjust default compiler warnings and errors
CFLAGS += -Wstrict-prototypes -Wold-style-definition -Werror=vla

.PHONY: debug
debug: CFLAGS += $(CDEBUG) -I../shared
debug: LDFLAGS += $(LDDEBUG)
debug: $(PROGNAME).gba
	@echo "[FINISH] Created $(PROGNAME).gba"

$(PROGNAME).gba: $(PROGNAME).elf client
	@echo "[LINK] Linking objects together to create $(PROGNAME).gba"
	@$(OBJCOPY) -O binary $(PROGNAME).elf $(PROGNAME).gba

$(PROGNAME).elf: crt0.o $(GCCLIB)/crtbegin.o $(GCCLIB)/crtend.o $(GCCLIB)/crti.o $(GCCLIB)/crtn.o $(OFILES) libc_sbrk.o
	$(CC) -o $(PROGNAME).elf $^ $(LDFLAGS)

.PHONY: client
client: client.c
	@gcc -o client client.c

.PHONY: mgba
mgba: debug
	@./client
	@echo "[EXECUTE] Running emulator mGBA"

.PHONY: gdb
gdb: debug
	@./client gdb
	@sleep 1
	@echo "[EXECUTE] Attempting to connect to GDB server."
	@echo "          Make sure mGBA is already running on the host machine!"
	@gdb-multiarch -ex 'file $(PROGNAME).elf' -ex 'target remote host.docker.internal:2345'

.PHONY: submit
submit: clean
	@rm -f submission.tar.gz
	@tar --exclude="examples" -czvf submission.tar.gz *

.PHONY: maze
maze: 
	@echo "[EXECUTE] Running emulator mGBA"
	@echo "          Please see emulator.log if this fails"
	@mgba-qt examples/maze/Maze.gba

.PHONY: mario
mario: 
	@echo "[EXECUTE] Running emulator mGBA"
	@echo "          Please see emulator.log if this fails"
	@mgba-qt examples/mario/Mario.gba

.PHONY: pong
pong: 
	@echo "[EXECUTE] Running emulator mGBA"
	@echo "          Please see emulator.log if this fails"
	@mgba-qt examples/pong/Pong.gba

.PHONY: clean
clean:
	@echo "[CLEAN] Removing all compiled files"
	rm -f *.o *.elf *.gba *.log */*.o *.sav */*/*.sav client
