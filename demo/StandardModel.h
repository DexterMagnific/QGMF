#ifndef STANDARDMODEL_H
#define STANDARDMODEL_H

// QGMF lib
#include "QGMF.h"

// Example of a basic model with one kind of node/edge/port

// port model
class StandardPortModel : public QGMF::PortModel {
  public:
    StandardPortModel()
      : PortModel(1, "StandardPortModel")
      , m_id(100)
    { }

    int id() const { return m_id; }
  private:
    int m_id;
};

static inline StandardPortModel stdPortModel;

// edge model: connects two StandardPortModels
class StandardEdgeModel : public QGMF::EdgeModel {
  public:
    StandardEdgeModel()
      : EdgeModel(1, "StandardEdgeModel", {stdPortModel},{stdPortModel})
    { }
};

// node model: has StandardPortModels
class StandardNodeModel : public QGMF::NodeModel {
  public:
    StandardNodeModel()
      : NodeModel(1, "StandardNodeModel", {stdPortModel})
      , m_foo(222)
    { }

    int foo() const { return m_foo; }

  private:
    int m_foo;
};

// root node (graph) model: has StandardNodeModels connected with StandardEdgeModels
class StandardGraphModel : public QGMF::GraphModel {
  public:
    StandardGraphModel()
      : QGMF::GraphModel(1, "StandardGraphModel", { }, {m_stdNodeModel}, {m_stdEdgeModel})
      , m_count(5)
    { }

    int count() const { return m_count; }

  private:
    StandardNodeModel m_stdNodeModel;
    StandardEdgeModel m_stdEdgeModel;

    int m_count;
};

#endif // STANDARDMODEL_H
