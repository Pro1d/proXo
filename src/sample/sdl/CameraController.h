#ifndef CAMERACONTROLLER_H
#define CAMERACONTROLLER_H

#include "core/math/type.h"
#include "core/model/Camera.h"
#include <SDL/SDL.h>
#include <map>

enum {
	MOVE_FORWARD,
	MOVE_BACKWARD,
	MOVE_UP,
	MOVE_DOWN,
	MOVE_LEFT,
	MOVE_RIGHT,
	ROTATE_BACKWARD,
	ROTATE_FORWARD,
	ROTATE_UP,
	ROTATE_DOWN,
	ROTATE_RIGHT,
	ROTATE_LEFT,
	ACTIONS_COUNT
};
enum { GRAB_MOUSE = ACTIONS_COUNT, FOV_INCREASE, FOV_DECREASE };
enum { MOUSE_X = -1, MOUSE_Y = -2 };

#define ROLL_LEFT ROTATE_BACKWARD
#define ROLL_RIGHT ROTATE_FORWARD
#define YAW_LEFT ROTATE_UP
#define YAW_RIGHT ROTATE_DOWN
#define PITCH_UP ROTATE_RIGHT
#define PITCH_DOWN ROTATE_LEFT

class CameraController {
public:
	CameraController();
	virtual ~CameraController();
	void setCamera(Camera* camera);
	void assignKey(int action, int key);
	void assignMouse(int action, int button);
	void setModifiersForStepByStep(int keys);
	void setModifiersHighSpeed(int keys);
	bool handleEvent(SDL_Event& event);
	void setTranslateSpeed(real speed);
	void setRotateSpeed(real speed);
	void updateCamera(real dt);
	void grabMouse(bool grab);

protected:
	void actionToMatrix(real dt);
	real getSpeed(int action, real dt);

private:
	std::map<int, int> mouseMapping;
	std::map<int, int> keyMapping;
	int stepByStepModifers;
	int highSpeedModifers;
	Camera* camera;
	bool isActionActive[ACTIONS_COUNT];
	bool isActionNew[ACTIONS_COUNT];
	bool isStepByStepEnabled;
	bool isHighSpeedEnabled;
	bool isMouseGrabbed;

	real highSpeedFactor;
	real rotationStep;
	real translateStep;
	positive stepDelay;
	positive timeLastStep;
	real rotationSpeed;
	real translateSpeed;
	real mouseSensivity;
	real rotateMouse[3];

	real motionMatrix[MAT4_SCALARS_COUNT];
};

#endif // CAMERACONTROLLER_H
