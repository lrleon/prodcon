
 set(WARN " -Wall -Wextra -Wcast-align -Wno-sign-compare -Wno-write-strings -Wno-parentheses ")

 set(FLAGS " -Ofast -DNDEBUG -pthread")
 set(OPTIONS "${FLAGS} ${WARN}")
 set(CMAKE_CXX_FLAGS " -std=c++17 ${OPTIONS} ")

 add_executable(prodcon test.cc)
 add_executable(gen-input gen-input.cc)


