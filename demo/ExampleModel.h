#ifndef EXAMPLEMODEL_H
#define EXAMPLEMODEL_H

// QGMF lib
#include "QGMF.h"

namespace GStreamerModel {
// Example of a basic model with one kind of node/edge/port

// port model
class GStreamerPort : public QGMF::PortModel {
  public:
    GStreamerPort()
      : PortModel(1, "StandardPortModel")
      , m_id(100)
    { }

    int id() const { return m_id; }
  private:
    int m_id;
};

static inline GStreamerPort gsPortModel;

// edge model: connects two StandardPortModels
class GStreamerConnexion : public QGMF::EdgeModel {
  public:
    GStreamerConnexion()
      : EdgeModel(1, "StandardEdgeModel", {gsPortModel},{gsPortModel})
    { }
};

static inline GStreamerConnexion gsConnectionModel;

// node model: has StandardPortModels
class GStreamerNode : public QGMF::NodeModel {
  public:
    GStreamerNode()
      : NodeModel(1, "StandardNodeModel", {gsPortModel})
      , m_foo(222)
    { }

    int foo() const { return m_foo; }

  private:
    int m_foo;
};

static inline GStreamerNode gsNodeModel;

// root node (graph) model: has StandardNodeModels connected with StandardEdgeModels
class GStreamerPipeline : public QGMF::GraphModel {
  public:
    GStreamerPipeline()
      : QGMF::GraphModel(1, "StandardGraphModel", { }, {gsNodeModel}, {gsConnectionModel})
      , m_count(5)
    { }

    int count() const { return m_count; }


  private:
    int m_count;
};

static inline GStreamerPipeline gsPipeline;

};

#endif // EXAMPLEMODEL_H
