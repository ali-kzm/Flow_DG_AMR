#include <CL/cl.h>
#include <mpi.h>
#include <p4est.h>

#include <iostream>

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    int rank = 0;
    int size = 1;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        cl_uint platform_count = 0;
        const cl_int opencl_status =
            clGetPlatformIDs(0, nullptr, &platform_count);

        std::cout << "Flow_DG_AMR project skeleton\n"
                  << "MPI ranks: " << size << "\n"
                  << "p4est version: " << P4EST_VERSION << "\n"
                  << "OpenCL platforms: "
                  << (opencl_status == CL_SUCCESS ? platform_count : 0)
                  << "\n";
    }

    MPI_Finalize();
    return 0;
}
