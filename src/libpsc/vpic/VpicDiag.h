
#ifndef VPIC_DIAG_H
#define VPIC_DIAG_H

#include "vpic_iface.h"
#include "vpic.h"

// ----------------------------------------------------------------------
// VpicDiag

struct VpicDiag {
  int interval;
  int energies_interval;
  int fields_interval;
  int ehydro_interval;
  int Hhydro_interval;
  int eparticle_interval;
  int Hparticle_interval;
  int restart_interval;

  // state
  int rtoggle;               // enables save of last 2 restart dumps for safety
  // Output variables
  DumpParameters fdParams;
  DumpParameters hedParams;
  DumpParameters hHdParams;
  std::vector<DumpParameters *> outputParams;

  VpicDiag(vpic_simulation* simulation);
  void init(int interval_);
  void setup();
  void run();

private:
  vpic_simulation* simulation_;
};



void vpic_simulation_diagnostics(vpic_simulation *simulation, VpicDiag *diag);
void vpic_simulation_setup_diagnostics(vpic_simulation *simulation, VpicDiag *diag);


inline VpicDiag::VpicDiag(vpic_simulation *simulation)
  : simulation_(simulation)
{
}

inline void VpicDiag::init(int interval_)
{
  rtoggle = 0;
  
  interval = interval_;
  fields_interval = interval_;
  ehydro_interval = interval_;
  Hhydro_interval = interval_;
  eparticle_interval = 8 * interval_;
  Hparticle_interval = 8 * interval_;
  restart_interval = 8000;

  energies_interval = 50;

  MPI_Comm comm = MPI_COMM_WORLD;
  mpi_printf(comm, "interval = %d\n", interval);
  mpi_printf(comm, "energies_interval: %d\n", energies_interval);
}

inline void VpicDiag::setup()
{
  vpic_simulation_setup_diagnostics(simulation_, this);
}

inline void VpicDiag::run()
{
  TIC vpic_simulation_diagnostics(simulation_, this); TOC(user_diagnostics, 1);
}

#endif
