#ifndef RANGE_HH
#define RANGE_HH

#include <iterator>
#include <utility>

template <typename Iterator>
class Range {
 public:
  Range(Iterator b, Iterator e) : m_begin(b), m_end(e) {}
  Range(Range&&) = default;
  Range(const Range&) = default;
  ~Range() = default;
  Range& operator=(Range&&) = default;
  Range& operator=(const Range&) = default;

  Iterator begin() const { return m_begin; }
  Iterator end() const { return m_end; }
  bool empty() const { return m_begin == m_end; }
  std::size_t size() const { return std::distance(m_begin, m_end); }

 private:
  Iterator m_begin;
  Iterator m_end;
};

template <typename Iterator>
Range<Iterator> makeRange(Iterator begin, Iterator end) {
  return Range<Iterator>(begin, end);
}

template <typename Iterator>
Range<Iterator> makeRange(std::pair<Iterator, Iterator> range) {
  return Range<Iterator>(range.first, range.second);
}

#endif