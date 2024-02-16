cmake_minimum_required(VERSION 3.5)

project(alp_research)

set(SOURCE_EXEC src/test.c)
set(SOURCE_LIB_CSV src/csv.c src/csv.h)

add_library(csv STATIC ${SOURCE_LIB_CSV})

add_executable(main ${SOURCE_EXEC})
target_link_libraries(main csv m)
