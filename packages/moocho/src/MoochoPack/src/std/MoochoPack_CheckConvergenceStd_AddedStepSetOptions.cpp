// @HEADER
// ***********************************************************************
// 
// Moocho: Multi-functional Object-Oriented arCHitecture for Optimization
//                  Copyright (2003) Sandia Corporation
// 
// Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive
// license for use of this work by or on behalf of the U.S. Government.
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
// Questions? Contact Roscoe A. Bartlett (rabartl@sandia.gov) 
// 
// ***********************************************************************
// @HEADER

#include <assert.h>

#include "../std/MoochoPack_CheckConvergenceStd_AddedStepSetOptions.hpp"
#include "OptionsFromStreamPack_StringToBool.hpp"

// Define the options
namespace {

  const int local_num_options = 2;

  enum local_EOptions {
    SCALE_KKT_ERROR_BY,
    SCALE_OPT_ERROR_BY_GF
  };

  const char* local_SOptions[local_num_options]	= {
    "scale_kkt_error_by",
    "scale_opt_error_by_Gf",
  };

}

namespace MoochoPack {

CheckConvergenceStd_AddedStepSetOptions::CheckConvergenceStd_AddedStepSetOptions(
        CheckConvergenceStd_AddedStep* target
      , const char opt_grp_name[] )
  :	OptionsFromStreamPack::SetOptionsFromStreamNode(
        opt_grp_name, local_num_options, local_SOptions )
    , OptionsFromStreamPack::SetOptionsToTargetBase<
      CheckConvergenceStd_AddedStep >( target )
{}

void CheckConvergenceStd_AddedStepSetOptions::setOption(
  int option_num, const std::string& option_value )
{
  using OptionsFromStreamPack::StringToBool;

  typedef CheckConvergenceStd_AddedStep target_t;
  switch( (local_EOptions)option_num ) {
      case SCALE_KKT_ERROR_BY:
    {
      const std::string &option = option_value.c_str();
      if( option == "SCALE_BY_ONE" )
        target().scale_kkt_error_by( target_t::SCALE_BY_ONE );
      else if( option == "SCALE_BY_NORM_2_X" )
        target().scale_kkt_error_by( target_t::SCALE_BY_NORM_2_X );
      else if( option == "SCALE_BY_NORM_INF_X" )
        target().scale_kkt_error_by( target_t::SCALE_BY_NORM_INF_X );
      else
        throw std::invalid_argument( "Error, incorrect value for "
          "\"scale_kkt_error_by\".  Only the options "
          "SCALE_BY_ONE, SCALE_BY_NORM_2_X, and SCALE_BY_NORM_INF_X "
          "are available" );
      break;
    }
    case SCALE_OPT_ERROR_BY_GF: {
      target().scale_opt_error_by_Gf(
        StringToBool( "scale_opt_error_by_Gf", option_value.c_str() ) );
      break;
    }
    default:
      TEST_FOR_EXCEPT(true);	// Local error only?
  }
}

}	// end namespace MoochoPack 
