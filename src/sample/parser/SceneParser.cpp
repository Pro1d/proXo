#include "SceneParser.h"
#include "../sdl/TextureLoader.h"
#include "StanfordParser.h"
#include "core/math/Matrix.h"
#include "core/math/Vector.h"
#include "core/model/Light.h"
#include "core/model/light/Ambient.h"
#include "core/model/light/Point.h"
#include "core/model/light/Spot.h"
#include "core/model/light/Sun.h"
#include <cstdio>
#include <cstring>

using namespace proxo;

enum {
	MATERIALS,
	OBJECTS,
	BODY,
	WORLD,
	SKYBOX,
	NAME,
	DATA,
	LIGHT,
	SUN,
	POINT,
	SPOT,
	DIR,
	POS,
	CUTOFF,
	FALLOFF,
	ATTENUATION,
	INTENSITY,
	COLOR,
	COLOR255,
	AMBIENT,
	DIFFUSE,
	SPECULAR,
	SHININESS,
	EMISSIVE,
	NORMAL,
	REFRACTIVE_INDEX,
	ABSORBTION,
	OBJECT,
	CONTAINER,
	END,
	T,
	S,
	R,
	TX,
	TY,
	TZ,
	RX,
	RY,
	RZ,
	MATERIAL,
	CAMERA,
	FOV,
	LOOKAT,
	ZMAX,
	APERTURE,
	FOCUS,
	AUTOFOCUS,
	TARGET,
	/** Ray Tracing
    REFRACTIVE_INDEX, REFRACTIVE_INDEX_R, REFRACTIVE_INDEX_G,
    REFRACTIVE_INDEX_B,
    REFLECT, REFLECT_R, REFLECT_G, REFLECT_B,
    // ABSORPTION, ABSORPTION_R, ABSORPTION_G, ABSORPTION_B, // <=> 1-COLOR
    */
	KEY_COUNT
};
static const char* keyName[KEY_COUNT] = { "materials", "objects", "body",
	"world", "skybox", "name", "data", "light", "sun", "point", "spot", "dir",
	"pos", "cutoff", "falloff", "attenuation", "intensity", "color", "color255",
	"ambient", "diffuse", "specular", "shininess", "emissive", "normal",
	"refractive_index", "absorption", "object", "container", "end", "t", "s",
	"r", "tx", "ty", "tz", "rx", "ry", "rz", "material", "camera", "fov",
	"lookat", "zmax", "aperture", "focus", "autofocus", "target" };
char commmentCharacter = '#';

enum {
	ST_MAIN,
	ST_MATERIALS,
	ST_OBJECTS,
	ST_WORLD,
	ST_BODY,
	ST_CAMERA,
	ST_SKYBOX,
	ST_ERROR
};

SceneParser::SceneParser()
{
	// ctor
}

bool SceneParser::readScene(
    const char* directoryName, const char* filename, Scene& scene)
{
	char fullPath[256];
	strcpy(fullPath, directoryName);
	strcat(fullPath, filename);
	directory = directoryName;
	file      = fopen(fullPath, "r");
	if(file == NULL)
		return false;

	state = ST_MAIN;
	char word[128];
	int key;
	bool endFile = false;

	while(state != ST_ERROR && !endFile) {
		switch(state) {
			case ST_MAIN:
				while(state == ST_MAIN) {
					if(!nextWord(word)) {
						endFile = true;
						break;
					}
					key = wordToKey(word);
					switch(key) {
						case MATERIALS:
							state = ST_MATERIALS;
							break;
						case OBJECTS:
							state = ST_OBJECTS;
							break;
						case SKYBOX:
							nextWord(word);
							/// TODO read skybox filename:word
							scene.skybox = NULL;
							break;
						case WORLD:
							state = ST_WORLD;
							break;
						case BODY:
							state = ST_BODY;
							break;
						case CAMERA:
							state = ST_CAMERA;
							break;
						default:
							if(word[0] != '\0') {
								state = ST_ERROR;
								printf("Error: Unexpected: \'%s\'\n", word);
							}
							break;
					}
				}
				break;
			case ST_MATERIALS:
				parseStateMaterials(scene);
				break;
			case ST_OBJECTS:
				parseStateObjects(scene);
				break;
			case ST_WORLD:
				parseWorld(scene);
				break;
			case ST_BODY:
				parseBody(scene);
				break;
			case ST_CAMERA:
				parseStateCamera(scene);
				break;
			default:
				state = ST_ERROR;
				printf("Error: Unexpected: \'%s\'\n", word);
				break;
		}
	}

	if(scene.world == NULL)
		scene.world = new Container(0,0,0);

	if(scene.body == NULL)
		scene.body = new Container(0,0,0);

	fclose(file);
	if(state == ST_ERROR || state != ST_MAIN)
		return false;
	return true;
}

