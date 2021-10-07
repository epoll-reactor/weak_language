#ifndef WEAKLANGUAGE_CUT_LAST_ITERATOR_HPP
#define WEAKLANGUAGE_CUT_LAST_ITERATOR_HPP

// clang-format off
template<class T, class U>
class cut_last_iterator {
public:
  cut_last_iterator(T begin, U end)
    : begin_(begin), end_(end) {}
  T begin() { return begin_; }
  U end() { return --end_; }
private:
  T begin_;
  U end_;
};
// clang-format on

template <typename Container>
auto cut_last(Container&& c) {
  return cut_last_iterator(c.begin(), c.end());
}

#endif//WEAKLANGUAGE_CUT_LAST_ITERATOR_HPP
