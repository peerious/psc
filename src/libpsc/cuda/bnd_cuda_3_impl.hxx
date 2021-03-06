
#pragma once

#include "bnd.hxx"
#include "psc_fields_cuda.h"

// ======================================================================
// BndCuda3
//
// just wrapping CudaBnd doing the actual work

struct CudaBnd;

template <typename MF>
struct BndCuda3 : BndBase
{
  using Mfields = MF;

  BndCuda3(const Grid_t& grid, const int ibn[3]);
  ~BndCuda3();

  void reset(const Grid_t& grid);
  void add_ghosts(MfieldsCuda& mflds, int mb, int me);
  void add_ghosts(MfieldsStateCuda& mflds, int mb, int me);
  void fill_ghosts(MfieldsCuda& mflds, int mb, int me);
  void fill_ghosts(MfieldsStateCuda& mflds, int mb, int me);

private:
  static CudaBnd* cbnd_;
  static int balance_generation_cnt_;
};
