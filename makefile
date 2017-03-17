# Usage: ./Typex rotors orientation init infile outfile
# where rotors == [L][M][R][LS][RS] rotors
#             (0 thru 7, no space, no repeats)
#       orientation == rotor orientations, 0 = forward, 1 = reverse
#             (binary 5-tuple)
#       init == initial position for [L][M][R][LS][RS] rotors
#             (A thru Z, no space)
#       infile == input file name
#
#       outfile == output file name
#
# Encryption example: ./Typex 27016 01100 ZXWAK plain.txt cipher.txt
# Decryption example: ./Typex 27016 01100 ZXWAK cipher.txt plain.txt
#
# Note: Input file must contain only upper case A thru Z. Spaces are OK in plaintext.

CC = gcc
CXX = g++
RM = rm -f
SRC1 = Typex.c
TARGET1 = Typex

ROTORS = 71625
ORIENTATION = 01010
INIT = ZWABA
IN_FILE = plain.txt
OUT_FILE = cipher.txt

SRC2 = compare_cipher.c
TARGET2 = Verify

SRC3 = find_cycles.cpp
TARGET3 = Find_cycles
PERMU_ORDER_FILE = permu_order.txt
PERMU_ORI_FILE = permu_ori.txt
START_LETTER = E

SRC4 = attack.c
TARGET4 = Attack
KEY_OUTPUT = key_output.txt

TARGET_CIPHER_FILE = cipher.txt

PARAMS1 = $(ROTORS) $(ORIENTATION) $(INIT) $(IN_FILE) $(OUT_FILE)
PARAMS2 = $(OUT_FILE) $(TARGET_CIPHER_FILE)
PARAMS3 = $(IN_FILE) $(TARGET_CIPHER_FILE) $(PERMU_ORDER_FILE) $(PERMU_ORI_FILE)
PARAMS4 = $(PERMU_ORDER_FILE) $(PERMU_ORI_FILE) $(KEY_OUTPUT)

RUN_TYPEX = ./$(TARGET1) $(PARAMS1)
RUN_VERIFY = ./$(TARGET2) $(PARAMS2)
RUN_CYCLE = ./$(TARGET3) $(PARAMS3)
RUN_ATTACK = ./$(TARGET4) $(PARAMS4)

.PHONY: all typex verify find_cycle attack clean run
all: typex verify

typex: $(SRC1)
	@$(CC) $(SRC1) -o $(TARGET1)

verify: $(SRC2)
	@$(CC) $(SRC2) -o $(TARGET2)

find_cycle: $(SRC3)
	@$(CXX) $(SRC3) -o $(TARGET3)

attack: $(SRC4)
	@$(CC) $(SRC4) -o $(TARGET4)

clean:
	@$(RM) $(OBJ)

run:
	@clear
	@$(RUN_TYPEX)
	@$(RUN_VERIFY)

runTypex:
	@$(RUN_TYPEX)

runVerify:
	@$(RUN_VERIFY)

runCycle:
	@$(RUN_CYCLE)

runAttack:
	@$(RUN_ATTACK)

rerun: clean all run
