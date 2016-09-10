#ifndef __BUFFER_H__
#define __BUFFER_H__

#include "../math/type.h"

#define BUF_Z_OFFSET	0
#define BUF_R_OFFSET	1
#define BUF_G_OFFSET	2
#define BUF_B_OFFSET	3

class Buffer {
public:
	Buffer(positive w, positive h);
	~Buffer();
	real * getPtr(positive x, positive y);

    real * data;
    positive width;
    positive height;
};

#endif
