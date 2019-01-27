
# include <cassert>
# include <iostream>
# include <fstream>
# include <sstream>
# include <functional>
# include <vector>
# include <queue>
# include <thread>
# include <condition_variable>
# include <set>

# include <tclap/CmdLine.h>

# define raise_domain_error_unless(cond)                                \
  if (__builtin_expect(not(cond), 0))                                        \
    ExceptionBuilder<std::domain_error>(), std::stringstream()

template <class E>
struct ExceptionBuilder
{
  int operator, (const std::ostream &s)
  {
    throw E(static_cast<const std::ostringstream &>(s).str());
    return 0;
  }
};

using namespace std;
using namespace TCLAP;

class ItemQueue
{
  queue<string> q;
  bool finished = false; // true when no more items will be read into the queue
  mutex m;
  condition_variable cond_var;

public:

  // Return true if queue has been marked as finished and queue is
  // empty. This indicates that no more readings are needed
  bool is_finished() noexcept
  {
    unique_lock<mutex> lock(m);
    return finished and q.empty();
  }

  // Mark the queue as finished and signal and wake up all waiting
  // threads so that they could finish
  void finish() noexcept
  {
    unique_lock<mutex> lock(m);
    finished = true;
  }

  void put(string && item) noexcept
  {
    unique_lock<mutex> lock(m);
    q.push(forward<string>(item));
    cond_var.notify_one(); // wake up a waiting thread that has performed get
  }

  optional<string> get() noexcept
  {
    unique_lock<mutex> lock(m);

    while (true)
      {
        if (not q.empty())
          {
            string item = move(q.front());
            q.pop();
            return item;
          }

        if (finished)
          return {};

        // in this case queue is empty but the writing in the queue
        // has not still finished. So we suspend and wait for
        // signaling when a item is put into the queue or the writing
        // finishes
        cond_var.wait(lock);
      }
  }
};

class OutputFile
{
  ofstream file;
  mutex m;

public:

  OutputFile(const string & name) : file(name) {}

  void write_vector(const vector<long> & v)
  {
    unique_lock<mutex> lock(m);
    for (size_t i = 0, n = v.size() - 1; i < n; ++i)
      file << v.at(i) << ",";
    file << v.back() << endl;
  }
};

// build a vector with the numbers contained in the item
vector<long> extract_numbers_from_item(string && item, char delim = ' ')
  noexcept
{
  vector<long> ret;
  string str;
  for (auto c : item)
    {
      if (c == delim)
        {
          if (str.size() > 0)
            {
              ret.push_back(atol(str.c_str()));
              str.erase();
            }
        }
      else
        str.push_back(c);
    }

  if (not str.empty())
    ret.push_back(atol(str.c_str()));

  return ret;
}

inline size_t select_pivot(const vector<long> & a, size_t l, size_t r)
  noexcept
{
  if (r - l <= 2)
    return r;

  size_t m = (r + l) / 2;
  size_t p = a.at(l) < a.at(m) ? m : l;
  return a.at(r) < a.at(m) ? r : p;
}


int partition(vector<long> & a, size_t l, size_t r) noexcept
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

void insertionsort(vector<long> & v, size_t l, size_t r) noexcept
{
  for (size_t i = l, j; i <= r; ++i)
    {
      long tmp = v.at(i);
      for (j = i; j > l and tmp < v.at(j - 1); --j)
        v.at(j) = v.at(j - 1);
      v.at(j) = tmp;
    }
}

void insertion_sort(vector<long> & v) noexcept
{
  insertionsort(v, 0, v.size() - 1);
}

constexpr size_t Quicksort_Threshold = 40;

void quicksort(vector<long> & a, size_t l, size_t r) noexcept
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

void quick_sort(vector<long> & v) noexcept
{
  quicksort(v, 0, v.size() - 1);
}

