#ifndef __STANFORD_PARSER_H__
#define __STANFORD_PARSER_H__

#include "core/model/Object.h"

class StanfordParser {
public:
	bool readObject(const char* filename, Object& object);

private:
	bool nextWord(char* w);
	int nextInteger();
	real nextReal();
	void skipLine();
	bool equals(const char* a, const char* b);
	int wordToKey(char* w);

	FILE* file;
};

#endif // __STANFORD_H__
