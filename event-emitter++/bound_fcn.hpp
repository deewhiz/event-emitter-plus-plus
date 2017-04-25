#ifndef EEPP_BOUND_FCN_HPP
#define EEPP_BOUND_FCN_HPP

#include "bound_values.hpp"

#include <functional>
#include <memory>
#include <typeindex>

namespace eepp
{

struct bound_base {
  virtual ~bound_base() {}
};

template <typename... params>
struct bound_fcn: public bound_base {
  struct function_interface {
    virtual ~function_interface() {}
    virtual void operator ()(params...) = 0;
    virtual bool operator ==(const function_interface &) = 0;
  };

  template<typename fcn, typename... bound_vals>
  struct func_with_args: public function_interface
  {
    func_with_args(fcn function, bound_vals... bound_values):
      unbound_fcn(function),
      bound_args(bound_values...),
      callback(std::bind(function, bound_values...))
    {}

    virtual ~func_with_args() {}

    virtual void operator ()(params... func_args)
    {
      callback(func_args...);
    }

    virtual bool operator ==(const function_interface &that)
    {
      // should only get here if the function types match; casting should be fine
      return bound_args == static_cast<const func_with_args &>(that).bound_args;
    }

    template <typename... fcn_args>
    using bound_fcn = decltype(std::bind(std::declval<fcn_args&>() ...));

    fcn unbound_fcn;
    bound_values<bound_vals...> bound_args;
    bound_fcn<fcn, bound_vals...> callback;
  };

  std::shared_ptr<function_interface> func_prototype;
  std::type_index bound_func_type;

public:
  template <typename fcn, typename... bound_vals>
  bound_fcn(fcn function, bound_vals... bound_values):
    func_prototype(new func_with_args<fcn, bound_vals...>(function, bound_values...)),
    bound_func_type(typeid(decltype(std::declval<func_with_args<fcn, bound_vals...>>())))
  {}

  virtual ~bound_fcn() {}

  void operator()(params... func_args)
  {
    (*func_prototype)(func_args...);
  }

  template <typename... that_params>
  bool operator ==(const bound_fcn<that_params...> &that)
  {
    return false;
  }

  bool operator ==(const bound_fcn &that)
  {
    return bound_func_type == that.bound_func_type && *func_prototype == *that.func_prototype;
  }
};

} // namespace eepp

#endif // EEPP_BOUND_FCN_HPP
