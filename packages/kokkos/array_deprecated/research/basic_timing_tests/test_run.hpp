/*
//@HEADER
// ************************************************************************
// 
//          Kokkos: Node API and Parallel Node Kernels
//              Copyright (2008) Sandia Corporation
// 
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Michael A. Heroux (maherou@sandia.gov) 
// 
// ************************************************************************
//@HEADER
*/

#include <impl/Kokkos_Timer.hpp>

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

template< class Scalar, class DeviceType >
struct TrivialForFunctor;

//
// This is a Kokkos functor that does a trivial amount of work.  We
// use it to measure the overhead of launching and waiting for a
// kernel.
//
template<class Scalar>
struct TrivialForFunctor< Scalar, KOKKOS_MACRO_DEVICE > {
  typedef KOKKOS_MACRO_DEVICE     device_type ;
  typedef device_type::size_type  size_type ;
  typedef Kokkos::ValueView<Scalar, device_type > value_type ;

  value_type X , Y , Z;

  TrivialForFunctor (const value_type & argX , 
		     const value_type & argY , 
		     const value_type & argZ)
    : X (argX), Y (argY), Z (argZ) {}


  KOKKOS_MACRO_DEVICE_FUNCTION
  void operator()( size_type iwork ) const
  { *Z = *X + *Y; }
};


namespace {
  //
  // Report statistics for the given timing data.
  //
  // out: Output stream to which to write.
  //
  // label: Timing label (to print first).
  //
  // times: array of length numOuterLoops.  Each entry is the _total_
  //   time (not averaged yet) for numInnerLoops trials of whatever
  //   operation was timed.  We compute and display averages here.
  //   Side effect: This array is sorted on output, but the values are
  //   not changed.
  //
  // numInnerLoops: Each entry of the times array is the total time in
  //   seconds of numInnerLoops runs.
  //
  // numOuterLoops: Number of entries in the times array; the number
  //   of trials (where each trial measures the total time of
  //   numInnerLoops runs).
  void
  report (std::ostream& out, 
	  const std::string& label, 
	  std::vector<double>& times,
	  const int numInnerLoops,
	  const int numOuterLoops)
  {
    using std::endl;
    const bool verbose = false;
    //
    // Sort times to find min, median, and max.
    //
    std::sort (times.begin(), times.end());
    const double minTime = times.front();
    const double median  = times[numOuterLoops / 2];
    const double maxTime = times.back();
    //
    // Report statistics.
    //
    out << label << endl
	<< "Number of (outer) trials: " << numOuterLoops << endl
	<< "Number of (inner) runs per trial: " << numInnerLoops << endl;
    out << "Min: " << minTime / numInnerLoops << endl
	<< "Median: " << median / numInnerLoops << endl
	<< "Max: " << maxTime / numInnerLoops << endl;
    if (verbose) {
      out << "Times: [";
      for (int i = 0; i < numOuterLoops; ++i) {
	out << times[i] / numInnerLoops;
	if (i == numOuterLoops - 1) {
	  out << "]" << endl;
	}
	else {
	  out << ", ";
	}
      }
    }
    out << endl; // Leave a space for the next report() call.
  }

  // Report timer resolution results.
  void
  reportTimerResolution (std::ostream& out, 
			 std::vector<double>& times, 
			 const int numOuterLoops)
  {
    using std::endl;
    //
    // Sort times to find min, median, and max.
    //
    std::sort (times.begin(), times.end());
    const double minTime = times.front();
    const double median  = times[numOuterLoops / 2];
    const double maxTime = times.back();
    //
    // Report statistics.
    //
    out << "Timer resolution (seconds):" << endl
	<< "Number of (outer) trials: " << numOuterLoops << endl;
    out << "Min: " << minTime << endl
	<< "Median: " << median << endl
	<< "Max: " << maxTime << endl;
    out << endl; // Leave a space for the next report() call.
  }

} // namespace (anonymous)



