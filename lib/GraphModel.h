#ifndef QGMF_GRAPHMODEL_H
#define QGMF_GRAPHMODEL_H

#include "NodeModel.h"

/**
 * Macro to declare and define a graph model
 */
#define QGMF_DECLARE_GRAPH_MODEL(name, str, nodemodels, edgemodels) \
  class name ## GraphModel : public QGMF::GraphModel { \
    public: \
      name ## GraphModel() \
        : QGMF::GraphModel(1, str, { }, nodemodels,edgemodels) \
      { } \
  }; \
  static inline const name ## GraphModel name;

namespace QGMF {

// Graphs are just Nodes
using GraphModel = NodeModel;

// Unsafe casting between Graph models
template <class T>
inline T GraphModel_cast(const GraphModel &p)
{
  // no check, no guarantee
  return static_cast<const T &>(p);
}

}

#endif // GRAPHMODEL_H
