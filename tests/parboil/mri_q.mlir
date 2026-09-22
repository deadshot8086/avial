module {
  func.func @mri_q(%arg0: i32, %arg1: i32, %arg2: memref<?x4xf32>, %arg3: memref<?xf32>, %arg4: memref<?xf32>, %arg5: memref<?xf32>, %arg6: memref<?xf32>, %arg7: memref<?xf32>, %arg8: memref<?xf32>, %arg9: memref<?xf32>) attributes {llvm.linkage = #llvm.linkage<external>} {
    %cst = arith.constant 6.28318548 : f32
    %0 = arith.index_cast %arg0 : i32 to index
    affine.for %arg10 = 0 to %0 {
      %2 = affine.load %arg3[%arg10] : memref<?xf32>
      %3 = affine.load %arg4[%arg10] : memref<?xf32>
      %4 = arith.mulf %2, %2 : f32
      %5 = arith.mulf %3, %3 : f32
      %6 = arith.addf %4, %5 : f32
      affine.store %6, %arg2[%arg10, 3] : memref<?x4xf32>
    }
    %1 = arith.index_cast %arg1 : i32 to index
    affine.for %arg10 = 0 to %0 {
      affine.for %arg11 = 0 to %1 {
        %2 = affine.load %arg2[%arg10, 0] : memref<?x4xf32>
        %3 = affine.load %arg5[%arg11] : memref<?xf32>
        %4 = arith.mulf %2, %3 : f32
        %5 = affine.load %arg2[%arg10, 1] : memref<?x4xf32>
        %6 = affine.load %arg6[%arg11] : memref<?xf32>
        %7 = arith.mulf %5, %6 : f32
        %8 = arith.addf %4, %7 : f32
        %9 = affine.load %arg2[%arg10, 2] : memref<?x4xf32>
        %10 = affine.load %arg7[%arg11] : memref<?xf32>
        %11 = arith.mulf %9, %10 : f32
        %12 = arith.addf %8, %11 : f32
        %13 = arith.mulf %12, %cst : f32
        %14 = arith.extf %13 : f32 to f64
        %15 = math.cos %14 : f64
        %16 = arith.truncf %15 : f64 to f32
        %17 = math.sin %14 : f64
        %18 = arith.truncf %17 : f64 to f32
        %19 = affine.load %arg2[%arg10, 3] : memref<?x4xf32>
        %20 = arith.mulf %19, %16 : f32
        %21 = affine.load %arg8[%arg11] : memref<?xf32>
        %22 = arith.addf %21, %20 : f32
        affine.store %22, %arg8[%arg11] : memref<?xf32>
        %23 = arith.mulf %19, %18 : f32
        %24 = affine.load %arg9[%arg11] : memref<?xf32>
        %25 = arith.addf %24, %23 : f32
        affine.store %25, %arg9[%arg11] : memref<?xf32>
      }
    }
    return
  }
}
