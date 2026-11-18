set(OKDCC_DIR ${CMAKE_CURRENT_LIST_DIR}/../../..)

file(GLOB_RECURSE okdcc_sources
  ${OKDCC_DIR}/logic/src/*.c
  ${OKDCC_DIR}/electric/src/*.c
  ${OKDCC_DIR}/ui/src/*.c)

idf_component_register(
  SRCS main.cc ${okdcc_sources}
  INCLUDE_DIRS
    .
    ${OKDCC_DIR}/src
    ${OKDCC_DIR}/logic/src
    ${OKDCC_DIR}/electric/src
    ${OKDCC_DIR}/ui/src)

if(OKDCC_DEBUG)
  target_compile_definitions(${COMPONENT_LIB} PRIVATE DCC_ASSERT)
endif()
