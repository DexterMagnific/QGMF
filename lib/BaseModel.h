#ifndef QGMF_BASEMODEL_H
#define QGMF_BASEMODEL_H

#include "QGMFGlobals.h"

#include <functional>
#include <algorithm>
#include <list>
#include <string>

namespace QGMF {

// typename for model lists. Model lists store models as const references
template<typename T>
using ModelList = std::list<std::reference_wrapper<const T>>;

// Base class for all models
class BaseModel {
  public:
    BaseModel(const BaseModel &) = delete;
    BaseModel(const BaseModel &&) = delete;

    BaseModel(int typeId, const std::string &name)
      : m_typeId(typeId)
      , m_name(std::move(name))
    { }

    /** Returns the unique type identifier of this model */
    int typeId() const { return m_typeId; }
    /** Returns the unique type name of this model */
    const std::string &typeName() const { return m_name; }

    bool operator ==(const BaseModel &other) const { return this == &other; }
    bool operator !=(const BaseModel &other) const { return this != &other; }

  private:
    int m_typeId;
    std::string m_name;
};

// Find a model T in a ModelList<T> using a predicate
template<typename T, typename Predicate>
inline const T & findModel(ModelList<T> t, Predicate p) {
  typename ModelList<T>::const_iterator it =
      std::find_if(t.cbegin(),t.cend(), p);
  return it == t.cend() ? T::null() : it->get();
}

// Find a model by its typeId in a ModelList<T> container
template<typename T>
inline const T & findModel(ModelList<T> t, int typeId) {
  return findModel(t, [&typeId](const typename ModelList<T>::value_type &arg) {
    return arg.get().typeId() == typeId; });
}

// Find a model by its typeId in a ModelList<T> container
template<typename T>
inline const T & findModel(ModelList<T> t, const std::string &typeName) {
  return findModel(t, [&typeName](const typename ModelList<T>::value_type &arg) {
    return arg.get().typeName() == typeName; });
}

// Find a model by its model
template<typename T>
inline const T & findModel(ModelList<T> t, const T &mdl) {
  return findModel(t, [&mdl](const typename ModelList<T>::value_type &arg) {
    return arg.get() == mdl; });
}

}

#endif // BASEMODEL_H
