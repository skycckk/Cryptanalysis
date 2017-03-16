#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef int bool;
#define true 1
#define false 0

static int permute_count = 0;

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

void swap(int *x, int i, int j)
{
    int temp = x[i];
    x[i] = x[j];
    x[j] = temp;
}

void permute(int *num, int i, int length, int **res)
{
	if (i == length)
	{
		
		for (int k = 0; k < length; k++)
		{
			res[permute_count][k] = num[k];
		}
		permute_count++;

		return;
	}

	for (int j = i; j < length; j++)
	{
		swap(num, i, j);
		permute(num, i + 1, length, res);
		swap(num, i, j); //backtrack
	}
}            

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

void simulator (int init_L, int init_M, int init_R, int init_LS, int init_RS, int text_len, int ***permu_table)
{
	// permu_table[text_len][26][2]: each step -> letter -> forward/reverse
    int i,
		cnt,
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
        int validate_cnt = 0;
        for (int i = 0; i < 26; i++) // every letters
        {
            inChar = (unsigned char)i;
            outChar = RS_inv[init_RS][LS_inv[init_LS][R_inv[cur_R][M_inv[cur_M][L_inv[cur_L][reflector[L[cur_L][M[cur_M][R[cur_R][LS[init_LS][RS[init_RS][inChar]]]]]]]]]]];    
            
            permu_table[cnt][i][0] = outChar;
            permu_table[cnt][outChar][1] = i; // inverse one

            validate_cnt += outChar;
        }
        if (validate_cnt != 325) {fprintf(stderr, "ERROR!! Duplicated letters\n"); exit(1);}

        cnt++;
    }// end while
    // printf("\n");

}// end simulator

void generate_permutations(int ***all_permu, const int text_len, int *rotor_index, int init_LMS[3], int init_ort[3])
{
	// Initial setting: 
	// rotors index from 0 ~ 7
	int numL = rotor_index[0];
	int numM = rotor_index[1];
	int numR = rotor_index[2];
	int numLS = rotor_index[3];
	int numRS = rotor_index[4];

	// rotors initial position from 0 ~ 25 (A ~ Z)
	int init_L = init_LMS[0];
	int init_M = init_LMS[1];
	int init_R = init_LMS[2];
	int init_LS = 3;
	int init_RS = 4;

	// rotors forword/reverse setting from 0 ~ 1
	ort[0] = init_ort[0];
	ort[1] = init_ort[1];
	ort[2] = init_ort[2];
	ort[3] = 0; // fix
	ort[4] = 0; // fix

	initRotors(numL, numM, numR, numLS, numRS);
	simulator(init_L, init_M, init_R, init_LS, init_RS, text_len, all_permu);
}

