
#include "psc_testing.h"
#include <mrc_profile.h>
#include <mrc_params.h>

#include <stdio.h>
#include <mpi.h>

int
main(int argc, char **argv)
{
  MPI_Init(&argc, &argv);
  libmrc_params_init(argc, argv);

  struct psc_mod_config conf_fortran = {
    .mod_particle = "fortran",
  };
  struct psc_mod_config conf_generic_c = {
    .mod_particle = "generic_c",
  };

  psc_create_test_xz(&conf_fortran);
  mfields_base_t *flds = &psc.flds;
  mparticles_base_t *particles = &psc.particles;
  //  psc_dump_particles("part-0");
  psc_push_particles(flds, particles);
  //  psc_dump_particles("part-1");
  psc_save_particles_ref(particles);
  psc_save_fields_ref(flds);
  psc_destroy();

  psc_create_test_xz(&conf_generic_c);
  psc_push_particles(flds, particles);
  //  psc_dump_particles("part-2");
  psc_check_particles_ref(particles, 1e-7, "push_part_xz -- generic_c");
  psc_check_currents_ref(flds, 1e-7);
  psc_destroy();

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
