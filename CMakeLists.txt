PROJECT( main )
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
add_compile_options(-std=c++11)
set(CMAKE_PREFIX_PATH "/usr/local/opencv-3.3.1/build")
FIND_PACKAGE( OpenCV REQUIRED )


AUX_SOURCE_DIRECTORY(. DIR_SRCS)
ADD_EXECUTABLE(main ${DIR_SRCS}  )
target_link_libraries( main ${OpenCV_LIBS} )








