
#include "psc_fields_as_single.h"
#include "psc_particles_as_single.h"

#define PSC_CHECKS_ORDER "2nd"

#include "psc_checks_common.cxx"

// ----------------------------------------------------------------------
// psc_checks_2nd_single_ops

struct psc_checks_2nd_single_ops : psc_checks_ops {
  using Wrapper_t = ChecksWrapper<Checks_<MparticlesSingle, MfieldsSingle>>;
  psc_checks_2nd_single_ops() {
    name                            = PSC_CHECKS_ORDER "_" PARTICLE_TYPE;
    size                            = Wrapper_t::size;
    setup                           = Wrapper_t::setup;
    destroy                         = Wrapper_t::destroy;
    //read                            = psc_checks_sub_read;
  }
} psc_checks_2nd_single_ops;



