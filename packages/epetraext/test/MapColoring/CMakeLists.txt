
INCLUDE(TribitsAddExecutableAndTest)

TRIBITS_ADD_EXECUTABLE_AND_TEST(
  EpetraExt_MapColoring_test
  SOURCES cxx_main.cpp
  ARGS -v
  COMM serial
  )
