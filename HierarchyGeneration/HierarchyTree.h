#ifndef HIERARCHYTREE_H
#define HIERARCHYTREE_H

#include <iostream>
#include <vector>
#include "HierarchyNode.h"
#include "Range.h"
class HierarchyTree {

private:
	//diff private tree parameters
	int query_type;
	float m_epsilon;
	int m_branchingFactor;
	int m_unitIntervalLength;
	float m_epsilon_threshold;

	//generic parameters
	std::vector<HierarchyNode *> m_nodeList;
	std::vector<std::vector<HierarchyNode *>> histograms;
	std::vector<float> m_diffPrivacySaved;
	HierarchyNode * m_rootNode;
	void printTree(HierarchyNode *, int);
	HierarchyNode * buildTree(std::vector<HierarchyNode *>);
	HierarchyNode * buildTree2(std::vector<HierarchyNode *>);

public:

	HierarchyTree(std::ifstream *, int); //constructor for normal tree building
	HierarchyTree(std::ifstream *, int, float, float); //constructor for noisy tree building
	~HierarchyTree(); //destructor

	std::vector<HierarchyNode *> getNodeList();
	HierarchyNode * getRootNode();

	void printTree();
	void printTreeVariance();
	void printHistograms();
	void printOptimalHistograms();

	void addNode(HierarchyNode *);
	void query(HierarchyNode *);

	void assignMagnitudes();
	void assignNoiseOnFQR(); //FQR is not a Differentially Private way to assign noise 
	void assignNoNoise();
	void assignEqualNoise();

	std::vector<double> HierarchyTree::Laplace(HierarchyNode *);
	void HierarchyTree::redoLaplace(HierarchyNode *, int);

	//FOR LATER COMPARISONS
	void buildEstimateThresholdTree(HierarchyNode*);

	//Automatic Suppression Algorithm 
	int ASA(double, float, float);
};

#endif