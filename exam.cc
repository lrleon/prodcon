
# include <cassert>
# include <iostream>
# include <functional>
# include <vector>
# include <queue>
# include <thread>
# include <condition_variable>
# include <set>

# include <utils.H>
# include <item_queue.H>
# include <outputfile.H>
# include <sorting.H>

using namespace std;

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

constexpr size_t Max_Num_Of_Threads = 4;

int main(int argc, char * argv[])
{
  raise_domain_error_unless(argc == 4)
    << "Usage is" << endl
    << "  ./test path-input-file path-output-file quick|merge" << endl;

  const string sort_algo = argv[3];
  raise_domain_error_unless(sort_algo == "quick" or sort_algo == "merge")
    << "Last parameter must be \"quick\" or \"merge\"" << endl;

  const string input_name = argv[1];
  ifstream input(input_name);
  raise_domain_error_unless(input.is_open()) << "cannot open " << input_name;

  ItemQueue q;
  OutputFile output(argv[2]);

  function<void(vector<long>&)> sort_method;
  if (sort_algo == "merge")
    sort_method = merge_sort;
  else
    sort_method = quick_sort;

  // Prepare the worker threads
  vector<thread> workers;
  for (size_t i = 0; i < Max_Num_Of_Threads; ++i)
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
