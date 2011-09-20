
#include "psc_testing.h"
#include "psc_push_particles.h"
#include <mrc_profile.h>
#include <mrc_params.h>

#include <stdio.h>
#include <mpi.h>

int
main(int argc, char **argv)
{
  MPI_Init(&argc, &argv);
  libmrc_params_init(argc, argv);

  struct psc_case *_case = psc_create_test_xz();
  psc_push_particles_set_type(ppsc->push_particles, "fortran");
  psc_case_setup(_case);
  mparticles_base_t *particles = ppsc->particles;
  //  psc_dump_particles("part-0");
  psc_push_particles_run(ppsc->push_particles, particles, ppsc->flds);
  //  psc_dump_particles("part-1");
  psc_save_particles_ref(ppsc, particles);
  psc_save_fields_ref(ppsc, ppsc->flds);
  psc_case_destroy(_case);

  _case = psc_create_test_xz();
  psc_push_particles_set_type(ppsc->push_particles, "generic_c");
  psc_case_setup(_case);
  particles = ppsc->particles;
  psc_push_particles_run(ppsc->push_particles, particles, ppsc->flds);
  //  psc_dump_particles("part-2");
  psc_check_particles_ref(ppsc, particles, 1e-7, "push_part_xz -- generic_c");
  psc_check_currents_ref(ppsc, ppsc->flds, 1e-7);
  psc_case_destroy(_case);

  _case = psc_create_test_xz();
  psc_push_particles_set_type(ppsc->push_particles, "1st");
  psc_case_setup(_case);
  particles = ppsc->particles;
  psc_push_particles_run(ppsc->push_particles, particles, ppsc->flds);
  //  psc_dump_particles("part-2");
  psc_check_particles_ref(ppsc, particles, 1e-1, "push_part_xz -- 1st");
  //  psc_check_currents_ref(ppsc, ppsc->flds, 1e-1);
  psc_case_destroy(_case);

  _case = psc_create_test_xz();
  psc_push_particles_set_type(ppsc->push_particles, "1sff");
  psc_case_setup(_case);
  particles = ppsc->particles;
  psc_push_particles_run(ppsc->push_particles, particles, ppsc->flds);
  //  psc_dump_particles("part-2");
  psc_check_particles_ref(ppsc, particles, 1e-1, "push_part_xz -- 1sff");
  //  psc_check_currents_ref(ppsc, ppsc->flds, 1e-1);
  psc_case_destroy(_case);

#ifdef USE_SSE2
  struct psc_mod_config conf_sse2 = {
    .mod_particle = "sse2",
  };
  psc_create_test_xz(&conf_sse2);
  psc_push_particles();
  //  psc_dump_particles("part-2");
  psc_check_particles_ref(1e-7, "push_part_xz -- sse2");
  psc_check_currents_ref(1e-6);
  psc_destroy();
#endif

  prof_print();

  MPI_Finalize();
}
