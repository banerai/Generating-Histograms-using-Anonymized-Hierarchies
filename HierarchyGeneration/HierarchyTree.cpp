#include <stdio.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <vector>
#include <string>
#include "C:\boost_1_65_1\boost\math\distributions\laplace.hpp"
#include "HierarchyTree.h"
#include "HierarchyNode.h"
#include "Range.h"
#include "RandomDeviate.h"

const int branching_factor = 2;

struct readNode {
	std::string beginning;
	std::string end;
	std::string malePop;
	std::string maleMoE;
	std::string femalePop;
	std::string femaleMoE;
};
//STORE VALUES IN ORDER
HierarchyTree::HierarchyTree(std::ifstream * file, int q) {
	std::string value;
	readNode line;
	while (getline(*file, value)) // read whole line into line
	{
		std::istringstream iss(value); // string stream
		getline(iss, line.beginning, ','); // read first part up to comma, ignore the comma
		getline(iss, line.end, ','); // read first part up to comma, ignore the comma
		getline(iss, line.malePop, ','); // read first part up to comma, ignore the comma
		getline(iss, line.maleMoE, ','); // read first part up to comma, ignore the comma
		getline(iss, line.femalePop, ','); // read first part up to comma, ignore the comma
		iss >> line.femaleMoE; // read the second part

		HierarchyNode * h = new HierarchyNode(std::stoi(line.beginning), std::stoi(line.end));
		h->setParent(NULL);
		h->addMaleData(std::stoi(line.malePop));
		h->addMaleMoE(std::stoi(line.maleMoE));
		h->addFemaleData(std::stoi(line.femalePop));
		h->addFemaleMoE(std::stoi(line.femaleMoE));
		h->setChildren((std::vector<HierarchyNode*>) NULL);

		m_nodeList.push_back(h);
	}
	m_rootNode = buildTree(m_nodeList);
	query_type = q;
	m_epsilon = -1;
}

//STORE VALUES IN ORDER
HierarchyTree::HierarchyTree(std::ifstream * file, int q, float epsilon, float epsilon_threshold) {
	std::string value;
	readNode line;
	while (getline(*file, value)) // read whole line into line
	{
		std::istringstream iss(value); // string stream
		getline(iss, line.beginning, ','); // read first part up to comma, ignore the comma
		getline(iss, line.end, ','); // read first part up to comma, ignore the comma
		getline(iss, line.malePop, ','); // read first part up to comma, ignore the comma
		getline(iss, line.maleMoE, ','); // read first part up to comma, ignore the comma
		getline(iss, line.femalePop, ','); // read first part up to comma, ignore the comma
		iss >> line.femaleMoE; // read the second part

		HierarchyNode * h = new HierarchyNode(std::stoi(line.beginning), std::stoi(line.end));
		h->setParent(NULL);
		h->addMaleData(std::stoi(line.malePop));
		h->addMaleMoE(std::stoi(line.maleMoE));
		h->addFemaleData(std::stoi(line.femalePop));
		h->addFemaleMoE(std::stoi(line.femaleMoE));
		h->setChildren((std::vector<HierarchyNode*>) NULL);

		m_nodeList.push_back(h);
	}
	m_rootNode = buildTree(m_nodeList);
	query_type = q;
	m_epsilon = epsilon;
	m_epsilon_threshold = epsilon_threshold;
}

void HierarchyTree::query(HierarchyNode * node) {
	std::vector<double> returnValues;
	//max - good threshold @ epsilon = 1: 0.03
	for (int i = 0; i < 4; i++) {
		returnValues.push_back(-1);
	}
	if (query_type == 1) {
		for (int i = 0; i < node->getMaleData().size(); i++) {
			if (returnValues[0] < node->getMaleData()[i])
				returnValues[0] = node->getMaleData()[i];
			if (returnValues[1] < node->getMaleMoEData()[i])
				returnValues[1] = node->getMaleMoEData()[i];
			if (returnValues[2] < node->getFemaleData()[i])
				returnValues[2] = node->getFemaleData()[i];
			if (returnValues[3] < node->getFemaleMoEData()[i])
				returnValues[3] = node->getFemaleMoEData()[i];
		}
	}
	//sum
	if (query_type == 2) {
		int sumOfMaleData = 0;
		int sumOfMaleMoE = 0;
		int sumOfFemaleData = 0;
		int sumOfFemaleMoE = 0;
		for (int i = 0; i < node->getMaleData().size(); i++) {
			sumOfMaleData += node->getMaleData()[i];
			sumOfMaleMoE += node->getMaleMoEData()[i];
			sumOfFemaleData += node->getFemaleData()[i];
			sumOfFemaleMoE += node->getFemaleMoEData()[i];
		}
		returnValues[0] = sumOfMaleData;
		returnValues[1] = sumOfMaleMoE;
		returnValues[2] = sumOfFemaleData;
		returnValues[3] = sumOfFemaleMoE;
	}
	//min - good thresholds @ epsilon = 1: 0.025 or 0.0275
	if (query_type == 3) {
		for (int i = 0; i < 4; i++) {
			returnValues[i] = 99999999;
		}
		for (int i = 0; i < node->getMaleData().size(); i++) {
			if (returnValues[0] > node->getMaleData()[i])
				returnValues[0] = node->getMaleData()[i];
			if (returnValues[1] > node->getMaleMoEData()[i])
				returnValues[1] = node->getMaleMoEData()[i];
			if (returnValues[2] > node->getFemaleData()[i])
				returnValues[2] = node->getFemaleData()[i];
			if (returnValues[3] > node->getFemaleMoEData()[i])
				returnValues[3] = node->getFemaleMoEData()[i];
		}
	}

	//etc
	//...

	node->m_answers = returnValues;
	node->m_correct_answers = returnValues;
}

