
using real_t = mparticles_t::real_t;

// ======================================================================
// quicksort

static inline int
get_cell_index(int p, const particle_t *part)
{
  Grid_t& grid = ppsc->grid;
  real_t dxi = 1.f / grid.dx[0];
  real_t dyi = 1.f / grid.dx[1];
  real_t dzi = 1.f / grid.dx[2];
  int *ldims = grid.ldims;
  int *ibn = ppsc->ibn;
  
  real_t u = part->xi * dxi;
  real_t v = part->yi * dyi;
  real_t w = part->zi * dzi;
  int j0 = nint(u) + ibn[0];
  int j1 = nint(v) + ibn[1];
  int j2 = nint(w) + ibn[2];
    
  return ((j2) * (ldims[1] + 2*ibn[1]) + j1) * (ldims[0] + 2*ibn[0]) + j0;
}

static inline int
get_cell_index_2x2x2(int p, const particle_t *part)
{
  Grid_t& grid = ppsc->grid;
  real_t dxi = 1.f / grid.dx[0];
  real_t dyi = 1.f / grid.dx[1];
  real_t dzi = 1.f / grid.dx[2];
  int *ldims = grid.ldims;
  int ibn[3] = { 2, 2, 2 }; // must be divisible by 2
  
  real_t u = part->xi * dxi;
  real_t v = part->yi * dyi;
  real_t w = part->zi * dzi;
  int j0 = nint(u) + ibn[0];
  int j1 = nint(v) + ibn[1];
  int j2 = nint(w) + ibn[2];
    
  return ((((j2 >> 1) * (ldims[1] + 2*ibn[1]) +
	   (j1 >> 1)) * (ldims[0] + 2*ibn[0]) +
	   (j0 >> 1)) << 0);// | ((j2 & 1) << 2) | ((j1 & 1) << 1) | (j0 & 1);
}


static int
compare(const void *_a, const void *_b)
{
  const particle_t *a = (const particle_t *) _a, *b = (const particle_t *) _b;

  if (get_cell_index(0, a) < get_cell_index(0, b)) {
    return -1;
  } else if (get_cell_index(0, a) == get_cell_index(0, b)) {
    return 0;
  } else {
    return 1;
  }
}

static void
psc_sort_qsort_run(struct psc_sort *sort, struct psc_mparticles *mprts_base)
{
  static int pr;
  if (!pr) {
    pr = prof_register("qsort_sort", 1., 0, 0);
  }
  mparticles_t mprts = mprts_base->get_as<mparticles_t>();

  prof_start(pr);
  for (int p = 0; p < mprts.n_patches(); p++) {
    mparticles_t::patch_t& prts = mprts[p];
    qsort(&*prts.begin(), prts.size(), sizeof(*prts.begin()), compare);
  }
  prof_stop(pr);

  mprts.put_as(mprts_base);
}

// ======================================================================
// counting sort

static void
psc_sort_countsort_run(struct psc_sort *sort, struct psc_mparticles *mprts_base)
{
  static int pr;
  if (!pr) {
    pr = prof_register("countsort_sort", 1., 0, 0);
  }

  mparticles_t mprts = mprts_base->get_as<mparticles_t>();

  prof_start(pr);
  for (int p = 0; p < mprts.n_patches(); p++) {
    struct psc_patch *patch = &ppsc->patch[p];
    mparticles_t::patch_t& prts = mprts[p];
    unsigned int n_prts = prts.size();
    
    int N = 1;
    for (int d = 0; d < 3; d++) {
      N *= patch->ldims[d] + 2 * ppsc->ibn[d];
    }
    unsigned int *cnts = (unsigned int *) malloc(N * sizeof(*cnts));
    memset(cnts, 0, N * sizeof(*cnts));
    
    // count
    PARTICLE_ITER_LOOP(prt_iter, prts.begin(), prts.end()) {
      unsigned int cni = get_cell_index(p, &*prt_iter);
      assert(cni < N);
      cnts[cni]++;
    }
    
    // calc offsets
    int cur = 0;
    for (int i = 0; i < N; i++) {
      int n = cnts[i];
      cnts[i] = cur;
    cur += n;
    }
    assert(cur == n_prts);
    
    // move into new position
    particle_t *particles2 = (particle_t *) malloc(n_prts * sizeof(*particles2));
    PARTICLE_ITER_LOOP(prt_iter, prts.begin(), prts.end()) {
      unsigned int cni = get_cell_index(0, &*prt_iter);
      memcpy(&particles2[cnts[cni]], &*prt_iter, sizeof(*particles2));
      cnts[cni]++;
    }
    
    // back to in-place
    memcpy(&*prts.begin(), particles2, n_prts * sizeof(*particles2));
    
    free(particles2);
    free(cnts);
  }
  
  prof_stop(pr);

  mprts.put_as(mprts_base);
}


