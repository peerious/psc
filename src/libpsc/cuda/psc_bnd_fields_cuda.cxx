
#include "psc_bnd_fields_private.h"

#include "cuda_iface.h"
#include "cuda_iface_bnd.h"
#include "psc_fields_cuda.h"
#include "bnd_fields.hxx"

#include "psc.h"
#include <mrc_profile.h>

struct BndFieldsCuda : BndFieldsBase
{
  // ----------------------------------------------------------------------
  // fill_ghosts_E

  void fill_ghosts_E(PscMfieldsBase mflds_base) override
  {
    const auto& grid = mflds_base->grid();
    if (grid.bc.fld_lo[0] == BND_FLD_PERIODIC &&
	grid.bc.fld_lo[1] == BND_FLD_PERIODIC &&
	grid.bc.fld_lo[2] == BND_FLD_PERIODIC) {
      return;
    }

    PscMfieldsCuda mf = mflds_base.get_as<PscMfieldsCuda>(EX, EX + 3);
    if (grid.bc.fld_lo[0] == BND_FLD_PERIODIC &&
	grid.bc.fld_lo[1] == BND_FLD_CONDUCTING_WALL &&
	grid.bc.fld_hi[1] == BND_FLD_CONDUCTING_WALL &&
	grid.bc.fld_lo[2] == BND_FLD_PERIODIC) {
      int d = 1;
      for (int p = 0; p < mf->n_patches(); p++) {
	if (psc_at_boundary_lo(ppsc, p, d)) {
	  cuda_conducting_wall_E_lo_y(mf->cmflds, p);
	}
	if (psc_at_boundary_hi(ppsc, p, d)) {
	  cuda_conducting_wall_E_hi_y(mf->cmflds, p);
	}
      }
    } else {
      assert(0);
    }
    mf.put_as(mflds_base, EX, EX + 3);
  }

  // ----------------------------------------------------------------------
  // fill_ghosts_H

  void fill_ghosts_H(PscMfieldsBase mflds_base) override
  {
    const auto& grid = mflds_base->grid();
    if (grid.bc.fld_lo[0] == BND_FLD_PERIODIC &&
	grid.bc.fld_lo[1] == BND_FLD_PERIODIC &&
	grid.bc.fld_lo[2] == BND_FLD_PERIODIC) {
      return;
    }

    PscMfieldsCuda mf = mflds_base.get_as<PscMfieldsCuda>(HX, HX + 3);
    if (grid.bc.fld_lo[0] == BND_FLD_PERIODIC &&
	grid.bc.fld_lo[1] == BND_FLD_CONDUCTING_WALL &&
	grid.bc.fld_hi[1] == BND_FLD_CONDUCTING_WALL &&
	grid.bc.fld_lo[2] == BND_FLD_PERIODIC) {
      int d = 1;
      for (int p = 0; p < mf->n_patches(); p++) {
	if (psc_at_boundary_lo(ppsc, p, d)) {
	  cuda_conducting_wall_H_lo_y(mf->cmflds, p);
	}
	if (psc_at_boundary_hi(ppsc, p, d)) {
	  cuda_conducting_wall_H_hi_y(mf->cmflds, p);

	}
      }
    } else {
      assert(0);
    }
    mf.put_as(mflds_base, HX, HX + 3);
  }

  // ----------------------------------------------------------------------
  // add_ghosts_J

  void add_ghosts_J(PscMfieldsBase mflds_base) override
  {
    const auto& grid = mflds_base->grid();
    if (grid.bc.fld_lo[0] == BND_FLD_PERIODIC &&
	grid.bc.fld_lo[1] == BND_FLD_PERIODIC &&
	grid.bc.fld_lo[2] == BND_FLD_PERIODIC) {
      return;
    }

    PscMfieldsCuda mf = mflds_base.get_as<PscMfieldsCuda>(JXI, JXI + 3);
    if (grid.bc.fld_lo[0] == BND_FLD_PERIODIC &&
	grid.bc.fld_lo[1] == BND_FLD_CONDUCTING_WALL &&
	grid.bc.fld_hi[1] == BND_FLD_CONDUCTING_WALL &&
	grid.bc.fld_lo[2] == BND_FLD_PERIODIC) {
      int d = 1;
      for (int p = 0; p < mf->n_patches(); p++) {
	if (psc_at_boundary_lo(ppsc, p, d)) {
	  cuda_conducting_wall_J_lo_y(mf->cmflds, p);
	}
	if (psc_at_boundary_hi(ppsc, p, d)) {
	  cuda_conducting_wall_J_hi_y(mf->cmflds, p);
	}
      }
    } else {
      assert(0);
    }
    mf.put_as(mflds_base, JXI, JXI + 3);
  }
};

// ======================================================================
// psc_bnd_fields: subclass "cuda"

struct psc_bnd_fields_ops_cuda : psc_bnd_fields_ops {
  using PscBndFields_t = PscBndFieldsWrapper<BndFieldsCuda>;
  psc_bnd_fields_ops_cuda() {
    name                  = "cuda";
    size                  = PscBndFields_t::size;
    setup                 = PscBndFields_t::setup;
    destroy               = PscBndFields_t::destroy;
  }
} psc_bnd_fields_cuda_ops;
