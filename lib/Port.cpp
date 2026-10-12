#include "Port.h"
#include "Node.h"

namespace QGMF {

Port::Port(const PortModel &model, Node *node, std::string name)
  : Instance(model, name)
  , m_node(node)
{
  if ( !node )
    return;

  node->addToPortList(this);
}

Port::~Port()
{
  EdgeList el = inEdges();
  for (Edge *e : el) {
    delete e;
  }
  el = outEdges();
  for (Edge *e : el) {
    delete e;
  }
  node()->removeFromPortList(this);

  //printf("Port %s removed\n", name().c_str()); fflush(stdout);
}

void Port::addToInEdgeList(Edge *e)
{
  m_inEdges.push_back(e);
}

void Port::addToOutEdgeList(Edge *e)
{
  m_outEdges.push_back(e);
}

void Port::removeFromOutEdgeList(Edge *e)
{
  m_outEdges.remove_if([&] (const typename EdgeList::value_type &arg) {
    return e == arg;
  });
}

void Port::removeFromInEdgeList(Edge *e)
{
  m_inEdges.remove_if([&] (const typename EdgeList::value_type &arg) {
    return e == arg;
  });
}

}
