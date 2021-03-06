#!/usr/bin/env bash
#
#PBS -l nodes=1:ppn=32
#PBS -l walltime=48:00:00
#PBS -j oe
#
# Arguments: ./mhd_wave_cpaw.sh [pdim [mode_num [ncells [v_par]]]]
#     pdim (str): direction of wave propogation, one of
#                 {x, y, z, xy, yz, zx, xyz}
#     mode_num (int): mode number in all dimensions of the box
#     ncells (int): number of grid cells in all directions
#     v_par (float): parallel flow, use -1.0 for a stationary wave
#
# Run Notes: Linearly polarized alfven wave. Use pdim and mode_num to
#            customize the domain, direction of wave propogation, and
#            wavelength
#

cd "${PBS_O_WORKDIR:-./}"

nprocs=2

pdim="${1:-x}"
mode_num=${2:-1}
if [ ${#pdim} -eq 1 ]; then
  ncells=${3:-512}
elif [ ${#pdim} -eq 2 ]; then
  ncells=${3:-64}
else
  ncells=${3:-32}
fi
v_par=${4:-0.0}

run_name="alfven_${pdim}_${ncells}"

if [ $pdim = 'x' ]; then 
  hx=1.0;          hy=0.1;          hz=0.1
  dmx=${ncells};   dmy=1;           dmz=1  # domain m[xyz]
  npx=${nprocs};   npy=1;           npz=1
  wmx=${mode_num}; wmy=0;           wmz=0  # wave mode number m[xyz]
elif [ $pdim = 'y' ]; then 
  hx=0.1;          hy=1.0;          hz=0.1
  dmx=1;           dmy=${ncells};   dmz=1  # domain m[xyz]
  npx=1;           npy=${nprocs};   npz=1
  wmx=0;           wmy=${mode_num}; wmz=0  # wave mode number m[xyz]
elif [ $pdim = 'z' ]; then 
  hx=0.1;          hy=0.1;          hz=1.0
  dmx=1;           dmy=1;           dmz=${ncells}  # domain m[xyz]
  npx=1;           npy=1;           npz=${nprocs}
  wmx=0;           wmy=0;           wmz=${mode_num}  # wave mode number m[xyz]
elif [ $pdim = 'xy' ]; then 
  hx=1.0;          hy=1.0;          hz=0.1
  dmx=${ncells};   dmy=${ncells};   dmz=1  # domain m[xyz]
  npx=${nprocs};   npy=1;           npz=1
  wmx=${mode_num}; wmy=${mode_num}; wmz=0  # IC wave mode number m[xyz]
elif [ $pdim = 'yz' ]; then 
  hx=0.1;          hy=1.0;          hz=1.0
  dmx=1;           dmy=${ncells};   dmz=${ncells}  # domain m[xyz]
  npx=1;           npy=${nprocs};   npz=1
  wmx=0;           wmy=${mode_num}; wmz=${mode_num}  # wave mode number m[xyz]
elif [ $pdim = 'zx' ]; then 
  hx=1.0;          hy=0.1;          hz=1.0
  dmx=${ncells};   dmy=1;           dmz=${ncells}  # domain m[xyz]
  npx=1;           npy=1;           npz=${nprocs}
  wmx=${mode_num}; wmy=0;           wmz=${mode_num}  # wave mode number m[xyz]
elif [ $pdim = 'xyz' ]; then 
  hx=1.0;          hy=1.0;          hz=1.0
  dmx=${ncells};   dmy=${ncells};   dmz=${ncells}  # domain m[xyz]
  npx=${nprocs};   npy=1;           npz=1
  wmx=${mode_num}; wmy=${mode_num}; wmz=${mode_num}  # wave mode number m[xyz]
else
  echo "Unknown pdim setting, choose one of {x, y, z, xy, yz, zx, xyz}" >&2
  exit 999
fi

MPIRUN="${MPIRUN:-mpirun}"
MPI_OPTS="-n ${nprocs} ${MPI_OPTS}"
GDB="${GDB:-gdb}"

bin="./mhd_wave"
cmd="${MPIRUN} ${MPI_OPTS} ${bin}"
# cmd="${MPIRUN} ${MPI_OPTS} xterm -e ${GDB} --args ${bin}"

${cmd}                                                                       \
  --cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc \
  --ccccccccccccccccccccc  "GENERAL"                                         \
  --run                        ${run_name}                                   \
  --cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc \
  --ccccccccccccccccccccc  "GRID"                                            \
  --mrc_domain_mx  ${dmx} --mrc_domain_my  ${dmy} --mrc_domain_mz  ${dmz}    \
  --mrc_crds_lx    0.0    --mrc_crds_ly    0.0    --mrc_crds_lz    0.0       \
  --mrc_crds_hx    ${hx}  --mrc_crds_hy    ${hy}  --mrc_crds_hz    ${hz}     \
  --mrc_domain_npx ${npx} --mrc_domain_npy ${npy} --mrc_domain_npz ${npz}    \
  --ggcm_mhd_crds_type         c                                             \
  --ggcm_mhd_crds_gen_type     mrc                                           \
  --cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc \
  --ccccccccccccccccccccc  "INITIAL CONDITION"                               \
  --ggcm_mhd_ic_type           cpaw                                          \
  --ggcm_mhd_ic_v_par          ${v_par}                                      \
  --ggcm_mhd_ic_polarization   0.0                                           \
  --ggcm_mhd_ic_mx ${wmx} --ggcm_mhd_ic_my ${wmy} --ggcm_mhd_ic_mz ${wmz}    \
  --cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc \
  --ccccccccccccccccccccc  "NUMERICS"                                        \
  --ggcm_mhd_step_type         c3_double                                     \
  --ggcm_mhd_do_vgrupd         0                                             \
  --cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc \
  --ccccccccccccccccccccc  "TIMESTEP"                                        \
  --ggcm_mhd_step_do_nwst      1                                             \
  --ggcm_mhd_thx               0.4                                           \
  --dtmin                      5e-5                                          \
  --mrc_ts_max_time            1.04                                          \
  --cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc \
  --ccccccccccccccccccccc  "SIM PARAMETERS"                                  \
  --ggcm_mhd_d_i               0.0                                           \
  --rrmin                      0.0                                           \
  --cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc \
  --ccccccccccccccccccccc  "OUTPUT"                                          \
  --ggcm_mhd_step_debug_dump   0                                             \
  --mrc_ts_output_every_time   0.05                                          \
  --mrc_io_sw                  0                                             \
  --ggcm_mhd_diag_fields       rr:pp:v:b:b1:j:e_cc:divb                      \
  --cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc \
  --ccccccccccccccccccccc  "MISC"                                            \
  --ggcm_mhd_do_badval_checks  1                                             \
  --monitor_conservation       0                                             \
  --cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc \
  2>&1 | tee ${run_name}.log

errcode=$?

# plot quick and dirty time series with:
# viscid_ts --slice x=0,y=0,z=0 -p vy -p by alfven_x_512.3d.xdmf

# plot final time step:
# viscid_2d --own --slice z=0 -p vy -p by -t -1 alfven_x_512.3d.xdmf

# plot movie:
# viscid_2d --own --slice z=0 -p vy -p by -a alfven_x_512.mp4 --np 2 \
#           alfven_x_512.3d.xdmf

exit ${errcode}

##
## EOF
##
