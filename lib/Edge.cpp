#include "Edge.h"

#include "Port.h"
#include "Node.h"

namespace QGMF {

Edge::Edge(const EdgeModel &model,
           Node *node,
           Port *src,
           Port *dst, std::string name)
  : Instance(model, name)
  , m_node(node)
  , m_src(src)
  , m_dst(dst)
  , m_untyped(false)
{
  node->addToEdgeList(this);
  src->addToOutEdgeList(this);
  dst->addToInEdgeList(this);
}

// Constructor for untyped/temporary edges
Edge::Edge(Node *node,
           Port *src,
           Port *dst, std::string name)
  : Instance(EdgeModel::null(), std::move(name))
  , m_node(node)
  , m_src(src)
  , m_dst(dst)
  , m_untyped(true)
{
  // Untyped edges are not added to the graph structure
}

Edge::~Edge()
{
  if ( !m_untyped ) {
    src()->removeFromOutEdgeList(this);
    dst()->removeFromInEdgeList(this);
    node()->removeFromEdgeList(this);
  }

  //printf("Edge %s removed\n", name().c_str()); fflush(stdout);
}

bool Edge::changeSrc(Port *new_src)
{
  // detach from src and dst so as not to alter canAddEdge() result
  // if it relies on number of connexions
  src()->removeFromOutEdgeList(this);
  dst()->removeFromInEdgeList(this);

  if ( node()->canAddEdge(model(), *new_src,*dst()) ) {
    // attach new src
    m_src = new_src;
    new_src->addToOutEdgeList(this);
    // reattach dst
    dst()->addToInEdgeList(this);
    return true;
  } else {
    // reattach src & dst
    src()->addToOutEdgeList(this);
    dst()->addToInEdgeList(this);
    return false;
  }
}

bool Edge::changeDst(Port *new_dst)
{
  // detach from src and dst so as not to alter canAddEdge() result
  // if it relies on number of connexions
  src()->removeFromOutEdgeList(this);
  dst()->removeFromInEdgeList(this);

  if ( node()->canAddEdge(model(), *src(),*new_dst) ) {
    // attach new dst
    m_dst = new_dst;
    new_dst->addToInEdgeList(this);
    // reattach src
    src()->addToOutEdgeList(this);
    return true;
  } else {
    // reattach src & dst
    src()->addToOutEdgeList(this);
    dst()->addToInEdgeList(this);
    return false;
  }
}

}
