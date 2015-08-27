include_directories(.)

add_executable(
    muda 
    main.cpp)
target_link_libraries(
    muda
    wt
    wthttp
    ${Boost_LIBRARIES})
