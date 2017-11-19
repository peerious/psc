
#ifndef PSC_FIELDS_VPIC_H
#define PSC_FIELDS_VPIC_H

#include "psc_fields_private.h"

#define FTYPE FTYPE_VPIC
#include "psc_fields_common.h"
#undef FTYPE

#include "vpic_iface.h"

// ----------------------------------------------------------------------

struct psc_mfields_vpic {
  FieldArray *vmflds_fields;
  struct HydroArray *vmflds_hydro;
};

#define psc_mfields_vpic(mflds) ({					\
      assert((struct psc_mfields_ops *) mflds->obj.ops == &psc_mfields_vpic_ops); \
      mrc_to_subobj(mflds, struct psc_mfields_vpic);			\
    })

#endif
