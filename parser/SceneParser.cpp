#include <cstdio>
#include <cstring>
#include "../core/math/Matrix.h"
#include "../core/math/Vector.h"
#include "SceneParser.h"
#include "StanfordParser.h"
#include "../core/model/Light.h"
#include "../core/model/light/Spot.h"
#include "../core/model/light/Sun.h"
#include "../core/model/light/Point.h"
#include "../core/model/light/Ambient.h"

enum {
    MATERIALS, OBJECTS, BODY, WORLD, SKYBOX,
    NAME, DATA,
    LIGHT, SUN, POINT, SPOT,
    DIR, POS,  CUTOFF, FALLOFF, ATTENUATION, INTENSITY,
    COLOR, COLOR255, AMBIENT, DIFFUSE, SPECULAR, SHININESS, EMISSIVE, NORMAL,
    OBJECT, CONTAINER, END,
    T, S, R, TX, TY, TZ, RX, RY, RZ,
    MATERIAL, CAMERA, FOV, LOOKAT, ZMAX,
    /** Ray Tracing
    REFRACTIVE_INDEX, REFRACTIVE_INDEX_R, REFRACTIVE_INDEX_G, REFRACTIVE_INDEX_B,
    REFLECT, REFLECT_R, REFLECT_G, REFLECT_B,
    // ABSORPTION, ABSORPTION_R, ABSORPTION_G, ABSORPTION_B, // <=> 1-COLOR
    */
    KEY_COUNT
};
static const char * keyName[KEY_COUNT] = {
    "materials","objects", "body", "world", "skybox",
    "name", "data",
    "light", "sun", "point", "spot",
    "dir", "pos", "cutoff", "falloff", "attenuation", "intensity",
    "color", "color255", "ambient", "diffuse", "specular", "shininess", "emissive", "normal",
    "object", "container", "end",
    "t", "s", "r", "tx", "ty", "tz", "rx", "ry", "rz",
    "material", "camera", "fov", "lookat", "zmax",
};
char commmentCharacter = '%';

enum {ST_MAIN, ST_MATERIALS, ST_OBJECTS, ST_WORLD, ST_BODY, ST_CAMERA, ST_SKYBOX, ST_ERROR};

SceneParser::SceneParser()
{
    //ctor
}

bool SceneParser::readScene(const char * directoryName, const char * filename, Scene & scene) {
    char fullPath[256];
    strcpy(fullPath, directoryName);
    strcat(fullPath, filename);
    directory = directoryName;
    file = fopen(fullPath, "r");
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
                    if(word[0] != '\0')
                        state = ST_ERROR;
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
            break;
        }
    }

    fclose(file);
    if(state == ST_ERROR || state != ST_MAIN)
        return false;
    return true;
}

void SceneParser::parseWorld(Scene & scene) {
    int o,c,l;
    if(scene.world != NULL || !nextInteger(o) || !nextInteger(c) || !nextInteger(l)) {
        state = ST_ERROR;
        return;
    }
    scene.world = new Container(o, c, l);
    parseContainer(*scene.world, NULL, scene);
    state = ST_MAIN;
}

void SceneParser::parseBody(Scene & scene) {
    int o,c,l;
    if(scene.body != NULL || !nextInteger(o) || !nextInteger(c) || !nextInteger(l)) {
        state = ST_ERROR;
        return;
    }
    scene.body = new Container(o, c, l);
    parseContainer(*scene.body, NULL, scene);
    state = ST_MAIN;
}

void SceneParser::parseStateCamera(Scene & scene) {
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
            if(!nextReal(fov) || fov <= 0 || fov >= 180) {
                state = ST_ERROR;
                break;
            }
            scene.camera.setFieldOfView(toRadians(fov));
            break;
        case ZMAX:
            real z;
            if(!nextReal(z)) {
                state = ST_ERROR;
                break;
            }
            scene.camera.setDepthMax(z);
            break;
        default:
            state = ST_ERROR;
            break;
        }
    }
    if(state == ST_CAMERA)
        state = ST_MAIN;
}

void SceneParser::parseStateMaterials(Scene & scene) {
    char word[128];
    real r;
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
        case END:
            state = ST_MAIN;
            break;
        default:
            state = ST_ERROR;
            break;
        }
    }
}

void SceneParser::parseStateObjects(Scene & scene) {
    char word[128];
    std::string currentName;
    StanfordParser objectParser;
    while(state == ST_OBJECTS && nextWord(word)) {
        int key = wordToKey(word);
        switch(key) {
        case NAME:
            if(!nextWord(word)) {
                state = ST_ERROR;
                break;
            }
            currentName = word;
            if(scene.objects.count(currentName) == 0)
                scene.objects[currentName] = new Object;
            break;
        case DATA: {
            if(!nextWord(word) || currentName.empty()) {
                state = ST_ERROR;
                break;
            }
            char fullPath[256];
            strcpy(fullPath, directory);
            strcat(fullPath, word);
            if(!objectParser.readObject(fullPath, *scene.objects[currentName])) {
                state = ST_ERROR;
                printf("Error loading object \"%s\"\n" ,fullPath);
            }
            } break;
        case COLOR:
            if(!nextWord(word) || currentName.empty()) {
                state = ST_ERROR;
                break;
            }
            // TODO Load Texture
            break;
        case NORMAL:
            if(!nextWord(word) || currentName.empty()) {
                state = ST_ERROR;
                break;
            }
            // TODO Load Texture
            break;
        case AMBIENT:
            if(!nextWord(word) || currentName.empty()) {
                state = ST_ERROR;
                break;
            }
            // TODO Load Texture
            break;
        case DIFFUSE:
            if(!nextWord(word) || currentName.empty()) {
                state = ST_ERROR;
                break;
            }
            // TODO Load Texture
            break;
        case SPECULAR:
            if(!nextWord(word) || currentName.empty()) {
                state = ST_ERROR;
                break;
            }
            // TODO Load Texture
            break;
        case END:
            state = ST_MAIN;
            break;
        default:
            state = ST_ERROR;
            break;
        }
    }
}