void merge(vector<long> & v, size_t l, size_t m, size_t r) noexcept
{
  size_t i, j, sz;
  sz = r - l + 1; // size of temporal vector for doing merge
  vector<long> b;
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

void mergesort(vector<long> & v, size_t l, size_t r) noexcept
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

void merge_sort(vector<long> & v) noexcept
{
  mergesort(v, 0, v.size() - 1);
}

// used for testing
bool is_sorted(const vector<long> & v) noexcept
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

void worker_thread(ItemQueue & q, OutputFile & output,
                   function<void(vector<long>&)> sort_method)
{
  while (true)
    {
      optional<string> item = q.get();
      if (not item.has_value())
        break; // if no item ==> writing into queue has finished

      vector<long> v = extract_numbers_from_item(move(item.value()));
      sort_method(v);
      output.write_vector(v);
    }
}

void read_input_and_dispatch_to_workers(const string & file_name,
                                        ItemQueue & q, OutputFile & output)
{
  ifstream input(file_name);
  string item;
  while (getline(input, item))
    q.put(move(item));
}

CmdLine cmd = { "test", ' ', "0" };

ValueArg<string> input_name =
  { "i", "input", "input file name", true, "", "input file name", cmd };

ValueArg<string> output_name =
  { "o", "output", "output file name", true, "", "output file name", cmd };

// Please note that the default value is 4 (the required one)
ValueArg<size_t> num_threads = { "n", "num_threads", "number of threads",
                                 false, 4, "number of threads", cmd };

SwitchArg test = { "t", "test", "run correctness test", cmd };

vector<string> sort_types = { "insertion", "merge", "std", "quick" };
ValuesConstraint<string> allowed_sort_types = sort_types;
ValueArg<string> sort_type =
  { "S", "sort-type", "sorting method", false, "insertion",
    &allowed_sort_types, cmd };

constexpr size_t Max_Num_Of_Threads = 16; // ok for some modern processors

// Comparator functor only used for testing purposes
struct CmpVector
{
  bool operator () (const vector<long> & v1, const vector<long> & v2) const
  {
    for (size_t i = 0, n = min(v1.size(), v2.size()); i < n; ++i)
      {
        auto & i1 = v1.at(i);
        auto & i2 = v2.at(i);
        if (i1 == i2)
          continue;
        return i1 < i2;
      }
    return v1.size() < v2.size();
  }
};

int main(int argc, char * argv[])
{
  cmd.parse(argc, argv);

  {
    ifstream input(input_name.getValue());
    raise_domain_error_unless(input.is_open())
      << "cannot open " << input_name.getValue();

    ItemQueue q;
    OutputFile output(output_name.getValue());

    function<void(vector<long>&)> sort_method;
    const string & sort_algo = sort_type.getValue();
    if (sort_algo == "insertion")
      sort_method = insertion_sort;
    else if (sort_algo == "merge")
      sort_method = merge_sort;
    else if (sort_algo == "quick")
      sort_method = quick_sort;
      sort_method = [] (vector<long> & v) { sort(v.begin(), v.end()); };

    raise_domain_error_unless(num_threads.getValue() < Max_Num_Of_Threads)
      << "Number of threads " << num_threads.getValue()
      << " is greater than the max allowed " << Max_Num_Of_Threads;;

    // Prepare the worker threads
    vector<thread> workers;
    for (size_t i = 0, n = num_threads.getValue(); i < n; ++i)
      {
        thread th(worker_thread, ref(q), ref(output), sort_method);
        workers.push_back(move(th));
      }

    // Now we read input file and put every line into the protected
    // queue. Suspended threads will resume and consume them as they
    // are inserted
    string line;
    while (getline(input, line))
      q.put(move(line));
    q.finish();

    for (auto & th : workers)
      th.join();
  }

  if (not test.getValue())
    return 0;

  cout << "Testing correctness" << endl;
  ifstream input(input_name.getValue()), output(output_name.getValue());
  string line;
  set<vector<long>, CmpVector> entries;
  size_t num_items = 0;
  while (getline(input, line))
    {
      vector<long> v = extract_numbers_from_item(move(line));
      sort(v.begin(), v.end());
      entries.insert(move(v));
      ++num_items;
    }

  bool error_found = false;
  for (size_t i = 1; getline(output, line); ++i)
    {
      vector<long> v = extract_numbers_from_item(move(line), ',');
      auto it = entries.find(v);
      if (it == entries.end())
        {
          cout << "Detected problem with line " << i << " of output" << endl;
          error_found = true;
        }
      --num_items;
    }

  if (num_items != 0)
    cout << num_items << " lines were not processed" << endl;

  if (not error_found)
    cout << "Everything is ok!" << endl;
}
