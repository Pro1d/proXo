#ifndef __KDTREE_H__
#define __KDTREE_H__

#include "core/common/Pool.h"
#include "core/math/type.h"
#include <cstdlib>

namespace proxo {

#define MINIMUM_FACES_TREE 50

enum {
	BOUND_X_MIN,
	BOUND_Y_MIN,
	BOUND_Z_MIN,
	BOUND_X_MAX,
	BOUND_Y_MAX,
	BOUND_Z_MAX
};

class KDTree {
public:
	KDTree(Pool& pool, positive* faces, positive faceBegin, positive faceEnd,
	    KDTree* parent = NULL);
	~KDTree();
	void build(Pool& pool);
	void setBounds(Pool& pool);
	void print(positive depth = 0);

	real bounds[6]; // xmin, ymin, zmin, xmax, ymax, zmax
	KDTree* middleSubTree;
	KDTree* firstSubTree;
	KDTree* secondSubTree;
	bool isLeaf;
	positive facesCount;
	positive* faces;
	bool isLocked[3];
};

class TreeStack {
public:
	TreeStack(positive sizeMax)
	    : treeStack(new KDTree*[sizeMax]), treeStackTop(treeStack),
	      distanceStack(new real[sizeMax]), distanceStackTop(distanceStack)
	{
	}
	~TreeStack()
	{
		delete[] treeStack;
		delete[] distanceStack;
	}
	void push(KDTree*& t, real d);
	void pop(KDTree*& t, real& d);
	KDTree **treeStack, **treeStackTop;
	real *distanceStack, *distanceStackTop;
};

inline void TreeStack::push(KDTree*& t, real d)
{
	*treeStackTop = t;
	treeStackTop++;
	*distanceStackTop = d;
	distanceStackTop++;
}

inline void TreeStack::pop(KDTree*& t, real& d)
{
	t = *--treeStackTop;
	d = *--distanceStackTop;
}

} // namespace proxo

#endif // __KDTREE_H__
