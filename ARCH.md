# Architecture

> Placeholder: detailed project architecture will be provided later.

## Current foundation

The project is intended to combine:

- p4est for adaptive Cartesian/quadtree mesh management
- MPI for distributed-memory mesh/domain parallelism
- Discontinuous finite element methods for the flow discretization
- OpenCL for accelerator-oriented compute kernels

No solver, element, field, assembly, time-integration, or physics class architecture is fixed yet.


## Mesh2D wrapper

`flow_dg_amr::Mesh2D` is the initial solver-side ownership wrapper around p4est.

Current responsibilities:

- own a `p4est_connectivity_t` for a unit-square domain;
- own the associated `p4est_t` forest;
- create the forest on a supplied MPI communicator;
- provide RAII destruction;
- expose basic rank/size and local/global quadrant counts;
- provide direct p4est handle access for infrastructure code that still needs it.

The wrapper intentionally does not yet define DG elements, Q1/Q2 data, face connectivity,
boundary-condition semantics, hanging-interface constraints, or solver field storage.
Those will be added as separate solver-facing abstractions once their interfaces are approved.
