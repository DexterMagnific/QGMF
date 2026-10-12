#ifndef QGMF_NODEMODEL_H
#define QGMF_NODEMODEL_H

#include "BaseModel.h"
#include "PortModel.h"
#include "EdgeModel.h"

/**
 * Macro to declare and define a simple node model with no subnodes
 */
#define QGMF_DECLARE_NODE_MODEL(name, str, portmodels) \
  class name ## NodeModel : public QGMF::NodeModel { \
    public: \
      name ## NodeModel() \
        : NodeModel(1, str, portmodels) \
      { } \
  }; \
  static inline const name ## NodeModel name;

/**
 * Macro to declare and define a simple node model with no subnodes
 */
#define QGMF_DECLARE_COMPLEX_NODE_MODEL(name, str, portmodels, nodemodels, edgemodels) \
  class name ## NodeModel : public QGMF::NodeModel { \
    public: \
      name ## NodeModel() \
        : NodeModel(1, str, portmodels, nodemodels, edgemodels) \
      { } \
  }; \
  static inline const name ## NodeModel name;

namespace QGMF {

class Node;
class Port;
class Edge;

class NodeModel;
// List of Node models
using NodeModelList = ModelList<NodeModel>;

// Class for Node models
class QGMF_PUBLIC NodeModel : public BaseModel {
  public:
    NodeModel(int typeId, const std::string &name,
              std::initializer_list<std::reference_wrapper<const PortModel>> ports = {},
              std::initializer_list<std::reference_wrapper<const NodeModel>> nodes = {},
              std::initializer_list<std::reference_wrapper<const EdgeModel>> edges = {})
      : BaseModel(typeId,name)
    {
      for (auto &mdl: ports)
        addPortModel(mdl);
      for (auto &mdl: nodes)
        addNodeModel(mdl);
      for (auto &mdl: edges)
          addEdgeModel(mdl);
    }

    const PortModelList &portModels() const { return m_portModels; }
    const NodeModelList &nodeModels() const { return m_nodeModels; }
    const EdgeModelList &edgeModels() const { return m_edgeModels; }

    /** Returns a list of edge models that can link between two port models */
    EdgeModelList edgeModels(const PortModel &srcMdl, const PortModel &dstMdl) const {
      EdgeModelList l;
      for (const EdgeModel &emdl: m_edgeModels) {
        if ( emdl.hasSrcPortModel(srcMdl) &&
             emdl.hasDstPortModel(dstMdl) ) {
          l.push_back(std::cref(emdl));
        }
      }
      return l;
    }

    void addPortModel(const PortModel &m) {
      if ( !findModel(m_portModels, [&](const typename PortModelList::value_type &arg) {
                      return ((m == arg) ||
                              (m.typeId() == typeId()) ||
                              (m.typeName() == typeName()));
    }))
        m_portModels.push_back(std::cref(m));
    }

    void addNodeModel(const NodeModel &m) {
      if ( !findModel(m_nodeModels, [&](const typename NodeModelList::value_type &arg) {
                      return ((m == arg) ||
                              (m.typeId() == typeId()) ||
                              (m.typeName() == typeName()));
    }))
        m_nodeModels.push_back(std::cref(m));
    }

    void addEdgeModel(const EdgeModel &m) {
      if ( !findModel(m_edgeModels, [&](const typename EdgeModelList::value_type &arg) {
                      return ((m == arg) ||
                              (m.typeId() == typeId()) ||
                              (m.typeName() == typeName()));
    }))
        m_edgeModels.push_back(std::cref(m));
    }

    const PortModel &portModel(int typeId) const { return findModel(m_portModels, typeId); }
    const PortModel &portModel(const std::string &typeName) const { return findModel(m_portModels, typeName); }

    const NodeModel &nodeModel(int typeId) const { return findModel(m_nodeModels, typeId); }
    const NodeModel &nodeModel(const std::string &typeName) const { return findModel(m_nodeModels, typeName); }

