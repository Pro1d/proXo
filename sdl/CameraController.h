#ifndef CAMERACONTROLLER_H
#define CAMERACONTROLLER_H

#include <map>
#include "../core/math/type.h"
#include "../core/model/Camera.h"
#include <SDL/SDL.h>

enum {
    MOVE_FORWARD, MOVE_BACKWARD, MOVE_UP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT,
    ROTATE_FORWARD, ROTATE_BACKWARD, ROTATE_UP, ROTATE_DOWN, ROTATE_LEFT, ROTATE_RIGHT,
    ACTIONS_COUNT
};

#define ROLL_RIGHT      ROTATE_FORWARD
#define ROLL_LEFT       ROTATE_BACKWARD
#define YAW_LEFT        ROTATE_UP
#define YAW_RIGHT       ROTATE_DOWN
#define PITCH_DOWN      ROTATE_LEFT
#define PITCH_UP        ROTATE_RIGHT

class CameraController {
    public:
        CameraController();
        virtual ~CameraController();
        void setCamera(Camera * camera);
        void assignKey(int action, int key);
        void setModifiersForStepByStep(int keys);
        bool handleEvent(SDL_Event & event);
        void setTranslateSpeed(real speed);
        void setRotateSpeed(real speed);
        void updateCamera(real dt);

    protected:
        void actionToMatrix(real dt);
        real getSpeed(int action, real dt);

    private:
        std::map<int, int> keyMapping;
        int stepByStepModifers;
        int highSpeedModifers;
        Camera * camera;
        bool isActionActive[ACTIONS_COUNT];
        bool isActionNew[ACTIONS_COUNT];
        bool isStepByStepEnabled;
        bool isHighSpeedEnabled;

        real highSpeedFactor;
        real rotationStep;
        real translateStep;
        positive stepDelay;
        positive timeLastStep;
        real rotationSpeed;
        real translateSpeed;

        real motionMatrix[MAT4_SCALARS_COUNT];
};

#endif // CAMERACONTROLLER_H
