#ifndef EMITTER_OBJECT_HPP
#define EMITTER_OBJECT_HPP

#include <event-emitter++/emitter.hpp>

/*! \brief  a simple class to expose the emit functionality of the emitter
 *
 */

struct generic_event
{
  enum
  {
    nothing,
    one_thing,
    two_things,
  };
};

class emitter_object: public eepp::emitter
{
public:
  template <typename... args>
  void emit_event(int event_id, args... a)
  {
    this->emit(event_id, a...);
  }
};


#endif // EMITTER_OBJECT_HPP
