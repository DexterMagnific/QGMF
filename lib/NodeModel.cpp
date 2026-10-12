#include "NodeModel.h"
#include "Port.h"
#include "Node.h"

namespace QGMF {

bool NodeModel::canAddEdge(const PortModel &src, const PortModel &dst, int typeId) const {
  const EdgeModel &emdl = edgeModel(typeId);
  return
      emdl.hasSrcPortModel(src) &&
      emdl.hasDstPortModel(dst);
}

}
