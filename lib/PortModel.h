#ifndef QGMF_PORTMODEL_H
#define QGMF_PORTMODEL_H

#include "QGMFGlobals.h"
#include "BaseModel.h"

namespace QGMF {

#define QGMF_MODELS(...) {__VA_ARGS__}
/**
 * Macro to declare and define a port model
 */
#define QGMF_DECLARE_PORT_MODEL(name, str) \
  class name ## PortModel : public QGMF::PortModel { \
    public: \
      name ## PortModel() \
        : PortModel(1, str) \
      { } \
  }; \
  static inline const name ## PortModel name;

class PortModel;
// List of port models
using PortModelList = ModelList<PortModel>;

class EdgeModel;
using EdgeModelList = ModelList<EdgeModel>;

// Class for Port models
class QGMF_PUBLIC PortModel: public BaseModel {
  public:
    PortModel(int typeId, const std::string &name)
      : BaseModel(typeId,name)
    { }

    static const PortModel &null() { return m_null; }
    bool isNull() const { return this == &m_null; }
    explicit operator bool() const { return !isNull(); }

  private:
    static const PortModel m_null;

    // TODO
    EdgeModelList m_outEdgeModels;
    EdgeModelList m_inEdgeModels;
};

inline const PortModel PortModel::m_null(0, "Null (invalid) port model");

// Unsafe casting between port models
template <class T>
inline T PortModel_cast(const PortModel &p)
{
  // no check, no guarantee
  return static_cast<const T &>(p);
}

}

#endif // PORTMODEL_H