int main (int argc, const char *argv[])
{
	if (argc < 3)
	{
		fprintf(stderr, "Usage: %s permu_order_file permu_ori_file\n", argv[0]);
		exit(0);
	}

	// read all cycles and orientation
	FILE *file_permu_order = fopen(argv[1], "r");
	FILE *file_permu_ori = fopen(argv[2], "r");
	char *line = NULL;
    size_t len = 0;
    size_t read;

	// max to 200 cycles
	const int MAX_CYCLE_COUNT = 200;
    int cycle_count = 0;
    int list_step_index_count[MAX_CYCLE_COUNT];
    int *list_step_index_list[MAX_CYCLE_COUNT]; 
    int *list_step_ori_list[MAX_CYCLE_COUNT];

    // read permutation step-index
	while ((read = getline(&line, &len, file_permu_order)) != -1)
    {
        // pre-calculate size
    	char *line_tmp = (char *)malloc(len * sizeof(char));
    	memcpy(line_tmp, line, len * sizeof(char));
        char *pch = strtok(line, " ");
        int step_count1 = 0;
        while (pch != NULL)
        {
        	if (strcmp(pch, "\n") == 0) break;
        	step_count1++;
        	pch = strtok(NULL, " ");
        }
        
        int *step_index = (int *)malloc(step_count1 * sizeof(int));
        pch = strtok(line_tmp, " ");
        step_count1 = 0;
        while (pch != NULL)
        {
        	if (strcmp(pch, "\n") == 0) break;
        	step_index[step_count1++] = atoi(pch);
        	pch = strtok(NULL, " ");
        }
        free(line_tmp);
        list_step_index_list[cycle_count] = step_index;
        list_step_index_count[cycle_count] = step_count1;

        // read permutation orientation
        int step_ori_size = 0;
        if ((read = getline(&line, &len, file_permu_ori)) != -1)
        {
        	char *line_tmp = (char *)malloc(len * sizeof(char));
    		memcpy(line_tmp, line, len * sizeof(char));
        	char *pch = strtok(line, " ");
        	int step_count2 = 0;
        	while (pch != NULL)
        	{
        		if (strcmp(pch, "\n") == 0) break;
        		step_count2++;
        		pch = strtok(NULL, " ");
       		}

       		if (step_count1 != step_count2) { fprintf(stderr, "incorrect file format!\n"); exit(0); }
       		int *step_ori = (int *)malloc(step_count2 * sizeof(int));
       		pch = strtok(line_tmp, " ");
        	step_count2 = 0;
        	while (pch != NULL)
        	{
        		if (strcmp(pch, "\n") == 0) break;
        		step_ori[step_count2++] = atoi(pch);
        		pch = strtok(NULL, " ");
       		}	
       		free(line_tmp);
        	list_step_ori_list[cycle_count] = step_ori;
        }
        cycle_count++;
    }

	int text_len = 200; // total steps (depends on the length of known-plaintext)
	// all_permu[N][26][2]: each step -> each letter -> forword/reverse
	int ***all_permu = (int ***)malloc(text_len * sizeof(int **));
	for (int i = 0; i < text_len; i++)
	{
		all_permu[i] = (int **)malloc(26 * sizeof(int *));
		for (int j = 0; j < 26; j++) 
			all_permu[i][j] = (int *)malloc(2 * sizeof(int));
	}

	// prepare the rotor-choosen: (8 choose 3): total = 8 * 7 * 6 = 336
	const int num_rotors = 8;
	int rotor_index[num_rotors];
    for (int i = 0; i < num_rotors; i++) rotor_index[i] = i;

	int total_permu = 1;
	for (int i = 1; i <= num_rotors; i++) total_permu *= i;
	int **permu_buf = (int **)malloc(total_permu * sizeof(int *));
	for (int i = 0; i < total_permu; i++)
	{
		int *tmp_permu_buf = (int *)malloc(num_rotors * sizeof(int));
		permu_buf[i] = tmp_permu_buf;
	}
	permute(rotor_index, 0, num_rotors, permu_buf);

    // prepare the initial orientation: total = 2 ^ 3 = 8
	int init_ort[8][3] = {{0,0,0}, {0,0,1}, {0,1,0}, {0,1,1},
						  {1,0,0}, {1,0,1}, {1,1,0}, {1,1,1}};

    // 8 choose 3 with permutation
	for (int rot_i = 0; rot_i < total_permu; rot_i++)
	{	
		if (rot_i % 120 != 0) continue;

		for (int i = 0; i < 3; i++) printf("%d ", permu_buf[rot_i][i]);
		printf(" -> rot_i = %d\n", rot_i);

        // all possible initial letter: total = 26 * 26 * 26 = 2 ^ 14
        fprintf(stderr, "rotor_i0:");
		for (int rotor_i0 = 0; rotor_i0 < 26; rotor_i0++)
		{
            fprintf(stderr, " %d", rotor_i0);
			for (int rotor_i1 = 0; rotor_i1 < 26; rotor_i1++)
			{
				for (int rotor_i2 = 0; rotor_i2 < 26; rotor_i2++)
				{
                    // possible orientation
                    // clock_t start_t = clock();
					for (int ort_i = 0; ort_i < 8; ort_i++)
					{
						int init_LMS[3] = {0};
						init_LMS[0] = rotor_i0;
						init_LMS[1] = rotor_i1;
						init_LMS[2] = rotor_i2;

                        // kernal part: this will go through all settings
                        // total work = 336 * 8 * 2 ^ 14 = 2 ^ 25.4
						generate_permutations(all_permu, text_len, permu_buf[rot_i], init_LMS, init_ort[ort_i]);
						for (int i = 0; i < 26; i++) // each guessing pair letter
						{
							bool cycle_has_hold = true;
							for (int j = 0; j < 10; j++) // each cycle
							{
								int next = i;
					    		int cc = list_step_index_count[j];
		    					if (cc == 0) break;

		    					int *step_index_list = list_step_index_list[j];
		    					int *step_ori_list = list_step_ori_list[j];

		    					for (int k = 0; k < cc; k++)
		    					{
		    						int step = step_index_list[k];
		    						int step_ori = step_ori_list[k];
		    						next = all_permu[step][next][step_ori];
		    					}
		    					if (next != i) cycle_has_hold = cycle_has_hold & false;
							}

							if (cycle_has_hold)
							{
								printf("************************************\n");
								printf("Putative rotor setting was found! %c\n", (i + 65));	
								printf("rot_index:");
								for (int k = 0; k < 5; k++) printf(" %d", permu_buf[rot_i][k]);
								printf("\ninit_ort:");
								for (int k = 0; k < 3; k++) printf(" %d", init_ort[ort_i][k]);
								printf("\n");
								printf("init_LMS = (%d, %d, %d)\n", rotor_i0, rotor_i1, rotor_i2);
							} 
						}	
					}	
                    // clock_t end_t = clock();
                    // double total_t = (double)(end_t - start_t) / (CLOCKS_PER_SEC / 1000.0);
                    // printf("Total time taken by CPU: %f\n", total_t);	
				}
			}	
		}
        fprintf(stderr, "\n");
	}				

	for (int i = 0; i < total_permu; i++) free(permu_buf[i]);
	free(permu_buf);	

    for (int i = 0; i < MAX_CYCLE_COUNT; i++)
    {
    	int cc = list_step_index_count[i];
    	if (cc == 0) break;

    	int *step_index_list = list_step_index_list[i];
    	int *step_ori_list = list_step_ori_list[i];

    	free(step_index_list);
    	free(step_ori_list);
    }	

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

	return 0;
}
