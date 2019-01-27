# ifndef UTILS_H
# define UTILS_H

# include <iostream>
# include <sstream>
# include <vector>
# include <queue>

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


// build a vector with the numbers contained in the item
inline std::vector<long> item_to_vector(std::string && item, char delim = ' ')
  noexcept
{
  std::vector<long> ret;
  std::string str;
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

inline std::priority_queue<long, std::vector<long>, std::greater<long>>
item_to_priority_queue(std::string && item)
{
  std::priority_queue<long, std::vector<long>, std::greater<long>> ret;
  std::string str;
  for (auto c : item)
    {
      if (c == ' ')
        {
          if (str.size() > 0)
            {
              ret.push(atol(str.c_str()));
              str.erase();
            }
        }
      else
        str.push_back(c);
    }

  if (not str.empty())
    ret.push(atol(str.c_str()));

  return ret;
}

# endif