void SceneParser::parseWorld(Scene& scene)
{
	int o, c, l;
	if(scene.world != NULL) {
		printf("Error: Block \'world\' is already defined\n");
		state = ST_ERROR;
		return;
	}
	if(!nextInteger(o) || !nextInteger(c) || !nextInteger(l)) {
		state = ST_ERROR;
		return;
	}
	scene.world = new Container(o, c, l);
	parseContainer(*scene.world, NULL, scene);
	state = ST_MAIN;
}

void SceneParser::parseBody(Scene& scene)
{
	int o, c, l;
	if(scene.body != NULL) {
		printf("Error: Block \'body\' is already defined\n");
		state = ST_ERROR;
		return;
	}
	if(!nextInteger(o) || !nextInteger(c) || !nextInteger(l)) {
		state = ST_ERROR;
		return;
	}
	scene.body = new Container(o, c, l);
	parseContainer(*scene.body, NULL, scene);
	state = ST_MAIN;
}

void SceneParser::parseStateCamera(Scene& scene)
{
	char word[128];
	real x[VEC3_SCALARS_COUNT];
	while(state == ST_CAMERA && nextWord(word)) {
		int key = wordToKey(word);
		switch(key) {
			case MATERIALS:
				state = ST_MATERIALS;
				break;
			case OBJECTS:
				state = ST_OBJECTS;
				break;
			case SKYBOX:
				state = ST_SKYBOX;
				break;
			case WORLD:
				state = ST_WORLD;
				break;
			case BODY:
				state = ST_BODY;
				break;
			case CAMERA:
				// ignore ?
				break;

			// only camera
			case LOOKAT:
				if(!nextReals(x, 3)) {
					state = ST_ERROR;
					break;
				}
				scene.camera.lookAt(x);
				break;
			case POS:
				if(!nextReals(x, 3)) {
					state = ST_ERROR;
					break;
				}
				scene.camera.setPosition(x);
				break;
			case DIR:
				if(!nextReals(x, 3)) {
					state = ST_ERROR;
					break;
				}
				scene.camera.setDirection(x);
				break;
			case FOV:
				real fov;
				if(!nextReal(fov)) {
					state = ST_ERROR;
					break;
				}
				else if(fov <= 0 || fov >= 180) {
					printf("Error: The value of fov must be in range 0..180\n");
					state = ST_ERROR;
					break;
				}
				scene.camera.setFieldOfView(toRadians(fov));
				scene.camera.updateProjection();
				break;
			case ZMAX:
				real z;
				if(!nextReal(z)) {
					state = ST_ERROR;
					break;
				}
				scene.camera.setDepthMax(z);
				scene.camera.updateProjection();
				break;
			case APERTURE:
				real radius;
				if(!nextReal(radius)) {
					state = ST_ERROR;
					break;
				}
				scene.camera.setAperture(radius);
				break;
			case FOCUS:
				real zfocus;
				if(!nextReal(zfocus)) {
					state = ST_ERROR;
					break;
				}
				scene.camera.setDistanceFocus(zfocus);
				break;
			case AUTOFOCUS:
				scene.camera.setAutoFocus(true);
				break;
			case TARGET:
				integer size[3];
				if(!nextIntegers(size, 3)) {
					state = ST_ERROR;
					return;
				}
				else if(size[0] <= 0 || size[1] <= 0 || size[2] < 0) {
					printf("Error: The render target's width and height must be > 0 and supersampling >= 0\n"); 
					state = ST_ERROR;
					break;
				}
				scene.camera.setRenderTarget((positive) size[0], (positive) size[1], (positive) size[2]);
				scene.camera.updateProjection();
				break;
			default:
				state = ST_ERROR;
				printf("Error: Unexpected: \'%s\'\n", word);
				break;
		}
	}
	if(state == ST_CAMERA)
		state = ST_MAIN;
}

