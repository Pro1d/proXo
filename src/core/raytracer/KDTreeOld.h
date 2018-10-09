#ifndef __KDTREE_H__
#define __KDTREE_H__

#include "core/common/Pool.h"
#include "core/math/type.h"
#include <cstdlib>

namespace proxo {

#define MINIMUM_FACES_TREE 8

enum {
	BOUND_X_MIN,
	BOUND_Y_MIN,
	BOUND_Z_MIN,
	BOUND_X_MAX,
	BOUND_Y_MAX,
	BOUND_Z_MAX
};

class KDTreeOld {
public:
	KDTreeOld(Pool& pool, positive* faces, positive faceBegin, positive faceEnd,
	    KDTreeOld* parent = NULL);
	~KDTreeOld();
	void build(Pool& pool);
  void chooseCut(Pool& pool, positive& cutAxis, real& cutValue);
	void setBounds(Pool& pool);
	void print(positive depth = 0);

	real bounds[6]; // xmin, ymin, zmin, xmax, ymax, zmax
	KDTreeOld* middleSubTree;
	KDTreeOld* firstSubTree;
	KDTreeOld* secondSubTree;
	bool isLeaf;
	positive facesCount;
	positive* faces;
	bool isLocked[3];
};

class TreeStack {
public:
	TreeStack(positive sizeMax)
	    : treeStack(new KDTreeOld*[sizeMax]), treeStackTop(treeStack),
	      distanceStack(new real[sizeMax]), distanceStackTop(distanceStack)
	{
	}
	~TreeStack()
	{
		delete[] treeStack;
		delete[] distanceStack;
	}
	bool empty();
	void clear();
	void push(KDTreeOld*& t, real d);
	void pop(KDTreeOld*& t, real& d);
	KDTreeOld **treeStack, **treeStackTop;
	real *distanceStack, *distanceStackTop;
};

inline bool TreeStack::empty()
{
	return treeStack == treeStackTop;
}

inline void TreeStack::clear()
{
	*treeStackTop = *treeStack;
	*distanceStackTop = *distanceStack;
}

inline void TreeStack::push(KDTreeOld*& t, real d)
{
	*treeStackTop = t;
	treeStackTop++;
	*distanceStackTop = d;
	distanceStackTop++;
}

inline void TreeStack::pop(KDTreeOld*& t, real& d)
{
	t = *--treeStackTop;
	d = *--distanceStackTop;
}

} // namespace proxo

#endif // __KDTREE_H__
