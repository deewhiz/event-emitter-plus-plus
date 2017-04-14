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
   * \return This emitter to chain method calls
   */
  template <typename ret>
  emitter &on(int event_id, ret(*fcn)())
  {
    attach_handler(event_id, bound_fcn<>(fcn), true);

    return *this;
  }

  /*! \brief Attach an event listener
   *
   * \param event_id The event to listen for
   * \param bound The bound_fnc to be used as a callback
   *
   * \return This emitter to chain method calls
   */
  template <typename... args>
  emitter &on(int event_id, const bound_fcn<args...> &bound)
  {
    attach_handler(event_id, bound);

    return *this;
  }

  /*! \brief Attach an event listener to be called only once
   *
   * \param event_id The event to listen for
   * \param fcn The function to be used as a callback
   *
   * \return This emitter to chain method calls
   */
  template <typename ret>
  emitter &once(int event_id, ret(*fcn)())
  {
    attach_handler(event_id, bound_fcn<>(fcn), true);

    return *this;
  }

  /*! \brief Attach an event listener to be called only once
   *
   * \param event_id The event to listen for
   * \param bound The bound_fnc to be used as a callback
   *
   * \return This emitter to chain method calls
   */
  template <typename... args>
  emitter &once(int event_id, const bound_fcn<args...> &bound)
  {
    attach_handler(event_id, bound, true);

    return *this;
  }

  /*! \brief Remove one event listener
   *
   * \param event_id The event to listen for
   * \param fcn The function to be used as a callback
   *
   * \return This emitter to chain method calls
   */
  template <typename ret>
  emitter &remove_handler(int event_id, ret(*fcn)())
  {
    attach_handler(event_id, bound_fcn<>(fcn), true);

    return *this;
  }

  /*! \brief Remove one event listener
   *
   * \param event_id The event to remove a listener for
   * \param bound The bound_fnc to remove
   *
   * \return This emitter to chain method calls
   */
  template <typename... args>
  emitter &remove_handler(int event_id, const bound_fcn<args...> &bound)
  {
    detach_handler(event_id, bound);

    return *this;
  }

  /*! \brief Remove event listeners for an event
   *
   * \param event_id The event to remove a listener for
   * \param bound The bound_fnc to remove
   *
   * \return This emitter to chain method calls
   */
  template <typename... args>
  emitter &remove_handlers(int event_id, const bound_fcn<args...> &bound)
  {
    detach_handlers(event_id, bound);

    return *this;
  }


  /*! \brief Remove all event listener for an event
   *
   * \param event_id The event to remove a listener for
   *
   * \return This emitter to chain method calls
   */
  emitter &remove_all(int event_id)
  {
    std::map<std::type_index, std::list<event_handler>>().swap(handlers[event_id]);

    return *this;
  }

  /*! \brief Remove all event listeners
   *
   * \return This emitter to chain method calls
   */
  emitter &remove_all()
  {
    std::map<int, std::map<std::type_index, std::list<event_handler>>>().swap(handlers);

    return *this;
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
  void attach_handler(int event_id, bound_fcn<args...> func, bool once = false)
  {
    handlers[event_id][typeid(bound_fcn<args...>)].push_back({
      std::shared_ptr<bound_base>(new bound_fcn<args...>(func)), once
    });
  }

  template <typename... args>
  void detach_handler(int event_id, const bound_fcn<args...> &bound)
  {
    if (handlers.count(event_id) && handlers[event_id].count(typeid(bound_fcn<args...>)))
    {
      std::list<event_handler> &handler_list = handlers[event_id][typeid(bound_fcn<args...>)];
      for (auto handler = handler_list.begin(); handler != handler_list.end(); handler++)
        if (static_cast<bound_fcn<args...> &>(*handler->callback) == bound)
        {
          handler_list.erase(handler);
          break;
        }
    }
  }

  template <typename... args>
  void detach_handlers(int event_id, const bound_fcn<args...> &bound)
  {
    if (handlers.count(event_id) && handlers[event_id].count(typeid(bound_fcn<args...>)))
    {
      std::list<event_handler> &handler_list = handlers[event_id][typeid(bound_fcn<args...>)];
      handler_list.remove_if([&](event_handler handler) {
        return static_cast<bound_fcn<args...> &>(*handler.callback) == bound;
      });
    }
  }

  struct event_handler
  {
    std::shared_ptr<bound_base> callback;
    bool one_time;
  };

  std::map<int, std::map<std::type_index, std::list<event_handler>>> handlers;
};

} // namespace eepp

#endif // EEPP_BOUND_EMITTER_HPP
