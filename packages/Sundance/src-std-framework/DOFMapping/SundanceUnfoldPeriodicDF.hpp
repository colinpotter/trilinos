/* @HEADER@ */
// ************************************************************************
// 
//                              Sundance
//                 Copyright (2005) Sandia Corporation
// 
// Copyright (year first published) Sandia Corporation.  Under the terms 
// of Contract DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government 
// retains certain rights in this software.
// 
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; either version 2.1 of the
// License, or (at your option) any later version.
//  
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//                                                                                 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA                                                                                
// Questions? Contact Kevin Long (krlong@sandia.gov), 
// Sandia National Laboratories, Livermore, California, USA
// 
// ************************************************************************
/* @HEADER@ */

#ifndef SUNDANCE_UNFOLD_PERIODIC_DF_H
#define SUNDANCE_UNFOLD_PERIODIC_DF_H

#include "SundanceDefs.hpp"
#include "SundanceDiscreteFunction.hpp"
#include "SundanceExpr.hpp"
#include "PlayaVectorType.hpp"


namespace Sundance
{

/** 
 * "Unfold" a periodic mesh to produce a 1D mesh without duplicated vertices
 */
Mesh unfoldPeriodicMesh(const Mesh& mesh);

/**
 * "Unfold" a periodic discrete space, producing a non-periodic discretization
 * of the same interval
 */
DiscreteSpace unfoldPeriodicDiscreteSpace(const DiscreteSpace& space);

/**
 * "Unfold" a discrete function defined on a periodic mesh, producing
 * a function usable on a non-periodic mesh of the same interval   
*/
Expr unfoldPeriodicDiscreteFunction(const Expr& f, const string& name="");


}


#endif
