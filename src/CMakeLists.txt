include_directories(.)

file(GLOB core_src core/*.cpp)
file(GLOB wtui_src wtui/*.cpp)

add_executable(
    muda 
    main.cpp
    ${core_src}
    ${wtui_src})
target_link_libraries(
    muda
    wt
    wthttp
    ${Boost_LIBRARIES})