void SceneParser::parseStateMaterials(Scene& scene)
{
	char word[128];
	real r;
	real a[3];
	std::string currentName;

	while(state == ST_MATERIALS && nextWord(word)) {
		int key = wordToKey(word);
		switch(key) {
			case NAME:
				if(!nextWord(word)) {
					state = ST_ERROR;
					break;
				}
				currentName = word;
				if(scene.materials.count(currentName) == 0)
					scene.materials[currentName] = new Material;
				break;
			case AMBIENT:
				if(!nextReal(r) || currentName.empty()) {
					state = ST_ERROR;
					break;
				}
				scene.materials[currentName]->ambient = r;
				break;
			case DIFFUSE:
				if(!nextReal(r) || currentName.empty()) {
					state = ST_ERROR;
					break;
				}
				scene.materials[currentName]->diffuse = r;
				break;
			case SPECULAR:
				if(!nextReal(r) || currentName.empty()) {
					state = ST_ERROR;
					break;
				}
				scene.materials[currentName]->specular = r;
				break;
			case SHININESS:
				if(!nextReal(r) || currentName.empty()) {
					state = ST_ERROR;
					break;
				}
				scene.materials[currentName]->shininess = r;
				break;
			case EMISSIVE:
				if(!nextReal(r) || currentName.empty()) {
					state = ST_ERROR;
					break;
				}
				scene.materials[currentName]->shininess = r;
				break;
			case REFRACTIVE_INDEX:
				if(!nextReal(r) || currentName.empty()) {
					state = ST_ERROR;
					break;
				}
				scene.materials[currentName]->refractiveIndex = r;
				break;
			case ABSORBTION:
				if(!nextReals(a, 3) || currentName.empty()) {
					state = ST_ERROR;
					break;
				}
				scene.materials[currentName]->depthAbsorption[0] = a[0];
				scene.materials[currentName]->depthAbsorption[1] = a[1];
				scene.materials[currentName]->depthAbsorption[2] = a[2];
				break;
			case END:
				state = ST_MAIN;
				break;
			default:
				state = ST_ERROR;
				printf("Error: Unexpected: \'%s\'\n", word);
				break;
		}
	}
}

