#ifndef QGMF_PORT_H
#define QGMF_PORT_H

#include "QGMFGlobals.h"

#include "PortModel.h"
#include "Instance.h"

namespace QGMF {

class Node;
class Edge;
using EdgeList = List<Edge>;

// List of Ports
using PortList = List<Port>;

// Class for Ports
class QGMF_PUBLIC Port final: public Instance<PortModel> {
  public:
    virtual ~Port();

  private:
    // ports can only be created using Node::addPort()
    Port(const PortModel &model, Node *node, std::string name = std::string());

    friend class Node;
    friend class Edge;

  public:
    /** Returns the node owning this port */
    Node *node() const { return m_node; }

    const EdgeList &inEdges() const { return m_inEdges; }
    const EdgeList &outEdges() const { return m_outEdges; }

    unsigned inEdgeCount() const { return m_inEdges.size(); }
    unsigned outEdgeCount() const { return m_outEdges.size(); }

  private:
    void addToInEdgeList(Edge *e);
    void addToOutEdgeList(Edge *e);
    void removeFromOutEdgeList(Edge *e);
    void removeFromInEdgeList(Edge *e);

    Node *m_node;
    // incoming/outgoing edges
    EdgeList m_inEdges;
    EdgeList m_outEdges;
};

}
#endif // PORT_H
