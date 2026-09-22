module {
  func.func @sgemm(%arg0: i32, %arg1: i32, %arg2: i32, %arg3: f32, %arg4: f32, %arg5: memref<?x?xf32>, %arg6: memref<?x?xf32>, %arg7: memref<?x?xf32>) attributes {llvm.linkage = #llvm.linkage<external>} {
    %cst = arith.constant 0.000000e+00 : f32
    %0 = arith.index_cast %arg0 : i32 to index
    %1 = arith.index_cast %arg1 : i32 to index
    %2 = arith.index_cast %arg2 : i32 to index
    affine.for %arg8 = 0 to %0 {
      affine.for %arg9 = 0 to %1 {
        %3 = affine.for %arg10 = 0 to %2 iter_args(%arg11 = %cst) -> (f32) {
          %8 = affine.load %arg5[%arg10, %arg8] : memref<?x?xf32>
          %9 = affine.load %arg6[%arg10, %arg9] : memref<?x?xf32>
          %10 = arith.mulf %8, %9 : f32
          %11 = arith.addf %arg11, %10 : f32
          affine.yield %11 : f32
        }
        %4 = affine.load %arg7[%arg9, %arg8] : memref<?x?xf32>
        %5 = arith.mulf %4, %arg4 : f32
        %6 = arith.mulf %arg3, %3 : f32
        %7 = arith.addf %5, %6 : f32
        affine.store %7, %arg7[%arg9, %arg8] : memref<?x?xf32>
      }
    }
    return
  }
}