void SceneParser::parseStateObjects(Scene& scene)
{
	char word[128];
	std::string currentName;
	StanfordParser objectParser;
	TextureLoader textureLoader;
	char fullPath[256];

	while(state == ST_OBJECTS && nextWord(word)) {
		int key = wordToKey(word);
		switch(key) {
			case NAME:
				if(!nextWord(word)) {
					state = ST_ERROR;
					break;
				}
				// Finish the previous object
				if(!currentName.empty()) {
					scene.objects[currentName]->texture =
					    textureLoader.createTexture();
				}
				currentName = word;
				if(scene.objects.count(currentName) == 0) {
					scene.objects[currentName] = new Object;
				}
				else {
					printf("Error: The object %s has already been created.",
					    currentName.c_str());
					state = ST_ERROR;
				}
				break;
			case DATA: {
				if(!nextWord(word) || currentName.empty()) {
					state = ST_ERROR;
					break;
				}
				char fullPath[256];
				strcpy(fullPath, directory);
				strcat(fullPath, word);
				if(!objectParser.readObject(
				       fullPath, *scene.objects[currentName])) {
					state = ST_ERROR;
					printf("Error loading object \"%s\"\n", fullPath);
				}
			} break;
			case NORMAL:
				if(!nextWord(word) || currentName.empty()) {
					state = ST_ERROR;
					break;
				}
				strcpy(fullPath, directory);
				strcat(fullPath, word);
				textureLoader.addImageFile(
				    std::string(fullPath), Texture::NORMAL_XYZ);
				break;
			case AMBIENT:
				if(!nextWord(word) || currentName.empty()) {
					state = ST_ERROR;
					break;
				}
				strcpy(fullPath, directory);
				strcat(fullPath, word);
				textureLoader.addImageFile(
				    std::string(fullPath), Texture::AMBIENT_I);
				break;
			case DIFFUSE:
				if(!nextWord(word) || currentName.empty()) {
					state = ST_ERROR;
					break;
				}
				strcpy(fullPath, directory);
				strcat(fullPath, word);
				textureLoader.addImageFile(
				    std::string(fullPath), Texture::DIFFUSE_RGB);
				break;
			case SPECULAR:
				if(!nextWord(word) || currentName.empty()) {
					state = ST_ERROR;
					break;
				}
				strcpy(fullPath, directory);
				strcat(fullPath, word);
				textureLoader.addImageFile(
				    std::string(fullPath), Texture::SPECULAR_I);
				break;
			case SHININESS:
				if(!nextWord(word) || currentName.empty()) {
					state = ST_ERROR;
					break;
				}
				strcpy(fullPath, directory);
				strcat(fullPath, word);
				textureLoader.addImageFile(
				    std::string(fullPath), Texture::SHININESS_I);
				break;
			case END:
				// Finish the previous object
				if(!currentName.empty()) {
					scene.objects[currentName]->texture =
					    textureLoader.createTexture();
				}
				state = ST_MAIN;
				break;
			default:
				state = ST_ERROR;
				printf("Error: Unexpected: \'%s\'\n", word);
				break;
		}
	}
}

void SceneParser::parseContainer(
    Container& container, mat4 matrix, Scene& scene)
{
	char word[128];
	real x[4];
	while(state != ST_ERROR && nextWord(word)) {
		int key = wordToKey(word);
		switch(key) {
			case T:
				if(matrix == NULL || !nextReals(x, 3)) {
					state = ST_ERROR;
					break;
				}
				applyTranslate(matrix, x[0], x[1], x[2]);
				break;
			case TX:
				if(matrix == NULL || !nextReals(x, 1)) {
					state = ST_ERROR;
					break;
				}
				applyTranslate(matrix, x[0], 0, 0);
				break;
			case TY:
				if(matrix == NULL || !nextReals(x, 1)) {
					state = ST_ERROR;
					break;
				}
				applyTranslate(matrix, 0, x[0], 0);
				break;
			case TZ:
				if(matrix == NULL || !nextReals(x, 1)) {
					state = ST_ERROR;
					break;
				}
				applyTranslate(matrix, 0, 0, x[0]);
				break;
			case S:
				if(matrix == NULL || !nextReals(x, 1)) {
					state = ST_ERROR;
					break;
				}
				applyScale(matrix, x[0]);
				break;
			case R:
				if(matrix == NULL || !nextReals(x, 4)) {
					state = ST_ERROR;
					break;
				}
				applyRotate(matrix, toRadians(x[0]), x[1], x[2], x[3]);
				break;
			case RX:
				if(matrix == NULL || !nextReals(x, 1)) {
					state = ST_ERROR;
					break;
				}
				applyRotate(matrix, toRadians(x[0]), 1, 0, 0);
				break;
			case RY:
				if(matrix == NULL || !nextReals(x, 1)) {
					state = ST_ERROR;
					break;
				}
				applyRotate(matrix, toRadians(x[0]), 0, 1, 0);
				break;
			case RZ:
				if(matrix == NULL || !nextReals(x, 1)) {
					state = ST_ERROR;
					break;
				}
				applyRotate(matrix, toRadians(x[0]), 0, 0, 1);
				break;
			case LIGHT:
				if(parseLight(container, scene))
					return;
				break;
			case OBJECT:
				if(parseObject(container, scene))
					return;
				break;
			case CONTAINER: {
				int o, c, l;
				if(!nextInteger(o) || !nextInteger(c) || !nextInteger(l)) {
					state = ST_ERROR;
					return;
				}
				Container* cont = new Container(o, c, l);
				mat4 mat        = newMatrix();
				container.addContainer(cont, mat);
				parseContainer(*cont, mat, scene);
			} break;
			case END:
				return;
			default:
				state = ST_ERROR;
				printf("Error: Unexpected: \'%s\'\n", word);
				break;
		}
	}
}

