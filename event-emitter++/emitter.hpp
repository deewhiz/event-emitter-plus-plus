#ifndef EEPP_BOUND_EMITTER_HPP
#define EEPP_BOUND_EMITTER_HPP

#include "bound_fcn.hpp"

#include <functional>
#include <map>
#include <list>
#include <typeindex>

namespace eepp
{

/*! \brief emitter is an object based event emitter
 *
 */

class emitter
{
public:
  emitter():
    handlers(new handler_map())
  {}

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
    attach_handler(event_id, bound_fcn<>(fcn));

    return *this;
  }

  /*! \brief Attach an event listener
   *
   * \param event_id The event to listen for
   * \param listener The bound_fnc to be used as a callback
   *
   * \return This emitter to chain method calls
   */
  template <typename... args>
  emitter &on(int event_id, const bound_fcn<args...> &listener)
  {
    attach_handler(event_id, listener);

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
   * \param listener The bound_fnc to be used as a callback
   *
   * \return This emitter to chain method calls
   */
  template <typename... args>
  emitter &once(int event_id, const bound_fcn<args...> &listener)
  {
    attach_handler(event_id, listener, true);

    return *this;
  }

  /*! \brief Remove one event listener
   *
   * \param event_id The event to remove a listener for
   * \param listener The listener function to be removed
   *
   */
  template <typename... args>
  bool remove_handler(int event_id, const bound_fcn<args...> &listener)
  {
    return detach_handler(event_id, listener);
  }

  /*! \brief Remove all event listener for an event
   *
   * \param event_id The event to remove a listener for
   *
   */
  void remove_handlers(int event_id)
  {
    if ((*handlers).count(event_id))
      (*handlers)[event_id].clear();
  }

  /*! \brief Remove all event listeners
   *
   */
  void remove_handlers()
  {
    (*handlers).clear();
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
    if ((*handlers).count(event_id) && (*handlers)[event_id].count(typeid(bound_fcn<args...>)))
    {
      std::list<event_handler> &handler_list = (*handlers)[event_id][typeid(bound_fcn<args...>)];

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
  void attach_handler(int event_id, const bound_fcn<args...> &func, bool once = false)
  {
    (*handlers)[event_id][typeid(bound_fcn<args...>)].push_back({
      std::shared_ptr<bound_base>(new bound_fcn<args...>(func)), once
    });
  }

  template <typename... args>
  bool detach_handler(int event_id, const bound_fcn<args...> &listener)
  {
    if (!(*handlers).count(event_id) || !(*handlers)[event_id].count(typeid(listener))) return false;

    std::list<event_handler> &handler_list = (*handlers)[event_id][typeid(listener)];
    std::list<event_handler>::iterator handler;

    for (handler = handler_list.begin(); handler != handler_list.end(); handler++)
      if (*static_cast<const bound_fcn<args...> *>(&*handler->callback) == listener)
      {
        handler_list.erase(handler);
        return true;
      }

    return false;
  }

  struct event_handler
  {
    std::shared_ptr<bound_base> callback;
    bool one_time;
  };


  typedef std::map<int, std::map<std::type_index, std::list<event_handler>>> handler_map;
  std::shared_ptr<handler_map> handlers;
};

} // namespace eepp

#endif // EEPP_BOUND_EMITTER_HPP
