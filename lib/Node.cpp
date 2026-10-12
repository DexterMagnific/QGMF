#include "Node.h"

namespace QGMF {

Node::~Node()
{
  //printf("WILL remove Node %s\n", name().c_str()); fflush(stdout);

  // remove all ports
  // will trigger removal of incoming/outgoing edges by parent
  PortList pl = m_ports;
  for (Port *p : pl) {
    delete p;
  }

  // remove all inner nodes
  NodeList nl = m_nodes;
  for (Node *n : nl) {
    delete n;
  }

  if ( parent() ) {
    parent()->removeFromNodeList(this);
  }

  //printf("Node %s removed\n", name().c_str()); fflush(stdout);
}

Node *Node::addNode(int typeId, const std::string name)
{
  if ( !canAddNode(typeId) )
    return nullptr;

  Node *n = new Node(model().nodeModel(typeId), this, name);
  return n;
}

Node *Node::addNode(const std::string &typeName, const std::string &name)
{
  if ( !canAddNode(typeName) )
    return nullptr;

  Node *n = new Node(model().nodeModel(typeName), this, name);
  return n;
}

Node *Node::addNode(const NodeModel &mdl, const std::string &name)
{
  if ( !canAddNode(mdl) )
    return nullptr;

  Node *n = new Node(mdl, this, name);
  return n;
}

void Node::addToNodeList(Node *n)
{
  m_nodes.push_back(n);
}

void Node::removeNode(Node *n)
{
  if ( !n )
    return;

  m_nodes.remove_if([&] (const typename NodeList::value_type &arg) {
    return n == arg;
  });

  delete n;
}

void Node::removeNode(int id)
{
  removeNode(node(id));
}

void Node::removeNode(const std::string &name)
{
  removeNode(node(name));
}


Port *Node::addPort(int typeId, const std::string name)
{
  if ( !canAddPort(typeId) )
    return nullptr;

  Port *n = new Port(model().portModel(typeId), this, name);
  return n;
}

Port *Node::addPort(const std::string &typeName, const std::string &name)
{
  if ( !canAddPort(typeName) )
    return nullptr;

  Port *n = new Port(model().portModel(typeName), this, name);
  return n;
}

Port *Node::addPort(const PortModel &mdl, const std::string &name)
{
  if ( !canAddPort(mdl) )
    return nullptr;

  Port *n = new Port(mdl, this, name);
  return n;
}

void Node::addToPortList(Port *p)
{
  m_ports.push_back(p);
}

void Node::removeFromNodeList(Node *n)
{
  m_nodes.remove_if([&] (const typename NodeList::value_type &arg) {
    return n == arg;
  });
}

void Node::removeFromEdgeList(Edge *e)
{
  m_edges.remove_if([&] (const typename EdgeList::value_type &arg) {
    return e == arg;
  });
}

void Node::removeFromPortList(Port *p)
{
  m_ports.remove_if([&] (const typename PortList::value_type &arg) {
    return p == arg;
  });
}

Edge *Node::addEdge(int typeId, Port *src, Port *dst, const std::string name)
{
  if ( !canAddEdge(typeId, *src,*dst) ) {
    return nullptr;
  }

  Edge *e = new Edge(model().edgeModel(typeId), this, src,dst, name);
  return e;
}

Edge *Node::addEdge(const std::string &typeName, Port *src, Port *dst, const std::string name)
{
  if ( !canAddEdge(typeName, *src,*dst) ) {
    return nullptr;
  }

  Edge *e = new Edge(model().edgeModel(typeName), this, src,dst, name);
  return e;
}

Edge *Node::addEdge(const EdgeModel &mdl, Port *src, Port *dst, const std::string name)
{
  if ( !canAddEdge(mdl, *src,*dst) ) {
    return nullptr;
  }

  Edge *e = new Edge(mdl, this, src,dst, name);
  return e;
}

void Node::addToEdgeList(Edge *e)
{
  m_edges.push_back(e);
}

}