bool SceneParser::parseObject(Container& container, Scene& scene)
{
	char word[128];
	real x[4];
	std::string name;

	if(!nextWord(word)) {
		printf("Error: Missing object\'s name\n");
		state = ST_ERROR;
		return false;
	}
	name = word;
	if(scene.objects.count(name) == 0) {
		printf("Error: Unknown object \'%s\'\n", word);
		state = ST_ERROR;
		return false;
	}

	mat4 matrix        = newMatrix();
	Object* object     = scene.objects[name];
	Material* material = scene.materials["default"];

	while(state != ST_ERROR && nextWord(word)) {
		int key = wordToKey(word);
		switch(key) {
			case T:
				if(!nextReals(x, 3)) {
					state = ST_ERROR;
					break;
				}
				applyTranslate(matrix, x[0], x[1], x[2]);
				break;
			case TX:
				if(!nextReals(x, 1)) {
					state = ST_ERROR;
					break;
				}
				applyTranslate(matrix, x[0], 0, 0);
				break;
			case TY:
				if(!nextReals(x, 1)) {
					state = ST_ERROR;
					break;
				}
				applyTranslate(matrix, 0, x[0], 0);
				break;
			case TZ:
				if(!nextReals(x, 1)) {
					state = ST_ERROR;
					break;
				}
				applyTranslate(matrix, 0, 0, x[0]);
				break;
			case S:
				if(!nextReals(x, 1)) {
					state = ST_ERROR;
					break;
				}
				applyScale(matrix, x[0]);
				break;
			case R:
				if(!nextReals(x, 4)) {
					state = ST_ERROR;
					break;
				}
				applyRotate(matrix, toRadians(x[0]), x[1], x[2], x[3]);
				break;
			case RX:
				if(!nextReals(x, 1)) {
					state = ST_ERROR;
					break;
				}
				applyRotate(matrix, toRadians(x[0]), 1, 0, 0);
				break;
			case RY:
				if(!nextReals(x, 1)) {
					state = ST_ERROR;
					break;
				}
				applyRotate(matrix, toRadians(x[0]), 0, 1, 0);
				break;
			case RZ:
				if(!nextReals(x, 1)) {
					state = ST_ERROR;
					break;
				}
				applyRotate(matrix, toRadians(x[0]), 0, 0, 1);
				break;
			case MATERIAL:
				if(!nextWord(word)) {
					state = ST_ERROR;
					break;
				}
				name = word;
				if(scene.materials.count(name) == 0) {
					printf("Error: Unknown material \'%s\'\n", word);
					state = ST_ERROR;
					break;
				}
				material = scene.materials[name];
				break;
			case LIGHT:
				object->updateBoundingSphere();
				container.addObject(object, matrix, material);
				return parseLight(container, scene);
			case OBJECT:
				object->updateBoundingSphere();
				container.addObject(object, matrix, material);
				return parseObject(container, scene);
			case CONTAINER: {
				object->updateBoundingSphere();
				container.addObject(object, matrix, material);
				int o, c, l;
				if(!nextInteger(o) || !nextInteger(c) || !nextInteger(l)) {
					state = ST_ERROR;
					return false;
				}
				Container* cont = new Container(o, c, l);
				mat4 mat        = newMatrix();
				container.addContainer(cont, mat);
				parseContainer(*cont, mat, scene);
			}
				return false;
			case END:
				object->updateBoundingSphere();
				container.addObject(object, matrix, material);
				return true;
			default:
				state = ST_ERROR;
				printf("Error: Unexpected: \'%s\'\n", word);
				break;
		}
	}
	return false;
}

