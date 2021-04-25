project(B)

add_library(ClassLib SHARED lib.cpp)

INSTALL(
    TARGETS ClassLib
    RUNTIME DESTINATION exe/bin
    LIBRARY DESTINATION exe/lib
)
