//@HEADER
// ************************************************************************
// 
//               Tpetra: Linear Algebra Services Package 
//                 Copyright 2011 Sandia Corporation
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

#ifndef __MatrixMarket_Banner_hpp
#define __MatrixMarket_Banner_hpp

#include <complex>
#include <ostream>
#include <string>

namespace Tpetra {
  namespace MatrixMarket {

    /// \class Banner
    /// \author Mark Hoemmen
    /// \brief Parse a Matrix Market banner line
    ///
    /// The first line of a Matrix Market - format file, the "banner
    /// line," contains information for interpreting the rest of the
    /// file.  This class parses the first line, canonicalizes the
    /// information therein, and stores it for later use.
    class Banner {
    public:
      /// Constructor
      ///
      /// \param line [in] The banner line to parse
      /// \param tolerant [in] Whether to parse tolerantly.
      ///   If false, we adhere strictly to the Matrix Market
      ///   standard.  If true, we allow all kinds of divergence
      ///   therefrom (including an entirely empty banner line,
      ///   for which we fill in some sensible defaults).
      Banner (const std::string& line, const bool tolerant=false);

      //! The object type (currently just "matrix")
      const std::string& objectType() const { return objectType_; }

      /// \brief Storage type of the matrix
      ///
      /// "coordinate" means a sparse matrix, stored as (i,j,Aij)
      /// triples with one-based indices i and j.  "array" means a dense
      /// matrix, stored in column-major order with one entry per line.
      const std::string& matrixType() const { return matrixType_; }

      /// \brief Data type of matrix entries
      ///
      /// Data type of the matrix entries: "real", "complex", "integer",
      /// or "pattern".
      const std::string& dataType() const { return dataType_; }

      /// \brief Symmetric storage type
      ///
      /// Describes whether and how symmetry is exploited when storing
      /// the matrix.  "general" means nonsymmetric: all the matrix
      /// entries are stored.  "symmetric", "skew-symmetric", and
      /// "hermitian" have their usual mathematical meaning, and also
      /// mean that only the upper or lower triangle (including the
      /// diagonal) of the matrix is stored.  There's no way to tell
      /// from the banner whether the upper or lower triangle is stored;
      /// you have to examine the matrix entries.  
      ///
      /// \note An error condition is possible for sparse matrices
      /// (matrixType() == "coordinate"): If only the upper or lower
      /// triangle is supposed to be there, and entries from both the
      /// upper and lower triangle are stored, the resulting matrix is
      /// undefined.  Users may choose to interpret this case as they
      /// wish (for example, exclusively, or additively).
      const std::string& symmType() const { return symmType_; }

    private:
      static std::string validateObjectType (const std::string& objectType, const bool tolerant=false);
      static std::string validateMatrixType (const std::string& matrixType, const bool tolerant=false);
      static std::string validateDataType (const std::string& dataType, const bool tolerant=false);
      static std::string validateSymmType (const std::string& symmType, const bool tolerant=false);

      //! Set the last \c howMany member data to default values
      void setDefaults (const int howMany);

      std::string objectType_, matrixType_, dataType_, symmType_;
    };

    /// Print out the "banner" of the Matrix Market file
    ///
    /// This function may be used to generate Matrix Market output
    /// files, since it prints the banner (first line of the Matrix
    /// Market file) in the canonical format.  However, we don't promise
    /// that it prints out in exactly the same format in which it was
    /// read in, especially (but not only) if it was read in "tolerant"
    /// mode.
    ///
    /// \note We don't print the endline at the end of the banner line.
    ///
    /// \param out [out] The output stream to which to print
    /// \param banner [in] The Matrix Market banner to print
    ///
    /// \return The output stream \c out
    std::ostream& operator<< (std::ostream& out, const Banner& banner);

  } // namespace MatrixMarket
} // namespace Tpetra

#endif // __MatrixMarket_Banner_hpp
