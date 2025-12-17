cmake_minimum_required(VERSION 3.10)
project(lab7)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.15.0
  TLS_VERIFY false
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


add_library(
  lab7_lib
  src/NPC/NPC.cpp
  src/NPC/squirrel.cpp
  src/NPC/elf.cpp
  src/NPC/bandit.cpp
  src/NPC/NPCFactory.cpp
  src/Observer/ConsoleObserver.cpp
  src/Observer/FileObserver.cpp
  src/Visitor/BattleVisitor.cpp
  src/Game/GameNPC.cpp
  src/Game/GameNPCFactory.cpp
  src/Game/MultithreadedBattleVisitor.cpp
  src/Game/GameEngine.cpp
)

add_executable(lab7_exe main.cpp)
target_link_libraries(lab7_exe PRIVATE lab7_lib)

enable_testing()

add_executable(tests tests/test.cpp)
target_link_libraries(tests lab7_lib gtest_main)


add_test(NAME MyProjectTests COMMAND tests)
