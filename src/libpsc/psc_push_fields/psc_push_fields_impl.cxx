
#include "psc_push_fields_private.h"

#include "psc_fields_single.h"
#include "psc_fields_c.h"

#include "psc_push_fields_impl.hxx"

// ======================================================================
// psc_push_fields: subclass "single"

struct psc_push_fields_ops psc_push_fields_single_ops = {
  .name                  = "single",
  .push_mflds_E          = psc_push_fields_sub_push_mflds_E<mfields_single_t>,
  .push_mflds_H          = psc_push_fields_sub_push_mflds_H<mfields_single_t>,
};

// ======================================================================
// psc_push_fields: subclass "c"

struct psc_push_fields_ops psc_push_fields_c_ops = {
  .name                  = "c",
  .push_mflds_E          = psc_push_fields_sub_push_mflds_E<mfields_c_t>,
  .push_mflds_H          = psc_push_fields_sub_push_mflds_H<mfields_c_t>,
};
