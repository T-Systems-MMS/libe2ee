cmake_minimum_required(VERSION 3.13)
project(pbc C)

set(CMAKE_C_STANDARD 11)

set(pbc_src_dir "${PROJECT_SOURCE_DIR}/../pbc")

include_directories(${pbc_src_dir})
include_directories(${pbc_src_dir}/ecc)
include_directories(${pbc_src_dir}/include)
include_directories(${pbc_src_dir}/misc)
include_directories(${pbc_src_dir}/pbc)

find_package(GMP REQUIRED)

add_library(pbc
        ${pbc_src_dir}/arith/dlog.c
        ${pbc_src_dir}/arith/fasterfp.c
        ${pbc_src_dir}/arith/fastfp.c
        ${pbc_src_dir}/arith/field.c
        ${pbc_src_dir}/arith/fieldquadratic.c
        ${pbc_src_dir}/arith/fp.c
        ${pbc_src_dir}/arith/init_random.c
        ${pbc_src_dir}/arith/montfp.c
        ${pbc_src_dir}/arith/multiz.c
        ${pbc_src_dir}/arith/naivefp.c
        ${pbc_src_dir}/arith/poly.c
        ${pbc_src_dir}/arith/random.c
        ${pbc_src_dir}/arith/ternary_extension_field.c
        ${pbc_src_dir}/arith/tinyfp.c
        ${pbc_src_dir}/arith/z.c
        ${pbc_src_dir}/ecc/a_param.c
        ${pbc_src_dir}/ecc/curve.c
        ${pbc_src_dir}/ecc/d_param.c
        ${pbc_src_dir}/ecc/e_param.c
        ${pbc_src_dir}/ecc/eta_T_3.c
        ${pbc_src_dir}/ecc/f_param.c
        ${pbc_src_dir}/ecc/g_param.c
        ${pbc_src_dir}/ecc/hilbert.c
        ${pbc_src_dir}/ecc/mnt.c
        ${pbc_src_dir}/ecc/mpc.c
        ${pbc_src_dir}/ecc/pairing.c
        ${pbc_src_dir}/ecc/param.c
        ${pbc_src_dir}/ecc/param.h
        ${pbc_src_dir}/ecc/singular.c
        ${pbc_src_dir}/guru/timefp.c
        ${pbc_src_dir}/misc/darray.c
        ${pbc_src_dir}/misc/extend_printf.c
        ${pbc_src_dir}/misc/get_time.c
        ${pbc_src_dir}/misc/memory.c
        ${pbc_src_dir}/misc/symtab.c
        ${pbc_src_dir}/misc/utils.c
        )
target_link_libraries(pbc ${GMP_LIBRARIES})
#target_compile_definitions(pbc PUBLIC PBC_DEBUG)
