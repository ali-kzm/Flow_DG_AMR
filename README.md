# Flow_DG_AMR

Research codebase for incompressible flow solvers using discontinuous finite elements, adaptive Cartesian meshes with p4est, MPI-based distributed mesh management, and OpenCL acceleration.

## Status

Project skeleton only. Numerical formulation, discretization details, data structures, solvers, and physics classes will be defined incrementally.

## Dependencies

- CMake >= 3.20
- C++20 compiler
- C compiler
- MPI
- OpenCL
- p4est v2.8.7 (included as `lib/p4est` git submodule)

## Clone

```bash
git clone --recurse-submodules https://github.com/ali-kzm/Flow_DG_AMR.git
cd Flow_DG_AMR
```

If the repository was cloned without submodules:

```bash
git submodule update --init --recursive
```

## Build

```bash
cmake -S . -B build
cmake --build build -j
```

Run the initial executable with:

```bash
./build/flow_dg_amr
```

For MPI:

```bash
mpirun -np 4 ./build/flow_dg_amr
```

## Project layout

```text
.
├── AGENT.md
├── ARCH.md
├── CMakeLists.txt
├── LICENSE
├── include/
├── lib/
│   └── p4est/
├── src/
└── tests/
```

## Documentation

- `ARCH.md`: architecture specification placeholder; detailed content will be supplied later.
- `AGENT.md`: agent behavior/instructions placeholder; detailed content will be supplied later.

## License

The project source is proprietary and all rights are reserved. See `LICENSE`.

Third-party components retain their own licenses. In particular, p4est is included as a separate git submodule and is governed by its upstream license.
