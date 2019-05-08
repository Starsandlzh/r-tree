include_directories(../include)

set (DIR rtree)
set (SOURCES
        Exhaustive
        Generator
        RTreeBulkLoad
        RTreeLoad
        RTreeQuery)


foreach (test ${SOURCES})
    add_executable(test-${DIR}-${test} ${DIR}/${test}.cc)
    target_link_libraries(test-${DIR}-${test} ${SIDX_LIB_NAME})
endforeach()
