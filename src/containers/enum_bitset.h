#ifndef ENUM_BITSET_H
#define ENUM_BITSET_H

#include "src/common/enum_misc.h"
#include <bitset>

namespace enum_sql
{

template<typename Enum, typename = use_if_enum<Enum>>
class enum_bitset : private std::bitset<static_cast<size_t> (enum_sql::enum_end (Enum ()))>
{
  using self = enum_bitset<Enum>;
  using base = std::bitset<static_cast<size_t> (enum_end (Enum ()))>;
  public:
  enum_bitset () {}

  bool operator[] (Enum e) const {return base::operator[] (enum_cast<size_t> (e)); }
  using base::all;
  using base::any;
  using base::none;
  using base::count;
  using base::size;

  self &flip () { base::flip (); return *this; }
  self &reset () { base::reset (); return *this; }
  self &set (Enum e, const bool state) { base::operator[] (enum_cast<size_t> (e)) = state; return *this; }
  self &set (const bool state)       {for (auto e : enum_range<Enum> ()) {set (e, state);} return *this; }
  self &operator|= (const self &rhs) { base::operator|= (static_cast<const base &> (rhs)); return *this; }
  self &operator&= (const self &rhs) { base::operator&= (static_cast<const base &> (rhs)); return *this; }
  self &operator^= (const self &rhs) { base::operator^= (static_cast<const base &> (rhs)); return *this; }
};
}
#endif // ENUM_BITSET_H
