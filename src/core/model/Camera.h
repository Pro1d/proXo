#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "core/math/type.h"

namespace proxo {

class Camera {
public:
	Camera();

	void setFieldOfView(real fov);
	real getFieldOfView();
	void setDepthMax(real zFar);
	void setFrustrum(real zNear, real zFar, real width, real height);
	void setOrthographics(real zNear, real zFar, real width, real height);
	void updateProjection();

	void lookAt(vec3 target);
	void setPosition(vec3 pos);
	void setDirection(vec3 dir);
	void setAperture(real radius);
	void setDistanceFocus(real dist);
	void setAutoFocus(bool enable);
	void setRenderTarget(positive w, positive h, positive ss);

	bool isShpereVisible(vec3 center, real radius);

	real projection[MAT4_SCALARS_COUNT];
	real position[MAT4_SCALARS_COUNT];

	real fieldOfView;
	real zNear, zFar;
	real xmin, xmax;
	real ymin, ymax;
	real screenWidth, screenHeight;
	positive targetWidth, targetHeight, supersampling;
	real aperture, distanceFocus;
	bool autofocus;
};

} // namespace proxo

#endif
