/*****************************************************************************
 * Zoltan Library for Parallel Applications                                  *
 * Copyright (c) 2000,2001,2002, Sandia National Laboratories.               *
 * For more info, see the README file in the top-level Zoltan directory.     *
 *****************************************************************************/
/*****************************************************************************
 * CVS File Information :
 *    $RCSfile$
 *    $Author$
 *    $Date$
 *    $Revision$
 ****************************************************************************/

#ifdef __cplusplus
/* if C++, define the rest of this header file as extern C */
extern "C" {
#endif

#include "hypergraph.h"



static ZOLTAN_HG_PACKING_FN packing_mxp;  /* maximal packing */
static ZOLTAN_HG_PACKING_FN packing_rep;  /* random edge packing */
static ZOLTAN_HG_PACKING_FN packing_rrp;  /* random, random edge packing */
static ZOLTAN_HG_PACKING_FN packing_rhp;  /* random, heavy edge packing */
static ZOLTAN_HG_PACKING_FN packing_grp;  /* greedy packing */
static ZOLTAN_HG_PACKING_FN packing_lhp;  /* locally heaviest packing */
static ZOLTAN_HG_PACKING_FN packing_pgp;  /* path growing packing */
static ZOLTAN_HG_PACKING_FN packing_aug2; /* augmenting path; length 2 */

/****************************************************************************/



int Zoltan_HG_Set_Packing_Fn(HGPartParams *hgp)
{
int found = 1;
  if      (!strcasecmp(hgp->redm_str, "mxp"))  hgp->packing = packing_mxp;
  else if (!strcasecmp(hgp->redm_str, "rep"))  hgp->packing = packing_rep;
  else if (!strcasecmp(hgp->redm_str, "rrp"))  hgp->packing = packing_rrp;
  else if (!strcasecmp(hgp->redm_str, "rhp"))  hgp->packing = packing_rhp;
  else if (!strcasecmp(hgp->redm_str, "grp"))  hgp->packing = packing_grp;
  else if (!strcasecmp(hgp->redm_str, "lhp"))  hgp->packing = packing_lhp;
  else if (!strcasecmp(hgp->redm_str, "pgp"))  hgp->packing = packing_pgp;
  else if (!strcasecmp(hgp->redm_str, "no"))   hgp->packing = NULL;
  else                            { found = 0; hgp->packing = NULL; }

  if (hgp->packing) {
    /* If reduction method is a packing, set the improvement and
       edge weight scaling functions accordingly. */

    /* Note: optimizer _aug1 is identical to packing_mxp -> aug1 was eliminated */
    if      (!strcasecmp(hgp->redmo_str,"aug1"))hgp->packing_opt = packing_mxp;
    else if (!strcasecmp(hgp->redmo_str,"aug2"))hgp->packing_opt = packing_aug2;
    else                                        hgp->packing_opt = NULL;
  }
  return found;
}

/****************************************************************************/



int Zoltan_HG_Packing (ZZ *zz, HGraph *hg, Packing pack, HGPartParams *hgp,
 int *limit)
{
int   err = ZOLTAN_OK;
float *old_ewgt = NULL, *new_ewgt = NULL;
char  *yo = "Zoltan_HG_Packing";

  ZOLTAN_TRACE_ENTER(zz, yo);

  /* Scale the weight of the edges */
  if (hg->vwgt && hgp->ews) {
     if (!(new_ewgt = (float*) ZOLTAN_MALLOC (hg->nEdge * sizeof(float)))) {
        ZOLTAN_PRINT_ERROR(zz->Proc, yo, "Insufficient memory.");
        ZOLTAN_TRACE_EXIT(zz, yo);
        return ZOLTAN_MEMERR;
        }
     Zoltan_HG_Scale_HGraph_Weight (zz, hg, new_ewgt, hgp->ews);
     old_ewgt = hg->ewgt;
     hg->ewgt = new_ewgt;
     }

  /* Do the packing */
  if (hgp->packing) {
     err = hgp->packing(zz, hg, pack, limit);
     if (err != ZOLTAN_OK && err != ZOLTAN_WARN)
        goto End;
     }

  /* Optimization */
  if (hgp->packing_opt != NULL)
     err = hgp->packing_opt (zz, hg, pack, limit);

End:
  /* Restore the old edge weights */
  if (hg->vwgt && hgp->ews)
     hg->ewgt = old_ewgt;

  ZOLTAN_FREE ((void**) &new_ewgt);
  ZOLTAN_TRACE_EXIT(zz, yo);
  return err;
}

/****************************************************************************/



/* Maximal packing. Visits all edges and packs edge if available. Time O(|I|) */
static int packing_mxp (ZZ *zz, HGraph *hg, Packing pack, int *limit)
{
  int i, j;

  for (i = 0; i < hg->nEdge && (*limit) > 0; i++) {
     for (j = hg->hindex[i]; j < hg->hindex[i+1]; j++)
        if (pack[hg->hvertex[j]] != hg->hvertex[j])
           break;
     if (j == hg->hindex[i+1]) {   /* if true, all vertices free for packing */
        for (j = hg->hindex[i]; j < (hg->hindex[i+1]-1) && (*limit) > 0; j++) {
           pack[hg->hvertex[j]] = hg->hvertex[j+1];
           (*limit)--;
           }
        pack[hg->hvertex[j]] = hg->hvertex[hg->hindex[i]];
        }
     }
  return ZOLTAN_OK;
}

/****************************************************************************/



/* Random Edge packing. Randomly traverses through all edges and packs if it
 * is available. Time O(|I|). */
static int packing_rep (ZZ *zz, HGraph *hg, Packing pack, int *limit)
{
int i, j, *edges=NULL, edge, random;
char *yo = "packing_rep";

  if (!(edges = (int*) ZOLTAN_MALLOC (hg->nEdge * sizeof(int)))) {
     ZOLTAN_PRINT_ERROR(zz->Proc, yo, "Insufficient memory.");
     return ZOLTAN_MEMERR;
     }
  for (i = 0; i < hg->nEdge; i++)
     edges[i] = i;

  for (i = hg->nEdge; i > 0 && (*limit) > 0; i--) {
     edge = edges[random=Zoltan_HG_Rand() % i];
     edges[random] = edges[i-1];

     for (j = hg->hindex[edge]; j < hg->hindex[edge+1]; j++)
        if (pack[hg->hvertex[j]] != hg->hvertex[j])
           break;
     if (j == hg->hindex[edge+1])  { /* if true, all vertices free for packing */
        for (j = hg->hindex[edge]; j < (hg->hindex[edge+1] -1)
         && (*limit) > 0; j++) {
            pack[hg->hvertex[j]] = hg->hvertex[j+1];
            (*limit)--;
            }
        pack[hg->hvertex[j]] = hg->hvertex[hg->hindex[edge]];
        }
     }
  ZOLTAN_FREE ((void**) &edges);
  return ZOLTAN_OK;
}

/****************************************************************************/



/* Random Random Packing. Randomly traverses through the vertices and for each
 * vertex it randomly takes one covering edge and packs it if it is available.
 * Time O(|I|). */
static int packing_rrp (ZZ *zz, HGraph *hg, Packing pack, int *limit)
{
int i, j, k, edge, random, *vertices = NULL, vertex, *del_edges = NULL, count;
char *yo = "packing_rrp";

  if (!(vertices  = (int*) ZOLTAN_MALLOC (hg->nVtx * sizeof(int)))
   || !(del_edges = (int*) ZOLTAN_CALLOC (hg->nEdge, sizeof(int))) ) {
      Zoltan_Multifree (__FILE__, __LINE__, 2, &vertices, &del_edges);
      ZOLTAN_PRINT_ERROR(zz->Proc, yo, "Insufficient memory.");
      return ZOLTAN_MEMERR;
      }
  for (i = 0; i < hg->nVtx;  i++)
     vertices[i] = i;

  for (i = hg->nVtx; i > 0 && (*limit) > 0; i--) {
     vertex = vertices[random = Zoltan_HG_Rand() % i];
     vertices[random] = vertices[i-1];
     if (pack[vertex] != vertex)
        continue;          /* vertex already packed, move on */

     count = 0;            /* count will be number of viable edges */
     for (k = hg->vindex[vertex]; k < hg->vindex[vertex+1]; k++) {
        edge = hg->vedge[k];
        if (del_edges[edge] == 1)
           continue;       /* edge has been deleted for use already */

        for (j = hg->hindex[edge]; j < hg->hindex[edge+1]; j++)
           if (pack[hg->hvertex[j]] != hg->hvertex[j])
             break;
        if (j == hg->hindex[edge+1])
           count++;                 /* found a possible edge */
        else
           del_edges[edge] = 1;     /* edge unusable, delete it from consideration */
        }
     if (count == 0)
        continue;                  /* vertex has no free edges available */

     random = Zoltan_HG_Rand() % count;      /* randomly select from available edges */
     for (k = hg->vindex[vertex]; k < hg->vindex[vertex+1]; k++) {
        edge = hg->vedge[k];
        if (del_edges[edge] == 0  &&  --count == random) {
           for (j = hg->hindex[edge]; j < (hg->hindex[edge+1] -1)
            && (*limit) > 0; j++) {
                pack[hg->hvertex[j]] = hg->hvertex[j+1];
                (*limit)--;
                }
           pack[hg->hvertex[j]] = hg->hvertex[hg->hindex[edge]];
           }
        del_edges[edge] = 1;
     }
  }
  Zoltan_Multifree (__FILE__, __LINE__, 2, &vertices, &del_edges);
  return ZOLTAN_OK;
}

/****************************************************************************/



/* Random Heavy Packing. Randomly traverses through the vertices
   and for each vertex it randomly takes the heaviest covering edge and packs
   it if it is available. Time O(|I|). */
static int packing_rhp (ZZ *zz, HGraph *hg, Packing pack, int *limit)
{
int i, j, k, *vertices = NULL, *del_edges = NULL, vertex, edge, size;
int number, best_edge, best_size, best_neighbors, random;
float best_ewgt;
char  *yo = "packing_rhp";

   if (!(vertices  = (int*) ZOLTAN_MALLOC (hg->nVtx * sizeof(int)))
    || !(del_edges = (int*) ZOLTAN_CALLOC (hg->nEdge, sizeof(int))) ) {
      Zoltan_Multifree (__FILE__, __LINE__, 2, &vertices, &del_edges);
      ZOLTAN_PRINT_ERROR(zz->Proc, yo, "Insufficient memory.");
      return ZOLTAN_MEMERR;
      }
   for (i = 0; i < hg->nVtx; i++)
      vertices[i] = i;

   for (i = hg->nVtx; i > 0 && (*limit) > 0; i--) {
      vertex = vertices[number = Zoltan_HG_Rand() % i];
      vertices[number] = vertices[i-1];
      if (pack[vertex] != vertex)
         continue;            /* vertex is already matched, move on */

      best_neighbors = 0;
      best_edge = best_size = -1;
      best_ewgt = -1.0;
      for (j = hg->vindex[vertex]; j < hg->vindex[vertex+1]; j++) {
         edge = hg->vedge[j];
         size = hg->hindex[edge+1] - hg->hindex[edge];
         if (del_edges[edge] == 0 && ((!(hg->ewgt) && size == best_size)
          || (hg->ewgt && hg->ewgt[edge] == best_ewgt && size == best_size)))
              best_neighbors++;
         else if (del_edges[edge]==0 && ((!(hg->ewgt) && size < best_size)
          || (hg->ewgt && (hg->ewgt[edge] >  best_ewgt
          || (hg->ewgt[edge] == best_ewgt && size < best_size))))) {
              best_neighbors = 1;
              best_edge = edge;
              best_ewgt = hg->ewgt[best_edge];
              best_size = hg->hindex[best_edge+1] - hg->hindex[best_edge];
             }
         }
      if (best_neighbors == 0)
         continue;

      if (best_neighbors > 1) {
         random = Zoltan_HG_Rand() % best_neighbors;
         for (j = hg->vindex[vertex]; j < hg->vindex[vertex+1]; j++) {
            edge = hg->vedge[j];
            size = hg->hindex[edge+1] - hg->hindex[edge];
            if (del_edges[edge] == 0 && ((!(hg->ewgt) && size == best_size)
              || (hg->ewgt && hg->ewgt[edge] == best_ewgt && size == best_size))
              && --best_neighbors == random) {
                 best_edge = edge;
                 break;
                 }
            }
         }

      for (j = hg->hindex[best_edge]; j < hg->hindex[best_edge+1]; j++)
         for (k = hg->vindex[hg->hvertex[j]]; k < hg->vindex[hg->hvertex[j]+1];
          k++)
             del_edges[hg->vedge[k]] = 1;

      for (j = hg->hindex[best_edge]; j < (hg->hindex[best_edge+1] -1)
       && (*limit) > 0; j++) {
          pack[hg->hvertex[j]] = hg->hvertex[j+1];
          (*limit)--;
          }
      pack[hg->hvertex[j]] = hg->hvertex[hg->hindex[best_edge]];
      }
   Zoltan_Multifree (__FILE__, __LINE__, 2, &vertices, &del_edges);
   return ZOLTAN_OK;
}

/****************************************************************************/



/* Greedy Packing. It sorts the edges due to their weight and size
   and packs them if they are available. Time O(|I|+|E|*log(|E|)).
   it guarantees an approximation of 1/k. */
static int packing_grp (ZZ *zz, HGraph *hg, Packing pack, int *limit)
{
int i, j, *size = NULL, *sorted = NULL;
char *yo = "packing_grp";

  /* Sort the hyperedges according to their weight and size */
  if (!(size   = (int*) ZOLTAN_MALLOC (hg->nEdge*sizeof(int)))
   || !(sorted = (int*) ZOLTAN_MALLOC (hg->nEdge*sizeof(int))) ) {
      Zoltan_Multifree (__FILE__, __LINE__, 2, &size, &sorted);
      ZOLTAN_PRINT_ERROR(zz->Proc, yo, "Insufficient memory.");
      return ZOLTAN_MEMERR;
      }
  for (i = 0; i < hg->nEdge; i++)
     size[i] = -(hg->hindex[i+1] - hg->hindex[i]);
  for (i = 0; i < hg->nEdge; i++)
     sorted[i] = i;
  Zoltan_quicksort_pointer_dec_float_int (sorted,hg->ewgt,size,0,hg->nEdge -1);
  ZOLTAN_FREE ((void **) &size);

  /* Match hyperedges along decreasing weight */
  for (i = 0; i < hg->nEdge && (*limit) > 0; i++) {
     for (j = hg->hindex[sorted[i]]; j < hg->hindex[sorted[i] +1]; j++)
        if (pack[hg->hvertex[j]] != hg->hvertex[j])
           break;
     if (j == hg->hindex[sorted[i]+1]) {
        for (j=hg->hindex[sorted[i]]; j < (hg->hindex[sorted[i] +1] -1)
         && (*limit) > 0; j++) {
             pack[hg->hvertex[j]] = hg->hvertex[j+1];
             (*limit)--;
             }
        pack[hg->hvertex[j]] = hg->hvertex[hg->hindex[sorted[i]]];
        }
     }
  ZOLTAN_FREE ((void**) &sorted);
  return ZOLTAN_OK;
}

/****************************************************************************/



/* Locally Heaviest packing. It is like the locally heaviest matching.
   It looks for locally heaviest edges by backtracking. For a current
   edge it has to check the weight of all intersecting edges. Edges
   may be checked several times, but it is amortized not more than
   time O(k*|I|) and guarantees an approximation of 1/k. */
static int lhp_pack (ZZ *zz, HGraph *hg, int edge, int *del_edge, int *Vindex,
 int *Vindex_old, Packing pack, int *limit)
{
int  i, j, vertex, next_edge, done = 0;

  for (i = hg->hindex[edge]; i < hg->hindex[edge+1]; i++)
     Vindex_old[i] = Vindex[hg->hvertex[i]];

  while (hg->ewgt && del_edge[edge] == 0 && done == 0 && (*limit) > 0) {
     done = 1;
     for (i = hg->hindex[edge]; i < hg->hindex[edge+1]; i++) {
        vertex = hg->hvertex[i];
        if (Vindex[vertex] < hg->vindex[vertex+1]) {
           next_edge = hg->vedge[Vindex[vertex]++];
           if (hg->ewgt[next_edge] > hg->ewgt[edge])
               lhp_pack(zz,hg,next_edge,del_edge,Vindex,Vindex_old,pack,limit);
           done=0;
           }
        }
     }

  if (del_edge[edge] == 0 && (*limit) > 0) {
     for (i = hg->hindex[edge]; i < hg->hindex[edge+1]-1 && (*limit) > 0; i++) {
        pack[hg->hvertex[i]] = hg->hvertex[i+1];
        (*limit)--;
        }
     pack[hg->hvertex[i]] = hg->hvertex[hg->hindex[edge]];
     for (i = hg->hindex[edge]; i < hg->hindex[edge+1]; i++) {
        vertex = hg->hvertex[i];
        for (j = hg->vindex[vertex]; j < hg->vindex[vertex+1]; j++)
           del_edge[hg->vedge[j]] = 1;
        }
     }
  else
     for (i = hg->hindex[edge]; i < hg->hindex[edge+1]; i++)
        if (pack[hg->hvertex[i]] == hg->hvertex[i])
           Vindex[hg->hvertex[i]] = Vindex_old[i];

  return ZOLTAN_OK;
}



static int packing_lhp (ZZ *zz, HGraph *hg, Packing pack, int *limit)
{
int  i, *Vindex = NULL, *Vindex_old = NULL, *del_edge = NULL;
char *yo="packing_lhp";

  if (!(del_edge   = (int*) ZOLTAN_CALLOC  (hg->nEdge,    sizeof(int)))
   || !(Vindex     = (int*) ZOLTAN_MALLOC ((hg->nVtx+1) * sizeof(int)))
   || !(Vindex_old = (int*) ZOLTAN_MALLOC  (hg->nInput  * sizeof(int))) ) {
      Zoltan_Multifree (__FILE__, __LINE__, 3, &del_edge, &Vindex, &Vindex_old);
      ZOLTAN_PRINT_ERROR(zz->Proc, yo, "Insufficient memory.");
      return ZOLTAN_MEMERR;
      }
  memcpy(Vindex, hg->vindex, (hg->nVtx+1) * sizeof(int));

  for (i = 0;  i< hg->nEdge && (*limit) > 0; i++)
     if (del_edge[i] == 0)
        lhp_pack(zz, hg, i, del_edge, Vindex, Vindex_old, pack, limit);

  Zoltan_Multifree (__FILE__, __LINE__, 3, &del_edge, &Vindex, &Vindex_old);
  return ZOLTAN_OK;
}

/****************************************************************************/



/* Path Growing Packing. It constructs two packings olong a path.
   The time is O(|I|) and the approximation is 1/(2(k-1)). */
static int packing_pgp (ZZ *zz, HGraph *hg, Packing pack, int *limit)
{
int i, j, k, side, vertex, edge, *Pack[2], limits[2], cur_edge, best_edge;
int *taken_edge = NULL, *taken_vertex = NULL, *size = NULL;
float best_weight, w[2];
char *yo = "packing_pgp";

  w[0] = w[1] = 0.0;
  limits[0] = limits[1] = (*limit);
  Pack[0] = pack;
  if (!(Pack[1]      = (int*) ZOLTAN_MALLOC (hg->nVtx * sizeof(int)))
   || !(taken_edge   = (int*) ZOLTAN_CALLOC (hg->nEdge, sizeof(int)))
   || !(taken_vertex = (int*) ZOLTAN_CALLOC (hg->nVtx,  sizeof(int)))
   || !(size         = (int*) ZOLTAN_CALLOC (hg->nEdge, sizeof(int))) ) {
       Zoltan_Multifree (__FILE__, __LINE__, 4, &Pack[1], &taken_edge,
        &taken_vertex, &size);
       ZOLTAN_PRINT_ERROR(zz->Proc, yo, "Insufficient memory.");
       return ZOLTAN_MEMERR;
       }
  for (i = 0; i < hg->nVtx; i++)
     Pack[1][i] = i;

  for (i = 0; i < hg->nEdge; i++) {
     side = -1;
     if ((taken_edge[i]) % 2 == 0 && limits[0] > 0)
        side = 0;
     else if ((taken_edge[i]/2) % 2 == 0 && limits[1] > 0)
        side = 1;
     if (side >= 0) {
        cur_edge = i;
        do {
           for (j = hg->hindex[cur_edge]; j < hg->hindex[cur_edge+1] -1
            && limits[side] > 0; j++) {
              vertex = hg->hvertex[j];
              Pack[side][vertex] = hg->hvertex[j+1];
              taken_vertex[vertex] += (1+side);
              for (k = hg->vindex[vertex]; k < hg->vindex[vertex+1]; k++)
                 if ((taken_edge[hg->vedge[k]] / (1+side)) % 2 == 0)
              taken_edge[hg->vedge[k]] += (1+side);
              limits[side]--;
              }
           vertex = hg->hvertex[j];
           Pack[side][vertex] = hg->hvertex[hg->hindex[cur_edge]];
           taken_vertex[vertex] += (1+side);
           for (k = hg->vindex[vertex]; k < hg->vindex[vertex+1]; k++)
              if ((taken_edge[hg->vedge[k]] / (1+side)) % 2 == 0)
                 taken_edge[hg->vedge[k]] += (1+side);
           w[side] += (hg->ewgt ? hg->ewgt[cur_edge] : 1.0);

           side = 1-side;

           for (j = hg->hindex[cur_edge]; j < hg->hindex[cur_edge+1]; j++)
              if ((taken_vertex[vertex = hg->hvertex[j]] / (1+side)) % 2 == 0)
                 for (k = hg->vindex[vertex]; k < hg->vindex[vertex+1]; k++)
                    if ((taken_edge[edge = hg->vedge[k]] / (1+side)) % 2 == 0)
                       size[edge]++;
           best_weight = 0.0;
           best_edge = -1;
           for (j = hg->hindex[cur_edge]; j < hg->hindex[cur_edge+1]; j++)
              if ((taken_vertex[vertex = hg->hvertex[j]] / (1+side)) % 2 == 0)
                 for (k = hg->vindex[vertex]; k < hg->vindex[vertex+1]; k++)
                    if ((taken_edge[edge = hg->vedge[k]] / (1+side)) % 2 ==0
                     && size[edge] > 0) {
                        if ((hg->ewgt ? hg->ewgt[edge] : 1.0) / size[edge]
                          > best_weight) {
                           best_edge = edge;
                           best_weight = (hg->ewgt ? hg->ewgt[best_edge] : 1.0);
                           }
                        size[edge] = 0;
                        }
           cur_edge = best_edge;
           }
        while (cur_edge>=0 && limits[side]>0)
          ;
        }
     }

  if (w[0] < w[1]) {
     memcpy (pack, Pack[1],(hg->nVtx) * sizeof(int));
     (*limit) = limits[1];
     }
  else
     (*limit) = limits[0];

  Zoltan_Multifree(__FILE__,__LINE__,4,&Pack[1],&taken_edge,&taken_vertex,&size);
  return ZOLTAN_OK;
}

/****************************************************************************/



/* optimizer passed on path augmentation of length 2 */
static int packing_aug2 (ZZ *zz, HGraph *hg, Packing pack, int *limit)
{
int i, j, edge, other_edge, vertex, next_vertex, intersecting_size;
int *covered_by = NULL, *intersecting = NULL;
float intersecting_weight;
char  *yo = "packing_aug2";

  if (!(covered_by   = (int*) ZOLTAN_MALLOC (hg->nVtx * sizeof(int)))
   || !(intersecting = (int*) ZOLTAN_CALLOC (hg->nEdge, sizeof(int)))  ) {
      Zoltan_Multifree (__FILE__, __LINE__, 2, &covered_by, &intersecting);
      ZOLTAN_PRINT_ERROR(zz->Proc, yo, "Insufficient memory.");
      return ZOLTAN_MEMERR;
      }

  for (i = 0; i < hg->nVtx; i++)
     covered_by[i] = -1;
  for (edge = 0; edge < hg->nEdge; edge++) {
     j = hg->hindex[edge];
     while (j < hg->hindex[edge+1] -1
      && pack[hg->hvertex[j]] == hg->hvertex[j+1])
        j++;
     if (j == hg->hindex[edge+1] -1 &&
      pack[hg->hvertex[j]] == hg->hvertex[hg->hindex[edge]])
         for (j = hg->hindex[edge]; j < hg->hindex[edge+1]; j++)
             covered_by[hg->hvertex[j]] = edge;
     }

  for (edge = 0; i < hg->nEdge; edge++)
     if (covered_by[hg->hvertex[hg->hindex[edge]]] != edge)
        {
        intersecting_weight = 0.0;
        intersecting_size = 0;
        for (i = hg->hindex[edge]; i < hg->hindex[edge+1]; i++)
           if ((other_edge = covered_by[hg->hvertex[i]]) >= 0 &&
            intersecting[other_edge] == 0) {
               intersecting[other_edge] = 1;
               intersecting_size += (hg->hindex[other_edge+1]
                - hg->hindex[other_edge]);
               intersecting_weight += (hg->ewgt ? hg->ewgt[other_edge] : 1.0);
               }

      for (i = hg->hindex[edge]; i < hg->hindex[edge+1]; i++)
          if ((j = covered_by[hg->hvertex[i]]) >= 0)
             intersecting[j] = 0;

      if ((hg->ewgt ? hg->ewgt[edge] : 1.0) > intersecting_weight
       ||((hg->ewgt ? hg->ewgt[edge] : 1.0) == intersecting_weight
       && hg->hindex[edge+1] - hg->hindex[edge] < intersecting_size)) {
          for (i = hg->hindex[edge]; i < hg->hindex[edge+1]; i++)
             if ((other_edge = covered_by[hg->hvertex[i]]) >= 0
              && covered_by[hg->hvertex[hg->hindex[other_edge]]] == other_edge) {
                  vertex = hg->hvertex[hg->hindex[other_edge]];
                  while (pack[vertex] != vertex) {
                     next_vertex = pack[vertex];
                     covered_by[vertex] = -1;
                     pack[vertex] = vertex;
                     (*limit)++;
                     vertex = next_vertex;
                     }
                  (*limit)--;
                  }
          for (j = hg->hindex[edge]; j < (hg->hindex[edge+1] -1)
           && (*limit) > 0; j++) {
              pack[hg->hvertex[j]] = hg->hvertex[j+1];
              (*limit)--;
              covered_by[hg->hvertex[j]] = edge;
              }
          pack[hg->hvertex[j]] = hg->hvertex[hg->hindex[edge]];
          covered_by[hg->hvertex[j]] = edge;
          }
      }
  Zoltan_Multifree (__FILE__, __LINE__, 2, &covered_by, &intersecting);
  return ZOLTAN_OK;
}



/****************************************************************************/

#ifdef __cplusplus
} /* closing bracket for extern "C" */
#endif
