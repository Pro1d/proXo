#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <vector>
#include "StanfordParser.h"
#include "../model/Object.h"

#define SUPPORTED_FORMAT_VERSION        "1.0"

enum {HEADER, HEADER_VERTEX, HEADER_FACE, VERTEX_LIST, FACE_LIST, END, ERROR};
enum {PLY, FORMAT, ASCII, COMMENT, END_HEADER,
    ELEMENT, VERTEX, FACE, EDGE, PROPERTY, LIST, VERTEX_INDICES,
    FLOAT, DOUBLE, CHAR, UCHAR, SHORT, USHORT, INT, UINT,
    X, Y, Z, RED, GREEN, BLUE, NX, NY, NZ, S, T,
    KEY_COUNT
};
static const char * keyName[KEY_COUNT] = {
    "ply", "format", "ascii", "comment", "end_header",
    "element", "vertex", "face", "edge", "property", "list", "vertex_indices",
    "float", "double", "char", "uchar", "short", "ushort", "int", "uint",
    "x", "y", "z", "red", "green", "blue", "nx", "ny", "nz", "s", "t"
};

bool StanfordParser::readObject(const char * filename, Object & object) {
    file = fopen(filename, "r");
    if(file == NULL) {
        printf("Cannot open file \"%s\"\n", filename);
        return false;
    }

    char word[128];
    int state = HEADER;
    int key;
    std::vector<int> vertexKeys;
    int vertexReadCount = 0;
    int faceReadCount = 0;

    while(nextWord(word)) {

        switch(state) {
        case HEADER:
            key = wordToKey(word);
            switch(key) {
            case PLY:
                break;
            case FORMAT:
                nextWord(word);
                key = wordToKey(word);
                if(key != ASCII) {
                    state = ERROR; // unsupported format
                    break;
                }
                nextWord(word);
                if(!equals(word, SUPPORTED_FORMAT_VERSION)) {
                    state = ERROR; // unsupported format
                    break;
                }
                break;
            case COMMENT:
                skipLine();
                break;
            case ELEMENT:
                nextWord(word);
                key = wordToKey(word);
                switch(key) {
                case VERTEX:
                    state = HEADER_VERTEX;
                    object.verticesCount = nextInteger();
                    break;
                }
                break;
            }
            break;

        case HEADER_VERTEX:
            key = wordToKey(word);
            switch(key) {
            case PROPERTY:
                // skip property type
                nextWord(word);
                // property name
                nextWord(word);
                key = wordToKey(word);
                vertexKeys.push_back(key);
                switch(key) {
                case X:
                    object.allocateVertices();
                    break;
                case NX:
                    object.allocateNormals();
                    break;
                case RED:
                    object.allocateColors();
                    break;
                case S:
                    object.allocateTextureMapping();
                    break;
                }
                break;
            case ELEMENT:
                nextWord(word);
                key = wordToKey(word);
                switch(key) {
                case FACE:
                    state = HEADER_FACE;
                    object.facesCount = nextInteger();
                    object.allocateFaces(object.facesCount);
                    break;
                }
                break;
            }

            break;
        case HEADER_FACE:
            key = wordToKey(word);
            switch(key) {
            case PROPERTY:
                skipLine();
                break;
            case END_HEADER:
                state = VERTEX_LIST;
                break;
            }
            break;

        case VERTEX_LIST:
            for(int i = 0; i < (int) vertexKeys.size(); i++) {
                real value = (i == 0) ? atof(word) : nextReal();
                switch(vertexKeys[i]) {
                case X:
                    object.vertices[vertexReadCount * VEC4_SCALARS_COUNT + 0] = value;
                    break;
                case Y:
                    object.vertices[vertexReadCount * VEC4_SCALARS_COUNT + 1] = value;
                    break;
                case Z:
                    object.vertices[vertexReadCount * VEC4_SCALARS_COUNT + 2] = value;
                    break;
                case NX:
                    object.normals[vertexReadCount * VEC4_SCALARS_COUNT + 0] = value;
                    break;
                case NY:
                    object.normals[vertexReadCount * VEC4_SCALARS_COUNT + 1] = value;
                    break;
                case NZ:
                    object.normals[vertexReadCount * VEC4_SCALARS_COUNT + 2] = value;
                    break;
                case RED:
                    object.colors[vertexReadCount * VEC4_SCALARS_COUNT + 0] = value/255;
                    break;
                case GREEN:
                    object.colors[vertexReadCount * VEC4_SCALARS_COUNT + 1] = value/255;
                    break;
                case BLUE:
                    object.colors[vertexReadCount * VEC4_SCALARS_COUNT + 2] = value/255;
                    break;
                case S:
                    object.texture_mapping[vertexReadCount * VEC2_SCALARS_COUNT + 0] = value;
                    break;
                case T:
                    object.texture_mapping[vertexReadCount * VEC2_SCALARS_COUNT + 1] = value;
                    break;
                }
            }
            object.vertices[vertexReadCount * VEC4_SCALARS_COUNT + 3] = 1;
            object.normals[vertexReadCount * VEC4_SCALARS_COUNT + 3] = 0;
            vertexReadCount++;
            if(vertexReadCount == (int) object.verticesCount)
                state = FACE_LIST;
            break;

        case FACE_LIST:
            int count = atoi(word);
            if(count > 3)
                printf("Warning, face with >3 vertices!"); // warning: only triangles are supported
            for(int i = 0; i < count; i++) {
                int vertex = nextInteger();
                if(i < 3)
                    object.faces[faceReadCount * 3 + i] = vertex;
            }
            faceReadCount++;
            if(faceReadCount == (int) object.facesCount)
                state = END;
            break;
        }

        if(state == END || state == ERROR)
            break;
    }
    fclose(file);

    if(state != END)
        return false;

    return true;
}

bool StanfordParser::nextWord(char * w) {
    return fscanf(file, "%s", w) == 1;
}

int StanfordParser::nextInteger() {
    char w[20];
    fscanf(file, "%s", w);
    return atoi(w);
}

real StanfordParser::nextReal() {
    char w[20];
    fscanf(file, "%s", w);
    return (real) atof(w);
}

void StanfordParser::skipLine() {
    int c;
    while((c = getc(file)) != '\n' && c != EOF);
}

bool StanfordParser::equals(const char * a, const char * b) {
    return strcmp(a, b) == 0;
}

int StanfordParser::wordToKey(char * w) {
    for(int i = 0; i < KEY_COUNT; i++) {
        if(equals(keyName[i], w))
            return i;
    }
    return -1;
}
