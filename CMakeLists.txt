cmake_minimum_required(VERSION 3.5)

project(alp_research)

set(SOURCE_EXEC src/test.c)
set(SOURCE_LIB_CSV src/csv.c src/csv.h src/csv_table.c src/csv_table.h)
set(SOURCE_LIB_ALPM src/ALPM.c src/ALPM.h)
set(SOURCE_LIB_BINRW src/binrw.c src/binrw.h)

add_library(csv STATIC ${SOURCE_LIB_CSV})
add_library(ALPM STATIC ${SOURCE_LIB_ALPM})
add_library(binrw STATIC ${SOURCE_LIB_BINRW})
add_executable(main ${SOURCE_EXEC})
target_link_libraries(main csv ALPM binrw)
