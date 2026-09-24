module  {
  func.func @matadd(%arg0: i32, %arg1: i32, %arg2: memref<?x4096xf32>, %arg3: memref<?x4096xf32>, %arg4: memref<?x4096xf32>) {
    %0 = arith.index_cast %arg0 : i32 to index
    %1 = arith.index_cast %arg1 : i32 to index
    
    affine.for %arg5 = 0 to 4096 {
      affine.for %arg6 = 0 to 4096 {
        %2 = affine.load %arg2[%arg5, %arg6] : memref<?x4096xf32>
        %3 = affine.load %arg3[%arg5, %arg6] : memref<?x4096xf32>
        %4 = arith.addf %2, %3 : f32
        affine.store %4, %arg4[%arg5, %arg6] : memref<?x4096xf32>
      }
    }
    return
  }
}