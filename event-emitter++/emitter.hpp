#ifndef EEPP_BOUND_EMITTER_HPP
#define EEPP_BOUND_EMITTER_HPP

#include "bound_fcn.hpp"

#include <functional>
#include <map>
#include <list>
#include <typeindex>

namespace eepp
{

class emitter
{
public:
  /*! \brief Attach an event listener
   *
   * \param event_id The event to listen for
   * \param fcn The function to be used as a callback
   *
   * \return A handler id to be used in when removing handlers
   */
  template <typename ret>
  uint64_t on(int event_id, ret(*fcn)())
  {
    return attach_handler(event_id, bound_fcn<>(fcn), true);
  }

  /*! \brief Attach an event listener
   *
   * \param event_id The event to listen for
   * \param bound The bound_fnc to be used as a callback
   *
   * \return A handler id to be used in when removing handlers
   */
  template <typename... args>
  uint64_t on(int event_id, const bound_fcn<args...> &bound)
  {
    return attach_handler(event_id, bound);
  }

  /*! \brief Attach an event listener to be called only once
   *
   * \param event_id The event to listen for
   * \param fcn The function to be used as a callback
   *
   * \return A handler id to be used in when removing handlers
   */
  template <typename ret>
  uint64_t once(int event_id, ret(*fcn)())
  {
    return attach_handler(event_id, bound_fcn<>(fcn), true);
  }

  /*! \brief Attach an event listener to be called only once
   *
   * \param event_id The event to listen for
   * \param bound The bound_fnc to be used as a callback
   *
   * \return A handler id to be used in when removing handlers
   */
  template <typename... args>
  uint64_t once(int event_id, const bound_fcn<args...> &bound)
  {
    return attach_handler(event_id, bound, true);
  }

  /*! \brief Remove one event listener
   *
   * \param event_id The event to remove a listener for
   * \param handler_id The handler id previous return by a call to on
   *
   */
  template <typename... args>
  void remove_handler(int event_id, uint64_t handler_id)
  {
    detach_handler(event_id, handler_id);
  }

  /*! \brief Remove all event listener for an event
   *
   * \param event_id The event to remove a listener for
   *
   */
  void remove_handlers(int event_id)
  {
    std::map<std::type_index, std::list<event_handler>>().swap(handlers[event_id]);
  }

  /*! \brief Remove all event listeners
   *
   */
  void remove_handlers()
  {
    std::map<int, std::map<std::type_index, std::list<event_handler>>>().swap(handlers);
  }

protected:
  /*! \brief Emits an event to call callbacks
   *
   * \param event_id The event to "emit"
   * \param ...a The arguments to be passed to each callback function
   */
  template <typename... args>
  void emit(int event_id, args ...a)
  {
    if (handlers.count(event_id) && handlers[event_id].count(typeid(bound_fcn<args...>)))
    {
      std::list<event_handler> &handler_list = handlers[event_id][typeid(bound_fcn<args...>)];

      auto handler = handler_list.begin();
      while (handler != handler_list.end())
      {
        (*static_cast<bound_fcn<args...> *>(&*(handler->callback)))(a...);

        if (handler->one_time)
          handler = handler_list.erase(handler);
        else
          handler++;
      }
    }
  }

private:
  template <typename... args>
  uint64_t  attach_handler(int event_id, bound_fcn<args...> func, bool once = false)
  {
    static uint64_t uid = 0;
    handlers[event_id][typeid(bound_fcn<args...>)].push_back({
      std::shared_ptr<bound_base>(new bound_fcn<args...>(func)), once, ++uid
    });

    return uid;
  }

  void detach_handler(int event_id, uint64_t handler_id)
  {
    if (!handlers.count(event_id)) return;

    std::map<std::type_index, std::list<event_handler>> &handler_map = handlers[event_id];
    std::map<std::type_index, std::list<event_handler>>::iterator handler_iter;

    for (handler_iter = handler_map.begin(); handler_iter != handler_map.end(); handler_iter++)
    {
      std::list<event_handler> &handler_list = handler_iter->second;
      std::list<event_handler>::iterator handler;

      for (handler = handler_list.begin(); handler != handler_list.end(); handler++)
        if (handler->id == handler_id)
        {
          handler_list.erase(handler);
          break;
        }
    }
  }

  struct event_handler
  {
    std::shared_ptr<bound_base> callback;
    bool one_time;
    uint64_t id;
  };

  std::map<int, std::map<std::type_index, std::list<event_handler>>> handlers;
};

} // namespace eepp

#endif // EEPP_BOUND_EMITTER_HPP
