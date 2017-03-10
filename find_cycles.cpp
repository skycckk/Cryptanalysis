#include <stdio.h>
#include <stdlib.h>
#include <vector>

#define PRINT_ADJ_LIST

std::vector< std::vector<int> > result;
std::vector<int> res;
bool visited[26] = {0};

// from an adjacent list to find every possible cycles starts from parent node
bool find_cycle(int parent, int root, std::vector<int> adj_list[26])
{
	if (root == parent) return true;
	if (parent == -1) parent = root;
	else visited[root] = true;

	const int adj_list_size = (int)adj_list[root].size();
	for (int i = 0; i < adj_list_size; i++)
	{
		int adj_val = adj_list[root][i];

		if (visited[adj_val] == true) continue;

		res.push_back(root);

		bool found = find_cycle(parent, adj_val, adj_list);
		if (found) {res.push_back(parent); result.push_back(res); res.pop_back();}

		res.pop_back();
	}
	visited[root] = false;

	return false;
}

int main(int argc, const char *argv[])
{
	if (argc < 3)
	{
		fprintf(stderr, "\n\nUsage: %s outfile_order outfile_ori\n\n", argv[0]);
		exit(0);
	}
	const int text_len = 6;
	char plain[text_len] = {'A','A','A','C','D','E'};//{'T','E','S','T','O','F','T','Y','P','E','X','C','I','P','H','E','R','M','A','C','H','I','N','E'};
	char cipher[text_len] = {'B','C','D','B','C','D'};//{'P','L','M','A','D','L','I','C','V','B','M','Y','S','U','D','T','L','W','P','R','M','V','V','H'};

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

	int parent = 'C' - 65;
	bool found = find_cycle(-1, parent, adj_list);	

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
    char outfname_order[100];
    char outfname_ori[100];
	sprintf(outfname_order, "%s", argv[1]);
	sprintf(outfname_ori, "%s", argv[2]);
	FILE *out_permu_oder = fopen(outfname_order, "w");
	FILE *out_permu_ori = fopen(outfname_ori, "w");
	if (out_permu_oder == NULL || out_permu_ori == NULL)
	{
        fprintf(stderr, "\n\nError opening file. Try again\n\n");
        return 0;
    }

	for (int i = 0; i < result.size(); i++)
	{
		std::vector<int> cycle = result[i];
		int cycle_size = (int)cycle.size();
		if (cycle_size <= 3) continue;

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

					fprintf(out_permu_oder, "%d ", k);
					fprintf(out_permu_ori, "%d ", ori);
					break;
				}
			}
		}
		fprintf(out_permu_oder, "\n");
		fprintf(out_permu_ori, "\n");

#ifdef PRINT_ADJ_LIST
		for (int j = 0; j < total_permu; j++) printf("(%d, %d) ", permu_order[j], permu_ori[j]);
		printf("\n");
#endif
		delete[] permu_order;
		delete[] permu_ori;
	}

	fclose(out_permu_oder);
	fclose(out_permu_ori);

	return 0;
}