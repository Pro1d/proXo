#ifndef SCENEPARSER_H
#define SCENEPARSER_H

#include "../core/model/Scene.h"
#include <cstdio>

class SceneParser
{
    public:
        SceneParser();
        bool readScene(const char * directoryName, const char * filename, Scene & scene);

    protected:
        void parseBody(Scene & scene);
        void parseWorld(Scene & scene);
        void parseStateCamera(Scene & scene);
        void parseStateMaterials(Scene & scene);
        void parseStateObjects(Scene & scene);
        void parseContainer(Container & container, mat4 matrix, Scene & scene);
        bool parseObject(Container & container, Scene & scene);
        bool parseLight(Container & container, Scene & scene);
        bool nextWord(char * w);
        bool nextInteger(int & out);
        bool nextReal(real & out);
        bool nextReals(real * out, int count);
        void skipLine();
        bool equals(const char * a, const char * b);
        int wordToKey(char * w);
    private:
        FILE * file;
        const char * directory;
        int state;
};

#endif // SCENEPARSER_H