// For each test, we time numInnerLoops trials.  We then repeat that
// timing loop numOuterLoops times, recording the time for each outer
// loop.  We compute and print the min, median, and max of each outer
// loop iteration.
template<class Scalar, class Device>
void 
test_run (const std::string& testName, 
	  const int numInnerLoops, 
	  const int numOuterLoops)
{
  using std::cerr;
  using std::cout;
  using std::endl;

  if (numInnerLoops <= 0 || numOuterLoops <= 0) {
    cerr << "Invalid number of inner or outer loops" << endl;
    return;
  }
  cout << testName << endl << endl;

  std::vector<double> timerResTimes (numOuterLoops);
  std::vector<double> kernelLaunchTimes (numOuterLoops);
  std::vector<double> copyTimes (numOuterLoops);

  typedef Kokkos::ValueView<Scalar, Device> value_type;
  value_type x = Kokkos::create_value<Scalar, Device>();
  value_type y = Kokkos::create_value<Scalar, Device>();
  value_type z = Kokkos::create_value<Scalar, Device>();

  // Fill x and y with some values that are not too large or small (we
  // don't want overflow or underflow; those might affect timings of
  // floating-point operations on some hardware).
  Kokkos::deep_copy(x, Scalar(1));
  Kokkos::deep_copy(y, Scalar(2));
  Kokkos::deep_copy(z, Scalar(0));
  // Make sure the transfers are done before collecting kernel timings.
  Device::wait_functor_completion();

  // Carter helpfully provides a gettimeofday() wrapper.
  Kokkos::Impl::Timer wall_clock;

  //
  // Measure timer resolution.  We do this over outerLoop trials.
  // Each trial loops until the timer has changed.  There's an upper
  // bound on the inner loop count, just in case the timer is broken.
  //
  for (int outerLoop = 0; outerLoop < numOuterLoops; ++outerLoop) {
    const double startTime = wall_clock.seconds();
    double endTime;
    // 10^9 iterations should take no more than a few seconds on
    // modern computers, unless the timer overhead is huge.
    const size_t maxTimerResolutionIters = 1000000000;

    // Keep k after the loop to test whether max iteration count was
    // reached.
    size_t k = 0; 
    for (k = 0; k < maxTimerResolutionIters; ++k) {
      endTime = wall_clock.seconds();
      if (endTime > startTime) {
	break;
      }
    }
    if (k == maxTimerResolutionIters) {
      cerr << "Failed to measure timer resolution; loop exceeded "
	   << maxTimerResolutionIters << " iterations with no change "
	   << "in timer value." << endl;
      return;
    } else if (endTime <= startTime) {
      cerr << "Failed to measure timer resolution; endTime = " << endTime
	   << " <= startTime = " << startTime << "." << endl;
      return;
    }
    timerResTimes[outerLoop] = endTime - startTime;
  }
  // Report timer resolution results.
  reportTimerResolution (cout, timerResTimes, numOuterLoops);

  //
  // Measure kernel launch + wait time.
  //
  for (int outerLoop = 0; outerLoop < numOuterLoops; ++outerLoop) {
    // kernelLaunchTime will be the total time of numInnerLoops kernel
    // launches + waits.
    const double startTime = wall_clock.seconds();
    for (int i = 0; i < numInnerLoops; ++i) {
      Kokkos::parallel_for (1, TrivialForFunctor<Scalar, Device>(x, y, z));
      Device::wait_functor_completion();
    }
    kernelLaunchTimes[outerLoop] = wall_clock.seconds() - startTime;
  }
  // Report kernel launch times.
  report (cout, "Kernel launch + wait times (seconds)", kernelLaunchTimes, 
	  numInnerLoops, numOuterLoops);

  //
  // Measure _just_ kernel launch time.  Only wait at the end, after
  // numInnerLoops trials.
  //
  for (int outerLoop = 0; outerLoop < numOuterLoops; ++outerLoop) {
    // kernelLaunchTime will be the total time of numInnerLoops kernel
    // launches, plus one wait at the end.
    const double startTime = wall_clock.seconds();
    for (int i = 0; i < numInnerLoops; ++i) {
      Kokkos::parallel_for (1, TrivialForFunctor<Scalar, Device>(x, y, z));
    }
    Device::wait_functor_completion();
    kernelLaunchTimes[outerLoop] = wall_clock.seconds() - startTime;
  }
  // Report kernel launch times.
  report (cout, "Kernel launch (no wait) times (seconds)", kernelLaunchTimes, 
	  numInnerLoops, numOuterLoops);

  //
  // Measure time of copying a scalar to the device and back.  We do a
  // wait after numInnerLoops runs to make sure that the copies
  // completed.
  //
  for (int outerLoop = 0; outerLoop < numOuterLoops; ++outerLoop) {
    // copyTime will be the total time of of numInnerLoops two-way
    // copies.
    const double startTime = wall_clock.seconds();
    Scalar source (1), target (1);
    for (int i = 0; i < numInnerLoops; ++i) {
      Kokkos::deep_copy (x, source);
      Kokkos::deep_copy (target, x);
    }
    Device::wait_functor_completion();
    copyTimes[outerLoop] = wall_clock.seconds() - startTime;
  }
  // Report copy times.
  report (cout, "Two-way (host to device and back) copy times (seconds)", 
	  copyTimes, numInnerLoops, numOuterLoops);

  //
  // Measure time of copying a scalar to the device repeatedly, with
  // no copies back to host.  We do a wait after numInnerLoops runs to
  // make sure that the copy completed.
  //
  for (int outerLoop = 0; outerLoop < numOuterLoops; ++outerLoop) {
    // copyTime will be the total time of of numInnerLoops one-way
    // copies.
    const double startTime = wall_clock.seconds();
    Scalar target (1);
    for (int i = 0; i < numInnerLoops; ++i) {
      Kokkos::deep_copy (target, x);
    }
    Device::wait_functor_completion();
    copyTimes[outerLoop] = wall_clock.seconds() - startTime;
  }
  // Report copy times.
  report (cout, "One-way (host to device) copy times (seconds)", 
	  copyTimes, numInnerLoops, numOuterLoops);
}


