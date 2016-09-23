#include "CameraController.h"
#include <cmath>
#include "../core/math/basics.h"
#include "../core/math/Matrix.h"
#include <SDL/SDL.h>

CameraController::CameraController() :
    rotationStep(10),
    translateStep(0.1),
    stepDelay(500),
    timeLastStep(0),
    rotationSpeed(10),
    translateSpeed(0.1)
{
    keyMapping[SDLK_w] = MOVE_FORWARD;
    keyMapping[SDLK_s] = MOVE_BACKWARD;
    keyMapping[SDLK_a] = MOVE_LEFT;
    keyMapping[SDLK_d] = MOVE_RIGHT;
    keyMapping[SDLK_SPACE] = MOVE_UP;
    keyMapping[SDLK_v] = MOVE_DOWN;

    keyMapping[SDLK_q] = YAW_LEFT;
    keyMapping[SDLK_e] = YAW_RIGHT;
    keyMapping[SDLK_z] = ROLL_LEFT;
    keyMapping[SDLK_x] = ROLL_RIGHT;
    keyMapping[SDLK_r] = PITCH_DOWN;
    keyMapping[SDLK_f] = PITCH_UP;

    stepByStepModifers = KMOD_LCTRL;

    for(positive i = 0; i < ACTIONS_COUNT; i++)
        isActionActive[i] = false;
}

CameraController::~CameraController()
{
    //dtor
}

void CameraController::setCamera(Camera * camera) {
    this->camera = camera;
}

void CameraController::setTranslateSpeed(real speed) {
    translateSpeed = speed;
}

void CameraController::setRotateSpeed(real speed) {
    rotationSpeed = speed;
}

void CameraController::assignKey(int action, int key) {
    keyMapping[key] = action;
}

void CameraController::setModifiersForStepByStep(int keys) {
    stepByStepModifers = keys;
}

bool CameraController::handleEvent(SDL_Event & event, real time) {
    bool handled = false;
    int activeModifiers = event.key.keysym.mod;
    bool stepByStepModeEnabled = ((activeModifiers & stepByStepModifers) == stepByStepModifers);
    bool down = false;
    switch(event.type) {
    case SDL_KEYDOWN:
        down = true;
    case SDL_KEYUP:
        if(keyMapping.count(event.key.keysym.sym) > 0) {
            isActionActive[keyMapping[event.key.keysym.sym]] = down;
            handled = true;
        }
        break;
    case SDL_MOUSEMOTION:
        break;
    case SDL_MOUSEBUTTONDOWN:
        down = true;
    case SDL_MOUSEBUTTONUP:
        break;
    }

    // Get motion magnitude
    real t = 0, r = 0;
    if(stepByStepModeEnabled) {
        positive now = SDL_GetTicks();
        if(now - timeLastStep >= stepDelay) {
            timeLastStep = now;
            t = translateStep;
            r = toRadians(rotationStep);
        }
    }
    else {
        t = time * translateSpeed;
        r = time * toRadians(rotationSpeed);
    }

    // Get the matrix for the current keys
    actionToMatrix(t, r);

    // Apply matrix to camera
    real tmpMat[MAT4_SCALARS_COUNT];
    copyMatrix(tmpMat, camera->position);
    multiplyMM(motionMatrix, tmpMat, camera->position);

    return handled;
}

void CameraController::actionToMatrix(real t, real r) {
    identity(motionMatrix);

    applyTranslate(motionMatrix,
                   ((int)isActionActive[MOVE_LEFT]-(int)isActionActive[MOVE_RIGHT]) * t,
                   ((int)isActionActive[MOVE_DOWN]-(int)isActionActive[MOVE_UP]) * t,
                   ((int)isActionActive[MOVE_FORWARD]-(int)isActionActive[MOVE_BACKWARD]) * t);

    applyRotate(motionMatrix, ((int)isActionActive[YAW_RIGHT]-(int)isActionActive[YAW_LEFT]) * r, 0, 1, 0);
    applyRotate(motionMatrix, ((int)isActionActive[PITCH_UP]-(int)isActionActive[PITCH_DOWN]) * r, 1, 0, 0);
    applyRotate(motionMatrix, ((int)isActionActive[ROLL_RIGHT]-(int)isActionActive[ROLL_LEFT]) * r, 0, 0, 1);
}
