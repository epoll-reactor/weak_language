#ifndef SINGLE_SHARED_PTR_HPP
#define SINGLE_SHARED_PTR_HPP

#include <memory>

template <typename T>
using shared_ptr_single = std::__shared_ptr<T, __gnu_cxx::_S_single>;

#endif // SINGLE_SHARED_PTR_HPP
