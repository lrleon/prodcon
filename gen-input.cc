
# include <iostream>
# include <random>

# include <tclap/CmdLine.h>

using namespace std;
using namespace TCLAP;

CmdLine cmd = { "gen-input", ' ', "0" };

ValueArg<size_t> num_items =
  { "n", "num-items", "number of items", false, 10, "number of items", cmd };

ValueArg<size_t> num =
  { "m", "numbers", "number of samples", false, 500, "number of samples", cmd };

ValueArg<long> lowest =
  { "L", "lower", "lowest value", false, 0, "lowest value", cmd };

ValueArg<long> highest =
  { "H", "highest", "highest value", false, 10000, "highest value", cmd };

ValueArg<long> seed = { "s", "seed", "seed", false, 0, "seed", cmd };

int main(int argc, char *argv[])
{
  cmd.parse(argc, argv);

  random_device r;
  mt19937 gen(seed.getValue());
  uniform_int_distribution<long> dis(lowest.getValue(), highest.getValue());

  for (size_t i = 0, n = num_items.getValue(), m = num.getValue(); i < n; ++i)
    {
      long sample = dis(gen);
      cout << sample;
      for (size_t k = 1; k < m; ++k)
        {
          sample = dis(gen);
          cout << " " << sample;
        }
      cout << endl;
    }
}

