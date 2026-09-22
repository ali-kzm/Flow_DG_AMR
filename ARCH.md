# Architecture

> Placeholder: detailed project architecture will be provided later.

## Current foundation

The project is intended to combine:

- p4est for adaptive Cartesian/quadtree mesh management
- MPI for distributed-memory mesh/domain parallelism
- Discontinuous finite element methods for the flow discretization
- OpenCL for accelerator-oriented compute kernels

No solver, element, field, assembly, time-integration, or physics class architecture is fixed yet.
