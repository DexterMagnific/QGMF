#ifndef QGMF_EDGEMODEL_H
#define QGMF_EDGEMODEL_H

#include "BaseModel.h"
#include "PortModel.h"

/**
 * Macro to declare and define an edge model
 */
#define QGMF_DECLARE_EDGE_MODEL(name, str, srcmodels,dstmodels) \
  class name ## EdgeModel : public QGMF::EdgeModel { \
    public: \
      name ## EdgeModel() \
        : EdgeModel(1, str, srcmodels,dstmodels) \
      { } \
  }; \
  static inline const name ## EdgeModel name;

namespace QGMF {

class Node;

class EdgeModel;
// List of Edge models
using EdgeModelList = ModelList<EdgeModel>;

// Class for Edge models
class QGMF_PUBLIC EdgeModel : public BaseModel {
  public:
    EdgeModel(int typeId, const std::string &name,
              std::initializer_list<std::reference_wrapper<const PortModel>> srcPorts = {},
              std::initializer_list<std::reference_wrapper<const PortModel>> dstPorts = {})
      : BaseModel(typeId,name)
    {
      for (auto &m : srcPorts)
        addSrcPortModel(m);
      for (auto &m : dstPorts)
        addDstPortModel(m);
    }

    const PortModelList &srcPortModels() const { return m_srcPortModels; }
    const PortModelList &dstPortModels() const { return m_dstPortModels; }

    void addSrcPortModel(const PortModel &m) {
      if ( !findModel(m_srcPortModels, [&](const typename PortModelList::value_type &arg) {
                      return ((m == arg) ||
                              (m.typeId() == typeId()) ||
                              (m.typeName() == typeName()));
    }))
        m_srcPortModels.push_back(std::cref(m));
    }

    void addDstPortModel(const PortModel &m) {
      if ( !findModel(m_dstPortModels, [&](const typename PortModelList::value_type &arg) {
                      return ((m == arg) ||
                              (m.typeId() == typeId()) ||
                              (m.typeName() == typeName()));
    }))
        m_dstPortModels.push_back(std::cref(m));
    }

    const PortModel &srcPortModel(int typeId) const { return findModel(m_srcPortModels, typeId); }
    const PortModel &srcPortModel(const std::string &typeName) const { return findModel(m_srcPortModels, typeName); }

    const PortModel &dstPortModel(int typeId) const { return findModel(m_dstPortModels, typeId); }
    const PortModel &dstPortModel(const std::string &typeName) const { return findModel(m_dstPortModels, typeName); }

    bool hasSrcPortModel(int typeId) const { return !srcPortModel(typeId).isNull(); }
    bool hasSrcPortModel(const std::string &typeName) const { return !srcPortModel(typeName).isNull(); }
    bool hasSrcPortModel(const PortModel &m) const { return !findModel(m_srcPortModels, m).isNull(); }

    bool hasDstPortModel(int typeId) const { return !dstPortModel(typeId).isNull(); }
    bool hasDstPortModel(const std::string &typeName) const { return !dstPortModel(typeName).isNull(); }
    bool hasDstPortModel(const PortModel &m) const { return !findModel(m_dstPortModels, m).isNull(); }

    static const EdgeModel &null() { return m_null; }
    bool isNull() const { return this == &m_null; }
    explicit operator bool() const { return !isNull(); }

  private:
    static const EdgeModel m_null;

    PortModelList m_srcPortModels;
    PortModelList m_dstPortModels;

};

inline const EdgeModel EdgeModel::m_null(0, "Null (invalid) edge model");

// Unsafe casting between Edge models
template <class T>
inline T EdgeModel_cast(const EdgeModel &p)
{
  // no check, no guarantee
  return static_cast<const T &>(p);
}

}

#endif // EDGEMODEL_H
