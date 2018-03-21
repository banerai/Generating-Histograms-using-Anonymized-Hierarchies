#include "Range.h"

Range::Range(int beginning, int end) {
	m_beginning = beginning;
	m_end = end;
}

Range::~Range() {
	delete &m_beginning;
	delete &m_end;
	delete &m_numOriginalValuesStored;
	delete[] m_originalValuesStored;
}

void Range::transferValues(Range * r) {
	for (int i = 0; i < r->m_numOriginalValuesStored; i++) {
		if (r->m_originalValuesStored[i] >= m_beginning) {
			m_originalValuesStored = (r->m_originalValuesStored + i);
			m_numOriginalValuesStored = (r->m_numOriginalValuesStored - i);
			break;
		}
	}
}

int Range::getEnd() { return m_end; }
int Range::getBeginning() { return m_beginning; }