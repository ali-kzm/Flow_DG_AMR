# Flow_DG_AMR

**Flow_DG_AMR** is a C++ research codebase for developing adaptive discontinuous finite element flow solvers on structured Cartesian meshes.

The project is being designed around four main technologies:

- **p4est** for adaptive quadtree mesh management and repartitioning,
- **MPI** for distributed-memory parallelism,
- **Discontinuous Galerkin / discontinuous finite element methods** for the numerical discretization,
- **OpenCL** for GPU and accelerator execution of compute-intensive local kernels.

The long-term goal is to keep the numerical formulation and physics implementation under direct project control while using p4est only as the adaptive mesh infrastructure.

> [!IMPORTANT]
> The repository is currently in the **infrastructure/bootstrap stage**. The build system, MPI/OpenCL detection, p4est integration, documentation structure, and a small environment-check executable are present. The actual DG flow solver, element classes, mesh abstraction, assembly/operators, time integration, boundary-condition system, adaptivity criteria, and OpenCL kernels are **not implemented yet**.

---

## Table of contents

- [Project goals](#project-goals)
- [Current status](#current-status)
- [Technology stack](#technology-stack)
- [Planned execution model](#planned-execution-model)
- [Repository layout](#repository-layout)
- [Requirements](#requirements)
- [Ubuntu setup](#ubuntu-setup)
- [Clone the repository](#clone-the-repository)
- [Build](#build)
- [Run](#run)
- [MPI usage](#mpi-usage)
- [OpenCL verification](#opencl-verification)
- [p4est integration](#p4est-integration)
- [CMake configuration](#cmake-configuration)
- [Testing](#testing)
- [Development workflow](#development-workflow)
- [Planned architecture](#planned-architecture)
- [Roadmap](#roadmap)
- [Troubleshooting](#troubleshooting)
- [Documentation](#documentation)
- [License](#license)

---

## Project goals

The project is intended to become a research-oriented flow solver in which the numerical method remains explicit and modifiable rather than being hidden behind a large FEM framework.

The main design goals are:

1. **Develop the flow physics and DG formulation directly in C/C++.**
2. **Use p4est only for adaptive Cartesian mesh management**, including refinement/coarsening, balancing, partitioning, neighborhood information, and ghost-layer support.
3. Support **adaptive mesh refinement (AMR)** while retaining a clean solver-side mesh representation.
4. Use **MPI** for coarse-grained distributed-memory parallelism.
5. Use **OpenCL** for data-parallel element, face, quadrature, residual, and related local operations where GPU execution is beneficial.
6. Keep the code modular enough that mesh management, finite-element data, numerical operators, physics, and accelerator execution remain separate concerns.
7. Keep architecture decisions documented in [ARCH.md](ARCH.md).

The exact mathematical formulation and class structure will be added incrementally as the project evolves.

---

## Current status

Current project version:

```text
0.1.0
```

### Implemented

The repository currently provides:

- CMake-based C/C++ build configuration,
- C++20 configuration,
- MPI discovery and linking,
- OpenCL discovery and linking,
- p4est integrated as a Git submodule under `lib/p4est`,
- p4est configured with MPI support through libsc,
- p4est examples and tests disabled in the parent build,
- p8est/p6est disabled because the current project target is 2D,
- a minimal executable named `flow_dg_amr`,
- an MPI startup/shutdown smoke test,
- p4est version reporting,
- OpenCL platform detection,
- initial project documentation,
- a test directory scaffold.

### Not yet implemented

The following are intentionally not fixed or implemented yet:

- solver equations,
- incompressible-flow formulation,
- DG weak form,
- basis functions,
- quadrature rules,
- Q1/Q2 element representation,
- mesh wrapper/data structures,
- p4est-to-solver mesh extraction,
- face connectivity,
- hanging-interface handling,
- boundary-condition representation,
- field/vector storage,
- numerical fluxes,
- residual/Jacobian operators,
- linear/nonlinear solvers,
- time integration,
- AMR indicators,
- prolongation/restriction,
- OpenCL device/context management,
- OpenCL kernels,
- MPI/OpenCL data ownership strategy,
- VTK/ParaView output,
- restart/checkpoint support.

These details should be introduced only after their interfaces and numerical requirements are agreed upon.

---

## Technology stack

| Component | Role |
|---|---|
| **C++20** | Main application, numerical infrastructure, solver-side data structures |
| **C** | Required by p4est and some low-level dependencies |
| **CMake >= 3.20** | Project configuration and build system |
| **p4est 2.8.7** | 2D adaptive quadtree forest, balancing, partitioning, ghost information |
| **libsc** | p4est support library; obtained recursively through the p4est submodule |
| **MPI** | Distributed-memory parallelism |
| **OpenCL** | GPU/accelerator compute interface |
| **CTest** | Test registration infrastructure |

The project currently pins p4est to the commit corresponding to **p4est v2.8.7**.

---

## Planned execution model

The intended high-level parallel model is:

```text
                         Flow_DG_AMR
                              |
              +---------------+---------------+
              |                               |
           MPI ranks                       OpenCL
              |                               |
      distributed subdomains          accelerator kernels
              |                               |
            p4est                    element/face operations
              |                               |
   AMR / balance / partition          local numerical work
              +---------------+---------------+
                              |
                         DG flow solver
```

The expected division of responsibility is:

### p4est / MPI side

p4est will primarily manage:

- adaptive quadtree topology,
- refinement and coarsening,
- 2:1 balancing when required,
- distributed partitioning,
- quadrant ownership,
- ghost quadrants,
- inter-rank mesh communication metadata.

### Solver-side C++ layer

The project will build its own solver-facing representation for information such as:

- elements,
- element coordinates,
- polynomial order,
- local degrees of freedom,
- element neighbors,
- face connectivity,
- physical boundary identifiers,
- refinement levels,
- local/ghost ownership,
- DG interface information.

The precise representation has not yet been finalized.

### OpenCL side

OpenCL is intended for highly parallel numerical kernels such as:

- evaluation at quadrature points,
- local volume operators,
- local face operators,
- numerical flux calculations,
- residual evaluation,
- vector updates,
- potentially matrix-free linear algebra.

The host-side mesh topology is expected to remain CPU-managed while suitable numerical work is offloaded to OpenCL devices.

---

## Repository layout

The current repository structure is:

```text
Flow_DG_AMR/
├── .gitignore
├── .gitmodules
├── AGENT.md
├── ARCH.md
├── CMakeLists.txt
├── LICENSE
├── README.md
│
├── include/
│   └── flow_dg_amr/
│       └── version.hpp
│
├── lib/
│   └── p4est/                 # Git submodule
│
├── src/
│   └── main.cpp
│
└── tests/
    └── CMakeLists.txt
```

As implementation grows, the source tree will be reorganized according to the architecture defined in `ARCH.md`.

---

## Requirements

A development machine currently needs:

- Git,
- CMake 3.20 or newer,
- a C++20-capable compiler,
- a C compiler,
- MPI development libraries,
- an OpenCL ICD loader and development headers,
- at least one OpenCL runtime/device if GPU or accelerator execution is required.

Typical compilers include GCC and Clang.

The current build configuration requires both MPI and OpenCL at CMake configure time.

---

## Ubuntu setup

For a typical Ubuntu development environment, install the basic build dependencies with:

```bash
sudo apt update

sudo apt install -y \
    build-essential \
    cmake \
    git \
    openmpi-bin \
    libopenmpi-dev \
    ocl-icd-opencl-dev \
    opencl-headers \
    clinfo
```

This installs the OpenCL loader and headers, but an actual OpenCL device runtime must also be supplied by the CPU/GPU vendor.

You can check whether OpenCL devices are visible with:

```bash
clinfo
```

You can check the MPI installation with:

```bash
mpirun --version
mpicxx --version
```

And the compiler/CMake versions with:

```bash
cmake --version
c++ --version
```

---

## Clone the repository

Because p4est itself contains the libsc dependency as a submodule, cloning **recursively** is strongly recommended.

```bash
git clone --recurse-submodules https://github.com/ali-kzm/Flow_DG_AMR.git
cd Flow_DG_AMR
```

If the project has already been cloned without submodules, initialize them with:

```bash
git submodule update --init --recursive
```

After initialization, the following should exist:

```text
lib/p4est/CMakeLists.txt
lib/p4est/sc/
```

To inspect the checked-out submodule revisions:

```bash
git submodule status --recursive
```

---

## Build

### Standard build

From the repository root:

```bash
cmake -S . -B build
cmake --build build -j
```

The executable is produced at:

```text
build/flow_dg_amr
```

### Release build

For optimized code:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

### Debug build

For debugging:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j
```

### Clean rebuild

When changing toolchains, MPI implementations, OpenCL installations, or dependency configuration, a clean configure is often safest:

```bash
rm -rf build
cmake -S . -B build
cmake --build build -j
```

---

## Run

Run the current environment/smoke-test executable directly:

```bash
./build/flow_dg_amr
```

A successful run should report information similar to:

```text
Flow_DG_AMR project skeleton
MPI ranks: 1
p4est version: 2.8.7
OpenCL platforms: 1
```

The exact OpenCL platform count depends on the machine.

At this stage, this executable does **not** solve a flow problem. It verifies that the executable can use the linked MPI, p4est, and OpenCL infrastructure.

---

## MPI usage

Run with multiple MPI ranks using:

```bash
mpirun -np 4 ./build/flow_dg_amr
```

The current program prints only from MPI rank 0, so a successful four-rank launch should report:

```text
MPI ranks: 4
```

MPI will later be used together with p4est for distributed adaptive meshes and inter-rank data exchange.

### Quick MPI verification

You can verify that MPI launches multiple processes independently of this project with:

```bash
mpirun -np 4 hostname
```

Four output lines indicate that four MPI processes were launched.

---

## OpenCL verification

The current executable calls:

```text
clGetPlatformIDs(...)
```

to count available OpenCL platforms.

If it prints:

```text
OpenCL platforms: 0
```

then the project may have compiled successfully while no usable OpenCL platform/runtime is currently exposed to the application.

Use:

```bash
clinfo
```

to inspect the installed platforms and devices.

Depending on the machine, the actual OpenCL runtime is provided separately by the CPU or GPU vendor.

The project does not currently select a platform/device or launch any OpenCL kernels.

---

## p4est integration

p4est is kept under:

```text
lib/p4est
```

as a Git submodule rather than copying its source into the project.

This keeps the upstream history and license separate and makes the exact dependency revision reproducible.

### Current p4est configuration

The root `CMakeLists.txt` currently configures p4est with:

```cmake
set(SC_ENABLE_MPI ON CACHE BOOL "Enable MPI support in libsc/p4est" FORCE)
set(P4EST_BUILD_TESTING OFF CACHE BOOL "Disable p4est tests" FORCE)
set(P4EST_BUILD_EXAMPLES OFF CACHE BOOL "Disable p4est examples" FORCE)
set(enable_p6est OFF CACHE BOOL "Disable p6est" FORCE)
set(enable_p8est OFF CACHE BOOL "Disable p8est" FORCE)
```

The p4est target is then added with:

```cmake
add_subdirectory(lib/p4est EXCLUDE_FROM_ALL)
```

and linked through:

```cmake
P4EST::P4EST
```

### Why p8est is disabled

The current project target is a two-dimensional Cartesian/quadtree mesh, so only p4est is required.

If a future 3D solver is introduced, the p8est configuration can be revisited separately.

### Updating p4est

Do not update the submodule casually because an upstream change can alter APIs or build behavior.

When an update is intentionally required:

```bash
cd lib/p4est
git fetch --tags
git checkout <desired-tag-or-commit>
git submodule update --init --recursive
cd ../..
git add lib/p4est
git commit
```

The parent repository records the exact p4est commit.

---

## CMake configuration

The current project requires CMake **3.20 or newer**.

The build searches for:

```cmake
find_package(MPI REQUIRED COMPONENTS C CXX)
find_package(OpenCL REQUIRED)
```

The application target links:

```cmake
MPI::MPI_CXX
OpenCL::OpenCL
P4EST::P4EST
```

### Project options

Currently available:

| Option | Default | Description |
|---|---:|---|
| `FLOW_DG_AMR_BUILD_TESTS` | `ON` | Enables the project test directory |

Disable the project test infrastructure with:

```bash
cmake -S . -B build -DFLOW_DG_AMR_BUILD_TESTS=OFF
```

### Compile commands

The project enables:

```cmake
CMAKE_EXPORT_COMPILE_COMMANDS=ON
```

so CMake generates:

```text
build/compile_commands.json
```

This is useful for clangd, IDE integration, static analysis, and other development tools.

---

## Testing

CTest infrastructure is enabled when:

```text
FLOW_DG_AMR_BUILD_TESTS=ON
```

After building, tests can be invoked with:

```bash
ctest --test-dir build --output-on-failure
```

At present, no numerical or unit-test targets have been added yet.

As implementation develops, tests should be introduced for isolated infrastructure and mathematical components before relying on full solver cases.

Expected future test areas include:

- mesh extraction,
- p4est connectivity conversion,
- face orientation,
- boundary detection,
- hanging interfaces,
- basis evaluation,
- quadrature,
- element operators,
- numerical fluxes,
- MPI ghost exchange,
- AMR transfer,
- OpenCL CPU/GPU consistency,
- conservation checks,
- manufactured-solution convergence.

---

## Development workflow

The repository is intended to evolve incrementally.

Before introducing a major subsystem:

1. define its responsibility,
2. define ownership of its data,
3. define its interface to neighboring subsystems,
4. record the approved architecture in `ARCH.md`,
5. add implementation in small reviewable changes,
6. add tests before depending on the new behavior elsewhere.

### Dependency boundaries

A useful design rule for the project is:

```text
p4est topology
     |
     v
mesh adapter / extraction
     |
     v
solver mesh + DG data
     |
     +--------> CPU/MPI operations
     |
     +--------> OpenCL buffers/kernels
```

Solver code should avoid spreading raw p4est internals throughout the codebase.

Likewise, OpenCL-specific details should remain isolated from high-level flow-physics definitions whenever practical.

---

## Planned architecture

The detailed architecture will live in [ARCH.md](ARCH.md). The current conceptual decomposition is only a direction, not a finalized API.

Potential major subsystems include:

### Mesh infrastructure

Responsibilities may include:

- ownership of the p4est forest,
- creation of rectangular connectivity,
- refinement/coarsening,
- balancing,
- repartitioning,
- local quadrant traversal,
- ghost construction,
- physical coordinate mapping,
- conversion into solver-side element data.

### DG discretization

Responsibilities may include:

- reference elements,
- basis functions,
- quadrature,
- physical/reference mapping,
- volume integration,
- surface integration,
- numerical fluxes,
- element-local operators.

### Field storage

Responsibilities may include:

- velocity,
- pressure,
- auxiliary variables,
- element-local degrees of freedom,
- local/ghost values,
- AMR transfer.

### Flow physics

Responsibilities may include:

- incompressible momentum equations,
- continuity/incompressibility treatment,
- material properties,
- source terms,
- boundary conditions,
- eventually multiphase-flow extensions.

### Parallel communication

Responsibilities may include:

- MPI ownership,
- ghost exchange,
- synchronization across partition boundaries,
- repartition data migration.

### OpenCL backend

Responsibilities may include:

- platform/device discovery,
- context and queue management,
- memory buffers,
- kernel compilation,
- kernel dispatch,
- synchronization,
- performance instrumentation.

### I/O

Responsibilities may eventually include:

- VTK output,
- ParaView-compatible adaptive-mesh visualization,
- diagnostics,
- checkpoints/restarts,
- performance logs.

No class names or exact interfaces in this section should be treated as final until they are defined in `ARCH.md`.

---

## Roadmap

The project is expected to develop approximately through the following technical stages, although the order can change as design decisions are made.

### Foundation

- [x] Repository structure
- [x] CMake project
- [x] C++20 configuration
- [x] MPI integration
- [x] OpenCL linkage
- [x] p4est submodule
- [x] p4est MPI-enabled build
- [x] Basic dependency smoke test

### Mesh layer

- [ ] Define solver-facing 2D mesh representation
- [ ] Create rectangular p4est forest
- [ ] Extract local elements
- [ ] Generate physical element coordinates
- [ ] Build face/neighborhood connectivity
- [ ] Identify physical boundaries
- [ ] Support ghost elements
- [ ] Represent coarse/fine DG interfaces
- [ ] Implement refinement/coarsening interface
- [ ] Implement mesh repartition/update path

### DG layer

- [ ] Define element polynomial representation
- [ ] Define basis functions
- [ ] Define quadrature
- [ ] Implement volume operators
- [ ] Implement surface operators
- [ ] Implement numerical flux infrastructure
- [ ] Validate convergence on simple scalar problems

### Flow solver

- [ ] Define incompressible-flow formulation
- [ ] Implement velocity/pressure storage
- [ ] Implement boundary conditions
- [ ] Implement time integration
- [ ] Implement pressure/velocity coupling or selected DG formulation
- [ ] Add verification problems

### OpenCL backend

- [ ] Device discovery/selection
- [ ] Context and command queue
- [ ] Buffer management
- [ ] Kernel build/cache system
- [ ] Element kernels
- [ ] Face kernels
- [ ] CPU/OpenCL consistency tests
- [ ] Performance benchmarking

### Adaptive solver

- [ ] Error/refinement indicators
- [ ] Field transfer during AMR
- [ ] Distributed repartition
- [ ] Conservation validation across refinement
- [ ] End-to-end adaptive flow examples

---

## Troubleshooting

### CMake reports that p4est is missing

Typical error:

```text
p4est submodule is missing
```

Fix:

```bash
git submodule update --init --recursive
```

Then reconfigure:

```bash
rm -rf build
cmake -S . -B build
```

---

### libsc is missing inside p4est

p4est uses libsc as its own submodule.

Run:

```bash
git submodule update --init --recursive
```

The `--recursive` part is important.

---

### CMake cannot find MPI

Verify:

```bash
which mpicc
which mpicxx
mpirun --version
```

On Ubuntu with Open MPI:

```bash
sudo apt install openmpi-bin libopenmpi-dev
```

Then remove the previous CMake cache and configure again:

```bash
rm -rf build
cmake -S . -B build
```

---

### CMake cannot find OpenCL

On Ubuntu, install the development loader and headers:

```bash
sudo apt install ocl-icd-opencl-dev opencl-headers
```

Then verify configuration again.

Remember that development headers/loaders are different from a vendor OpenCL runtime.

---

### The program reports zero OpenCL platforms

Run:

```bash
clinfo
```

If `clinfo` also sees no platform, install or repair the appropriate OpenCL runtime for the machine's CPU/GPU.

---

### MPI runs only one rank

Do not launch the executable directly when testing multiple ranks.

Use:

```bash
mpirun -np 4 ./build/flow_dg_amr
```

The current executable reports the communicator size from rank 0.

---

### Build behavior looks inconsistent after dependency changes

Delete the build directory:

```bash
rm -rf build
```

and configure from scratch:

```bash
cmake -S . -B build
cmake --build build -j
```

This avoids stale CMake cache values.

---

## Documentation

The repository uses three main top-level documentation files.

### `README.md`

User/developer entry point covering:

- purpose,
- dependencies,
- setup,
- building,
- running,
- project status,
- high-level development direction.

### `ARCH.md`

Architecture source of truth.

It is currently a placeholder and will eventually describe the approved:

- subsystem boundaries,
- data structures,
- ownership rules,
- class/interfaces,
- mesh-to-solver translation,
- MPI model,
- OpenCL model,
- DG organization.

### `AGENT.md`

Development rules for coding agents working in this repository.

It is currently a placeholder and will be replaced with the final project-specific instructions.

---

## License

Copyright © 2026 Ali Kzm.

The **Flow_DG_AMR project source is proprietary and all rights are reserved**.

No permission is granted to use, copy, modify, distribute, sublicense, publish, sell, disclose, reverse engineer, or create derivative works from the project except where prior written permission has been granted by the copyright holder.

See [LICENSE](LICENSE) for the complete terms.

### Third-party software

Third-party dependencies retain their own licenses.

In particular, p4est and its dependencies are separate upstream projects and are not relicensed by this repository's proprietary license. Their respective upstream license terms continue to apply to those components.

---

## Project note

Flow_DG_AMR is a research/development codebase. Numerical interfaces and internal architecture may change substantially while the solver is being designed.

Major numerical or structural decisions should therefore be documented before implementation rather than inferred from temporary code.
