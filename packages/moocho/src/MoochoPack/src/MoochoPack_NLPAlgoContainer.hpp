// ////////////////////////////////////////////////////////////////////////////
// rSQPAlgoContainer.h
//
// Copyright (C) 2001 Roscoe Ainsworth Bartlett
//
// This is free software; you can redistribute it and/or modify it
// under the terms of the "Artistic License" (see the web site
//   http://www.opensource.org/licenses/artistic-license.html).
// This license is spelled out in the file COPYING.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// above mentioned "Artistic License" for more details.

#ifndef RSQP_ALGO_CONTAINER_H
#define RSQP_ALGO_CONTAINER_H

#include "rSQPAlgoClientInterface.h"
#include "rSQPAlgoInterface.h"
#include "rSQPAlgo_Config.h"
#include "StandardCompositionMacros.h"

namespace ReducedSpaceSQPPack {

///
/** Implementation for rSQPAlgo solver.
 *
 * Acts as a container for rSQPAlgo.  This class is hidden from clients
 * by not exposing it to them in header files.
 */
class rSQPAlgoContainer : public rSQPAlgoClientInterface {
public:

	/** @name Constructors / initializers */
	//@{

	/// Members for <<std comp>> of the algorithm object algo.
	STANDARD_COMPOSITION_MEMBERS( rSQPAlgoInterface, algo )

	/// Construct a container with no configuration object set.
	rSQPAlgoContainer()
	{}

	//@}

	/** @name Overridden from rSQPAlgoClientInterface */
	//@{

	///
	void set_config(const config_ptr_t& config);
	///
	config_ptr_t& get_config();
	///
	const config_ptr_t& get_config() const;
	///
	rSQPAlgo_Config& config();
	///
	const rSQPAlgo_Config& config() const;

	//@}

	/** @name Overridden from rSQPSolverClientInterface */
	//@{

	///
	EFindMinReturn find_min();
	///
	void configure_algorithm(std::ostream* trase_out);
	///
	void print_algorithm(std::ostream& out) const;
	///
	void set_algo_timing( bool algo_timing );
	///
	bool algo_timing() const;
	///
	void print_algorithm_times( std::ostream& out ) const;

	//@}

private:

	config_ptr_t			config_;

#ifdef DOXYGEN_COMPILE
	///
	rSQPAlgoInterface       *algo_interface;
#endif

	// Assert that the object has been set up properly and throw exception if it has not
	void assert_valid_setup() const;

	// Not defined and not to be called
	rSQPAlgoContainer(const rSQPAlgoContainer&);
	rSQPAlgoContainer& operator=(const rSQPAlgoContainer&);

};	// end class rSQPAlgoContainer

}	// end namespace ReducedSpaceSQPPack

#endif	// RSQP_ALGO_CONTAINER_H
