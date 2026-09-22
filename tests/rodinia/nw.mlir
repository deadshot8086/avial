module {
  func.func @needleman_wunsch(%arg0: i32, %arg1: i32, %arg2: memref<?x?xi32>, %arg3: memref<?x?xi32>, %arg4: i32) attributes {llvm.linkage = #llvm.linkage<external>} {
    %0 = arith.index_cast %arg0 : i32 to index
    %1 = arith.index_cast %arg1 : i32 to index
    affine.for %arg5 = 1 to %0 {
      affine.for %arg6 = 1 to %1 {
        %2 = affine.load %arg2[%arg5 - 1, %arg6 - 1] : memref<?x?xi32>
        %3 = affine.load %arg3[%arg5, %arg6] : memref<?x?xi32>
        %4 = arith.addi %2, %3 : i32
        %5 = affine.load %arg2[%arg5, %arg6 - 1] : memref<?x?xi32>
        %6 = arith.subi %5, %arg4 : i32
        %7 = affine.load %arg2[%arg5 - 1, %arg6] : memref<?x?xi32>
        %8 = arith.subi %7, %arg4 : i32
        %9 = arith.cmpi sgt, %4, %6 : i32
        %10 = arith.select %9, %4, %6 : i32
        %11 = arith.cmpi sgt, %10, %8 : i32
        %12 = arith.select %11, %10, %8 : i32
        affine.store %12, %arg2[%arg5, %arg6] : memref<?x?xi32>
      }
    }
    return
  }
}
