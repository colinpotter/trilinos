
INCLUDE(TribitsAddExecutableAndTest)

TRIBITS_ADD_EXECUTABLE_AND_TEST(
  Amesos_ex1
  SOURCES ex1.cpp
  ARGS
  COMM serial mpi
  NUM_MPI_PROCS 1-4
  )
