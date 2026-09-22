#map = affine_map<(d0) -> (d0)>
#map1 = affine_map<(d0) -> (d0 + 1)>
module {
  func.func @lud(%arg0: i32, %arg1: memref<?x?xf32>) attributes {llvm.linkage = #llvm.linkage<external>} {
    %0 = arith.index_cast %arg0 : i32 to index
    affine.for %arg2 = 0 to %0 {
      affine.for %arg3 = #map(%arg2) to %0 {
        %1 = affine.load %arg1[%arg2, %arg3] : memref<?x?xf32>
        %2 = affine.for %arg4 = 0 to #map(%arg2) iter_args(%arg5 = %1) -> (f32) {
          %3 = affine.load %arg1[%arg2, %arg4] : memref<?x?xf32>
          %4 = affine.load %arg1[%arg4, %arg3] : memref<?x?xf32>
          %5 = arith.mulf %3, %4 : f32
          %6 = arith.subf %arg5, %5 : f32
          affine.yield %6 : f32
        }
        affine.store %2, %arg1[%arg2, %arg3] : memref<?x?xf32>
      }
      affine.for %arg3 = #map1(%arg2) to %0 {
        %1 = affine.load %arg1[%arg3, %arg2] : memref<?x?xf32>
        %2 = affine.for %arg4 = 0 to #map(%arg2) iter_args(%arg5 = %1) -> (f32) {
          %5 = affine.load %arg1[%arg3, %arg4] : memref<?x?xf32>
          %6 = affine.load %arg1[%arg4, %arg2] : memref<?x?xf32>
          %7 = arith.mulf %5, %6 : f32
          %8 = arith.subf %arg5, %7 : f32
          affine.yield %8 : f32
        }
        %3 = affine.load %arg1[%arg2, %arg2] : memref<?x?xf32>
        %4 = arith.divf %2, %3 : f32
        affine.store %4, %arg1[%arg3, %arg2] : memref<?x?xf32>
      }
    }
    return
  }
}