std::vector<double> HierarchyTree::Laplace(HierarchyNode * node) {
	//then assign noise
	const double dMean = 0;
	if (m_epsilon > 0) {
		for (int i = 0; i < node->m_answers.size(); i++) {
			if (node->m_respectiveNoise[i] != -1) {
				CLaplaceDeviate laplacian;
				double dAdditiveNoise = (dMean + laplacian.Get()) * (node->m_respectiveNoise[i]);
				node->m_answers[i] += dAdditiveNoise;
			}
		}
	}

	return node->m_answers;
}

void HierarchyTree::redoLaplace(HierarchyNode * node, int index) {
	//then assign noise
	const double dMean = 0;
	if (m_epsilon > 0) {
		if (node->m_respectiveNoise[index] != -1) {
			CLaplaceDeviate laplacian;
			double dAdditiveNoise = (dMean + laplacian.Get()) * (node->m_respectiveNoise[index]);
			node->m_answers[index] += dAdditiveNoise;
		}
	}
}

void HierarchyTree::assignEqualNoise() {
	for (int i = 0; i < m_nodeList.size(); i++)
		query(m_nodeList[i]);

	for (int i = 0; i < m_nodeList.size(); i++) {
		for (int j = 0; j < 4; j++) {
			if (m_nodeList[i]->m_respectiveNoise.size() < 4) {
				m_nodeList[i]->m_respectiveNoise.push_back((float)(m_epsilon/m_nodeList.size()));
			}
			else {
				m_nodeList[i]->m_respectiveNoise[j] = ((float)(m_epsilon / m_nodeList.size()));
			}
		}
	}
}

void HierarchyTree::assignNoNoise() {
	for (int i = 0; i < m_nodeList.size(); i++)
		query(m_nodeList[i]);

	for (int i = 0; i < m_nodeList.size(); i++) {
		std::vector<double> queryResponse = m_nodeList[i]->m_answers;
		for (int j = 0; j < 4; j++) {
			if (m_nodeList[i]->m_respectiveNoise.size() < 4) {
				m_nodeList[i]->m_respectiveNoise.push_back(-1);
			}
			else {
				m_nodeList[i]->m_respectiveNoise[j] = -1;
			}
		}
	}
}


void HierarchyTree::assignMagnitudes() {
	std::vector<double> summationVector;
	for (int i = 0; i < 4; i++) {
		summationVector.push_back(0);
	}

	for (int i = 0; i < m_nodeList.size(); i++) {
		std::vector<double> queryResponse = m_nodeList[i]->m_correct_answers;
		summationVector[0] += queryResponse[0];
		summationVector[1] += queryResponse[1];
		summationVector[2] += queryResponse[2];
		summationVector[3] += queryResponse[3];
	}

	for (int i = 0; i < m_nodeList.size(); i++) {
		std::vector<double> queryResponse = m_nodeList[i]->m_correct_answers;
		for (int j = 0; j < 4; j++) {
			if (m_nodeList[i]->m_magnitude.size() < 4) {
				m_nodeList[i]->m_magnitude.push_back(((float)(queryResponse[j] / summationVector[j]))*m_epsilon);
			}
			else {
				m_nodeList[i]->m_magnitude[j] = ((float)(queryResponse[j] / summationVector[j]))*m_epsilon;
			}
		}
	}
}

void HierarchyTree::assignNoiseOnFQR() {
	for (int i = 0; i < m_nodeList.size(); i++)
		query(m_nodeList[i]);

	std::vector<double> summationVector;
	for (int i = 0; i < 4; i++) {
		summationVector.push_back(0);
	}

	for (int i = 0; i < m_nodeList.size(); i++) {
		std::vector<double> queryResponse = m_nodeList[i]->m_answers;
		summationVector[0] += queryResponse[0];
		summationVector[1] += queryResponse[1];
		summationVector[2] += queryResponse[2];
		summationVector[3] += queryResponse[3];
	}

	for (int i = 0; i < m_nodeList.size(); i++) {
		std::vector<double> queryResponse = m_nodeList[i]->m_answers;
		for (int j = 0; j < 4; j++) {
			if (m_nodeList[i]->m_respectiveNoise.size() < 4) {
				m_nodeList[i]->m_respectiveNoise.push_back(((float) (queryResponse[j] / summationVector[j]))*m_epsilon);
			}
			else {
				m_nodeList[i]->m_respectiveNoise[j] = ((float)(queryResponse[j] / summationVector[j]))*m_epsilon;
			}
		}
	}
}

HierarchyTree::~HierarchyTree() {
	int initialSize = m_nodeList.size();
	for (int i = 0; i < initialSize; i++) {
		HierarchyNode * node = m_nodeList.back();
		node->~HierarchyNode();
		m_nodeList.pop_back();
		delete node;
	}
}

std::vector<HierarchyNode *> HierarchyTree::getNodeList() { return m_nodeList; }
HierarchyNode * HierarchyTree::getRootNode() { return m_rootNode; }

