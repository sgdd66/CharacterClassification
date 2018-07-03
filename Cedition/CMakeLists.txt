PROJECT( main )
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

if(CMAKE_COMPILER_IS_GNUCXX)
    #add_compile_options(-std=c++11)
    set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
    message(STATUS "optional:-std=c++11")   
endif(CMAKE_COMPILER_IS_GNUCXX)

AUX_SOURCE_DIRECTORY(. DIR_SRCS)
ADD_EXECUTABLE(mainC ${DIR_SRCS}  )

TARGET_LINK_LIBRARIES(mainC -lm)







