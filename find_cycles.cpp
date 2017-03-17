#include <stdio.h>
#include <stdlib.h>
#include <vector>

#undef PRINT_ADJ_LIST

bool demo_mode = false;
std::vector< std::vector<int> > result;
std::vector<int> res;
bool visited[26] = {0};

char* readCiphertext(const char *file_path, int &out_len)
{
	// Typex version: transfer space to 'X'
    FILE *fp = fopen(file_path, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    int c;
    int len = 0;
    while ((c = fgetc(fp)) != EOF)
    {
        char new_c = (char)c;
        if ((new_c >= 'a' && new_c <= 'z') || (new_c >= 'A' && new_c <= 'Z') || (new_c == ' '))
        {
        	len++;
#ifdef PRINT_ADJ_LIST            
        	printf("(%c, %d)\n", new_c, len);
#endif
        }
    }

    out_len = len;
    char *ctext = (char *)malloc(len * sizeof(char));
    rewind(fp);
    len = 0;
    while ((c = fgetc(fp)) != EOF)
    {
    	char new_c = (char)c;
    	if ((new_c >= 'a' && new_c <= 'z') || (new_c >='A' && new_c <= 'Z') || (new_c == ' '))
    	{
    		if (new_c == ' ') new_c = 'X';
    		ctext[len] = new_c;
    		len++;
    	}
    }

    fclose(fp);

    return ctext;
}

// from an adjacent list to find every possible cycles starts from parent node
bool find_cycle(int parent, int root, std::vector<int> adj_list[26], int init_index, int cycle_threshold)
{
	if (result.size() > cycle_threshold) return false;

	if (root == parent) return true;
	if (parent == -1) parent = root;
	else visited[root] = true;

	const int adj_list_size = (int)adj_list[root].size();
	for (int i = init_index; i < adj_list_size; i++)
	{
		int adj_val = adj_list[root][i];

		if (visited[adj_val] == true) continue;

		res.push_back(root);

		bool found = find_cycle(parent, adj_val, adj_list, 0, cycle_threshold);
		if (found) {res.push_back(parent); result.push_back(res); res.pop_back();}

		res.pop_back();
	}
	visited[root] = false;

	// printf("Result size: %d\n", (int)result.size());

	return false;
}

void construct_cycle(char starting_letter, char *plain, char *cipher, int text_len)
{
	// construct a adjacency list
	std::vector<int> adj_list[26];
	for (int i = 0; i < text_len; i++)
	{
		int s_letter = plain[i] - 65;
		int t_letter = cipher[i] - 65;
		adj_list[s_letter].push_back(t_letter);
		adj_list[t_letter].push_back(s_letter);
	}

#ifdef PRINT_ADJ_LIST
	// print adjacency list
	for (int i = 0; i < 26; i++)
	{
		printf("[%c]: ", i + 65);
		for (int j = 0; j < adj_list[i].size(); j++)
			printf("%c ", adj_list[i][j] + 65);
		printf("\n");
	}
#endif

	int parent = starting_letter - 65;
	for (int i = 0; i < adj_list[parent].size(); i++)
	{
		int num_cycle_threshold = 2 * (i + 1);
		int adj_init_index = i;
		if (demo_mode)
		{
			adj_init_index = 0;
			num_cycle_threshold = 9999;
		}
		bool found = find_cycle(-1, parent, adj_list, adj_init_index, num_cycle_threshold);
	}

	if (result.size() < 1) return;

	// remove "dupclicate cycle"
	// WARNING: the dupclicate here could be in different step i, but we just temporary ignore it
	for (int i = 0; i < result.size() - 1; i++)
	{
		std::vector<int> row = result[i];
		for (int j = i + 1; j < result.size(); j++)
		{
			std::vector<int> row_target = result[j];
			if (!(row == row_target)) break;
			result.erase(result.begin() + j);
			j--;
		}
	}
#ifdef PRINT_ADJ_LIST
	printf("\n\n");
	for (int i = 0; i < result.size(); i++)
	{
		std::vector<int> tmp = result[i];
		int tmp_size = (int)result[i].size();
		if (tmp_size <= 3) continue;
		for (int j = 0; j < tmp_size; j++)
		{
			printf("%c ", tmp[j] + 65);
		}
		printf("\n");
	}
	printf("\n\n");
#endif	
}

void find_pair_write_to_file(char start_letter, char *plain, char *cipher, int text_len, FILE *out_order, FILE *out_ori)
{
    // from each cycle, calculate directions from pairs
    // output:
    //   permu_order: which permutation (Pi) from inChar to outChar
    //   permu_ori: 0 indicates forward, 1 indicates inverse from inChar to outChar
    // example:
    //    index: 0 1 2 3 4 5
    //   inChar: A A A C D E
    //  outChar: B C D B C D
    //  Cycle from 'C': C->B->A->D->C would output
    //  permu_order:  3  0  2  4
    //    permu_ori:  0  1  0  1 
    for (int i = 0; i < result.size(); i++)
    {
        std::vector<int> cycle = result[i];
        int cycle_size = (int)cycle.size();
        if (cycle_size <= 3) continue;

        fprintf(out_order, "%c ", start_letter);
        fprintf(out_ori, "%c ", start_letter);

        int total_permu = cycle_size - 1;
        int *permu_order = new int[total_permu];
        int *permu_ori = new int[total_permu];
        for (int j = 0; j < cycle_size - 1; j++)
        {
            // letter pair
            int letter1 = cycle[j];
            int letter2 = cycle[j + 1];

            for (int k = 0; k < text_len; k++)
            {
                int ori = -1;
                if ((plain[k] - 65) == letter1 && (cipher[k] - 65) == letter2)
                    ori = 0;
                else if ((plain[k] - 65) == letter2 && (cipher[k] - 65) == letter1)
                    ori = 1;

                // pair found
                if (ori != -1)
                {
                    permu_ori[j] = ori;
                    permu_order[j] = k;

                    fprintf(out_order, "%d ", k);
                    fprintf(out_ori, "%d ", ori);
                    break;
                }
            }
        }
        fprintf(out_order, "\n");
        fprintf(out_ori, "\n");

#ifdef PRINT_ADJ_LIST
        for (int j = 0; j < total_permu; j++) printf("(%d, %d) ", permu_order[j], permu_ori[j]);
        printf("\n");
#endif
        delete[] permu_order;
        delete[] permu_ori;
    }    
}

int main(int argc, const char *argv[])
{
	char outfname_order[100];
    char outfname_ori[100];
	if (argc < 5)
	{
		fprintf(stderr, "\n\nUsage: %s infile_in infile_out outfile_order outfile_ori start_letter\n\n", argv[0]);
		fprintf(stderr, "\n\nUsing demo mode...\n\n");
		demo_mode = true;
	}

	int text_len = 0;
	char *plain = NULL;
	char *cipher = NULL;
    char start_letter = 'A';
    bool is_auto_mode = false; // run all letters from A to Z
	if (!demo_mode)
	{
		char infname_in[100];
		char infname_out[100];
		sprintf(infname_in, "%s", argv[1]);
		sprintf(infname_out, "%s", argv[2]);

		sprintf(outfname_order, "%s", argv[3]);
		sprintf(outfname_ori, "%s", argv[4]);

        if (argc > 5) memcpy(&start_letter, argv[5], sizeof(char));
        else is_auto_mode = true;

		int text_in_len, text_out_len;
		text_in_len = text_out_len = 0;
    	plain = readCiphertext(infname_in, text_in_len);
    	cipher = readCiphertext(infname_out, text_out_len);
    	if (text_in_len != text_out_len)
    	{
	    	fprintf(stderr, "\n\n Length of input files are not the same\n\n");
    		exit(0);
    	}					
    	text_len = text_in_len;
	}
	else
	{
		const int demo_input_size = 6;
		char plain_array[demo_input_size] = {'A','A','A','C','D','E'};//{'T','E','S','T','O','F','T','Y','P','E','X','C','I','P','H','E','R','M','A','C','H','I','N','E'};
		char cipher_array[demo_input_size] = {'B','C','D','B','C','D'};//{'P','L','M','A','D','L','I','C','V','B','M','Y','S','U','D','T','L','W','P','R','M','V','V','H'};
		text_len = demo_input_size;
		plain = (char *)malloc(text_len);
		cipher = (char *)malloc(text_len);
		memcpy(plain, plain_array, text_len);
		memcpy(cipher, cipher_array, text_len);
		sprintf(outfname_order, "%s", "demo_permu_order.txt");
		sprintf(outfname_ori, "%s", "demo_permu_ori.txt");
	}

    FILE *out_permu_oder = fopen(outfname_order, "w");
    FILE *out_permu_ori = fopen(outfname_ori, "w");
    if (out_permu_oder == NULL || out_permu_ori == NULL)
    {
        fprintf(stderr, "\n\nError opening file. Try again\n\n");
        return 0;
    }

    if (is_auto_mode)
    {
        for (int i = 0; i < 26; i++)
        {
            char ch = i + 'A';
            construct_cycle(ch, plain, cipher, text_len);
            find_pair_write_to_file(ch, plain, cipher, text_len, out_permu_oder, out_permu_ori);
        }
    }
    else
    {
        construct_cycle(start_letter, plain, cipher, text_len);
        find_pair_write_to_file(start_letter, plain, cipher, text_len, out_permu_oder, out_permu_ori);   
    }

	if (plain)
		free(plain);

	if (cipher)
		free(cipher);

	fclose(out_permu_oder);
	fclose(out_permu_ori);

	return 0;
}