bool SceneParser::parseLight(Container& container, Scene& scene)
{
	real x[4];
	mat4 matrix = newMatrix();
	char word[128];
	SpotLight* spot;
	PointLight* point;
	SunLight* sun;
	AmbientLight* ambient;
	Light* light = NULL;

	while(state != ST_ERROR && nextWord(word)) {
		int key = wordToKey(word);
		switch(key) {
			case T:
				if(!nextReals(x, 3)) {
					state = ST_ERROR;
					break;
				}
				applyTranslate(matrix, x[0], x[1], x[2]);
				break;
			case TX:
				if(!nextReals(x, 1)) {
					state = ST_ERROR;
					break;
				}
				applyTranslate(matrix, x[0], 0, 0);
				break;
			case TY:
				if(!nextReals(x, 1)) {
					state = ST_ERROR;
					break;
				}
				applyTranslate(matrix, 0, x[0], 0);
				break;
			case TZ:
				if(!nextReals(x, 1)) {
					state = ST_ERROR;
					break;
				}
				applyTranslate(matrix, 0, 0, x[0]);
				break;
			case S:
				if(!nextReals(x, 1)) {
					state = ST_ERROR;
					break;
				}
				applyScale(matrix, x[0]);
				break;
			case R:
				if(!nextReals(x, 4)) {
					state = ST_ERROR;
					break;
				}
				applyRotate(matrix, toRadians(x[0]), x[1], x[2], x[3]);
				break;
			case RX:
				if(!nextReals(x, 1)) {
					state = ST_ERROR;
					break;
				}
				applyRotate(matrix, toRadians(x[0]), 1, 0, 0);
				break;
			case RY:
				if(!nextReals(x, 1)) {
					state = ST_ERROR;
					break;
				}
				applyRotate(matrix, toRadians(x[0]), 0, 1, 0);
				break;
			case RZ:
				if(!nextReals(x, 1)) {
					state = ST_ERROR;
					break;
				}
				applyRotate(matrix, toRadians(x[0]), 0, 0, 1);
				break;
			case DIR:
				if(!nextReals(x, 3)) {
					state = ST_ERROR;
					break;
				}
				normalize(x);
				light->setDirection(x);
				break;
			case POS:
				if(!nextReals(x, 3)) {
					state = ST_ERROR;
					break;
				}
				light->setPosition(x);
				break;
			// case LOOKAT:break;

			case COLOR:
				if(!nextReals(x, 3)) {
					state = ST_ERROR;
					break;
				}
				light->setColor(x);
				break;
			case FALLOFF:
				if(!nextReals(x, 1)) {
					state = ST_ERROR;
					break;
				}
				light->setFallOff(x[0]);
				break;
			case CUTOFF:
				if(!nextReals(x, 1)) {
					state = ST_ERROR;
					break;
				}
				light->setCutOff(x[0]);
				break;
			case INTENSITY:
				if(!nextReals(x, 1)) {
					state = ST_ERROR;
					break;
				}
				light->setIntensity(x[0]);
				break;
			case ATTENUATION:
				if(!nextReals(x, 1)) {
					state = ST_ERROR;
					break;
				}
				light->setReductionFactor(x[0]);
				break;

			case SPOT:
				if(light != NULL) {
					printf("Error: \'%s\', light type is already defined\n", word);
					state = ST_ERROR;
					break;
				}
				spot  = new SpotLight;
				light = spot;
				break;
			case SUN:
				if(light != NULL) {
					printf("Error: \'%s\', light type is already defined\n", word);
					state = ST_ERROR;
					break;
				}
				sun   = new SunLight;
				light = sun;
				break;
			case POINT:
				if(light != NULL) {
					printf("Error: \'%s\', light type is already defined\n", word);
					state = ST_ERROR;
					break;
				}
				point = new PointLight;
				light = point;
				break;
			case AMBIENT:
				if(light != NULL) {
					printf("Error: \'%s\', light type is already defined\n", word);
					state = ST_ERROR;
					break;
				}
				ambient = new AmbientLight;
				light   = ambient;
				break;

			case LIGHT:
				if(light == NULL) {
					printf("Error: uncomplete light definition\n");
					delete[] matrix;
					state = ST_ERROR;
				}
				else {
					container.addLight(light, matrix);
					return parseLight(container, scene);
				}
				return false;
			case OBJECT:
				if(light == NULL) {
					printf("Error: uncomplete light definition\n");
					delete[] matrix;
					state = ST_ERROR;
				}
				else {
					container.addLight(light, matrix);
					return parseObject(container, scene);
				}
				return false;
			case CONTAINER:
				if(light == NULL) {
					printf("Error: uncomplete light definition\n");
					delete[] matrix;
					state = ST_ERROR;
				}
				else {
					container.addLight(light, matrix);
					int o, c, l;
					if(!nextInteger(o) || !nextInteger(c) || !nextInteger(l)) {
						state = ST_ERROR;
						return false;
					}
					Container* cont = new Container(o, c, l);
					mat4 mat        = newMatrix();
					container.addContainer(cont, mat);
					parseContainer(*cont, mat, scene);
				}
				return false;
			case END:
				if(light == NULL) {
					printf("Error: uncomplete light definition\n");
					delete[] matrix;
					state = ST_ERROR;
				}
				else {
					container.addLight(light, matrix);
				}
				return true;
			default:
				state = ST_ERROR;
				printf("Error: Unexpected: \'%s\'\n", word);
				break;
		}
	}
	return false;
}

