#ifndef QGMF_NODE_H
#define QGMF_NODE_H

#include "QGMFGlobals.h"

#include "NodeModel.h"
#include "Instance.h"

#include "Port.h"
#include "Edge.h"

namespace QGMF {

// List of Nodes
using NodeList = List<Node>;

// Class for Nodes
class QGMF_PUBLIC Node final: public Instance<NodeModel> {
  public:
    // can only create "root" nodes (aka graphs)
    Node(const NodeModel &model, const std::string &name = std::string())
      : Instance(model, std::move(name))
      , m_parent(nullptr)
    { }

    virtual ~Node();

  private:
    // creating a node with a parent: only possible through addNode() of
    // an existing node
    Node(const NodeModel &model, Node *parent, const std::string &name = std::string())
      : Instance(model, std::move(name))
      , m_parent(parent)
    {
      if ( !parent )
        return;
      parent->addToNodeList(this);
    }

  public:

    // structure
    Node *parent() const { return m_parent; }

    // lists/counters
    const NodeList &nodes() const { return m_nodes; }
    const PortList &ports() const { return m_ports; }
    const EdgeList &edges() const { return m_edges; }

    unsigned nodeCount() const { return m_nodes.size(); }
    unsigned edgeCount() const { return m_edges.size(); }
    unsigned portCount() const { return m_ports.size(); }

    // finders
    Node *node(int id) const { return findInstance(m_nodes, id); }
    Node *node(const std::string &name) const { return findInstance(m_nodes, name); }
    bool hasNode(int id) const { return node(id) != nullptr; }
    bool hasNode(const std::string &name) const { return node(name) != nullptr; }

    Port *port(int id) const { return findInstance(m_ports, id); }
    Port *port(const std::string &name) const { return findInstance(m_ports, name); }
    bool hasPort(int id) const { return port(id) != nullptr; }
    bool hasPort(const std::string &name) const { return port(name) != nullptr; }

    Edge *edge(int id) const { return findInstance(m_edges, id); }
    Edge *edge(const std::string &name) const { return findInstance(m_edges, name); }
    bool hasEdge(int id) const { return edge(id) != nullptr; }
    bool hasEdge(const std::string &name) const { return edge(name) != nullptr; }

    /** Returns a list of edge models that can link between two ports */
    EdgeModelList edgeModels(const Port &src, const Port &dst) const {
      const EdgeModelList mdls = model().edgeModels(src.model(),dst.model());
      EdgeModelList l;
      for (const EdgeModel &emdl: mdls) {
        if ( canAddEdge(emdl, src,dst) ) {
          l.push_back(std::cref(emdl));
        }
      }
      return l;
    }

    // node insertion/removal
    bool canAddNode(int typeId) const { return model().canAddNode(typeId); }
    bool canAddNode(const std::string &typeName) const { return model().canAddNode(typeName); }
    bool canAddNode(const NodeModel &mdl) const { return model().canAddNode(mdl); }

    // FIXME list of node models traversed twice
    Node *addNode(int typeId, const std::string name = std::string());
    Node *addNode(const std::string &typeName, const std::string &name = std::string());
    Node *addNode(const NodeModel &mdl, const std::string &name = std::string());

    void removeNode(Node *n);
    void removeNode(int id);
    void removeNode(const std::string &name);

    // port insertion/removal
    bool canAddPort(int typeId) const { return model().canAddPort(typeId); }
    bool canAddPort(const std::string &typeName) const { return model().canAddPort(typeName); }
    bool canAddPort(const PortModel &mdl) const { return model().canAddPort(mdl); }

    Port *addPort(int typeId, const std::string name = std::string());
    Port *addPort(const std::string &typeName, const std::string &name = std::string());
    Port *addPort(const PortModel &mdl, const std::string &name = std::string());

    // edge insertion/removal
    // TODO allow adding "forward" edges from parent to inner node,
    // and from inner node to parent
    bool canAddEdge(int typeId, const Port &src, const Port &dst) const {
      return
          &src != &dst && // no self edge
          src.node() != dst.node() && // no edge to sibling // TODO make configurable
          src.node()->parent() == this &&
          dst.node()->parent() == this &&
          model().canAddEdge(src.model(), dst.model(), typeId); // model's decision
    }
    bool canAddEdge(const std::string &typeName, const Port &src, const Port &dst) const {
      return
          &src != &dst &&
          src.node() != dst.node() &&
          src.node()->parent() == this &&
          dst.node()->parent() == this &&
          model().canAddEdge(src.model(), dst.model(), typeName);
    }
    bool canAddEdge(const EdgeModel &mdl, const Port &src, const Port &dst) const {
      return
          &src != &dst &&
          src.node() != dst.node() &&
          src.node()->parent() == this &&
          dst.node()->parent() == this &&
          model().canAddEdge(src.model(), dst.model(), mdl);
    }

    Edge *addEdge(int typeId, Port *src, Port *dst, const std::string name = std::string());
    Edge *addEdge(const std::string &typeName, Port *src, Port *dst, const std::string name = std::string());
    Edge *addEdge(const EdgeModel &mdl, Port *src, Port *dst, const std::string name = std::string());

  private:
    // Unsafe methods. not accessible.
    void addToNodeList(Node *n);
    void addToEdgeList(Edge *e);
    void addToPortList(Port *p);

    void removeFromNodeList(Node *n);
    void removeFromEdgeList(Edge *e);
    void removeFromPortList(Port *p);

    friend class Port;
    friend class Edge;

    void setParent(Node *n) { m_parent = n; }

    Node *m_parent;
    NodeList m_nodes;
    PortList m_ports;
    EdgeList m_edges;

    // TODO add pred() and succ() nodes
};

}
#endif // Node_H
