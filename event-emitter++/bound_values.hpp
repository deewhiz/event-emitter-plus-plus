#ifndef EEPP_BOUND_VALUES_HPP
#define EEPP_BOUND_VALUES_HPP

#include <functional>
#include <tuple>

namespace eepp
{

template <typename... bound_args>
class bound_values
{
public:
  bound_values(bound_args... args):
    arg_list(std::make_tuple(args...))
  {}

  bool operator ==(const bound_values &that)
  {
    return compare(arg_list, that.arg_list);
  }

protected:
  // compare non-placeholder elements using '=='
  template <typename param>
  typename std::enable_if<!std::is_placeholder<param>::value, bool>::type
    compare_element(const param &lhs, const param &rhs)
  {
    return lhs == rhs;
  }

  // "compares" placeholder elements
  // assumes the arguments are the same place holder and thus always returns true
  template <typename param, typename = typename std::enable_if<std::is_placeholder<param>::value, bool>::type>
  bool compare_element(const param &, const param &)
  {
    return true;
  }

  template<std::size_t index = 0, typename... bound_args>
  typename std::enable_if<index < sizeof...(bound_args), bool>::type
    compare(const std::tuple<bound_args...>& t, const std::tuple<bound_args...> & t2)
  {
    std::cout << "comparing element " << index << std::endl;
    return compare_element(std::get<index>(t), std::get<index>(t2)) &&
        compare<index + 1, bound_args...>(t, t2);
  }

  // specialization: reached the end of the tuple
  template<std::size_t index = 0, typename... bound_args>
  typename std::enable_if<index == sizeof...(bound_args), bool>::type
    compare(const std::tuple<bound_args...> &, const std::tuple<bound_args...> &)
  {
    return true;
  }

  std::tuple<bound_args...> arg_list;
};

} // namespace eepp

#endif // EEPP_BOUND_VALUES_HPP