bool SceneParser::filterComment(const char* w)
{
	if(w[0] == commmentCharacter) {
		skipLine();
		return true;
	}
	else {
		return false;
	}
}

bool SceneParser::nextWord(char* w)
{
	w[0]      = '\0';
	int count = fscanf(file, "%s", w);
	if(count == 1) {
		if(filterComment(w))
			return nextWord(w);
		else
			return true;
	}
	else {
		return false;
	}
}

bool SceneParser::nextInteger(int& out)
{
	char w[20];
	char* endptr;
	bool success = false;

	if(fscanf(file, "%s", w) == 1) {
		if(filterComment(w))
			return nextInteger(out);
		else {
			out     = strtoll(w, &endptr, 10);
			success = (*endptr == '\0');
		}
	}

	if(!success)
		printf("Error: %d nextInteger failed\n", state);

	return success;
}

bool SceneParser::nextIntegers(integer* out, int count)
{
	char w[20];
	char* endptr;
	int total = count;
	while(--count >= 0) {
		bool success = false;
		if(fscanf(file, "%s", w) == 1) {
			if(!filterComment(w)) {
				*out = (integer) strtoll(w, &endptr, 10);
				out++;
				success = (*endptr == '\0');
			}
			else {
				count++;
				success = true;
			}
		}

		if(!success) {
			printf("Error: %d nextIntegers failed %d/%d\n", state, count + 1, total);
			return false;
		}
	}

	return true;
}

bool SceneParser::nextReal(real& out)
{
	char w[20];
	char* endptr;
	bool success = false;

	if(fscanf(file, "%s", w) == 1) {
		if(filterComment(w))
			return nextReal(out);
		else {
			out     = strtod(w, &endptr);
			success = (*endptr == '\0');
		}
	}

	if(!success)
		printf("Error: %d nextReal failed\n", state);

	return success;
}

bool SceneParser::nextReals(real* out, int count)
{
	char w[20];
	char* endptr;
	int total = count;
	while(--count >= 0) {
		bool success = false;
		if(fscanf(file, "%s", w) == 1) {
			if(!filterComment(w)) {
				*out = (real) strtod(w, &endptr);
				out++;
				success = (*endptr == '\0');
			}
			else {
				count++;
				success = true;
			}
		}

		if(!success) {
			printf("Error: %d nextReals failed %d/%d\n", state, count + 1, total);
			return false;
		}
	}

	return true;
}

void SceneParser::skipLine()
{
	int c;
	while((c = getc(file)) != '\n' && c != EOF)
		;
}

bool SceneParser::equals(const char* a, const char* b)
{
	return strcmp(a, b) == 0;
}

int SceneParser::wordToKey(char* w)
{
	for(int i = 0; i < KEY_COUNT; i++) {
		if(equals(keyName[i], w)) {
			return i;
		}
	}
	printf("Error: %d Unknown key word \"%s\"\n", state, w);
	return -1;
}
