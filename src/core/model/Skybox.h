#ifndef __SKYBOX_H__
#define __SKYBOX_H__

namespace proxo {

class Skybox {
public:
	vec4 getColor(vec4 dir);
	vec4 data;
};

} // namespace proxo

#endif
