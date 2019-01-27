# ifndef OUTPUTFILE_H
# define OUTPUTFILE_H

# include <iostream>
# include <fstream>
# include <vector>
# include <queue>
# include <mutex>

# include <utils.H>

/** This is a simple thread safe file writer.

    The constructor, who expects a file name, creates a new text file
    where vectors of longs will be written.

    The `write_vector()` method writes the vector content with its
    values separated by commas. Each vector occupies a full line.
 */
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
};


#endif
