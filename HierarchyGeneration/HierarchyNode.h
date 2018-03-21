#pragma once
#include <iostream>
#include <vector>
#include "Range.h"

class HierarchyNode {

private:
	

	HierarchyNode * m_parentNode;
	Range * m_currentRange;
	std::vector<HierarchyNode*> m_childNodes;
	std::vector<int> m_maleData;
	std::vector<int> m_femaleData;
	std::vector<int> m_maleMoE;
	std::vector<int> m_femaleMoE;
	

public:
	std::vector<double> m_answers;
	std::vector<double> m_correct_answers;
	std::vector<double> m_sse;
	std::vector<float> m_respectiveNoise;
	std::vector<float> m_magnitude;
	std::vector<float> m_privacySaved;
	int m_visited;
	std::vector<int> m_unsuppressed;
	std::vector<int> m_excluded_from_histogram;

	HierarchyNode(int, int); //constructor
	~HierarchyNode(); //destructor

	HierarchyNode * getParent();
	Range * getRange();
	int getNumOfChildren();
	std::vector<HierarchyNode*> getChildren();
	void addMaleData(int);
	void addFemaleData(int);
	void addMaleMoE(int);
	void addFemaleMoE(int);
	void setParent(HierarchyNode *);
	void setChildren(std::vector<HierarchyNode*>);
	void setNoise(std::vector<float>);

	std::vector<int> getMaleData();
	std::vector<int> getFemaleData();
	std::vector<int> getMaleMoEData();
	std::vector<int> getFemaleMoEData();
	
	void splitNode(int*); //splits range according to numbers in array given its pointer and updates hierarchy 
};