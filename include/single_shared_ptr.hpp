#ifndef SINGLE_SHARED_PTR_HPP
#define SINGLE_SHARED_PTR_HPP

#include <memory>

template <typename T>
using shared_ptr_single = std::__shared_ptr<T, __gnu_cxx::_S_single>;

//void test()
//{
//    std::shared_ptr<int> ptr_1 = std::make_shared<int>(2);
//    shared_ptr_single<int> ptr_2 = std::make_shared<int>(2);
//    shared_ptr_single<int> copy_ptr(ptr_1.get());
//}

#endif // SINGLE_SHARED_PTR_HPP
