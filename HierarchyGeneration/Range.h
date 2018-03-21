#pragma once
//NEED TO MAKE THIS MORE PROTECTED
class Range {
private:
	int m_beginning;
	int m_end;

public:
	int m_numOriginalValuesStored;
	int * m_originalValuesStored;

	Range(int, int); //constructor
	~Range(); //destructor
	
	void setRange(int, int);

	void transferValues(Range*); //stores values in order
	void resizeRange(int, int); //resizes the range and updates its children respectively
	int getBeginning();
	int getEnd();
	
};