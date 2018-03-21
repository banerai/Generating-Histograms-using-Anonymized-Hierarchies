#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fstream>

#include "HierarchyTree.h"


int main() {
	//seed random number generator
	srand((unsigned)time(NULL));

	float d, g;
	printf("Enter epsilon: ");
	scanf("%f", &d);
	printf("\n");
	printf("Enter magnitude threshold: ");
	scanf("%f", &g);
	printf("\n");

	while (1) {
		int q;
		printf("Enter query type: ");
		scanf("%d", &q);
		printf("\n");
		//retrieve dataset
		std::ifstream read("C:/Users/baner/Dropbox/Purdue Share/CS490 Clifton/490_data.csv", std::ifstream::in); // declare file stream: http://www.cplusplus.com/reference/iostream/ifstream/
		std::ifstream * readStar = &read;

		HierarchyTree * tree = new HierarchyTree(readStar, q, d, g);

		while (1) {
			int n = 0;
			printf("Which Noise? ");
			scanf("%d", &n);
			printf("\n");
			if (n == 0) break;
			else if (n == 1) tree->assignNoNoise();
			else if (n == 2) tree->assignNoiseOnFQR();	
			else if (n == 3) tree->assignEqualNoise();
			tree->assignMagnitudes();
			for (int i = 0; i < tree->getNodeList().size(); i++) {
				tree->Laplace(tree->getNodeList()[i]);
			}
			tree->printTree();
			tree->printTreeVariance();
			tree->printHistograms();
		}

	}
	return 0;
}