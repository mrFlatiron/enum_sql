#ifndef ENUM_VECTOR_H
#define ENUM_VECTOR_H

#include "src/common/enum_range.h"
#include "src/common/enum_misc.h"

namespace enum_sql
{
  template<typename Enum, typename T, typename = use_if_enum<Enum>>
  class enum_vector : private std::vector<T>
  {
  private:
    using self = enum_vector<Enum, T>;
    using base = std::vector<T>;
  public:
    enum_vector () : base (static_cast<size_t> (enum_end (Enum ()))) {}

    T &operator [] (Enum e) { return base::operator [] (static_cast<size_t> (e)); }
    const T &operator [] (Enum e) const { return base::operator [] (static_cast<size_t> (e)); }

    using base::size;

    using base::begin;
    using base::cbegin;
    using base::end;
    using base::cend;
    using base::rbegin;
    using base::crbegin;
    using base::rend;
    using base::crend;
  };
}
#endif // ENUM_VECTOR_H
