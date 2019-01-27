# ifndef OUTPUTFILE_H
# define OUTPUTFILE_H

# include <iostream>
# include <fstream>
# include <vector>
# include <queue>
# include <mutex>

class OutputFile
{
  std::ofstream file;
  std::mutex m;

public:

  OutputFile(const std::string & name) : file(name) {}

  void write_vector(const std::vector<long> & v)
  {
    std::unique_lock<std::mutex> lock(m);
    for (size_t i = 0, n = v.size() - 1; i < n; ++i)
      file << v.at(i) << ",";
    file << v.back() << std::endl;
  }

  void write_priority_queue
  (std::priority_queue<long, std::vector<long>, std::greater<long>> && q)
  {
    std::unique_lock<std::mutex> lock(m);
    while (q.size() > 1)
      {
        file << q.top() << ",";
        //std::cout << q.top() << ",";
        q.pop();
      }
    file << q.top() << std::endl;
    //std::cout << q.top() << std::endl;
  }
};


#endif
