/*
  Program to simulate Typex cipher
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Print flags
// print rotors, reflector, and stator permutations
//#define PR_PERMS
// print output for each step of encryption/decryption
//#define PR_STEPS
// print the key
//#define PR_KEY
// print rotor stepping
//#define PR_ROTORS


FILE *in,
     *out;

int Lrotor[26],
    Mrotor[26],
    Rrotor[26],
    Lstator[26],
    Rstator[26],
    reflector[26],
    notch[9],
    ort[5];

int L[26][26],
    M[26][26],
    R[26][26],
    L_inv[26][26],
    M_inv[26][26],
    R_inv[26][26],
    LS[26][26],
    RS[26][26],
    LS_inv[26][26],
    RS_inv[26][26];

char letter[26] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ",
     rot[8][26] = {"QWECYJIBFKMLTVZPOHUDGNRSXA",// 8 rotors to choose from
                   "AJDKSIRUXBLHWTMCQGZNPYFVOE",
                   "BDFHJLCPRTXVZNYEIWGAKMUSQO",
                   "ESOVPZJAYQUIRHXLNFTGKDCMWB",
                   "VZBRGITYUPSDNHLXAWMJQOFECK",
                   "FVPJIAOYEDRZXWGCTKUQSBNMHL",
                   "KZGLIUCJEHADXRYWVTNSFQPMOB",
                   "ZLVGOIFTYWUEPMABNCXRQSDKHJ"},
     ref[26] = "YRUHQSLDPXNGOKMIEBFZCWVJAT",// only 1 reflector
     step[9] = "ACEINQTVY";// 9 notches on each rotor (same positions on all rotors)


int stepRotor (int pos, int n, int rev) {
    int t;
    if (rev == 1) {
        t = pos - 1;
        if (t < 0) {
            t = 25;
        }
    }
    else {
        t = pos + 1;
        if (t >= n) {
            t = 0;
        }
    }
    return(t);

}// end stepRotor

void reverse (int x) {
    int i,
        j,
        **newrotor;

    newrotor = (int**) calloc(26, sizeof(int*));
    for (i = 0; i < 26; ++i) {
        newrotor[i] = (int*) calloc(26, sizeof(int));
    }

    // reverse Left rotor
    if (x == 0) {
        for (i = 0; i < 26; ++i) {
            for (j = 0; j < 26; ++j) {
                newrotor[i][j] = L[i][(26 - j) % 26];
            }
        }
        for (i = 0; i < 26; ++i) {
            for (j = 0; j < 26; ++j) {
                L[i][j] = newrotor[i][j];
            }
        }
    }
    // reverse Middle rotor
    if (x == 1) {
        for (i = 0; i < 26; ++i) {
            for (j = 0; j < 26; ++j) {
                newrotor[i][j] = M[i][(26 - j) % 26];
            }
        }
        for (i = 0; i < 26; ++i) {
            for (j = 0; j < 26; ++j) {
                M[i][j] = newrotor[i][j];
            }
        }
    }
    // reverse Right rotor
    if (x == 2) {
        for (i = 0; i < 26; ++i) {
            for (j = 0; j < 26; ++j) {
                newrotor[i][j] = R[i][(26 - j) % 26];
            }
        }
        for (i = 0; i < 26; ++i) {
            for (j = 0; j < 26; ++j) {
                R[i][j] = newrotor[i][j];
            }
        }
    }
    // reverse Left Stator
    if (x == 3) {
        for (i = 0; i < 26; ++i) {
            for (j = 0; j < 26; ++j) {
                newrotor[i][j] = LS[i][(26 - j) % 26];
            }
        }
        for (i = 0; i < 26; ++i) {
            for (j = 0; j < 26; ++j) {
                LS[i][j] = newrotor[i][j];
            }
        }
    }
    // reverse Right Stator
    if (x == 4) {
        for (i = 0; i < 26; ++i) {
            for (j = 0; j < 26; ++j) {
                newrotor[i][j] = RS[i][(26 - j) % 26];
            }
        }
        for (i = 0; i < 26; ++i) {
            for (j = 0; j < 26; ++j) {
                RS[i][j] = newrotor[i][j];
            }
        }
    }

    for (i = 0; i < 26; ++i) {
        free(newrotor[i]);
    }
    free(newrotor);

}// end reverse

void getInversePerm (int invPerm[], int perm[], int n) {
    int i;

    for (i = 0; i < n; ++i) {
        invPerm[perm[i]] = i;
    }// next i

}// end getInversePerm

void initRotors (int numL, int numM, int numR, int numLS, int numRS) {
    int i,
        j;

    // initialize rotor and reflector arrays
    for (i = 0; i < 26; ++i) {
        Lrotor[i] = (int)rot[numL][i] - 65;
        Mrotor[i] = (int)rot[numM][i] - 65;
        Rrotor[i] = (int)rot[numR][i] - 65;
        Lstator[i] = (int)rot[numLS][i] - 65;
        Rstator[i] = (int)rot[numRS][i] - 65;
        reflector[i] = (int)ref[i] - 65;

    }// next i
    // sets initial permutation
    for (i = 0; i < 26; ++i) {
        L[0][i] = Lrotor[i];
        M[0][i] = Mrotor[i];
        R[0][i] = Rrotor[i];
        LS[0][i] = Lstator[i];
        RS[0][i] = Rstator[i];
    }// next i
    // sets permutation for all other letters
    for (i = 1; i < 26; ++i) {
        for (j = 0; j < 26; ++j) {
            L[i][j] = (Lrotor[(i + j) % 26] + 26 - i) % 26;
            M[i][j] = (Mrotor[(i + j) % 26] + 26 - i) % 26;
            R[i][j] = (Rrotor[(i + j) % 26] + 26 - i) % 26;
            LS[i][j] = (Lstator[(i + j) % 26] + 26 - i) % 26;
            RS[i][j] = (Rstator[(i + j) % 26] + 26 - i) % 26;
        }// next j
    }// next i
    // reverse rotors
    for (i = 0; i < 5; ++i) {
        if (ort[i] == 1) {
            reverse(i);
        }
    }

    // find inverse permutation
    for (i = 0; i < 26; ++i) {
        getInversePerm(L_inv[i], L[i], 26);
        getInversePerm(M_inv[i], M[i], 26);
        getInversePerm(R_inv[i], R[i], 26);
        getInversePerm(LS_inv[i], LS[i], 26);
        getInversePerm(RS_inv[i], RS[i], 26);
    }// next i

    // initialize notches
    for (i = 0; i < 9; ++i) {
        notch[i] = (int)step[i] - 65;
    }
}// end initRotors


void simulator (int init_L, int init_M, int init_R, int init_LS, int init_RS) {
    int i,
		cnt,
        space,
        temp,
        cur_L,
        cur_M,
        cur_R,
        stepR,
        stepM,
        stepL;

    unsigned char inChar,
                  outChar;

    cur_L = init_L;
    cur_M = init_M;
    cur_R = init_R;
    stepR = 0;
    stepM = 0;
    stepL = 0;

	cnt = 0;
    // encryption/decryption and stepping part
    while (1) {
        temp = fgetc(in);
//		printf("plain[%d] = %c\n", cnt, temp);
		++cnt;
        if (temp == EOF) {
            break;
        }
        space = temp;
        temp -= 65;
        space -= 32;
		// convert space to X
        if (space == 0) {
            temp = 23;
        }
		// validate input
        if (temp < 0 || temp > 25) {
            fprintf(stderr, "\nError (%d) --- all input characters must be upper case A thru Z or a space\n", cnt);
            exit(0);
        }

        inChar = (unsigned char)temp;

        // R is fast rotor
        // M is medium rotor
        // L is slow rotor
        // rotors step _before_ encryption/decryption
        // check all possible notches
        for (i = 0; i < 9; ++i) {
            // all 3 step (step left and middle here)
            if (cur_M == notch[i]) {
                if (ort[0] == 1) {
                    cur_L = stepRotor(cur_L, 26, 1);
                }
                else {
                    cur_L = stepRotor(cur_L, 26, 0);
                }
                if (ort[1] == 1) {
                    cur_M = stepRotor(cur_M, 26, 1);
                }
                else {
                    cur_M = stepRotor(cur_M, 26, 0);
                }
                stepL++;
                stepM++;
            }
            else {
                // M and R both step (step middle here)
                if (cur_R == notch[i]) {
                    if (ort[1] == 1) {
                        cur_M = stepRotor(cur_M, 26, 1);
                    }
                    else {
                        cur_M = stepRotor(cur_M, 26, 0);
                    }
                    stepM++;
                }
            }
        }
        // step right (fast) rotor --- always steps
        if (ort[2] == 1) {
            cur_R = stepRotor(cur_R, 26, 1);
        }
        else {
            cur_R = stepRotor(cur_R, 26, 0);
        }
        stepR++;

        // Typex transformation
#ifdef PR_ROTORS
        printf("[%d]", cur_R);
        printf("[%d]", cur_M);
        printf("[%d]\n", cur_L);
#endif

#ifdef PR_STEPS
        printf("\ninChar = %c\n", letter[inChar]);
        printf("RS[%d][%d] = ", init_RS, inChar);
        temp = RS[init_RS][inChar];
        printf("%c\n", letter[temp]);
        printf("LS[%d][%d] = ", init_LS, temp);
        temp = LS[init_LS][temp];
        printf("%c\n", letter[temp]);
        printf("R[%d][%d] = ", cur_R, temp);
        temp = R[cur_R][temp];
        printf("%c\n", letter[temp]);
        printf("M[%d][%d] = ", cur_M, temp);
        temp = M[cur_M][temp];
        printf("%c\n", letter[temp]);
        printf("L[%d][%d] = ", cur_L, temp);
        temp = L[cur_L][temp];
        printf("%c\n", letter[temp]);
        printf("reflector[%d] = ", temp);
        temp = reflector[temp];
        printf("%c\n", letter[temp]);
        printf("L_inv[%d][%d] = ", cur_L, temp);
        temp = L_inv[cur_L][temp];
        printf("%c\n", letter[temp]);
        printf("M_inv[%d][%d] = ", cur_M, temp);
        temp = M_inv[cur_M][temp];
        printf("%c\n", letter[temp]);
        printf("R_inv[%d][%d] = ", cur_R, temp);
        temp = R_inv[cur_R][temp];
        printf("%c\n", letter[temp]);
        printf("LS_inv[%d][%d] = ", init_LS, temp);
        temp = LS_inv[init_LS][temp];
        printf("%c\n", letter[temp]);
        printf("RS_inv[%d][%d] = ", init_RS, temp);
        temp = RS_inv[init_RS][temp];
        printf("%c\n", letter[temp]);
#endif
		// encryption/decryption result
        outChar = RS_inv[init_RS][LS_inv[init_LS][R_inv[cur_R][M_inv[cur_M][L_inv[cur_L][reflector[L[cur_L][M[cur_M][R[cur_R][LS[init_LS][RS[init_RS][inChar]]]]]]]]]]];

#ifdef PR_STEPS
        printf("letter[temp] = %c, letter[outChar] = %c\n", letter[temp], letter[outChar]);
#endif

        printf("%c", letter[outChar]);
        fprintf(out, "%c", letter[outChar]);
    }// end while
    printf("\n\n");

#ifdef PR_ROTORS
    printf("stepR = %d\n", stepR);
    printf("stepM = %d\n", stepM);
    printf("stepL = %d\n", stepL);
#endif
}// end simulator


int main (int argc, const char *argv[]) {
    int i,
        j,
        n,
        init_L,
        init_M,
        init_R,
        init_LS,
        init_RS,
        numL,
        numM,
        numR,
        numLS,
        numRS;

    char infname[100],
         outfname[100];

    if (argc != 6) {
oops:   fprintf(stderr, "\n\nUsage: %s rotors orientation init infile outfile\n\n",
                argv[0]);
        fprintf(stderr, "where rotors == [L][M][R][LS][RS] rotors\n            (0 thru 7, no space, no repeats)\n");
        fprintf(stderr, "      orientation == rotor orientations, 0 = forward, 1 = reverse\n");
        fprintf(stderr,"            (binary 5-tuple)\n");
        fprintf(stderr, "      init == initial position for [L][M][R][LS][RS] rotors\n");
        fprintf(stderr,"            (A thru Z, no space)\n");
        fprintf(stderr, "      infile == input file name\n\n");
        fprintf(stderr, "      outfile == output file name\n\n");
        fprintf(stderr, "Encryption example: %s 27016 01100 ZXWAK plain.txt cipher.txt\n\n", argv[0]);
        fprintf(stderr, "Decryption example: %s 27016 01100 ZXWAK cipher.txt plain.txt\n\n", argv[0]);
        fprintf(stderr, "Note: Input file must contain only upper case A thru Z. Spaces are OK in plaintext.\n\n");
        exit(0);
    }

    if (strlen(argv[1]) != 5) {
        fprintf(stderr, "\nError --- must specify 5 rotors\n");
        goto oops;
    }
    numL = argv[1][0] - 48;
    if (numL < 0 || numL > 7) {
        fprintf(stderr, "\nError --- left rotor must be 0 thru 7\n");
        goto oops;
    }
    numM = argv[1][1] - 48;
    if (numM < 0 || numM > 7) {
        fprintf(stderr, "\nError --- middle rotor must be 0 thru 7\n");
        goto oops;
    }
    numR = argv[1][2] - 48;
    if (numR < 0 || numR > 7) {
        fprintf(stderr, "\nError --- right rotor must be 0 thru 7\n");
        goto oops;
    }
    numLS = argv[1][3] - 48;
    if (numLS < 0 || numLS > 7) {
        fprintf(stderr, "\nError --- left stator must be 0 thru 7\n");
        goto oops;
    }
    numRS = argv[1][4] - 48;
    if (numRS < 0 || numRS > 7) {
        fprintf(stderr, "\nError --- right stator must be 0 thru 7\n");
        goto oops;
    }
    if ((numL == numM) || (numL == numR) || (numL == numLS) || (numL == numRS) || (numM == numR) || (numM == numLS) || (numM == numRS) || (numR == numLS) || (numR == numRS) || (numLS == numRS))  {
        fprintf(stderr, "\nError --- rotors must be distinct\n");
        goto oops;
    }

    // orientation check
    if (strlen(argv[2]) != 5) {
        fprintf(stderr, "\nError --- rotor orientation must be 5 characters, each 0 or 1\n");
        goto oops;
    }
    for (i = 0; i < 5; ++i) {
        ort[i] = argv[2][i] - 48;
        if (ort[i] < 0 || ort[i] > 1) {
            fprintf(stderr, "\nError --- rotor orientation must be 5 characters, each 0 or 1\n");
            goto oops;
        }
    }

    // init input check
    if (strlen(argv[3]) != 5) {
        fprintf(stderr, "\nError --- init must be five characters, each upper case A thru Z\n");
        goto oops;
    }
    init_L = argv[3][0] - 65;
    if (init_L < 0 || init_L > 25) {
        fprintf(stderr, "\nError --- init must be five characters, each upper case A thru Z\n");
        goto oops;
    }
    init_M = argv[3][1] - 65;
    if (init_M < 0 || init_M > 25) {
        fprintf(stderr, "\nError --- init must be five characters, each upper case A thru Z\n");
        goto oops;
    }
    init_R = argv[3][2] - 65;
    if (init_R < 0 || init_R > 25) {
        fprintf(stderr, "\nError --- init must be five characters, each upper case A thru Z\n");
        goto oops;
    }
    init_LS = argv[3][3] - 65;
    if (init_LS < 0 || init_LS > 25) {
        fprintf(stderr, "\nError --- init_s must be a five characters, each upper case A thru Z\n");
        goto oops;
    }
    init_RS = argv[3][4] - 65;
    if (init_RS < 0 || init_RS > 25) {
        fprintf(stderr, "\nError --- init must be a five characters, each upper case A thru Z\n");
        goto oops;
    }

    // filename input check
    sprintf(infname, "%s", argv[4]);
    in = fopen(infname, "r");
    if (in == NULL) {
        fprintf(stderr, "\n\nError opening file %s\nTry again\n\n", infname);
        goto oops;
    }

    // filename output check
    sprintf(outfname, "%s", argv[5]);
    out = fopen(outfname, "w");
    if (out == NULL) {
        fprintf(stderr, "\n\nError opening file %s\nTry again\n\n", outfname);
        goto oops;
    }

    initRotors(numL, numM, numR, numLS, numRS);

#ifdef PR_PERMS
    printf("L perms\n");
    for (i = 0; i < 26; ++i) {
        printf("L[%2d] = ", i);
        for (j = 0; j < 26; ++j) {
            printf("%c", letter[L[i][j]]);
        }
        printf("\n");
    }
    printf("\n");
    printf("M perms\n");
    for (i = 0; i < 26; ++i) {
        printf("M[%2d] = ", i);
        for (j = 0; j < 26; ++j) {
            printf("%c", letter[M[i][j]]);
        }
        printf("\n");
    }
    printf("\n");
    printf("R perms\n");
    for (i = 0; i < 26; ++i) {
        printf("R[%2d] = ", i);
        for (j = 0; j < 26; ++j) {
            printf("%c", letter[R[i][j]]);
        }
        printf("\n");
    }
    printf("\n");
    printf("LS perms\n");
    for (i = 0; i < 26; ++i) {
        printf("LS[%2d] = ", i);
        for (j = 0; j < 26; ++j) {
            printf("%c", letter[LS[i][j]]);
        }
        printf("\n");
    }
    printf("\n");
    printf("RS perms\n");
    for (i = 0; i < 26; ++i) {
        printf("RS[%2d] = ", i);
        for (j = 0; j < 26; ++j) {
            printf("%c", letter[RS[i][j]]);
        }
        printf("\n");
    }
    printf("\n");
    printf("L_inv perms\n");
    for (i = 0; i < 26; ++i) {
        printf("L_inv[%2d] = ", i);
        for (j = 0; j < 26; ++j) {
            printf("%c", letter[L_inv[i][j]]);
        }
        printf("\n");
    }
    printf("\n");
    printf("M_inv perms\n");
    for (i = 0; i < 26; ++i) {
        printf("M_inv[%2d] = ", i);
        for (j = 0; j < 26; ++j) {
            printf("%c", letter[M_inv[i][j]]);
        }
        printf("\n");
    }
    printf("\n");
    printf("R_inv perms\n");
    for (i = 0; i < 26; ++i) {
        printf("R_inv[%2d] = ", i);
        for (j = 0; j < 26; ++j) {
            printf("%c", letter[R_inv[i][j]]);
        }
        printf("\n");
    }
    printf("\n");
    printf("LS_inv perms\n");
    for (i = 0; i < 26; ++i) {
        printf("LS_inv[%2d] = ", i);
        for (j = 0; j < 26; ++j) {
            printf("%c", letter[LS_inv[i][j]]);
        }
        printf("\n");
    }
    printf("\n");
    printf("RS_inv perms\n");
    for (i = 0; i < 26; ++i) {
        printf("RS_inv[%2d] = ", i);
        for (j = 0; j < 26; ++j) {
            printf("%c", letter[RS_inv[i][j]]);
        }
        printf("\n");
    }
    printf("\n");
    printf("reflector\n");
    for (i = 0; i < 26; ++i) {
        printf("%c", letter[reflector[i]]);
    }
    printf("\n\n");
#endif

#ifdef PR_KEY
    printf("\nKey:\n");
    printf("rotors (L,M,R,LS,RS) = (%d,%d,%d,%d,%d)\n", numL, numM, numR, numLS, numRS);
    printf("initial rotor positions (L,M,R,LS,RS) = (%c,%c,%c,%c,%c)\n",
        (char)(init_L + 65), (char)(init_M + 65), (char)(init_R + 65), (char)(init_LS + 65), (char)(init_RS + 65));
    printf("\nOutput:\n");
#endif

    simulator(init_L, init_M, init_R, init_LS, init_RS);

    fclose(in);
    fclose(out);

}// end main
