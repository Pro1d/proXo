#include "CameraController.h"
#include "core/math/Matrix.h"
#include "core/math/basics.h"
#include <SDL/SDL.h>
#include <cmath>

using namespace proxo;

CameraController::CameraController()
    : isStepByStepEnabled(false), isHighSpeedEnabled(false),
      isMouseGrabbed(false), highSpeedFactor(3), rotationStep(10),
      translateStep(0.1), stepDelay(300), timeLastStep(0), rotationSpeed(10),
      translateSpeed(0.1), mouseSensivity(0.08)
{
	rotateMouse[0] = 0;
	rotateMouse[1] = 0;
	rotateMouse[2] = 0;
	for(positive i = 0; i < ACTIONS_COUNT; i++) {
		isActionActive[i] = false;
		isActionNew[i]    = false;
	}
}

CameraController::~CameraController()
{
	// dtor
}

void CameraController::setCamera(Camera* camera)
{
	this->camera = camera;
}

void CameraController::setTranslateSpeed(real speed)
{
	translateSpeed = speed;
}

void CameraController::setRotateSpeed(real speed)
{
	rotationSpeed = speed;
}

void CameraController::assignKey(int action, int key)
{
	keyMapping[key] = action;
}

void CameraController::assignMouse(int action, int button)
{
	mouseMapping[button] = action;
}

void CameraController::setModifiersForStepByStep(int keys)
{
	stepByStepModifers = keys;
}

void CameraController::setModifiersHighSpeed(int keys)
{
	highSpeedModifers = keys;
}

bool CameraController::handleEvent(SDL_Event& event)
{
	bool handled = false;

	for(int i          = 0; i < ACTIONS_COUNT; i++)
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
				handled                                           = true;
			}
			break;
		case SDL_MOUSEMOTION:
			if(event.motion.xrel != 0 && isMouseGrabbed) {
				printf("mouse %d (%d) %d (%d)\n", event.motion.x,
				    event.motion.xrel, event.motion.y, event.motion.yrel);
				if(mouseMapping.count(MOUSE_X) > 0) {
					int action = mouseMapping[MOUSE_X];
					if(ROTATE_BACKWARD <= action && action <= ROTATE_LEFT) {
						rotateMouse[(action - ROTATE_BACKWARD) / 2] +=
						    (((action - ROTATE_BACKWARD) & 1) ? -1 : 1)
						    * mouseSensivity * event.motion.xrel;
					}
				}
			}
			if(event.motion.yrel != 0 && isMouseGrabbed) {
				if(mouseMapping.count(MOUSE_Y) > 0) {
					int action = mouseMapping[MOUSE_Y];
					if(ROTATE_BACKWARD <= action && action <= ROTATE_LEFT) {
						rotateMouse[(action - ROTATE_BACKWARD) / 2] +=
						    (((action - ROTATE_BACKWARD) & 1) ? -1 : 1)
						    * mouseSensivity * event.motion.yrel;
					}
				}
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
			down = true;
		case SDL_MOUSEBUTTONUP:
			if(mouseMapping.count(event.button.button) > 0) {
				switch(mouseMapping[event.button.button]) {
					case GRAB_MOUSE:
						grabMouse(down);
						break;
					case FOV_DECREASE:
						camera->setFieldOfView(std::max(toRadians(10),
						    camera->getFieldOfView() - toRadians(10)));
						break;
					case FOV_INCREASE:
						camera->setFieldOfView(std::min(toRadians(170),
						    camera->getFieldOfView() + toRadians(10)));
						break;
				}
				handled = true;
			}
			break;
	}

	return handled;
}

real CameraController::getSpeed(int action, real dt)
{
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

void CameraController::actionToMatrix(real dt)
{
	identity(motionMatrix);

	int activeModifiers = SDL_GetModState();
	isStepByStepEnabled =
	    ((activeModifiers & stepByStepModifers) == stepByStepModifers);
	isHighSpeedEnabled =
	    ((activeModifiers & highSpeedModifers) == highSpeedModifers);

	applyRotate(motionMatrix, getSpeed(ROLL_RIGHT, dt) - getSpeed(ROLL_LEFT, dt)
	        + toRadians(rotateMouse[0]),
	    0, 0, 1);
	applyRotate(motionMatrix, getSpeed(YAW_RIGHT, dt) - getSpeed(YAW_LEFT, dt)
	        + toRadians(rotateMouse[1]),
	    0, 1, 0);
	applyRotate(motionMatrix, getSpeed(PITCH_UP, dt) - getSpeed(PITCH_DOWN, dt)
	        + toRadians(rotateMouse[2]),
	    1, 0, 0);

	applyTranslate(motionMatrix,
	    getSpeed(MOVE_LEFT, dt) - getSpeed(MOVE_RIGHT, dt),
	    getSpeed(MOVE_DOWN, dt) - getSpeed(MOVE_UP, dt),
	    getSpeed(MOVE_FORWARD, dt) - getSpeed(MOVE_BACKWARD, dt));

	rotateMouse[0] = 0;
	rotateMouse[1] = 0;
	rotateMouse[2] = 0;
}

void CameraController::updateCamera(real dt)
{
	// Get the matrix for the current keys
	actionToMatrix(dt);

	// Apply matrix to camera
	real tmpMat[MAT4_SCALARS_COUNT];
	copyMatrix(tmpMat, camera->position);
	multiplyMM(motionMatrix, tmpMat, camera->position);

	Uint32 now = SDL_GetTicks();
	if(now - timeLastStep > stepDelay)
		timeLastStep = now;
}

void CameraController::grabMouse(bool grab)
{
	SDL_ShowCursor(!grab);
	SDL_WM_GrabInput(grab ? SDL_GRAB_ON : SDL_GRAB_OFF);
	isMouseGrabbed = grab;
}