//initializes a binary tree with no privacy
HierarchyNode * HierarchyTree::buildTree(std::vector<HierarchyNode *> nodeList) {
	std::vector<HierarchyNode *> usedList;

	std::vector<HierarchyNode *> nextLevelList;
	for (int i = 0; i < nodeList.size(); i++) {
		for (int j = 0; j < nodeList.size(); j++) {
			if ((nodeList[i]->getRange()->getEnd() + 1) == nodeList[j]->getRange()->getBeginning()) {
				int bool_i = 0;
				int bool_j = 0;
				for (int k = 0; k < usedList.size(); k++) {
					if (nodeList[i] == nodeList[k]) {
						bool_i = 1;
						break;
					}
				}
				for (int k = 0; k < usedList.size(); k++) {
					if (nodeList[j] == nodeList[k]) {
						bool_j = 1;
						break;
					}
				}
				if (!(bool_i == 1 || bool_j == 1)) {
					HierarchyNode * p = new HierarchyNode(nodeList[i]->getRange()->getBeginning(), nodeList[j]->getRange()->getEnd());
					std::vector<HierarchyNode *> newVec;
					newVec.push_back(nodeList[i]);
					newVec.push_back(nodeList[j]);
					p->setChildren(newVec);

					for (int k = 0; k < nodeList[i]->getMaleData().size(); k++) { p->addMaleData(nodeList[i]->getMaleData()[k]); }
					for (int k = 0; k < nodeList[j]->getMaleData().size(); k++) { p->addMaleData(nodeList[j]->getMaleData()[k]); }
					for (int k = 0; k < nodeList[i]->getFemaleData().size(); k++) { p->addFemaleData(nodeList[i]->getFemaleData()[k]); }
					for (int k = 0; k < nodeList[j]->getFemaleData().size(); k++) { p->addFemaleData(nodeList[j]->getFemaleData()[k]); }
					for (int k = 0; k < nodeList[i]->getMaleMoEData().size(); k++) { p->addMaleMoE(nodeList[i]->getMaleMoEData()[k]); }
					for (int k = 0; k < nodeList[j]->getMaleMoEData().size(); k++) { p->addMaleMoE(nodeList[j]->getMaleMoEData()[k]); }
					for (int k = 0; k < nodeList[i]->getFemaleMoEData().size(); k++) { p->addFemaleMoE(nodeList[i]->getFemaleMoEData()[k]); }
					for (int k = 0; k < nodeList[j]->getFemaleMoEData().size(); k++) { p->addFemaleMoE(nodeList[j]->getFemaleMoEData()[k]); }

					p->setParent(NULL);
					nodeList[i]->setParent(p);
					nodeList[j]->setParent(p);

					nextLevelList.push_back(p);

					usedList.push_back(nodeList[i]);
					usedList.push_back(nodeList[j]);

					if (nodeList.size() - usedList.size() == 1) {
						for (int k = 0; k < nodeList.size(); k++) {
							if (std::find(usedList.begin(), usedList.end(), nodeList[k]) != usedList.end()) {
								/* v contains x */
							}
							else {
								/* v does not contain x */
								nextLevelList.push_back(nodeList[k]);
								usedList.push_back(nodeList[k]);
							}
						}

					}

				}
			}
		}
	}
	for (int i = 0; i < nextLevelList.size(); i++) {
		m_nodeList.push_back(nextLevelList[i]);
	}
	if (nextLevelList.size() == 1)
		return nextLevelList[0];
	else 
		return buildTree(nextLevelList);
}

