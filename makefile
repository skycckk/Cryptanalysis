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

TARGET = Typex
CC = gcc
CXX = g++
RM = rm -f
SRC = Typex.c

ROTORS = 71625
ORIENTATION = 01010
INIT = ZWABA
IN_FILE = plain.txt
OUT_FILE = cipher.txt

PARAMS = $(ROTORS) $(ORIENTATION) $(INIT) $(IN_FILE) $(OUT_FILE)

.PHONY: all clean run
all: $(SRC)
	@$(CC) $(SRC) -o $(TARGET)

clean:
	@$(RM) $(OBJ)

run:
	@./$(TARGET) $(PARAMS)

rerun: clean all run
