#ifndef __TEXTURE_H__
#define __TEXTURE_H__

class Texture {
	public:
		extern inline vec4 getValue(integer u, integer v); // data[((u & sizeMask) | ((v & sizeMask) << log2size)) << 2]
		vec4 data;
		//positive size;
		positive log2size;
		positive sizeMask;
};

#endif