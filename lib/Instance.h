#ifndef QGMF_INSTANCE_H
#define QGMF_INSTANCE_H

#include "QGMFGlobals.h"

#include <list>
#include <string>

#include "BaseModel.h"

namespace QGMF {

// typename for instance lists. Instance lists store instances as pointers
template<typename T>
using List = std::list<T*>;

// forward decls
class Port;
class Edge;
class Node;

// Base class for all instances, templated by a model
template <typename T>
class Instance {
  protected:
    // FIXME maybe remove useless id, or keep it internal
    Instance(const T &model, const std::string &name = std::string())
      : m_model(model)
      , m_id(0)
      , m_name(std::move(name))
      , m_graphics(nullptr)
      , m_data(nullptr)
    { }

    // can't copy/move: node/port/edge relationship
    // must be consistent
    Instance(const Instance &) = delete;
    Instance(const Instance &&) = delete;

  public:
    const T &model() const { return m_model; }

    int id() const { return m_id; }
    void setId(int id) { m_id = id; }

    const std::string &name() const { return m_name; }
    void setName(const std::string &name) { m_name = std::move(name); }

    bool operator ==(const Instance &other) const { return this == &other; }
    bool operator !=(const Instance &other) const { return this != &other; }

    // convenience functions to set a graphics object. Not used by us.
    void setGraphicsObject(void *o) { m_graphics = o; }
    void *graphicsObject() const { return m_graphics; }

    // set custom data. Not used by us
    void setData(void *d) { m_data = d; }
    void *data() const { return m_data; }

  private:
    const T &m_model;
    int m_id;
    std::string m_name;
    void *m_graphics;
    void *m_data;
};

// Find an instance by its id in a List<T> container
template<typename T>
inline T * findInstance(List<T> t, int id) {
  typename List<T>::const_iterator it =
      std::find_if(t.cbegin(),t.cend(),
                   [&id](const typename List<T>::value_type &arg)
  { return arg->id() == id; });
  return it == t.cend() ? nullptr : *it;
}

// Find an instance by its name in a List<T> container
template<typename T>
inline T * findInstance(List<T> t, const std::string &name) {
  typename List<T>::const_iterator it =
      std::find_if(t.cbegin(),t.cend(),
                   [&name](const typename List<T>::value_type &arg)
  { return arg->name() == name; });
  return it == t.cend() ? nullptr : *it;
}

}
#endif // INSTANCE_H
