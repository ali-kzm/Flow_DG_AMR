#pragma once

#include <mpi.h>
#include <p4est.h>
#include <p4est_connectivity.h>

#include <cstdint>

namespace flow_dg_amr {

class Mesh2D {
public:
    explicit Mesh2D(MPI_Comm communicator = MPI_COMM_WORLD,
                    int initial_level = 0);

    ~Mesh2D();

    Mesh2D(const Mesh2D&) = delete;
    Mesh2D& operator=(const Mesh2D&) = delete;

    Mesh2D(Mesh2D&& other) noexcept;
    Mesh2D& operator=(Mesh2D&& other) noexcept;

    [[nodiscard]] p4est_t* forest() noexcept;
    [[nodiscard]] const p4est_t* forest() const noexcept;

    [[nodiscard]] p4est_connectivity_t* connectivity() noexcept;
    [[nodiscard]] const p4est_connectivity_t* connectivity() const noexcept;

    [[nodiscard]] MPI_Comm communicator() const noexcept;
    [[nodiscard]] int mpi_rank() const noexcept;
    [[nodiscard]] int mpi_size() const noexcept;

    [[nodiscard]] std::int64_t local_quadrant_count() const noexcept;
    [[nodiscard]] std::int64_t global_quadrant_count() const noexcept;

private:
    void reset() noexcept;

    MPI_Comm communicator_ = MPI_COMM_NULL;
    p4est_connectivity_t* connectivity_ = nullptr;
    p4est_t* forest_ = nullptr;
};

} // namespace flow_dg_amr
