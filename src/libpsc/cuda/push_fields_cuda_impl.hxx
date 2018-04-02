
#include "push_fields.hxx"
#include "dim.hxx"
#include "psc_fields_cuda.h"
#include "cuda_iface.h"

struct PushFieldsCuda : PushFieldsBase
{
  // ----------------------------------------------------------------------
  // push_E

  void push_E(MfieldsCuda& mflds, double dt_fac, dim_yz tag)
  {
    cuda_push_fields_E_yz(mflds.cmflds, dt_fac * ppsc->dt);
  }

  // dispatch -- FIXME, mostly same as non-cuda dispatch
  void push_E(PscMfieldsBase mflds_base, double dt_fac) override
  {
    auto& mflds = mflds_base->get_as<MfieldsCuda>(JXI, HX + 3);
    
    const auto& grid = mflds.grid();
    using Bool3 = Vec3<bool>;
    Bool3 invar{grid.isInvar(0), grid.isInvar(1), grid.isInvar(2)};

    if (invar == Bool3{true, false, false}) {
      push_E(mflds, dt_fac, dim_yz{});
    } else {
      assert(0);
    }
    
    mflds_base->put_as(mflds, EX, EX + 3);
  }

  // ----------------------------------------------------------------------
  // push_H

  void push_H(MfieldsCuda& mflds, double dt_fac, dim_yz tag)
  {
    cuda_push_fields_H_yz(mflds.cmflds, dt_fac * ppsc->dt);
  }

  // dispatch -- FIXME, mostly same as non-cuda dispatch
  void push_H(PscMfieldsBase mflds_base, double dt_fac) override
  {
    auto& mflds = mflds_base->get_as<MfieldsCuda>(JXI, HX + 3);
    
    const auto& grid = mflds.grid();
    using Bool3 = Vec3<bool>;
    Bool3 invar{grid.isInvar(0), grid.isInvar(1), grid.isInvar(2)};

    if (invar == Bool3{true, false, false}) {
      push_H(mflds, dt_fac, dim_yz{});
    } else {
      assert(0);
    }

    mflds_base->put_as(mflds, HX, HX + 3);
  }
};
