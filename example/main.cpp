#include "emitter_object.hpp"

#include <iostream>

void on_two(double *a, int b)
{
  std::cout << "-------- on_two -------" << std::endl <<
               "a: " << *a << " b: " << b << std::endl <<
               "-----------------------" << std::endl << std::endl;
}

void on_none()
{
  std::cout << "------- on_none -------" << std::endl <<
               "-----------------------" << std::endl << std::endl;
}

static int id = 3113;
class test
{
public:
  test():
    test_id(id++)
  {}

  void on_one(double *d) {
    std::cout << "---- test::on_one -----" << std::endl <<
                 "object id: " << test_id << " *d: " << *d << std::endl <<
                 "-----------------------" << std::endl << std::endl;
  }

  static void on_static(int i) {
    std::cout << "--- test::on_static ---" << std::endl <<
                 "i: " << i << std::endl <<
                 "-----------------------" << std::endl << std::endl;
  }

  const int test_id;
};

void run_emitter()
{
  emitter_object e;
  test t;

  // bind functions to add as listeners
  eepp::bound_fcn<double *, int> on_two1(&on_two, std::placeholders::_1, std::placeholders::_2);
  eepp::bound_fcn<double *> on_two2(&on_two, std::placeholders::_1, 8421);
  eepp::bound_fcn<int> on_obj_static(&test::on_static, std::placeholders::_1);
  eepp::bound_fcn<double *> on_obj_mem(&test::on_one, &t, std::placeholders::_1);

  // attach bound functions as listeners
  e.on(generic_event::nothing, &on_none);
  e.on(generic_event::two_things, on_two1);
  e.on(generic_event::two_things, on_two2);
  e.on(generic_event::one_thing, on_obj_static);


  // !WARNING! `t` that was bound in on_obj_mem MUST outlive `e` unless the handler is removed! //
  e.on(generic_event::one_thing, on_obj_mem);
  e.on(generic_event::one_thing, on_obj_mem);
  e.remove_handler(generic_event::one_thing, on_obj_mem);

  double d = 100;

  std::cout << "emit \"nothing\": " << std::endl;
  e.emit_event(generic_event::nothing);

  std::cout << "emit \"one_thing\": " << std::endl;
  e.emit_event(generic_event::one_thing, 71);
  e.emit_event(generic_event::one_thing, &d);

  std::cout << "emit \"two_things (bound b: 8421)\": " << std::endl;
  e.emit_event(generic_event::two_things, &d);

  std::cout << "emit \"two_things\" (should do nothing)" << std::endl;
  e.emit_event(generic_event::two_things, 1, 2);

}

int main(int argc, char *argv[])
{
  (void) argc;
  (void) argv;

  run_emitter();

  return 0;
}
