
# include <cassert>
# include <iostream>
# include <functional>
# include <vector>
# include <queue>
# include <thread>
# include <condition_variable>
# include <set>

# include <tclap/CmdLine.h>

# include <utils.H>
# include <item_queue.H>
# include <outputfile.H>
# include <sorting.H>

using namespace std;
using namespace TCLAP;

void worker_thread(ItemQueue & q, OutputFile & output,
                   function<void(vector<long>&)> sort_method)
{
  while (true)
    {
      optional<string> item = q.get();
      if (not item.has_value())
        break; // if no item ==> writing into queue has finished

      vector<long> v = item_to_vector(move(item.value()));
      sort_method(v);
      output.write_vector(v);
    }
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
    else
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
      vector<long> v = item_to_vector(move(line));
      sort(v.begin(), v.end());
      entries.insert(move(v));
      ++num_items;
    }

  bool error_found = false;
  for (size_t i = 1; getline(output, line); ++i)
    {
      vector<long> v = item_to_vector(move(line), ',');
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