// ======================================================================
// counting sort 2
// use a separate array of cell indices 

struct psc_sort_countsort2 {
  int blocksize[3];
  int mask;
};

#define VAR(x) (void *)offsetof(struct psc_sort_countsort2, x)
static struct param psc_sort_countsort2_descr[] = {
  { "blocksize"     , VAR(blocksize)       , PARAM_INT3(1, 1, 1)   },
  { "mask"          , VAR(mask)            , PARAM_INT(0)          },
  {},
};
#undef VAR


static void
psc_sort_countsort2_run(struct psc_sort *sort, struct psc_mparticles *mprts_base)
{
  struct psc_sort_countsort2 *cs2 = mrc_to_subobj(sort, struct psc_sort_countsort2);

  static int pr;
  if (!pr) {
    pr = prof_register("countsort2_sort", 1., 0, 0);
  }

  mparticles_t mprts = mprts_base->get_as<mparticles_t>();

  prof_start(pr);
  const Grid_t& grid = ppsc->grid;
  for (int p = 0; p < mprts.n_patches(); p++) {
    mparticles_t::patch_t& prts = mprts[p];
    unsigned int n_prts = prts.size();

    unsigned int mask = cs2->mask;
    struct cell_map map;
    int N = cell_map_init(&map, grid.ldims, cs2->blocksize);

    unsigned int *cnis = (unsigned int *) malloc(n_prts * sizeof(*cnis));
    int i = 0;
    for (auto prt_iter = prts.begin(); prt_iter != prts.end(); ++prt_iter, ++i) {
      particle_t *p = &*prt_iter;
      real_t dxi = 1.f / grid.dx[0];
      real_t dyi = 1.f / grid.dx[1];
      real_t dzi = 1.f / grid.dx[2];
      real_t xi[3] = { p->xi * dxi, p->yi * dyi, p->zi * dzi };
      int pos[3];
      for (int d = 0; d < 3; d++) {
	pos[d] = fint(xi[d]);
#if 1
	if (pos[d] < 0 || pos[d] >= grid.ldims[d]) {
	  printf("i %d d %d pos %d // %d xi %g dxi %g\n",
		 i, d, pos[d], grid.ldims[d],
		 (&p->xi)[d], 1. / grid.dx[d]);
	}
#endif
	if (pos[d] == grid.ldims[d]) {
	  pos[d]--;
	}
      }
      
      cnis[i] = cell_map_3to1(&map, pos) & ~mask;
      assert(cnis[i] < N);
    }
    cell_map_free(&map);
    
    unsigned int *cnts = (unsigned int *) malloc(N * sizeof(*cnts));
    memset(cnts, 0, N * sizeof(*cnts));
    
    // count
    for (int i = 0; i < n_prts; i++) {
      unsigned int cni = cnis[i];
      cnts[cni]++;
    }
    
    // calc offsets
    int cur = 0;
    for (int i = 0; i < N; i++) {
      int n = cnts[i];
      cnts[i] = cur;
      cur += n;
    }
    assert(cur == n_prts);
    
    // move into new position
    particle_t *particles2 = (particle_t *) malloc(n_prts * sizeof(*particles2));
    for (int i = 0; i < n_prts; i++) {
      unsigned int cni = cnis[i];
      int n = 1;
      while (i+n < n_prts && cnis[i+n] == cni) {
	n++;
      }
      memcpy(&particles2[cnts[cni]], &prts[i], n * sizeof(*particles2));
      cnts[cni] += n;
      i += n - 1;
    }
    
    // back to in-place
    memcpy(&*prts.begin(), particles2, n_prts * sizeof(*particles2));
    
    free(particles2);
    free(cnis);
    free(cnts);
  }

  prof_stop(pr);

  mprts.put_as(mprts_base);
}


