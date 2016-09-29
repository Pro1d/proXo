#ifndef __KDTREE_H__
#define __KDTREE_H__

#include "../common/Pool.h"
#include "../math/type.h"

class KDTree {
public:
    void build(Pool & pool);
    void setBounds(Pool & pool);

    real bounds[6]; // xmin, ymin, zmin, xmax, ymax, zmax
    KDTree * middleSubTree;
    KDTree * firstSubTree;
    KDTree * secondSubTree;
    positive facesCount;
    positive * faces;
    bool atomicDimension[3];
};

#endif // __KDTREE_H__