//NEEDS MORE WORK FOR 3-CHILDREN HIERARCHY
//initializes a binary tree with no privacy
HierarchyNode * HierarchyTree::buildTree2(std::vector<HierarchyNode *> nodeList) {
	std::vector<HierarchyNode *> usedList;

	std::vector<HierarchyNode *> nextLevelList;
	for (int i = 0; i < nodeList.size(); i++) {
		for (int j = 0; j < nodeList.size(); j++) {
			for (int k = 0; k < nodeList.size(); k++) {
				std::vector<HierarchyNode *> newVec;
				if ((nodeList[i]->getRange()->getEnd() + 1) == nodeList[j]->getRange()->getBeginning()) {
					int bool_i = 0;
					int bool_j = 0;
					for (int l = 0; l < usedList.size(); l++) {
						if (nodeList[i] == nodeList[l]) {
							bool_i = 1;
							break;
						}
					}
					for (int l = 0; l < usedList.size(); l++) {
						if (nodeList[j] == nodeList[l]) {
							bool_j = 1;
							break;
						}
					}
					if (!(bool_i == 1 || bool_j == 1)) {
						HierarchyNode * p = new HierarchyNode(nodeList[i]->getRange()->getBeginning(), nodeList[j]->getRange()->getEnd());
						newVec.push_back(nodeList[i]);
						newVec.push_back(nodeList[j]);
						p->setChildren(newVec);

						for (int l = 0; l < nodeList[i]->getMaleData().size(); l++) { p->addMaleData(nodeList[i]->getMaleData()[l]); }
						for (int l = 0; l < nodeList[j]->getMaleData().size(); l++) { p->addMaleData(nodeList[j]->getMaleData()[l]); }
						for (int l = 0; l < nodeList[i]->getFemaleData().size(); l++) { p->addFemaleData(nodeList[i]->getFemaleData()[l]); }
						for (int l = 0; l < nodeList[j]->getFemaleData().size(); l++) { p->addFemaleData(nodeList[j]->getFemaleData()[l]); }
						for (int l = 0; l < nodeList[i]->getMaleMoEData().size(); l++) { p->addMaleMoE(nodeList[i]->getMaleMoEData()[l]); }
						for (int l = 0; l < nodeList[j]->getMaleMoEData().size(); l++) { p->addMaleMoE(nodeList[j]->getMaleMoEData()[l]); }
						for (int l = 0; l < nodeList[l]->getFemaleMoEData().size(); l++) { p->addFemaleMoE(nodeList[i]->getFemaleMoEData()[l]); }
						for (int l = 0; l < nodeList[l]->getFemaleMoEData().size(); l++) { p->addFemaleMoE(nodeList[j]->getFemaleMoEData()[l]); }

						p->setParent(NULL);
						nodeList[i]->setParent(p);
						nodeList[j]->setParent(p);

						nextLevelList.push_back(p);

						usedList.push_back(nodeList[i]);
						usedList.push_back(nodeList[j]);

						if (nodeList.size() - usedList.size() == 1) {
							for (int l = 0; l < nodeList.size(); l++) {
								if (std::find(usedList.begin(), usedList.end(), nodeList[l]) != usedList.end()) {
									/* v contains x */
								}
								else {
									/* v does not contain x */
									nextLevelList.push_back(nodeList[l]);
									usedList.push_back(nodeList[l]);
								}
							}

						}

					}
				}
				if ((nodeList[j]->getRange()->getEnd() + 1) == nodeList[k]->getRange()->getBeginning()) {
					int bool_j = 0;
					int bool_k = 0;
					for (int l = 0; l < usedList.size(); l++) {
						if (nodeList[j] == nodeList[l]) {
							bool_j = 1;
							break;
						}
					}
					for (int l = 0; l < usedList.size(); l++) {
						if (nodeList[k] == nodeList[l]) {
							bool_k = 1;
							break;
						}
					}
					if (!(bool_j == 1 || bool_k == 1)) {
						HierarchyNode * p = new HierarchyNode(nodeList[j]->getRange()->getBeginning(), nodeList[k]->getRange()->getEnd());

						newVec.push_back(nodeList[k]);
						p->setChildren(newVec);

						for (int k = 0; k < nodeList[i]->getMaleData().size(); k++) { p->addMaleData(nodeList[i]->getMaleData()[k]); }
						for (int k = 0; k < nodeList[j]->getMaleData().size(); k++) { p->addMaleData(nodeList[j]->getMaleData()[k]); }
						for (int k = 0; k < nodeList[i]->getFemaleData().size(); k++) { p->addFemaleData(nodeList[i]->getFemaleData()[k]); }
						for (int k = 0; k < nodeList[j]->getFemaleData().size(); k++) { p->addFemaleData(nodeList[j]->getFemaleData()[k]); }
						for (int k = 0; k < nodeList[i]->getMaleMoEData().size(); k++) { p->addMaleMoE(nodeList[i]->getMaleMoEData()[k]); }
						for (int k = 0; k < nodeList[j]->getMaleMoEData().size(); k++) { p->addMaleMoE(nodeList[j]->getMaleMoEData()[k]); }
						for (int k = 0; k < nodeList[i]->getFemaleMoEData().size(); k++) { p->addFemaleMoE(nodeList[i]->getFemaleMoEData()[k]); }
						for (int k = 0; k < nodeList[j]->getFemaleMoEData().size(); k++) { p->addFemaleMoE(nodeList[j]->getFemaleMoEData()[k]); }

						p->setParent(NULL);
						nodeList[i]->setParent(p);
						nodeList[j]->setParent(p);

						nextLevelList.push_back(p);

						usedList.push_back(nodeList[i]);
						usedList.push_back(nodeList[j]);

						if (nodeList.size() - usedList.size() == 1) {
							for (int k = 0; k < nodeList.size(); k++) {
								if (std::find(usedList.begin(), usedList.end(), nodeList[k]) != usedList.end()) {
									/* v contains x */
								}
								else {
									/* v does not contain x */
									nextLevelList.push_back(nodeList[k]);
									usedList.push_back(nodeList[k]);
								}
							}

						}

					}
				}
			}
		}
	}
	for (int i = 0; i < nextLevelList.size(); i++) {
		m_nodeList.push_back(nextLevelList[i]);
	}
	if (nextLevelList.size() == 1)
		return nextLevelList[0];
	else
		return buildTree(nextLevelList);
}

