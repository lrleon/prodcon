# ifndef SORTING_H
# define SORTING_H

# include <cassert>
# include <vector>

/** This header contains the sorting methods.

    Two sorting algorithms are implemented: quicksort and top-down
    mergesort.

    Because both methods are recursive and relatively expensive for
    small vectors, the methods uses a threshold on the sorting size
    and call the insertion sort if the size is less than the
    threshold. Insertion sort is a lot much simpler and with better
    performance on small sizes.

 */

static inline
void insertionsort(std::vector<long> & v, long l, long r) noexcept
{
  for (long i = l, j; i <= r; ++i)
    {
      long tmp = v.at(i);
      for (j = i; j > l and tmp < v.at(j - 1); --j)
        v.at(j) = v.at(j - 1);
      v.at(j) = tmp;
    }
}

inline void insertion_sort(std::vector<long> & v)
{
  insertionsort(v, 0, v.size() - 1);
}


/// Select the pivot for quicksort partition as the median of extremes
/// and the center of vector `a`
static inline long select_pivot(const std::vector<long> & a, long l, long r)
  noexcept
{
  if (r - l <= 2)
    return r;

  long m = (r + l) / 2;
  long p = a.at(l) < a.at(m) ? m : l;
  return a.at(r) < a.at(m) ? r : p;
}

// Sedgewick version of quicksort partition
static inline long partition(std::vector<long> & a, long l, long r) noexcept
{
  long p = select_pivot(a, l, r);
  std::swap(a[p], a[r]);

  long pivot = a[r];
  long i = l - 1, j = r;
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

static constexpr long Quicksort_Threshold = 40; // Selected by
                                                // intuition. Needed
                                                // more tests

/** Special recursive quicksort which:

    1- Call to insertion sort for partitions less than Quicksort_Threshold.

    2- Because bad cases could eventually recursion O(n) times and
       easily cause a stack overflow (n is the
       number of elements), the partition size is inspected and the
       smallest partition is sorted first. In this way, we guarantee
       that recursion chain is not longer than O(log n). This feature
       could be critical if the quicksort is called from
       threads. because threads tend to have less stack than a
       traditional process.

*/
static inline void quicksort(std::vector<long> & a, long l, long r) noexcept
{
  if (l > r)
    return;

  if (r - l < Quicksort_Threshold)
    {
      insertionsort(a, l, r);
      return;
    }

  long pivot = partition(a, l, r);
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

inline void quick_sort(std::vector<long> & v) noexcept
{
  quicksort(v, 0, v.size() - 1);
}

/** Merge a vector which has two sorted sections.

    This routine uses an additional vector allocated in the heap. I
    prefer in the heap because I am using threads and to allocate it
    in the stack could overflow the thread stack.
 */
static inline void merge(std::vector<long> & v, long l, long m, long r)
  noexcept
{
  long i, j, sz;
  sz = r - l + 1; // size of temporal vector for doing merge
  std::vector<long> b;
  b.reserve(sz);

  for (i = l; i <= m; ++i)
    b.push_back(v.at(i));

  for (j = r; j > m; --j) // inverted copy of v[m+1,r]
    b.push_back(v.at(j));

  for (long k = l, i = 0, j = sz - 1; k <= r; ++k)
    if (b.at(i) < b.at(j))
      v.at(k) = b.at(i++);
    else
      v.at(k) = b.at(j--);
}

/// sort v with top-down mergesort
inline void mergesort(std::vector<long> & v, long l, long r) noexcept
{
  if (l >= r)
    return;

  const long m = (l + r)/2;

  mergesort(v, l, m);
  mergesort(v, m + 1, r);

  if (v.at(m) < v.at(m + 1)) // avoid call to merge if v is already sorted
    return;

  merge(v, l, m, r);
}

inline void merge_sort(std::vector<long> & v) noexcept
{
  mergesort(v, 0, v.size() - 1);
}

// Return true if v is sorted; used for testing
inline bool is_sorted(const std::vector<long> & v) noexcept
{
  if (v.empty())
    return true;

  long prev = v.at(0);
  for (long i = 1, n = v.size(); i < n; ++i)
    {
      auto & curr = v.at(i);
      if (curr > prev)
        return false;

      prev = curr;
    }
  return true;
}

# endif