    const EdgeModel &edgeModel(int typeId) const { return findModel(m_edgeModels, typeId); }
    const EdgeModel &edgeModel(const std::string &typeName) const { return findModel(m_edgeModels, typeName); }

    bool hasNodeModel(int typeId) const { return !nodeModel(typeId).isNull(); }
    bool hasNodeModel(const std::string &typeName) const { return !nodeModel(typeName).isNull(); }
    bool hasNodeModel(const NodeModel &m) const { return !findModel(m_nodeModels, m).isNull(); }

    bool hasEdgeModel(int typeId) const { return !edgeModel(typeId).isNull(); }
    bool hasEdgeModel(const std::string &typeName) const { return !edgeModel(typeName).isNull(); }
    bool hasEdgeModel(const EdgeModel &m) const { return !findModel(m_edgeModels, m).isNull(); }

    bool hasPortModel(int typeId) const { return !portModel(typeId).isNull(); }
    bool hasPortModel(const std::string &typeName) const { return !portModel(typeName).isNull(); }
    bool hasPortModel(const PortModel &m) const { return !findModel(m_portModels, m).isNull(); }

    virtual bool canAddNode(int typeId) const { return hasNodeModel(typeId); }
    bool canAddNode(const std::string &typeName) const { return canAddNode(nodeModel(typeName).typeId()); }
    bool canAddNode(const NodeModel &m) const { return canAddNode(m.typeId()); }

    virtual bool canAddPort(int typeId) const { return hasPortModel(typeId); }
    bool canAddPort(const std::string &typeName) const { return canAddPort(portModel(typeName).typeId()); }
    bool canAddPort(const PortModel &m) const { return canAddPort(m.typeId()); }

    virtual bool canAddEdge(const PortModel &src, const PortModel &dst, int typeId) const;
    bool canAddEdge(const PortModel &src, const PortModel &dst, const std::string &typeName) const { return canAddEdge(src,dst, edgeModel(typeName).typeId()); }
    bool canAddEdge(const PortModel &src, const PortModel &dst, const EdgeModel &m) const { return canAddEdge(src,dst, m.typeId()); }

    /** Returns edge models that have the given port model as a source */
    EdgeModelList outEdgeModels(const PortModel &p) const {
      EdgeModelList l;
      std::copy_if(m_edgeModels.begin(), m_edgeModels.end(), std::back_inserter(l), [&p](const EdgeModel &mdl) {
          return mdl.hasSrcPortModel(p);
      });
      return l;
    }

    /** Returns edge models that have the given port model as a destination */
    EdgeModelList inEdgeModels(const PortModel &p) const {
      EdgeModelList l;
      std::copy_if(m_edgeModels.begin(), m_edgeModels.end(), std::back_inserter(l), [&p](const EdgeModel &mdl) {
          return mdl.hasDstPortModel(p);
      });
      return l;
    }

    /**
     *  Returns whether the given port model is exclusively a sink.
     *  Sink ports are ports that cannot have outgoing edges
     */
    bool isOnlySink(const PortModel &p) const {
      return outEdgeModels(p).size() == 0;
    }

    /**
     *  Returns whether the given port model is exclusively a source
     *  Source ports are ports that cannot have incoming edges
     */
    bool isOnlySource(const PortModel &p) const {
      return inEdgeModels(p).size() == 0;
    }

    static const NodeModel &null() { return m_null; }
    bool isNull() const { return this == &m_null; }
    explicit operator bool() const { return !isNull(); }

  private:
    static const NodeModel m_null;

    PortModelList m_portModels;
    NodeModelList m_nodeModels;
    EdgeModelList m_edgeModels;
};

inline const NodeModel NodeModel::m_null(0, "Null (invalid) node model");

// Unsafe casting between Node models
template <class T>
inline T NodeModel_cast(const NodeModel &p)
{
  // no check, no guarantee
  return static_cast<T>(p);
}

}
#endif // NODEMODEL_H
