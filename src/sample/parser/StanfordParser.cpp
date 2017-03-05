#include "StanfordParser.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

using namespace proxo;

#define SUPPORTED_FORMAT_VERSION "1.0"

enum { HEADER, HEADER_VERTEX, HEADER_FACE, VERTEX_LIST, FACE_LIST, END, ERROR };
enum {
	PLY,
	FORMAT,
	ASCII,
	COMMENT,
	END_HEADER,
	ELEMENT,
	VERTEX,
	FACE,
	EDGE,
	PROPERTY,
	LIST,
	VERTEX_INDICES,
	FLOAT,
	DOUBLE,
	CHAR,
	UCHAR,
	SHORT,
	USHORT,
	INT,
	UINT,
	X,
	Y,
	Z,
	RED,
	GREEN,
	BLUE,
	NX,
	NY,
	NZ,
	S,
	T,
	KEY_COUNT
};
static const char* keyName[KEY_COUNT] = { "ply", "format", "ascii", "comment",
	"end_header", "element", "vertex", "face", "edge", "property", "list",
	"vertex_indices", "float", "double", "char", "uchar", "short", "ushort",
	"int", "uint", "x", "y", "z", "red", "green", "blue", "nx", "ny", "nz", "s",
	"t" };

bool StanfordParser::readObject(const char* filename, Object& object)
{
	file = fopen(filename, "r");
	if(file == NULL) {
		printf("Cannot open file \"%s\"\n", filename);
		return false;
	}

	std::vector<positive> additionnalFaces;

	char word[128];
	int state = HEADER;
	int key;
	std::vector<int> vertexKeys;
	int vertexReadCount = 0;
	int faceReadCount   = 0;

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
								state                = HEADER_VERTEX;
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
								state             = HEADER_FACE;
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
							object.vertices[vertexReadCount * VEC4_SCALARS_COUNT
							    + 0] = value;
							break;
						case Y:
							object.vertices[vertexReadCount * VEC4_SCALARS_COUNT
							    + 1] = value;
							break;
						case Z:
							object.vertices[vertexReadCount * VEC4_SCALARS_COUNT
							    + 2] = value;
							break;
						case NX:
							object.normals[vertexReadCount * VEC4_SCALARS_COUNT
							    + 0] = value;
							break;
						case NY:
							object.normals[vertexReadCount * VEC4_SCALARS_COUNT
							    + 1] = value;
							break;
						case NZ:
							object.normals[vertexReadCount * VEC4_SCALARS_COUNT
							    + 2] = value;
							break;
						case RED:
							object.colors[vertexReadCount * VEC4_SCALARS_COUNT
							    + 0] = value / 255;
							break;
						case GREEN:
							object.colors[vertexReadCount * VEC4_SCALARS_COUNT
							    + 1] = value / 255;
							break;
						case BLUE:
							object.colors[vertexReadCount * VEC4_SCALARS_COUNT
							    + 2] = value / 255;
							break;
						case S:
							object.texture_mapping[vertexReadCount
							        * VEC2_SCALARS_COUNT
							    + 0] = value;
							break;
						case T:
							object.texture_mapping[vertexReadCount
							        * VEC2_SCALARS_COUNT
							    + 1] = value;
							break;
					}
				}
				object.vertices[vertexReadCount * VEC4_SCALARS_COUNT + 3] = 1;
				object.normals[vertexReadCount * VEC4_SCALARS_COUNT + 3]  = 0;
				vertexReadCount++;
				if(vertexReadCount == (int) object.verticesCount)
					state = FACE_LIST;
				break;

			case FACE_LIST:
				int count = atoi(word);
				positive firstVertex, previousVertex;
				for(int i = 0; i < count; i++) {
					int vertex = nextInteger();
					if(i == 0)
						firstVertex = vertex;
					if(i < 3)
						object.faces[faceReadCount * 3 + i] = vertex;
					else {
						additionnalFaces.push_back(firstVertex);
						additionnalFaces.push_back(previousVertex);
						additionnalFaces.push_back(vertex);
					}
					previousVertex = vertex;
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

	if(additionnalFaces.size() > 0) {
		positive* moreFaces =
		    new positive[(object.facesCount + additionnalFaces.size() / 3) * 3];
		memcpy(
		    moreFaces, object.faces, object.facesCount * 3 * sizeof(positive));
		memcpy(moreFaces + object.facesCount * 3, additionnalFaces.data(),
		    additionnalFaces.size() * sizeof(positive));
		delete[] object.faces;
		object.faces = moreFaces;
		object.facesCount += additionnalFaces.size() / 3;
	}

	if(state != END)
		return false;

	return true;
}

bool StanfordParser::nextWord(char* w)
{
	return fscanf(file, "%s", w) == 1;
}

int StanfordParser::nextInteger()
{
	char w[20];
	char* endptr;

	if(!nextWord(w))
		return 0;

	integer value = strtoll(w, &endptr, 10);
	if(endptr == w || *endptr != '\0')
		return 0;

	return value;
}

real StanfordParser::nextReal()
{
	char w[20];
	char* endptr;

	if(!nextWord(w))
		return 0.0;

	real value = strtod(w, &endptr);
	if(endptr == w || *endptr != '\0')
		return 0.0;

	return value;
}

void StanfordParser::skipLine()
{
	int c;
	while((c = getc(file)) != '\n' && c != EOF)
		;
}

bool StanfordParser::equals(const char* a, const char* b)
{
	return strcmp(a, b) == 0;
}

int StanfordParser::wordToKey(char* w)
{
	for(int i = 0; i < KEY_COUNT; i++) {
		if(equals(keyName[i], w))
			return i;
	}
	return -1;
}
