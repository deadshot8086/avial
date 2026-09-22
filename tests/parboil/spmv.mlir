module {
  func.func @spmv(%arg0: i32, %arg1: i32, %arg2: memref<?xi32>, %arg3: memref<?xi32>, %arg4: memref<?xi32>, %arg5: memref<?xf32>, %arg6: memref<?xf32>, %arg7: memref<?xi32>, %arg8: memref<?xf32>) attributes {llvm.linkage = #llvm.linkage<external>} {
    %c0 = arith.constant 0 : index
    %c1 = arith.constant 1 : index
    %cst = arith.constant 0.000000e+00 : f32
    %0 = arith.index_cast %arg1 : i32 to index
    %1 = arith.index_cast %arg0 : i32 to index
    affine.for %arg9 = 0 to %0 {
      affine.for %arg10 = 0 to %1 {
        %2 = arith.index_cast %arg10 : index to i32
        %3 = affine.load %arg2[%arg10] : memref<?xi32>
        %4 = arith.index_cast %3 : i32 to index
        %5 = scf.for %arg11 = %c0 to %4 step %c1 iter_args(%arg12 = %cst) -> (f32) {
          %8 = memref.load %arg3[%arg11] : memref<?xi32>
          %9 = arith.addi %8, %2 : i32
          %10 = arith.index_cast %9 : i32 to index
          %11 = memref.load %arg4[%10] : memref<?xi32>
          %12 = memref.load %arg5[%10] : memref<?xf32>
          %13 = arith.index_cast %11 : i32 to index
          %14 = memref.load %arg6[%13] : memref<?xf32>
          %15 = arith.mulf %12, %14 : f32
          %16 = arith.addf %arg12, %15 : f32
          scf.yield %16 : f32
        }
        %6 = affine.load %arg7[%arg10] : memref<?xi32>
        %7 = arith.index_cast %6 : i32 to index
        memref.store %5, %arg8[%7] : memref<?xf32>
      }
    }
    return
  }
}
