// /////////////////////////////////////////////
// RTOp_ROp_max_step.c
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

#include "RTOp_ROp_max_step.h"
#include "RTOp_obj_value_vtbl.h"
#include "RTOp_obj_free_free.h"
#include "RTOp_get_reduct_op.hpp"
#include "RTOp_reduct_min_value.h"

static int RTOp_ROp_max_step_reduct_obj_reinit(
  const struct RTOp_RTOp_vtbl_t* vtbl, const void* obj_data
  , RTOp_ReductTarget reduct_obj )
{
  *((RTOp_value_type*)reduct_obj) = RTOp_ROp_max_step_inf;
  return 0;
}

static int ROp_max_step_apply_op(
  const struct RTOp_RTOp_vtbl_t* vtbl, const void* obj_data
  , const int num_vecs, const struct RTOp_SubVector vecs[]
  , const int num_targ_vecs, const struct RTOp_MutableSubVector targ_vecs[]
  , RTOp_ReductTarget reduct_obj )
{
  // Declare locals
  const RTOp_value_type     beta     = *(RTOp_value_type*)obj_data;
  RTOp_value_type           *alpha   =  (RTOp_value_type*)reduct_obj;
  RTOp_index_type           sub_dim  = 0;
  const RTOp_value_type     *v0_val = NULL, *v1_val = NULL;
  ptrdiff_t                 v0_val_s = 0, v1_val_s = 0;
  register RTOp_index_type  k;
  RTOp_value_type           alpha_tmp;
  // Validate the input
  if( num_vecs != 2 )                       return RTOp_ERR_INVALID_NUM_VECS;
  if( num_targ_vecs != 0 )                  return RTOp_ERR_INVALID_NUM_TARG_VECS;
  if( vecs[0].sub_dim != vecs[1].sub_dim )  return RTOp_ERR_INCOMPATIBLE_VECS;
  // Get local variables to vector data
  sub_dim  = vecs[0].sub_dim;
  v0_val   = vecs[0].values;  v0_val_s = vecs[0].values_stride;
  v1_val   = vecs[1].values;  v1_val_s = vecs[1].values_stride;
  // Perform the reduction operation:
  //     max alpha s.t. v[0] + alpha * v[1] >= beta
  for( k = 0; k < sub_dim; ++k, v0_val += v0_val_s, v1_val += v1_val_s ) {
    alpha_tmp = (beta - (*v0_val))/(*v1_val);
    *alpha = ( (0 <= alpha_tmp && alpha_tmp < *alpha) ? alpha_tmp : *alpha );
  }
  return 0; // success!
}

/// Name of this reduction operator class
const char RTOp_ROp_max_step_name[] = "ROp_max_step";

/// Virtual function table
const struct RTOp_RTOp_vtbl_t RTOp_ROp_max_step_vtbl =
{
  &RTOp_obj_value_vtbl
  ,&RTOp_obj_value_vtbl
  ,RTOp_ROp_max_step_reduct_obj_reinit
  ,ROp_max_step_apply_op
  ,RTOp_reduct_min_value
  ,RTOp_get_reduct_min_value_op
};

// Class specific functions

int RTOp_ROp_max_step_construct( RTOp_value_type beta, struct RTOp_RTOp* op )
{
  op->vtbl = &RTOp_ROp_max_step_vtbl;
  op->vtbl->obj_data_vtbl->obj_create(NULL,NULL,&op->obj_data);
  *((RTOp_value_type*)op->obj_data) = beta;
  return 0; // success?
}

int RTOp_ROp_max_step_destroy( struct RTOp_RTOp* op )
{
  op->vtbl->obj_data_vtbl->obj_free(NULL,NULL,&op->obj_data);
  op->vtbl      = NULL;
  return 0; // success?
}

int RTOp_ROp_max_step_set_beta( RTOp_value_type beta, struct RTOp_RTOp* op )
{
  *((RTOp_value_type*)op->obj_data) = beta;
  return 0; // success?
}

RTOp_value_type  RTOp_ROp_max_step_inf = +1e+50;

RTOp_value_type  RTOp_ROp_max_step_val(RTOp_ReductTarget reduct_obj)
{
  return *(RTOp_value_type*)reduct_obj;
}
