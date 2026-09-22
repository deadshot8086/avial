#map = affine_map<()[s0] -> (s0 - 1)>
#map1 = affine_map<(d0) -> (d0 + 1)>
#map2 = affine_map<(d0) -> (d0)>
module {
  func.func @gaussian(%arg0: i32, %arg1: memref<?x?xf32>, %arg2: memref<?x?xf32>, %arg3: memref<?xf32>, %arg4: memref<?xf32>) attributes {llvm.linkage = #llvm.linkage<external>} {
    %0 = arith.index_cast %arg0 : i32 to index
    affine.for %arg5 = 0 to #map()[%0] {
      affine.for %arg6 = #map1(%arg5) to %0 {
        %1 = affine.load %arg2[%arg6, %arg5] : memref<?x?xf32>
        %2 = affine.load %arg2[%arg5, %arg5] : memref<?x?xf32>
        %3 = arith.divf %1, %2 : f32
        affine.store %3, %arg1[%arg6, %arg5] : memref<?x?xf32>
      }
      affine.for %arg6 = #map1(%arg5) to %0 {
        affine.for %arg7 = #map2(%arg5) to %0 {
          %6 = affine.load %arg1[%arg6, %arg5] : memref<?x?xf32>
          %7 = affine.load %arg2[%arg5, %arg7] : memref<?x?xf32>
          %8 = arith.mulf %6, %7 : f32
          %9 = affine.load %arg2[%arg6, %arg7] : memref<?x?xf32>
          %10 = arith.subf %9, %8 : f32
          affine.store %10, %arg2[%arg6, %arg7] : memref<?x?xf32>
        }
        %1 = affine.load %arg1[%arg6, %arg5] : memref<?x?xf32>
        %2 = affine.load %arg3[%arg5] : memref<?xf32>
        %3 = arith.mulf %1, %2 : f32
        %4 = affine.load %arg3[%arg6] : memref<?xf32>
        %5 = arith.subf %4, %3 : f32
        affine.store %5, %arg3[%arg6] : memref<?xf32>
      }
    }
    affine.for %arg5 = 0 to %0 {
      %1 = affine.load %arg3[-%arg5 + symbol(%0) - 1] : memref<?xf32>
      %2 = affine.for %arg6 = 0 to #map2(%arg5) iter_args(%arg7 = %1) -> (f32) {
        %5 = affine.load %arg2[-%arg5 + symbol(%0) - 1, -%arg6 + symbol(%0) - 1] : memref<?x?xf32>
        %6 = affine.load %arg4[-%arg6 + symbol(%0) - 1] : memref<?xf32>
        %7 = arith.mulf %5, %6 : f32
        %8 = arith.subf %arg7, %7 : f32
        affine.yield %8 : f32
      }
      %3 = affine.load %arg2[-%arg5 + symbol(%0) - 1, -%arg5 + symbol(%0) - 1] : memref<?x?xf32>
      %4 = arith.divf %2, %3 : f32
      affine.store %4, %arg4[-%arg5 + symbol(%0) - 1] : memref<?xf32>
    }
    return
  }
}
