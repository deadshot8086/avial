#map = affine_map<()[s0] -> (s0 + 2)>
#map1 = affine_map<()[s0] -> (s0 - 1)>
#map2 = affine_map<(d0) -> (d0 + 1)>
module {
  func.func @tpacf(%arg0: i32, %arg1: i32, %arg2: i32, %arg3: memref<?x3xf32>, %arg4: memref<?x?x3xf32>, %arg5: memref<?xf32>, %arg6: memref<?xi64>, %arg7: memref<?xi64>, %arg8: memref<?xi64>) attributes {llvm.linkage = #llvm.linkage<external>} {
    %c0_i32 = arith.constant 0 : i32
    %c2_i32 = arith.constant 2 : i32
    %c0_i64 = arith.constant 0 : i64
    %c1_i32 = arith.constant 1 : i32
    %c1_i64 = arith.constant 1 : i64
    %0 = arith.index_cast %arg2 : i32 to index
    %1 = arith.index_cast %arg0 : i32 to index
    affine.for %arg9 = 0 to #map()[%0] {
      affine.store %c0_i64, %arg6[%arg9] : memref<?xi64>
      affine.store %c0_i64, %arg7[%arg9] : memref<?xi64>
      affine.store %c0_i64, %arg8[%arg9] : memref<?xi64>
    }
    affine.for %arg9 = 0 to #map1()[%1] {
      affine.for %arg10 = #map2(%arg9) to %1 {
        %3 = affine.load %arg3[%arg9, 0] : memref<?x3xf32>
        %4 = affine.load %arg3[%arg10, 0] : memref<?x3xf32>
        %5 = arith.mulf %3, %4 : f32
        %6 = affine.load %arg3[%arg9, 1] : memref<?x3xf32>
        %7 = affine.load %arg3[%arg10, 1] : memref<?x3xf32>
        %8 = arith.mulf %6, %7 : f32
        %9 = arith.addf %5, %8 : f32
        %10 = affine.load %arg3[%arg9, 2] : memref<?x3xf32>
        %11 = affine.load %arg3[%arg10, 2] : memref<?x3xf32>
        %12 = arith.mulf %10, %11 : f32
        %13 = arith.addf %9, %12 : f32
        %14:2 = scf.while (%arg11 = %arg2, %arg12 = %c0_i32) : (i32, i32) -> (i32, i32) {
          %18 = arith.addi %arg12, %c1_i32 : i32
          %19 = arith.cmpi sgt, %arg11, %18 : i32
          scf.condition(%19) %arg11, %arg12 : i32, i32
        } do {
        ^bb0(%arg11: i32, %arg12: i32):
          %18 = arith.addi %arg12, %arg11 : i32
          %19 = arith.divsi %18, %c2_i32 : i32
          %20 = arith.index_cast %19 : i32 to index
          %21 = memref.load %arg5[%20] : memref<?xf32>
          %22 = arith.cmpf oge, %13, %21 : f32
          %23 = arith.select %22, %19, %arg11 : i32
          %24 = arith.select %22, %arg12, %19 : i32
          scf.yield %23, %24 : i32, i32
        }
        %15 = arith.index_cast %14#1 : i32 to index
        %16 = memref.load %arg5[%15] : memref<?xf32>
        %17 = arith.cmpf oge, %13, %16 : f32
        scf.if %17 {
          %18 = memref.load %arg6[%15] : memref<?xi64>
          %19 = arith.addi %18, %c1_i64 : i64
          memref.store %19, %arg6[%15] : memref<?xi64>
        } else {
          %18 = arith.index_cast %14#0 : i32 to index
          %19 = memref.load %arg5[%18] : memref<?xf32>
          %20 = arith.cmpf olt, %13, %19 : f32
          scf.if %20 {
            %21 = arith.addi %14#0, %c1_i32 : i32
            %22 = arith.index_cast %21 : i32 to index
            %23 = memref.load %arg6[%22] : memref<?xi64>
            %24 = arith.addi %23, %c1_i64 : i64
            memref.store %24, %arg6[%22] : memref<?xi64>
          } else {
            %21 = memref.load %arg6[%18] : memref<?xi64>
            %22 = arith.addi %21, %c1_i64 : i64
            memref.store %22, %arg6[%18] : memref<?xi64>
          }
        }
      }
    }
    %2 = arith.index_cast %arg1 : i32 to index
    affine.for %arg9 = 0 to %2 {
      affine.for %arg10 = 0 to #map1()[%1] {
        affine.for %arg11 = #map2(%arg10) to %1 {
          %3 = affine.load %arg4[%arg9, %arg10, 0] : memref<?x?x3xf32>
          %4 = affine.load %arg4[%arg9, %arg11, 0] : memref<?x?x3xf32>
          %5 = arith.mulf %3, %4 : f32
          %6 = affine.load %arg4[%arg9, %arg10, 1] : memref<?x?x3xf32>
          %7 = affine.load %arg4[%arg9, %arg11, 1] : memref<?x?x3xf32>
          %8 = arith.mulf %6, %7 : f32
          %9 = arith.addf %5, %8 : f32
          %10 = affine.load %arg4[%arg9, %arg10, 2] : memref<?x?x3xf32>
          %11 = affine.load %arg4[%arg9, %arg11, 2] : memref<?x?x3xf32>
          %12 = arith.mulf %10, %11 : f32
          %13 = arith.addf %9, %12 : f32
          %14:2 = scf.while (%arg12 = %arg2, %arg13 = %c0_i32) : (i32, i32) -> (i32, i32) {
            %18 = arith.addi %arg13, %c1_i32 : i32
            %19 = arith.cmpi sgt, %arg12, %18 : i32
            scf.condition(%19) %arg12, %arg13 : i32, i32
          } do {
          ^bb0(%arg12: i32, %arg13: i32):
            %18 = arith.addi %arg13, %arg12 : i32
            %19 = arith.divsi %18, %c2_i32 : i32
            %20 = arith.index_cast %19 : i32 to index
            %21 = memref.load %arg5[%20] : memref<?xf32>
            %22 = arith.cmpf oge, %13, %21 : f32
            %23 = arith.select %22, %19, %arg12 : i32
            %24 = arith.select %22, %arg13, %19 : i32
            scf.yield %23, %24 : i32, i32
          }
          %15 = arith.index_cast %14#1 : i32 to index
          %16 = memref.load %arg5[%15] : memref<?xf32>
          %17 = arith.cmpf oge, %13, %16 : f32
          scf.if %17 {
            %18 = memref.load %arg7[%15] : memref<?xi64>
            %19 = arith.addi %18, %c1_i64 : i64
            memref.store %19, %arg7[%15] : memref<?xi64>
          } else {
            %18 = arith.index_cast %14#0 : i32 to index
            %19 = memref.load %arg5[%18] : memref<?xf32>
            %20 = arith.cmpf olt, %13, %19 : f32
            scf.if %20 {
              %21 = arith.addi %14#0, %c1_i32 : i32
              %22 = arith.index_cast %21 : i32 to index
              %23 = memref.load %arg7[%22] : memref<?xi64>
              %24 = arith.addi %23, %c1_i64 : i64
              memref.store %24, %arg7[%22] : memref<?xi64>
            } else {
              %21 = memref.load %arg7[%18] : memref<?xi64>
              %22 = arith.addi %21, %c1_i64 : i64
              memref.store %22, %arg7[%18] : memref<?xi64>
            }
          }
        }
      }
      affine.for %arg10 = 0 to %1 {
        affine.for %arg11 = 0 to %1 {
          %3 = affine.load %arg3[%arg10, 0] : memref<?x3xf32>
          %4 = affine.load %arg4[%arg9, %arg11, 0] : memref<?x?x3xf32>
          %5 = arith.mulf %3, %4 : f32
          %6 = affine.load %arg3[%arg10, 1] : memref<?x3xf32>
          %7 = affine.load %arg4[%arg9, %arg11, 1] : memref<?x?x3xf32>
          %8 = arith.mulf %6, %7 : f32
          %9 = arith.addf %5, %8 : f32
          %10 = affine.load %arg3[%arg10, 2] : memref<?x3xf32>
          %11 = affine.load %arg4[%arg9, %arg11, 2] : memref<?x?x3xf32>
          %12 = arith.mulf %10, %11 : f32
          %13 = arith.addf %9, %12 : f32
          %14:2 = scf.while (%arg12 = %arg2, %arg13 = %c0_i32) : (i32, i32) -> (i32, i32) {
            %18 = arith.addi %arg13, %c1_i32 : i32
            %19 = arith.cmpi sgt, %arg12, %18 : i32
            scf.condition(%19) %arg12, %arg13 : i32, i32
          } do {
          ^bb0(%arg12: i32, %arg13: i32):
            %18 = arith.addi %arg13, %arg12 : i32
            %19 = arith.divsi %18, %c2_i32 : i32
            %20 = arith.index_cast %19 : i32 to index
            %21 = memref.load %arg5[%20] : memref<?xf32>
            %22 = arith.cmpf oge, %13, %21 : f32
            %23 = arith.select %22, %19, %arg12 : i32
            %24 = arith.select %22, %arg13, %19 : i32
            scf.yield %23, %24 : i32, i32
          }
          %15 = arith.index_cast %14#1 : i32 to index
          %16 = memref.load %arg5[%15] : memref<?xf32>
          %17 = arith.cmpf oge, %13, %16 : f32
          scf.if %17 {
            %18 = memref.load %arg8[%15] : memref<?xi64>
            %19 = arith.addi %18, %c1_i64 : i64
            memref.store %19, %arg8[%15] : memref<?xi64>
          } else {
            %18 = arith.index_cast %14#0 : i32 to index
            %19 = memref.load %arg5[%18] : memref<?xf32>
            %20 = arith.cmpf olt, %13, %19 : f32
            scf.if %20 {
              %21 = arith.addi %14#0, %c1_i32 : i32
              %22 = arith.index_cast %21 : i32 to index
              %23 = memref.load %arg8[%22] : memref<?xi64>
              %24 = arith.addi %23, %c1_i64 : i64
              memref.store %24, %arg8[%22] : memref<?xi64>
            } else {
              %21 = memref.load %arg8[%18] : memref<?xi64>
              %22 = arith.addi %21, %c1_i64 : i64
              memref.store %22, %arg8[%18] : memref<?xi64>
            }
          }
        }
      }
    }
    return
  }
}
