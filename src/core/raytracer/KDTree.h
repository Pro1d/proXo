#ifndef __KDTREE_H__
#define __KDTREE_H__

#include "core/common/Pool.h"
#include "core/math/type.h"
#include <cstdlib>

namespace proxo {

enum {
	BOUND_X_MIN,
	BOUND_Y_MIN,
	BOUND_Z_MIN,
	BOUND_X_MAX,
	BOUND_Y_MAX,
	BOUND_Z_MAX
};

struct Node {
  const bool is_leaf;
  Node(bool is_leaf) : is_leaf{is_leaf} {}
  virtual ~Node() = default;
};
struct Branch : Node {
  Branch(positive cutAxis, real cutValue) : Node{false}, cutAxis{cutAxis}, cutValue{cutValue} {}
  virtual ~Branch() {
    if(left) delete left;
    if(right) delete right;
  }
  Node* left{NULL};
  Node* right{NULL};
  positive cutAxis;
  real cutValue;
};
struct Leaf : Node {
  Leaf(positive facesCount) : Node{true}, facesCount{facesCount}, facePtrs{facesCount ? new positive*[facesCount] : NULL} {}
  virtual ~Leaf() {
    if(facePtrs) delete[] facePtrs;
  }
  positive facesCount{0};
  positive** facePtrs{NULL};
};

class KDTree {
private:
  static void buildNode(Node*& node, real bounds[6], const Pool& pool, positive** facePtrs, positive facesCount, positive d= 0);
  static bool chooseCut(const Pool& pool, real bounds[6], positive** facePtrs, positive facesCount, positive& cutAxis, real& cutValue, positive cutFaceCounts[2]);
	void setBounds(const Pool& pool);
  positive updateDepthMax();
public:
	KDTree(const Pool& pool);
	~KDTree();

	real bounds[6]; // xmin, ymin, zmin, xmax, ymax, zmax
  Node* root{NULL};
  positive depthMax{0};
};

class NodeStack {
public:
  struct Element {
    Node* node;
    real tmin, tmax;
  };
	NodeStack(positive sizeMax)
	    : stack(new Element[sizeMax]), top(stack)
	{
	}
  NodeStack(const NodeStack&) = delete;
	~NodeStack()
	{
		delete[] stack;
	}
	inline bool empty() {
    return stack == top;
  }
	inline void clear() {
    top = stack;
  }
	inline void push(Element e) {
    *top = e;
    ++top;
  }
	inline Element pop() {
    return *--top;
  }
private:
	Element* const stack;
  Element* top;
};

} // namespace proxo

#endif // __KDTREE_H__
