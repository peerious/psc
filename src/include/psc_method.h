
#ifndef PSC_METHOD_H
#define PSC_METHOD_H

#include "psc.h"

BEGIN_C_DECLS

MRC_CLASS_DECLARE(psc_method, struct psc_method);

void psc_method_do_setup(struct psc_method *method, struct psc *psc);
void psc_method_setup_partition(struct psc_method *method, struct psc *psc, uint *n_prts_by_patch);
void psc_method_set_ic_particles(struct psc_method *method, struct psc *psc, uint *n_prts_by_patch);
void psc_method_set_ic_fields(struct psc_method *method, struct psc *psc);
void psc_method_initialize(struct psc_method *method, struct psc *psc);
void psc_method_output(struct psc_method *method, struct psc *psc);

END_C_DECLS

#endif

