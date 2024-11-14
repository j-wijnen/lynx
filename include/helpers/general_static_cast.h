#pragma once

template<class Tin, class Tout>
inline Tout
general_static_cast(Tin value)
{
  return static_cast<Tout>(value);
}

template<class Tout>
inline Tout
general_static_cast(ADReal value)
{
  return static_cast<Tout>(value.value());
}