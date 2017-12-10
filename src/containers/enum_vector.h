#ifndef ENUM_VECTOR_H
#define ENUM_VECTOR_H

#include "src/common/enum_range.h"
#include "src/common/enum_misc.h"

template<typename Enum, typename T, typename = use_if_enum<Enum>>
class enum_vector
{
private:
  std::vector<T> m_data;
public:
  enum_vector ()
  {
    for (auto e : enum_range<Enum> ())
      {
        (void)e;
        m_data.push_back (T ());
      }
  }

  T &operator[] (Enum e)
  {
    return m_data[enum_cast<size_t> (e)];
  }

  const T &operator [] (Enum e) const
  {
    size_t i = static_cast<size_t> (e);
    return m_data[i];
  }
};

#endif // ENUM_VECTOR_H
