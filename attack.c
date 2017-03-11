#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
                   "FVPJIAOYEDRZXWGCTKUQSBNMHL", // FVPJIAOYEDRZXWGCTKUQSBNMHL
                   "KZGLIUCJEHADXRYWVTNSFQPMOB",
                   "ZLVGOIFTYWUEPMABNCXRQSDKHJ"},
     ref[26] = "YRUHQSLDPXNGOKMIEBFZCWVJAT",// only 1 reflector
     step[9] = "ACEINQTVY";// 9 notches on each rotor (same positions on all rotors)    

     // Generate a initial setting for P0
     // Go through to Pn (also keep the inverse one as well) with A-Z as input
     // e.g. We have P0 P1 P2 P3 P4 P5 P6 P7
     //      each one has 26 input and 26 * 2 output (forward/reverse)
     //      Table size: N * 26 * 2
     // NOTE: different initial setting has different output table mapping

     // Todo rotor-attacking:
     // S(C) == K ---->>> P2 P4 P3 P2- P4- P5 P7-  <<<--- S(C) == K
     // For each initial setting from setting pools
     //   For each guess (S(C) == K?)
     //     For each cycles:
     //       Table loop-up by P0~Pn with corresponding input/output
     //     if all equations hold, keep the setting
     //     else remove this setting from pools              

int stepRotor (int pos, int n, int rev)
{
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

void reverse (int x)
{
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

void getInversePerm (int invPerm[], int perm[], int n)
{
    int i;

    for (i = 0; i < n; ++i) {
        invPerm[perm[i]] = i;
    }// next i

}// end getInversePerm

void initRotors (int numL, int numM, int numR, int numLS, int numRS)
{
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

void simulator (int init_L, int init_M, int init_R, int init_LS, int init_RS, int ch, int text_len, int ***permu_table)
{
	// permu_table[text_len][26][2]: each step -> letter -> forward/reverse
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
    // following is the permutation of each step i to text_len steps
    while (cnt < text_len) {
        temp = ch;

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
		// encryption/decryption result
        // Reflector <- L <- M <- R <- LS <- RS <- inChar
        // Reflector -> L -> M -> R -> LS -> RS -> outChar
        outChar = RS_inv[init_RS][LS_inv[init_LS][R_inv[cur_R][M_inv[cur_M][L_inv[cur_L][reflector[L[cur_L][M[cur_M][R[cur_R][LS[init_LS][RS[init_RS][inChar]]]]]]]]]]];
        permu_table[cnt][temp][0] = outChar;
        // printf("%c", letter[outChar]);

		outChar = RS[init_RS][LS[init_LS][R[cur_R][M[cur_M][L[cur_L][reflector[L_inv[cur_L][M_inv[cur_M][R_inv[cur_R][LS_inv[init_LS][RS_inv[init_RS][inChar]]]]]]]]]]];        
		permu_table[cnt][temp][1] = outChar;
        // printf("%c", letter[outChar]);

        cnt++;
    }// end while
    // printf("\n");

}// end simulator

void generate_permutations()
{
	// Initial setting: 
	// rotors index from 0 ~ 7
	int numL = 0;
	int numM = 1;
	int numR = 2;
	int numLS = 3;
	int numRS = 4;

	// rotors initial position from 0 ~ 25 (A ~ Z)
	int init_L = 0;
	int init_M = 1;
	int init_R = 2;
	int init_LS = 3;
	int init_RS = 4;

	// rotors forword/reverse setting from 0 ~ 1
	ort[0] = 0;
	ort[1] = 0;
	ort[2] = 0;
	ort[3] = 0;
	ort[4] = 0;

	// text
	int text_len = 60; // total steps (depends on the length of known-plaintext)
	// all_permu[N][26][2]: each step -> each letter -> forword/reverse
	int ***all_permu = (int ***)malloc(text_len * sizeof(int **));
	for (int i = 0; i < text_len; i++)
	{
		all_permu[i] = (int **)malloc(26 * sizeof(int *));
		for (int j = 0; j < 26; j++) 
			all_permu[i][j] = (int *)malloc(2 * sizeof(int));
	}

	initRotors(numL, numM, numR, numLS, numRS);

	for (int i = 0; i < 26; i++) // all letters
		simulator(init_L, init_M, init_R, init_LS, init_RS, letter[i], text_len, all_permu);
	
	for (int i = 0; i < text_len; i++)
	{
		for (int j = 0; j < 26; j++)
		{
			free(all_permu[i][j]);
			all_permu[i][j] = NULL;
		}
		free(all_permu[i]);
		all_permu[i] = NULL;
	}
	free(all_permu);
	all_permu = NULL;	
}

int main (int argc, const char *argv[])
{
	generate_permutations();

	return 0;
}
