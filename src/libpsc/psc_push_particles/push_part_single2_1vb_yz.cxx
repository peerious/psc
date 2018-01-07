
#include "psc_push_particles_private.h"

#include "psc_particles_as_single.h"
#include "psc_fields_as_single.h"

#define DIM DIM_YZ

#include "inc_defs.h"

#define ORDER ORDER_1ST
#define CALC_J CALC_J_1VB_2D
#define EXT_PREPARE_SORT

#include "1vb.c"

// ======================================================================
// psc_push_particles: subclass "1vb2_single"

struct psc_push_particles_ops psc_push_particles_1vb2_single_ops = {
  .name                  = "1vb2_single2",
  .push_mprts_yz         = psc_push_particles_push_mprts_yz,
  .particles_type        = PARTICLE_TYPE,
};

