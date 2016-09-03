#ifndef __CONTAINER_H__
#define __CONTAINER_H__

class Container {
	public:
		
	private:
		Object * objects;
		mat4 objectMatrices;
		
		Container * containers;
		mat4 containerMatrices;
		
		Light * lights;
		mat4 lightMatrices;
		
		positive objectsCount;
		positive containersCount;
		positive lightsCount;
};

#endif