void HierarchyTree::printTree(HierarchyNode * p, int indent) {
	if (p != NULL) {
		for (int i = 0; i < p->getChildren().size(); i++)
			if (p->getChildren()[i]) printTree(p->getChildren()[i], indent + 4);
		if (indent) {
			std::cout << std::setw(indent) << ' ';
		}

		p->m_visited = 1;

		for (int i = 0; i < 4; i++) {
			p->m_sse.push_back(0);
			p->m_privacySaved.push_back(0);
		}
			
		if(m_diffPrivacySaved.size() < 4)
			for (int i = 0; i < 4; i++)
				m_diffPrivacySaved.push_back(0);
		
		

		std::vector<double> q = p->m_answers;
		std::vector<float> rNoise;
		if (q.size() > 0) {
			//tfw you write crappy code just to get the job done
			std::cout << "[" << p->getRange()->getBeginning() << ", " << p->getRange()->getEnd() << "]" << " - Query Answers: {";
			if (p->m_magnitude[0] < m_epsilon_threshold && p->m_respectiveNoise[0] >= 0 && !ASA(p->m_answers[0], p->m_respectiveNoise[0], 0.05)) {
				p->m_unsuppressed.push_back(0);
				std::cout << "*" << ",";
				rNoise.push_back(p->m_respectiveNoise[0]);
				HierarchyNode * goodAncestor = p;
				while (goodAncestor->m_magnitude[0] < m_epsilon_threshold) {
					goodAncestor = goodAncestor->getParent();
					if (goodAncestor == NULL) break;
					rNoise[0] += goodAncestor->m_respectiveNoise[0];
				}
				if (goodAncestor != NULL) {
					int bool_i = 1;
					for (int i = 0; i < goodAncestor->getChildren().size(); i++) {
						if (goodAncestor->getChildren()[i]->m_visited == 1 && goodAncestor->getChildren()[i]->m_unsuppressed[0] == 1 || 
							goodAncestor->getChildren()[i]->m_visited == 0) { bool_i = 0; break; }
					}
					//if (goodAncestor == NULL) printf("\nHELP\n");
					int bool_j = 1;
					for (int j = 0; j < histograms[0].size(); j++) {
						if (histograms[0][j] == goodAncestor) { bool_j = 0; break; }
					}
					if (bool_i == 1 && bool_j == 1) histograms[0].push_back(goodAncestor);
				}
				/*
				p->m_answers[0] = goodAncestor->m_correct_answers[0];
				p->m_correct_answers[0] = goodAncestor->m_correct_answers[0];
				p->m_respectiveNoise[0] = goodAncestor->m_respectiveNoise[0];
				redoLaplace(p, 0);
				p->m_sse[0] = (p->m_answers[0] - p->m_correct_answers[0]) * (p->m_answers[0] - p->m_correct_answers[0]);
				*/
				if (rNoise[0] > m_diffPrivacySaved[0])
					m_diffPrivacySaved[0] = (rNoise[0]);
				p->m_privacySaved[0] = (rNoise[0]);
				p->m_sse[0] = -1;
			}
			else {
				rNoise.push_back(0);
				p->m_unsuppressed.push_back(1);
				if(p->getParent() != NULL)
					p->getParent()->m_excluded_from_histogram[0] = 1;
				//leaf case //NEED TO FIX AS CANNOT ACCESS GETCHILDREN[0]
				if(p->getChildren().size() == 0) histograms[0].push_back(p);
				else {
					//rest of the cases
					if (p != NULL) {
						int bool_i = 1;
						for (int i = 0; i < p->getChildren().size(); i++) {
							if (p->getChildren()[i]->m_visited == 1 && p->getChildren()[i]->m_unsuppressed[0] == 1 ||
								p->getChildren()[i]->m_visited == 0) {
								bool_i = 0; break;
							}
						}
						int bool_j = 1;
						for (int j = 0; j < histograms[0].size(); j++) {
							if (histograms[0][j] == p) { bool_j = 0; break; }
						}
						int bool_k = 0;
						for (int k = 0; k < p->getChildren().size(); k++) {
							if (p->getChildren()[k]->m_visited == 1 && p->getChildren()[k]->m_unsuppressed[0] == 1) {
								bool_k = 1; break;
							}
						}
						if (bool_i == 1 && bool_j == 1) histograms[0].push_back(p);
						if (bool_k == 1) {
							for (int i = 0; i < p->getChildren().size(); i++) {
								int bool_l = 0;
								for (int j = 0; j < histograms[0].size(); j++) {
									if (histograms[0][j] == p->getChildren()[i]) { bool_l = 1; break; }
								}
								if (bool_l != 1 && p->getChildren()[i]->m_visited == 1 && p->m_excluded_from_histogram[0] == 1)
									if(p->getChildren()[i]->m_excluded_from_histogram[0] == 0)
										histograms[0].push_back(p->getChildren()[i]);
							}
						}
					}
				}
				//p->m_privacySaved.push_back(rNoise[0]);
				p->m_sse[0] = (q[0] - p->m_correct_answers[0]) * (q[0] - p->m_correct_answers[0]);
				std::cout << q[0] << ",";
			}
			if (p->m_magnitude[1] < m_epsilon_threshold && p->m_respectiveNoise[1] >= 0 && !ASA(p->m_answers[1], p->m_respectiveNoise[1], 0.05)) {
				p->m_unsuppressed.push_back(0);
				std::cout << "*" << ",";
				rNoise.push_back(p->m_respectiveNoise[1]);
				HierarchyNode * goodAncestor = p;
				while (goodAncestor->m_magnitude[1] < m_epsilon_threshold) {
					goodAncestor = goodAncestor->getParent();
					if (goodAncestor == NULL) break;
					rNoise[1] += goodAncestor->m_respectiveNoise[1];
				}
				if (goodAncestor != NULL) {
					int bool_i = 1;
					for (int i = 0; i < goodAncestor->getChildren().size(); i++) {
						if (goodAncestor->getChildren()[i]->m_visited == 1 && goodAncestor->getChildren()[i]->m_unsuppressed[1] == 1 ||
							goodAncestor->getChildren()[i]->m_visited == 0) {
							bool_i = 0; break;
						}
					}
					int bool_j = 1;
					for (int j = 0; j < histograms[1].size(); j++) {
						if (histograms[1][j] == goodAncestor) { bool_j = 0; break; }
					}
					if (bool_i == 1 && bool_j == 1) histograms[1].push_back(goodAncestor);
				}
				/*
				p->m_answers[1] = goodAncestor->m_correct_answers[1];
				p->m_correct_answers[1] = goodAncestor->m_correct_answers[1];
				p->m_respectiveNoise[1] = goodAncestor->m_respectiveNoise[1];
				redoLaplace(p, 1);
				p->m_sse[1] = (p->m_answers[1] - p->m_correct_answers[1]) * (p->m_answers[1] - p->m_correct_answers[1]);
				*/
				if (rNoise[1] > m_diffPrivacySaved[1])
					m_diffPrivacySaved[1] = (rNoise[1]);
				p->m_privacySaved[1] = (rNoise[1]);
				p->m_sse[1] = -1;
			}
			else {
				rNoise.push_back(0);
				p->m_unsuppressed.push_back(1);
				if(p->getParent() != NULL)
					p->getParent()->m_excluded_from_histogram[1] = 1;
				//leaf case
				if (p->getChildren().size() == 0) histograms[1].push_back(p);
				else {
					//rest of the cases
					if (p != NULL) {
						int bool_i = 1;
						for (int i = 0; i < p->getChildren().size(); i++) {
							if (p->getChildren()[i]->m_visited == 1 && p->getChildren()[i]->m_unsuppressed[1] == 1 ||
								p->getChildren()[i]->m_visited == 0) {
								bool_i = 0; break;
							}
						}
						int bool_j = 1;
						for (int j = 0; j < histograms[1].size(); j++) {
							if (histograms[1][j] == p) { bool_j = 0; break; }
						}
						int bool_k = 0;
						for (int k = 0; k < p->getChildren().size(); k++) {
							if (p->getChildren()[k]->m_visited == 1 && p->getChildren()[k]->m_unsuppressed[1] == 1) {
								bool_k = 1; break;
							}
						}
						if (bool_i == 1 && bool_j == 1) histograms[1].push_back(p);
						if (bool_k == 1) {
							for (int i = 0; i < p->getChildren().size(); i++) {
								int bool_l = 0;
								for (int j = 0; j < histograms[1].size(); j++) {
									if (histograms[1][j] == p->getChildren()[i]) { bool_l = 1; break; }
								}
								if (bool_l != 1 && p->getChildren()[i]->m_visited == 1 && p->m_excluded_from_histogram[1] == 1)
									if (p->getChildren()[i]->m_excluded_from_histogram[1] == 0)
										histograms[1].push_back(p->getChildren()[i]);
							}
						}
					}
				}
				//p->m_privacySaved.push_back(rNoise[1]);
				p->m_sse[1] = (q[1] - p->m_correct_answers[1]) * (q[1] - p->m_correct_answers[1]);
				std::cout << q[1] << ",";
			}
				
			if (p->m_magnitude[2] < m_epsilon_threshold && p->m_respectiveNoise[2] >= 0 && !ASA(p->m_answers[2], p->m_respectiveNoise[2], 0.05)) {
				p->m_unsuppressed.push_back(0);
				std::cout << "*" << ",";
				rNoise.push_back(p->m_respectiveNoise[2]);
				HierarchyNode * goodAncestor = p;
				while (goodAncestor->m_magnitude[2] < m_epsilon_threshold) {
					goodAncestor = goodAncestor->getParent();
					if (goodAncestor == NULL) break;
					rNoise[2] += goodAncestor->m_respectiveNoise[2];
				}
				if (goodAncestor != NULL) {
					int bool_i = 1;
					for (int i = 0; i < goodAncestor->getChildren().size(); i++) {
						if (goodAncestor->getChildren()[i]->m_visited == 1 && goodAncestor->getChildren()[i]->m_unsuppressed[2] == 1 ||
							goodAncestor->getChildren()[i]->m_visited == 0) {
							bool_i = 0; break;
						}
					}
					int bool_j = 1;
					for (int j = 0; j < histograms[2].size(); j++) {
						if (histograms[2][j] == goodAncestor) { bool_j = 0; break; }
					}
					if (bool_i == 1 && bool_j == 1) histograms[2].push_back(goodAncestor);
				}
				/*
				p->m_answers[2] = goodAncestor->m_correct_answers[2];
				p->m_correct_answers[2] = goodAncestor->m_correct_answers[2];
				p->m_respectiveNoise[2] = goodAncestor->m_respectiveNoise[2];
				redoLaplace(p, 2);
				p->m_sse[2] = (p->m_answers[2] - p->m_correct_answers[2]) * (p->m_answers[2] - p->m_correct_answers[2]);
				*/
				if (rNoise[2] > m_diffPrivacySaved[2])
					m_diffPrivacySaved[2] = (rNoise[2]);
				p->m_privacySaved[2] = (rNoise[2]);
				p->m_sse[2] = -1; 
			}
			else {
				rNoise.push_back(0);
				p->m_unsuppressed.push_back(1);
				if (p->getParent() != NULL)
					p->getParent()->m_excluded_from_histogram[2] = 1;
				//leaf case
				if (p->getChildren().size() == 0) histograms[2].push_back(p);
				else {
					//rest of the cases
					if (p != NULL) {
						int bool_i = 1;
						for (int i = 0; i < p->getChildren().size(); i++) {
							if (p->getChildren()[i]->m_visited == 1 && p->getChildren()[i]->m_unsuppressed[2] == 1 ||
								p->getChildren()[i]->m_visited == 0) {
								bool_i = 0; break;
							}
						}
						int bool_j = 1;
						for (int j = 0; j < histograms[2].size(); j++) {
							if (histograms[2][j] == p) { bool_j = 0; break; }
						}
						int bool_k = 0;
						for (int k = 0; k < p->getChildren().size(); k++) {
							if (p->getChildren()[k]->m_visited == 1 && p->getChildren()[k]->m_unsuppressed[2] == 1) {
								bool_k = 1; break;
							}
						}
						if (bool_i == 1 && bool_j == 1) histograms[2].push_back(p);
						if (bool_k == 1) {
							for (int i = 0; i < p->getChildren().size(); i++) {
								int bool_l = 0;
								for (int j = 0; j < histograms[2].size(); j++) {
									if (histograms[2][j] == p->getChildren()[i]) { bool_l = 1; break; }
								}
								if (bool_l != 1 && p->getChildren()[i]->m_visited == 1 && p->m_excluded_from_histogram[2] == 1)
									if (p->getChildren()[i]->m_excluded_from_histogram[2] == 0)
										histograms[2].push_back(p->getChildren()[i]);
							}
						}
					}
				}
				//p->m_privacySaved.push_back(rNoise[2]);
				p->m_sse[2] = (q[2] - p->m_correct_answers[2]) * (q[2] - p->m_correct_answers[2]);
				std::cout << q[2] << ",";
			}
				
			if (p->m_magnitude[3] < m_epsilon_threshold && p->m_respectiveNoise[3] >= 0 && !ASA(p->m_answers[3], p->m_respectiveNoise[3], 0.05)) {
				p->m_unsuppressed.push_back(0);
				std::cout << "*" << "} - SSE's: {";
				rNoise.push_back(p->m_respectiveNoise[3]);
				HierarchyNode * goodAncestor = p;
				while (goodAncestor->m_magnitude[3] < m_epsilon_threshold) {
					goodAncestor = goodAncestor->getParent();
					if (goodAncestor == NULL) break;
					rNoise[3] += goodAncestor->m_respectiveNoise[3];
				}
				if (goodAncestor != NULL) {
					int bool_i = 1;
					for (int i = 0; i < goodAncestor->getChildren().size(); i++) {
						if (goodAncestor->getChildren()[i]->m_visited == 1 && goodAncestor->getChildren()[i]->m_unsuppressed[3] == 1 ||
							goodAncestor->getChildren()[i]->m_visited == 0) {
							bool_i = 0; break;
						}
					}
					int bool_j = 1;
					for (int j = 0; j < histograms[3].size(); j++) {
						if (histograms[3][j] == goodAncestor) { bool_j = 0; break; }
					}
					if (bool_i == 1 && bool_j == 1) histograms[3].push_back(goodAncestor);
				}
				/*
				p->m_answers[3] = goodAncestor->m_correct_answers[3];
				p->m_correct_answers[3] = goodAncestor->m_correct_answers[3];
				p->m_respectiveNoise[3] = goodAncestor->m_respectiveNoise[3];
				redoLaplace(p, 3);
				p->m_sse[3] = (p->m_answers[3] - p->m_correct_answers[3]) * (p->m_answers[3] - p->m_correct_answers[3]);
				*/
				if (rNoise[3] > m_diffPrivacySaved[3])
					m_diffPrivacySaved[3] = (rNoise[3]);
				p->m_privacySaved[3] = (rNoise[3]);
				p->m_sse[3] = -1;
			}
			else {
				rNoise.push_back(0);
				p->m_unsuppressed.push_back(1);
				if (p->getParent() != NULL)
					p->getParent()->m_excluded_from_histogram[3] = 1;
				//leaf case
				if (p->getChildren().size() == 0) histograms[3].push_back(p);
				else {
					//rest of the cases
					if (p != NULL) {
						int bool_i = 1;
						for (int i = 0; i < p->getChildren().size(); i++) {
							if (p->getChildren()[i]->m_visited == 1 && p->getChildren()[i]->m_unsuppressed[3] == 1 ||
								p->getChildren()[i]->m_visited == 0) {
								bool_i = 0; break;
							}
						}
						int bool_j = 1;
						for (int j = 0; j < histograms[3].size(); j++) {
							if (histograms[3][j] == p) { bool_j = 0; break; }
						}
						int bool_k = 0;
						for (int k = 0; k < p->getChildren().size(); k++) {
							if (p->getChildren()[k]->m_visited == 1 && p->getChildren()[k]->m_unsuppressed[3] == 1) {
								bool_k = 1; break;
							}
						}
						if (bool_i == 1 && bool_j == 1) histograms[3].push_back(p);
						if (bool_k == 1) {
							for (int i = 0; i < p->getChildren().size(); i++) {
								int bool_l = 0;
								for (int j = 0; j < histograms[3].size(); j++) {
									if (histograms[3][j] == p->getChildren()[i]) { bool_l = 1; break; }
								}
								if(bool_l != 1 && p->getChildren()[i]->m_visited == 1 && p->m_excluded_from_histogram[3] == 1)
									if (p->getChildren()[i]->m_excluded_from_histogram[3] == 0)
										histograms[3].push_back(p->getChildren()[i]);
							}
						}
					}
				}
				//p->m_privacySaved.push_back(rNoise[3]);
				p->m_sse[3] = (q[3] - p->m_correct_answers[3]) * (q[3] - p->m_correct_answers[3]);
				std::cout << q[3] << "} - SSE's: {";
			}

			for (int i = 0; i < 4; i++) {
				if (p->m_sse[i] < 0) {
					p->m_sse[i] = m_epsilon * 100; // for some reason SSE's are always below epsilon
					std::cout << "n/a";
				}
				else
					std::cout << p->m_sse[i];

				if (i < 3)
					std::cout << ",";
			}
			std::cout << "} - Privacy Saved: {";
			for (int i = 0; i < 4; i++) {
				std::cout << rNoise[i];
				if (i < 3)
					std::cout << ",";
			}
			std::cout << "}\n";
			//std::cout << "[" << p->getRange()->getBeginning() << ", " << p->getRange()->getEnd() << "]" << " - {" << q[0] << ", " << q[1] << ", " << q[2] << ", " << q[3] << "}\n ";
		}
		
	}
}

