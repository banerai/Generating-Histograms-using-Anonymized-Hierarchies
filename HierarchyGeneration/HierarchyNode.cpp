#include <stdio.h>
#include <vector>
#include <boost/math/distributions/laplace.hpp>
#include "HierarchyNode.h"
#include "Range.h"

HierarchyNode::HierarchyNode(int beginning, int end) {
	m_currentRange = new Range(beginning, end);
	m_visited = 0;
	for (int i = 0; i < 4; i++)
		m_excluded_from_histogram.push_back(0);
}

void HierarchyNode::addMaleData(int data) {
	m_maleData.push_back(data);
}

void HierarchyNode::addFemaleData(int data) {
	m_femaleData.push_back(data);
}
void HierarchyNode::addMaleMoE(int data) {
	m_maleMoE.push_back(data);
}

void HierarchyNode::addFemaleMoE(int data) {
	m_femaleMoE.push_back(data);
}

void HierarchyNode::setParent(HierarchyNode * node) {
	m_parentNode = node;
}

void HierarchyNode::setChildren(std::vector<HierarchyNode*> children) {
	m_childNodes = children;
}

void HierarchyNode::setNoise(std::vector<float> respective_noise) {
	m_respectiveNoise = respective_noise;
}

HierarchyNode::~HierarchyNode() {
	delete m_parentNode;
	m_currentRange->~Range();
	delete m_currentRange;
	int initialSize = m_childNodes.size();
	for (int i = 0; i < initialSize; i++)
		m_childNodes.pop_back();
}

HierarchyNode * HierarchyNode::getParent() { return m_parentNode; }
Range * HierarchyNode::getRange() { return m_currentRange; }
std::vector<HierarchyNode*> HierarchyNode::getChildren() { return m_childNodes; }
std::vector<int> HierarchyNode::getMaleData() { return m_maleData; }
std::vector<int> HierarchyNode::getFemaleData() { return m_femaleData; }
std::vector<int> HierarchyNode::getMaleMoEData() { return m_maleMoE; }
std::vector<int> HierarchyNode::getFemaleMoEData() { return m_femaleMoE; }