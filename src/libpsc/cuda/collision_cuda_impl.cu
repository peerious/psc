
#include "collision_cuda_impl.hxx"

#include "cuda_collision.cuh"

// ======================================================================
// CollisionCuda

template<typename BS>
CollisionCuda<BS>::CollisionCuda(MPI_Comm comm, int interval, double nu)
  : coll_{comm, interval, nu},
    fwd_{new cuda_collision<cuda_mparticles<BS>>{interval, nu}}
{}

template<typename BS>
void CollisionCuda<BS>::operator()(MparticlesCuda<BS>& _mprts)
{
  (*fwd_)(_mprts.cmprts());
  auto& mprts = _mprts.template get_as<MparticlesSingle>();
  sort_(mprts);
  coll_(mprts);
  _mprts.put_as(mprts);
}

template struct CollisionCuda<BS144>;