void HierarchyTree::addNode(HierarchyNode * node) {
	m_nodeList.push_back(node);
}

void HierarchyTree::printTree() {
	CLaplaceDeviate laplacian;
	m_epsilon_threshold = laplacian.Get() * m_epsilon_threshold * 0.1 + m_epsilon_threshold;
		

	std::vector<HierarchyNode *> hist1; histograms.push_back(hist1);
	std::vector<HierarchyNode *> hist2; histograms.push_back(hist2);
	std::vector<HierarchyNode *> hist3; histograms.push_back(hist3);
	std::vector<HierarchyNode *> hist4; histograms.push_back(hist4);
	

	HierarchyNode * p;
	for (int i = 0; i < m_nodeList.size(); i++) {
		if (m_nodeList[i]->getParent() == NULL) {
			p = m_nodeList[i];
			printTree(p, 0);
		}
	}
}

void HierarchyTree::printTreeVariance() {
	std::vector<double> treeVariances;
	for (int i = 0; i < 4; i++)
		treeVariances.push_back(0);
	for (int i = 0; i < m_nodeList.size(); i++) {
		for (int j = 0; j < 4; j++) {
			if (m_nodeList[i]->m_sse[j] != m_epsilon * 100)
				treeVariances[j] += m_nodeList[i]->m_sse[j];
		}
	}

	printf("\nEpsilon Threshold: %f\n", m_epsilon_threshold);
	std::cout << "Hierarchy SSE's: {" << treeVariances[0] << "," << treeVariances[1] << "," << treeVariances[2] << "," << treeVariances[3] << "}\n";
	std::cout << "Total Privacy Saved: {" << m_diffPrivacySaved[0] << "," << m_diffPrivacySaved[1] << "," << m_diffPrivacySaved[2] << "," << m_diffPrivacySaved[3] << "}\n";
}

