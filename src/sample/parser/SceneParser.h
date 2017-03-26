#ifndef SCENEPARSER_H
#define SCENEPARSER_H

#include "core/model/Scene.h"
#include <cstdio>

class SceneParser {
public:
	SceneParser();
	bool readScene(
	    const char* directoryName, const char* filename, proxo::Scene& scene);

protected:
	void parseBody(proxo::Scene& scene);
	void parseWorld(proxo::Scene& scene);
	void parseStateCamera(proxo::Scene& scene);
	void parseStateMaterials(proxo::Scene& scene);
	void parseStateObjects(proxo::Scene& scene);
	void parseContainer(proxo::Container& container, proxo::mat4 matrix, proxo::Scene& scene);
	bool parseObject(proxo::Container& container, proxo::Scene& scene);
	bool parseLight(proxo::Container& container, proxo::Scene& scene);
	bool filterComment(const char* w);
	bool nextWord(char* w);
	bool nextInteger(int& out);
	bool nextIntegers(proxo::integer* out, int count);
	bool nextReal(proxo::real& out);
	bool nextReals(proxo::real* out, int count);
	void skipLine();
	bool equals(const char* a, const char* b);
	int wordToKey(char* w);

private:
	FILE* file;
	const char* directory;
	int state;
};

#endif // SCENEPARSER_H
