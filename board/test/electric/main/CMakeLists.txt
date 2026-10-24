set(OKDCC_DIR ${CMAKE_CURRENT_LIST_DIR}/../../..)

file(GLOB_RECURSE okdcc_sources ${OKDCC_DIR}/electric/src/*.c)

idf_component_register(
  SRCS main.cc ${okdcc_sources}
  INCLUDE_DIRS
    .
    ${OKDCC_DIR}/src
    ${OKDCC_DIR}/electric/src)

if(OKDCC_DEBUG)
  target_compile_definitions(${COMPONENT_LIB} PRIVATE DCC_ASSERT)
endif()