void HierarchyTree::printHistograms() {
	std::vector<float> totalPrivacySaved;
	std::vector<float> sumSSEs;
	for (int i = 0; i < 4; i++) {
		totalPrivacySaved.push_back(0);
		sumSSEs.push_back(0);
	}
	std::cout << "\nAnonymized Histograms:\n";
	for (int i = 0; i < 4; i++) {
		std::cout << "\tHistogram " << i << "\n";
		for (int j = 0; j < histograms[i].size(); j++) {
			if (histograms[i][j]->m_sse[i] >= 0) {
				if (histograms[i][j]->m_sse[i] != m_epsilon * 100)
					sumSSEs[i] += histograms[i][j]->m_sse[i];
				for (int k = 0; k < histograms[i][j]->getChildren().size(); k++) {
					if (histograms[i][j]->getChildren()[k]->m_privacySaved[i] > totalPrivacySaved[i])
						totalPrivacySaved[i] = histograms[i][j]->getChildren()[k]->m_privacySaved[i];
				}
				if(histograms[i][j]->m_sse[i] != m_epsilon * 100)
					std::cout << "\t\t[" << histograms[i][j]->getRange()->getBeginning() << ", " << histograms[i][j]->getRange()->getEnd() << "] - SSE: " << histograms[i][j]->m_sse[i] << "\n";
				else 
					std::cout << "\t\t[" << histograms[i][j]->getRange()->getBeginning() << ", " << histograms[i][j]->getRange()->getEnd() << "] - SSE: n/a\n";
			} 
		}
		std::cout << "\t\tTotal Privacy Saved: " << totalPrivacySaved[i];
		std::cout << "\n\t\tTotal SSE: " << sumSSEs[i];
		std::cout << "\n";
	}
}

