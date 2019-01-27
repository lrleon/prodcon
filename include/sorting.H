# ifndef SORTING_H
# define SORTING_H

# include <vector>

inline size_t select_pivot(const std::vector<long> & a, size_t l, size_t r)
  noexcept
{
  if (r - l <= 2)
    return r;

  size_t m = (r + l) / 2;
  size_t p = a.at(l) < a.at(m) ? m : l;
  return a.at(r) < a.at(m) ? r : p;
}

int partition(std::vector<long> & a, size_t l, size_t r) noexcept
{
  size_t p = select_pivot(a, l, r);
  std::swap(a[p], a[r]);

  long pivot = a[r];
  size_t i = l - 1, j = r;
  while (true)
    {
      while (a.at(++i) < pivot) { /* nothing to do */ }

      while (pivot < a.at(--j))
        if (j == l)
          break;

      if (i >= j)
        break;

      std::swap(a.at(i), a.at(j));
    }

  std::swap(a.at(i), a.at(r));

  return i;
}

void insertionsort(std::vector<long> & v, size_t l, size_t r) noexcept
{
  for (size_t i = l, j; i <= r; ++i)
    {
      long tmp = v.at(i);
      for (j = i; j > l and tmp < v.at(j - 1); --j)
        v.at(j) = v.at(j - 1);
      v.at(j) = tmp;
    }
}

void insertion_sort(std::vector<long> & v) noexcept
{
  insertionsort(v, 0, v.size() - 1);
}

constexpr size_t Quicksort_Threshold = 40;

void quicksort(std::vector<long> & a, size_t l, size_t r) noexcept
{
  if (r - l < Quicksort_Threshold)
    {
      insertionsort(a, l, r);
      return;
    }

  size_t pivot = partition(a, l, r);
  // smaller partition must be sorted first so we consume O(log n) of stack
  if (pivot - l < r - pivot)
    {
      quicksort(a, l, pivot - 1);
      quicksort(a, pivot + 1, r);
    }
  else
    {
      quicksort(a, pivot + 1, r);
      quicksort(a, l, pivot - 1);
    }
}

void quick_sort(std::vector<long> & v) noexcept
{
  quicksort(v, 0, v.size() - 1);
}

void merge(std::vector<long> & v, size_t l, size_t m, size_t r) noexcept
{
  size_t i, j, sz;
  sz = r - l + 1; // size of temporal vector for doing merge
  std::vector<long> b;
  b.reserve(sz);

  for (i = l; i <= m; ++i)
    b.push_back(v.at(i));

  for (j = r; j > m; --j) // inverted copy of v[m+1,r]
    b.push_back(v.at(j));

  for (size_t k = l, i = 0, j = sz - 1; k <= r; ++k)
    if (b.at(i) < b.at(j))
      v.at(k) = b.at(i++);
    else
      v.at(k) = b.at(j--);
}

void mergesort(std::vector<long> & v, size_t l, size_t r) noexcept
{
  if (l >= r)
    return;

  const size_t m = (l + r)/2;

  mergesort(v, l, m);
  mergesort(v, m + 1, r);

  if (v.at(m) < v.at(m + 1)) // avoid call to merge if v is already sorted
    return;

  merge(v, l, m, r);
}

void merge_sort(std::vector<long> & v) noexcept
{
  mergesort(v, 0, v.size() - 1);
}

// used for testing
bool is_sorted(const std::vector<long> & v) noexcept
{
  if (v.empty())
    return true;

  long prev = v.at(0);
  for (size_t i = 1, n = v.size(); i < n; ++i)
    {
      auto & curr = v.at(i);
      if (curr > prev)
        return false;

      prev = curr;
    }
  return true;
}

# endif
