#ifndef EEPP_BOUND_FCN_HPP
#define EEPP_BOUND_FCN_HPP

#include <functional>
#include <memory>

namespace eepp
{

struct bound_base {
  virtual ~bound_base() {}
};

template <typename... args>
struct bound_fcn: public bound_base {
  struct function_proto {
    virtual ~function_proto() {}
    virtual bool operator== (const function_proto &that) = 0;
    virtual void operator()(args...) = 0;
  };

  template<typename fcn, typename... bvs>
  struct func_args: public function_proto
  {
    func_args(fcn f, bvs... bv):
      unbound_fcn(f),
      callback(std::bind(f, bv...))
    {}
    virtual ~func_args() {}

    virtual bool operator== (const function_proto &that)
    {
      return unbound_fcn == static_cast<const func_args<fcn, bvs...> &>(that).unbound_fcn;
    }

    virtual void operator()(args... a)
    {
      callback(a...);
    }

    template <typename... fcn_args>
    using bound_fcn = decltype(std::bind(std::declval<fcn_args&>() ...));

    fcn unbound_fcn;
    bound_fcn<fcn, bvs...> callback;
  };

  std::shared_ptr<function_proto> fp;

public:
  template <typename fcn, typename... bvs>
  bound_fcn(fcn f, bvs... bv):
    fp(new func_args<fcn, bvs...>(f, bv...))
  {}

  virtual ~bound_fcn() {}

  bool operator== (const bound_fcn &that)
  {
    return *fp == *that.fp;
  }

  void operator()(args... a)
  {
    (*fp)(a...);
  }
};

} // namespace eepp

#endif // EEPP_BOUND_FCN_HPP