void SceneParser::parseContainer(Container & container, mat4 matrix, Scene & scene) {
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
            int o,c,l;
            if(!nextInteger(o) || !nextInteger(c) || !nextInteger(l)) {
                state = ST_ERROR;
                return;
            }
            Container * cont = new Container(o, c, l);
            mat4 mat = newMatrix();
            container.addContainer(cont, mat);
            parseContainer(*cont, mat, scene);
            } break;
        case END:
            return;
        default:
            state = ST_ERROR;
            break;
        }
    }
}

bool SceneParser::parseObject(Container & container, Scene & scene) {
    char word[128];
    real x[4];
    std::string name;

    if(!nextWord(word)) {
        state = ST_ERROR;
        return false;
    }
    name = word;
    if(scene.objects.count(name) == 0) {
        state = ST_ERROR;
        return false;
    }

    mat4 matrix = newMatrix();
    Object * object = scene.objects[name];
    Material * material = scene.materials["default"];

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
            int o,c,l;
            if(!nextInteger(o) || !nextInteger(c) || !nextInteger(l)) {
                state = ST_ERROR;
                return false;
            }
            Container * cont = new Container(o, c, l);
            mat4 mat = newMatrix();
            container.addContainer(cont, mat);
            parseContainer(*cont, mat, scene);
            } return false;
        case END:
            object->updateBoundingSphere();
            container.addObject(object, matrix, material);
            return true;
        default:
            state = ST_ERROR;
            break;
        }
    }
    return false;
}

bool SceneParser::parseLight(Container & container, Scene & scene) {
    real x[4];
    mat4 matrix = newMatrix();
    char word[128];
    SpotLight * spot;
    PointLight * point;
    SunLight * sun;
    AmbientLight * ambient;
    Light * light = NULL;

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
                state = ST_ERROR;
                break;
            }
            spot = new SpotLight;
            light = spot;
            break;
        case SUN:
            if(light != NULL) {
                state = ST_ERROR;
                break;
            }
            sun = new SunLight;
            light = sun;
            break;
        case POINT:
            if(light != NULL) {
                state = ST_ERROR;
                break;
            }
            point = new PointLight;
            light = point;
            break;
        case AMBIENT:
            if(light != NULL) {
                state = ST_ERROR;
                break;
            }
            ambient = new AmbientLight;
            light = ambient;
            break;

        case LIGHT:
            if(light == NULL) {
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
                delete[] matrix;
                state = ST_ERROR;
            }
            else {
                container.addLight(light, matrix);
                int o,c,l;
                if(!nextInteger(o) || !nextInteger(c) || !nextInteger(l)) {
                    state = ST_ERROR;
                    return false;
                }
                Container * cont = new Container(o, c, l);
                mat4 mat = newMatrix();
                container.addContainer(cont, mat);
                parseContainer(*cont, mat, scene);
            }
            return false;
        case END:
            if(light == NULL) {
                delete[] matrix;
                state = ST_ERROR;
            } else {
                container.addLight(light, matrix);
            }
            return true;
        default:
            state = ST_ERROR;
            break;
        }
    }
    return false;
}

bool SceneParser::filterComment(const char * w) {
    if(w[0] == commmentCharacter) {
        skipLine();
        return true;
    }
    else {
        return false;
    }
}

bool SceneParser::nextWord(char * w) {
    w[0] = '\0';
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

bool SceneParser::nextInteger(int & out) {
    char w[20];
    if(fscanf(file, "%s", w) == 1) {
        if(filterComment(w))
            return nextInteger(out);
        else {
            out = atoi(w);
            return true;
        }
    }
    else {
        printf("%d nextInteger failed\n", state);
        return false;
    }
}

bool SceneParser::nextReal(real & out) {
    char w[20];
    if(fscanf(file, "%s", w) == 1) {
        if(filterComment(w))
            return nextReal(out);
        else {
            out = (real) atof(w);
            return true;
        }
    }
    else {
        printf("%d nextReal failed\n", state);
        return false;
    }
}

bool SceneParser::nextReals(real * out, int count) {
    char w[20];
    int total = count;
    while(--count >= 0) {
        if(fscanf(file, "%s", w) == 1) {
            if(!filterComment(w)) {
                *out = (real) atof(w);
                out++;
            } else {
                count++;
            }
        }
        else {
            printf("%d nextReals failed %d/%d\n", state, count+1, total);
            return false;
        }
    }
    return true;
}

void SceneParser::skipLine() {
    int c;
    while((c = getc(file)) != '\n' && c != EOF);
}

bool SceneParser::equals(const char * a, const char * b) {
    return strcmp(a, b) == 0;
}

int SceneParser::wordToKey(char * w) {
    for(int i = 0; i < KEY_COUNT; i++) {
        if(equals(keyName[i], w)) {
            return i;
        }
    }
    printf("%d Unknown key word \"%s\"\n", state, w);
    return -1;
}
