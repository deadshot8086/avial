#map = affine_map<()[s0] -> (s0 - 1)>
module {
  func.func @stencil(%arg0: i32, %arg1: i32, %arg2: i32, %arg3: i32, %arg4: memref<?x?x?xf32>, %arg5: memref<?x?x?xf32>, %arg6: f32, %arg7: f32) attributes {llvm.linkage = #llvm.linkage<external>} {
    %c2_i32 = arith.constant 2 : i32
    %c1_i32 = arith.constant 1 : i32
    %0 = arith.index_cast %arg2 : i32 to index
    %1 = arith.index_cast %arg1 : i32 to index
    %2 = arith.index_cast %arg0 : i32 to index
    %3 = arith.divsi %arg3, %c2_i32 : i32
    %4 = arith.index_cast %3 : i32 to index
    affine.for %arg8 = 0 to %4 {
      affine.for %arg9 = 1 to #map()[%0] {
        affine.for %arg10 = 1 to #map()[%1] {
          affine.for %arg11 = 1 to #map()[%2] {
            %7 = affine.load %arg4[%arg9 + 1, %arg10, %arg11] : memref<?x?x?xf32>
            %8 = affine.load %arg4[%arg9 - 1, %arg10, %arg11] : memref<?x?x?xf32>
            %9 = arith.addf %7, %8 : f32
            %10 = affine.load %arg4[%arg9, %arg10 + 1, %arg11] : memref<?x?x?xf32>
            %11 = arith.addf %9, %10 : f32
            %12 = affine.load %arg4[%arg9, %arg10 - 1, %arg11] : memref<?x?x?xf32>
            %13 = arith.addf %11, %12 : f32
            %14 = affine.load %arg4[%arg9, %arg10, %arg11 + 1] : memref<?x?x?xf32>
            %15 = arith.addf %13, %14 : f32
            %16 = affine.load %arg4[%arg9, %arg10, %arg11 - 1] : memref<?x?x?xf32>
            %17 = arith.addf %15, %16 : f32
            %18 = arith.mulf %17, %arg7 : f32
            %19 = affine.load %arg4[%arg9, %arg10, %arg11] : memref<?x?x?xf32>
            %20 = arith.mulf %19, %arg6 : f32
            %21 = arith.subf %18, %20 : f32
            affine.store %21, %arg5[%arg9, %arg10, %arg11] : memref<?x?x?xf32>
          }
        }
      }
      affine.for %arg9 = 1 to #map()[%0] {
        affine.for %arg10 = 1 to #map()[%1] {
          affine.for %arg11 = 1 to #map()[%2] {
            %7 = affine.load %arg5[%arg9 + 1, %arg10, %arg11] : memref<?x?x?xf32>
            %8 = affine.load %arg5[%arg9 - 1, %arg10, %arg11] : memref<?x?x?xf32>
            %9 = arith.addf %7, %8 : f32
            %10 = affine.load %arg5[%arg9, %arg10 + 1, %arg11] : memref<?x?x?xf32>
            %11 = arith.addf %9, %10 : f32
            %12 = affine.load %arg5[%arg9, %arg10 - 1, %arg11] : memref<?x?x?xf32>
            %13 = arith.addf %11, %12 : f32
            %14 = affine.load %arg5[%arg9, %arg10, %arg11 + 1] : memref<?x?x?xf32>
            %15 = arith.addf %13, %14 : f32
            %16 = affine.load %arg5[%arg9, %arg10, %arg11 - 1] : memref<?x?x?xf32>
            %17 = arith.addf %15, %16 : f32
            %18 = arith.mulf %17, %arg7 : f32
            %19 = affine.load %arg5[%arg9, %arg10, %arg11] : memref<?x?x?xf32>
            %20 = arith.mulf %19, %arg6 : f32
            %21 = arith.subf %18, %20 : f32
            affine.store %21, %arg4[%arg9, %arg10, %arg11] : memref<?x?x?xf32>
          }
        }
      }
    }
    %5 = arith.remsi %arg3, %c2_i32 : i32
    %6 = arith.cmpi eq, %5, %c1_i32 : i32
    scf.if %6 {
      affine.for %arg8 = 1 to #map()[%0] {
        affine.for %arg9 = 1 to #map()[%1] {
          affine.for %arg10 = 1 to #map()[%2] {
            %7 = affine.load %arg4[%arg8 + 1, %arg9, %arg10] : memref<?x?x?xf32>
            %8 = affine.load %arg4[%arg8 - 1, %arg9, %arg10] : memref<?x?x?xf32>
            %9 = arith.addf %7, %8 : f32
            %10 = affine.load %arg4[%arg8, %arg9 + 1, %arg10] : memref<?x?x?xf32>
            %11 = arith.addf %9, %10 : f32
            %12 = affine.load %arg4[%arg8, %arg9 - 1, %arg10] : memref<?x?x?xf32>
            %13 = arith.addf %11, %12 : f32
            %14 = affine.load %arg4[%arg8, %arg9, %arg10 + 1] : memref<?x?x?xf32>
            %15 = arith.addf %13, %14 : f32
            %16 = affine.load %arg4[%arg8, %arg9, %arg10 - 1] : memref<?x?x?xf32>
            %17 = arith.addf %15, %16 : f32
            %18 = arith.mulf %17, %arg7 : f32
            %19 = affine.load %arg4[%arg8, %arg9, %arg10] : memref<?x?x?xf32>
            %20 = arith.mulf %19, %arg6 : f32
            %21 = arith.subf %18, %20 : f32
            affine.store %21, %arg5[%arg8, %arg9, %arg10] : memref<?x?x?xf32>
          }
        }
      }
    }
    return
  }
}
