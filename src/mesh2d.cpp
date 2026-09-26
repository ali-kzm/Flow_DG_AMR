#include "flow_dg_amr/mesh2d.hpp"

#include <stdexcept>
#include <utility>

namespace flow_dg_amr {

Mesh2D::Mesh2D(MPI_Comm communicator, int initial_level)
    : communicator_(communicator)
{
    if (communicator_ == MPI_COMM_NULL) {
        throw std::invalid_argument("Mesh2D requires a valid MPI communicator");
    }

    if (initial_level < 0 || initial_level > P4EST_QMAXLEVEL) {
        throw std::out_of_range("Mesh2D initial_level is outside the p4est range");
    }

    connectivity_ = p4est_connectivity_new_unitsquare();
    if (connectivity_ == nullptr) {
        throw std::runtime_error("Failed to create p4est unit-square connectivity");
    }

    forest_ = p4est_new_ext(
        communicator_,
        connectivity_,
        0,
        initial_level,
        1,
        0,
        nullptr,
        nullptr);

    if (forest_ == nullptr) {
        p4est_connectivity_destroy(connectivity_);
        connectivity_ = nullptr;
        throw std::runtime_error("Failed to create p4est forest");
    }
}

Mesh2D::~Mesh2D()
{
    reset();
}

Mesh2D::Mesh2D(Mesh2D&& other) noexcept
    : communicator_(other.communicator_),
      connectivity_(std::exchange(other.connectivity_, nullptr)),
      forest_(std::exchange(other.forest_, nullptr))
{
    other.communicator_ = MPI_COMM_NULL;
}

Mesh2D& Mesh2D::operator=(Mesh2D&& other) noexcept
{
    if (this != &other) {
        reset();

        communicator_ = other.communicator_;
        connectivity_ = std::exchange(other.connectivity_, nullptr);
        forest_ = std::exchange(other.forest_, nullptr);
        other.communicator_ = MPI_COMM_NULL;
    }

    return *this;
}

p4est_t* Mesh2D::forest() noexcept
{
    return forest_;
}

const p4est_t* Mesh2D::forest() const noexcept
{
    return forest_;
}

p4est_connectivity_t* Mesh2D::connectivity() noexcept
{
    return connectivity_;
}

const p4est_connectivity_t* Mesh2D::connectivity() const noexcept
{
    return connectivity_;
}

MPI_Comm Mesh2D::communicator() const noexcept
{
    return communicator_;
}

int Mesh2D::mpi_rank() const noexcept
{
    return forest_ != nullptr ? forest_->mpirank : -1;
}

int Mesh2D::mpi_size() const noexcept
{
    return forest_ != nullptr ? forest_->mpisize : 0;
}

std::int64_t Mesh2D::local_quadrant_count() const noexcept
{
    return forest_ != nullptr
        ? static_cast<std::int64_t>(forest_->local_num_quadrants)
        : 0;
}

std::int64_t Mesh2D::global_quadrant_count() const noexcept
{
    return forest_ != nullptr
        ? static_cast<std::int64_t>(forest_->global_num_quadrants)
        : 0;
}

void Mesh2D::reset() noexcept
{
    if (forest_ != nullptr) {
        p4est_destroy(forest_);
        forest_ = nullptr;
    }

    if (connectivity_ != nullptr) {
        p4est_connectivity_destroy(connectivity_);
        connectivity_ = nullptr;
    }

    communicator_ = MPI_COMM_NULL;
}

} // namespace flow_dg_amr
