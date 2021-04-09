#pragma once

#include <array>

namespace Tron
{
template <typename T, std::size_t Dim>
class SmallVector
{
 public:
  std::size_t size() const { return mSize; }
  bool empty() const { return mSize == 0; }
  void clear() { mSize = 0; }
  const T& operator[](std::size_t i) const { return mData[i]; }
  void push_back(T value) { mData[mSize++] = value; }

 private:
  std::size_t mSize = 0;
  std::array<T, Dim> mData;
};

}  // namespace Tron
