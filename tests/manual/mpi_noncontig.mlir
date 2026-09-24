// @noncontig takes a column slice: strides [8,1] over dims [8,2], so the outer
// stride (8) exceeds the inner extent (2) and the rows are not adjacent.
// --lower-to-llvm must describe it with an MPI derived datatype.
//
// @contig takes a row slice of the same array: strides [8,1] over dims [2,8],
// which is contiguous, and must lower without complaint.
module attributes {dlti.dl_spec = #dlti.dl_spec<"MPI:Implementation" = "OpenMPI">} {
  func.func @noncontig(%buf: memref<8x8xf32>) {
    %tag = arith.constant 0 : i32
    %dst = arith.constant 1 : i32
    %comm = mpi.comm_world : !mpi.comm
    %sv = memref.subview %buf[0, 0] [8, 2] [1, 1]
        : memref<8x8xf32> to memref<8x2xf32, strided<[8, 1]>>
    %r = mpi.send(%sv, %tag, %dst, %comm)
        : memref<8x2xf32, strided<[8, 1]>>, i32, i32 -> !mpi.retval
    return
  }
}
