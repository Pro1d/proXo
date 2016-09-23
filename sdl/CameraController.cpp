#include "CameraController.h"
#include <cmath>
#include "../core/math/basics.h"
#include "../core/math/Matrix.h"
#include <SDL/SDL.h>

CameraController::CameraController() :
    highSpeedFactor(3),
    rotationStep(10),
    translateStep(0.1),
    stepDelay(300),
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
    highSpeedModifers = KMOD_LALT;

    for(positive i = 0; i < ACTIONS_COUNT; i++) {
        isActionActive[i] = false;
        isActionNew[i] = false;
    }
    isStepByStepEnabled = false;
    isHighSpeedEnabled = false;
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

bool CameraController::handleEvent(SDL_Event & event) {
    bool handled = false;

    for(int i = 0; i < ACTIONS_COUNT; i++)
        isActionNew[i] = false;

    bool down = false;
    switch(event.type) {
    case SDL_KEYDOWN:
        down = true;
    case SDL_KEYUP:
        if(keyMapping.count(event.key.keysym.sym) > 0) {
            isActionActive[keyMapping[event.key.keysym.sym]] = down;
            if(!isActionActive[keyMapping[event.key.keysym.sym]])
                isActionNew[keyMapping[event.key.keysym.sym]] = true;
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

    return handled;
}

real CameraController::getSpeed(int action, real dt) {
    if(!isActionActive[action])
        return 0;

    // Get motion magnitude
    real t = 0, r = 0;
    if(isStepByStepEnabled) {
        positive now = SDL_GetTicks();
        if(isActionNew[action])
            timeLastStep = now;
        if(now == timeLastStep || now - timeLastStep >= stepDelay) {
            t = translateStep;
            r = toRadians(rotationStep);
        }
    }
    else {
        t = dt * translateSpeed;
        r = dt * toRadians(rotationSpeed);
    }

    if(isHighSpeedEnabled) {
        t *= highSpeedFactor;
        r *= highSpeedFactor;
    }

    if(action <= MOVE_RIGHT)
        return t;
    else
        return r;
}

void CameraController::actionToMatrix(real dt) {
    identity(motionMatrix);

    int activeModifiers = SDL_GetModState();
    isStepByStepEnabled = ((activeModifiers & stepByStepModifers) == stepByStepModifers);
    isHighSpeedEnabled = ((activeModifiers & highSpeedModifers) == highSpeedModifers);


    applyTranslate(motionMatrix,
                   getSpeed(MOVE_LEFT, dt)-getSpeed(MOVE_RIGHT, dt),
                   getSpeed(MOVE_DOWN, dt)-getSpeed(MOVE_UP, dt),
                   getSpeed(MOVE_FORWARD, dt)-getSpeed(MOVE_BACKWARD, dt));

    applyRotate(motionMatrix, getSpeed(YAW_RIGHT, dt)-getSpeed(YAW_LEFT, dt), 0, 1, 0);
    applyRotate(motionMatrix, getSpeed(PITCH_UP, dt)-getSpeed(PITCH_DOWN, dt), 1, 0, 0);
    applyRotate(motionMatrix, getSpeed(ROLL_RIGHT, dt)-getSpeed(ROLL_LEFT, dt), 0, 0, 1);
}

void CameraController::updateCamera(real dt) {
    // Get the matrix for the current keys
    actionToMatrix(dt);

    // Apply matrix to camera
    real tmpMat[MAT4_SCALARS_COUNT];
    copyMatrix(tmpMat, camera->position);
    multiplyMM(motionMatrix, tmpMat, camera->position);

    Uint32 now =  SDL_GetTicks();
    if(now - timeLastStep > stepDelay)
        timeLastStep = now;
}

