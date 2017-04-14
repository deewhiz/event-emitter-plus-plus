message("Building bound event emitter example")

set(PROJ_SRC
    ${INCLUDE_ROOT}/event-emitter++/bound_fcn.hpp
    ${INCLUDE_ROOT}/event-emitter++/emitter.hpp
    emitter_object.hpp
    main.cpp
)

add_executable("bound_emitter" ${PROJ_SRC})
