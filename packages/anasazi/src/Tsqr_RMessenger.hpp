#ifndef __TSQR_RMessenger_hpp
#define __TSQR_RMessenger_hpp

#include <algorithm>
#include <vector>
#include "Tsqr_MatView.hpp"
#include "Tsqr_MessengerBase.hpp"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

namespace TSQR {
  /// \class RMessenger
  /// \brief Send, receive, and broadcast square R factors
  ///
  /// Object that handles sending, receiving, and broadcasting square
  /// upper triangular matrices containing data of type Scalar.
  template< class Ordinal, class Scalar >
  class RMessenger {
  public:
    typedef Scalar scalar_type;
    typedef Ordinal ordinal_type;

    /// \brief Constructor
    RMessenger (MessengerBase< Scalar >* const messenger) :
      messenger_ (messenger) {}

    template< class ConstMatrixViewType >
    void
    send (const ConstMatrixViewType& R, const int destProc)
    {
      pack (R);
      messenger_->send (&buffer_[0], buffer_.size(), destProc, 0);
    }

    template< class MatrixViewType >
    void
    recv (MatrixViewType& R, const int srcProc)
    {
      const typename MatrixViewType::ordinal_type ncols = R.ncols();
      const Ordinal buflen = buffer_length (ncols);
      buffer_.resize (buflen);
      messenger_->recv (&buffer_[0], buflen, srcProc, 0);
      unpack (R);
    }

    template< class MatrixViewType >
    void
    broadcast (MatrixViewType& R, const int rootProc)
    {
      const int myRank = messenger_->rank();
      if (myRank == rootProc)
	pack (R);
      messenger_->broadcast (&buffer_[0], buffer_length (R.ncols()), rootProc);
      if (myRank != rootProc)
	unpack (R);
    }

  private:
    MessengerBase< Scalar >* const messenger_;
    std::vector< Scalar > buffer_;

    // Default construction and copying don't make sense, so they are
    // forbidden syntactically.
    RMessenger ();
    RMessenger (const RMessenger&);
    RMessenger& operator= (const RMessenger&);

    /// Buffer length as a function of R factor dimension
    /// 
    /// \param ncols [in] Number of columns (and number of rows)
    ///   in the R factor input.
    Ordinal buffer_length (const Ordinal ncols) const {
      return (ncols * (ncols + Ordinal(1))) / Ordinal(2);
    }

    template< class ConstMatrixViewType >
    void
    pack (const ConstMatrixViewType& R)
    {
      typedef typename ConstMatrixViewType::scalar_type view_scalar_type;
      typedef typename ConstMatrixViewType::ordinal_type view_ordinal_type;
      typedef typename std::vector< Scalar >::iterator iter_type;

      const view_ordinal_type ncols = R.ncols();
      const Ordinal buf_length = buffer_length (ncols);
      buffer_.resize (buf_length);
      iter_type iter = buffer_.begin();
      for (view_ordinal_type j = 0; j < ncols; ++j)
	{
	  const view_scalar_type* const R_j = &R(0,j);
	  std::copy (R_j, R_j + (j+1), iter);
	  iter += (j+1);
	}
    }

    template< class MatrixViewType >
    void
    unpack (MatrixViewType& R)
    {
      typedef typename MatrixViewType::ordinal_type view_ordinal_type;
      typedef typename std::vector< Scalar >::const_iterator const_iter_type;

      const view_ordinal_type ncols = R.ncols();
      const_iter_type iter = buffer_.begin();
      for (view_ordinal_type j = 0; j < ncols; ++j)
	{
	  std::copy (iter, iter + (j+1), &R(0,j));
	  iter += (j+1);
	}
    }
  };


  /// \brief Distribute a stack of R factors
  ///
  /// \param R_stack [in] nprocs*ncols by ncols stack of square upper
  ///   triangular matrices.  The whole stack is stored in
  ///   column-major order.
  ///
  /// \param R_local [out] ncols by ncols upper triangular matrix,
  ///   stored in column-major order (in unpacked form).
  ///
  /// \param messenger [in/out] Object that handles communication 
  ///
  template< class MatrixViewType, class ConstMatrixViewType >
  void
  scatterStack (const ConstMatrixViewType& R_stack, 
		MatrixViewType& R_local,
		MessengerBase< typename MatrixViewType::scalar_type >* const messenger)
  {
    typedef typename MatrixViewType::ordinal_type ordinal_type;
    typedef typename MatrixViewType::scalar_type scalar_type;
    typedef ConstMatView< ordinal_type, scalar_type > const_view_type;

    const int nprocs = messenger->size();
    const int my_rank = messenger->rank();

    if (my_rank == 0)
      {
	const ordinal_type ncols = R_stack.ncols();

	// Copy data from top ncols x ncols block of R_stack into R_local.
	const_view_type R_stack_view (ncols, ncols, R_stack.get(), R_stack.lda());
	R_local.copy (R_stack_view);

	// Loop through all other processors, sending each the next
	// ncols x ncols block of R_stack.
	RMessenger< ordinal_type, scalar_type > sender (messenger);
	for (int destProc = 1; destProc < nprocs; ++destProc)
	  {
	    const scalar_type* const R_ptr = R_stack.get() + destProc*ncols;
	    const_view_type R_stack_view (ncols, ncols, R_ptr, R_stack.lda());
	    sender.send (R_stack_view, destProc);
	  }
      }
    else
      {
	const int srcProc = 0;
	R_local.fill (scalar_type(0));
	RMessenger< ordinal_type, scalar_type > receiver (messenger);
	receiver.recv (R_local, srcProc);
      }
  }




  template< class MatrixViewType, class ConstMatrixViewType >
  void
  gatherStack (MatrixViewType& R_stack, 
	       ConstMatrixViewType& R_local,
	       MessengerBase< typename MatrixViewType::scalar_type >* const messenger)
  {
    typedef typename MatrixViewType::ordinal_type ordinal_type;
    typedef typename MatrixViewType::scalar_type scalar_type;
    typedef MatView< ordinal_type, scalar_type > matrix_view_type;

    const int nprocs = messenger->size();
    const int my_rank = messenger->rank();

    if (my_rank == 0)
      {
	const ordinal_type ncols = R_stack.ncols();

	// Copy data from R_local into top ncols x ncols block of R_stack.
	matrix_view_type R_stack_view (ncols, ncols, R_stack.get(), R_stack.lda());
	R_stack_view.copy (R_local);

	// Loop through all other processors, fetching their matrix data.
	RMessenger< ordinal_type, scalar_type > receiver (messenger);
	for (int srcProc = 1; srcProc < nprocs; ++srcProc)
	  {
	    const scalar_type* const R_ptr = R_stack.get() + srcProc*ncols;
	    matrix_view_type R_stack_view (ncols, ncols, R_ptr, R_stack.lda());
	    // Fill (the lower triangle) with zeros, since
	    // RMessenger::recv() only writes to the upper triangle.
	    R_stack_view.fill (scalar_type (0));
	    receiver.recv (R_stack_view, srcProc);
	  }
      }
    else
      {
	// We only read R_stack on Proc 0, not on this proc.
	// Send data from R_local to Proc 0.
	const int destProc = 0;
	RMessenger< ordinal_type, scalar_type > sender (messenger);
	sender.send (R_local, destProc);
      }
    messenger->barrier();
  }

} // namespace TSQR

#endif // __TSQR_RMessenger_hpp