void HierarchyTree::printOptimalHistograms() {
	std::vector<float> totalPrivacySaved;
	std::vector<float> sumSSEs;
	for (int i = 0; i < 4; i++) {
		totalPrivacySaved.push_back(0);
		sumSSEs.push_back(0);
	}
	std::cout << "\nOptimal Histograms:\n";
	for (int i = 0; i < 4; i++) {
		std::cout << "\tHistogram " << i << "\n";
		for (int j = 0; j < histograms[i].size(); j++) {
			//WORK HERE
			if (histograms[i][j]->m_sse[i] >= 0) {
				sumSSEs[i] += histograms[i][j]->m_sse[i];
				for (int k = 0; k < histograms[i][j]->getChildren().size(); k++) {
					if (histograms[i][j]->getChildren()[k]->m_privacySaved[i] > totalPrivacySaved[i])
						totalPrivacySaved[i] = histograms[i][j]->getChildren()[k]->m_privacySaved[i];
				}
				if (histograms[i][j]->m_sse[i] != m_epsilon * 100)
					std::cout << "\t\t[" << histograms[i][j]->getRange()->getBeginning() << ", " << histograms[i][j]->getRange()->getEnd() << "] - SSE: " << histograms[i][j]->m_sse[i] << "\n";
				else
					std::cout << "\t\t[" << histograms[i][j]->getRange()->getBeginning() << ", " << histograms[i][j]->getRange()->getEnd() << "] - SSE set to " << histograms[i][j]->m_sse[i] << " because non-usable data is not desired\n";
			}
		}
		std::cout << "\t\tTotal Privacy Saved: " << totalPrivacySaved[i];
		std::cout << "\n\t\tTotal SSE: " << sumSSEs[i];
		std::cout << "\n";
	}
}

//Automatic Suppression Algorithm 
//HARD CODED PARAMETER IN PRINTTREE: 0.05 as chance threshold
//given that we have the noise distributed in some way
//we will redistribute the noise until no answer can be nonsensical
//in a certain chance threshold
//e.g. 0.5 chance that an answer can come out as -2 is bad 
int HierarchyTree::ASA(double answer, float noise, float chanceThreshold) {
	int prob = 0;
	int successThreshold = (int)(chanceThreshold * 100);
	for (int i = 0; i < 100; i++) {
		CLaplaceDeviate laplacian;
		double dAdditiveNoise = laplacian.Get() * noise;
		double tempAnswer = answer;
		tempAnswer += dAdditiveNoise;
		if (tempAnswer < 0)
			prob++;
	}
	if (prob >= successThreshold)
		return 1;
	else
		return 0;
}
