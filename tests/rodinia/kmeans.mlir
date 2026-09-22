module {
  func.func @kmeans(%arg0: i32, %arg1: i32, %arg2: i32, %arg3: f32, %arg4: memref<?x?xf32>, %arg5: memref<?x?xf32>, %arg6: memref<?xi32>, %arg7: memref<?x?xf32>, %arg8: memref<?xi32>) attributes {llvm.linkage = #llvm.linkage<external>} {
    %c0 = arith.constant 0 : index
    %c1 = arith.constant 1 : index
    %cst = arith.constant 1.000000e+00 : f32
    %cst_0 = arith.constant 1.000000e+30 : f32
    %c1_i32 = arith.constant 1 : i32
    %c0_i32 = arith.constant 0 : i32
    %cst_1 = arith.constant 0.000000e+00 : f32
    %0 = llvm.mlir.undef : i32
    %1 = arith.index_cast %arg0 : i32 to index
    %2 = arith.index_cast %arg2 : i32 to index
    %3 = arith.index_cast %arg1 : i32 to index
    %4 = arith.index_cast %arg1 : i32 to index
    %5 = arith.index_cast %arg1 : i32 to index
    %6 = arith.index_cast %arg1 : i32 to index
    %7 = scf.while (%arg9 = %0) : (i32) -> i32 {
      scf.for %arg10 = %c0 to %2 step %c1 {
        memref.store %c0_i32, %arg8[%arg10] : memref<?xi32>
        scf.for %arg11 = %c0 to %3 step %c1 {
          memref.store %cst_1, %arg7[%arg10, %arg11] : memref<?x?xf32>
        }
      }
      %8 = scf.for %arg10 = %c0 to %1 step %c1 iter_args(%arg11 = %cst_1) -> (f32) {
        %11:2 = scf.for %arg12 = %c0 to %2 step %c1 iter_args(%arg13 = %cst_0, %arg14 = %c0_i32) -> (f32, i32) {
          %18 = arith.index_cast %arg12 : index to i32
          %19 = scf.for %arg15 = %c0 to %4 step %c1 iter_args(%arg16 = %cst_1) -> (f32) {
            %23 = memref.load %arg4[%arg10, %arg15] : memref<?x?xf32>
            %24 = memref.load %arg5[%arg12, %arg15] : memref<?x?xf32>
            %25 = arith.subf %23, %24 : f32
            %26 = arith.mulf %25, %25 : f32
            %27 = arith.addf %arg16, %26 : f32
            scf.yield %27 : f32
          }
          %20 = arith.cmpf olt, %19, %arg13 : f32
          %21 = arith.select %20, %19, %arg13 : f32
          %22 = arith.select %20, %18, %arg14 : i32
          scf.yield %21, %22 : f32, i32
        }
        %12 = memref.load %arg6[%arg10] : memref<?xi32>
        %13 = arith.cmpi ne, %12, %11#1 : i32
        %14 = scf.if %13 -> (f32) {
          %18 = arith.addf %arg11, %cst : f32
          scf.yield %18 : f32
        } else {
          scf.yield %arg11 : f32
        }
        memref.store %11#1, %arg6[%arg10] : memref<?xi32>
        %15 = arith.index_cast %11#1 : i32 to index
        %16 = memref.load %arg8[%15] : memref<?xi32>
        %17 = arith.addi %16, %c1_i32 : i32
        memref.store %17, %arg8[%15] : memref<?xi32>
        scf.for %arg12 = %c0 to %5 step %c1 {
          %18 = memref.load %arg4[%arg10, %arg12] : memref<?x?xf32>
          %19 = memref.load %arg7[%15, %arg12] : memref<?x?xf32>
          %20 = arith.addf %19, %18 : f32
          memref.store %20, %arg7[%15, %arg12] : memref<?x?xf32>
        }
        scf.yield %14 : f32
      }
      %9 = scf.for %arg10 = %c0 to %2 step %c1 iter_args(%arg11 = %arg9) -> (i32) {
        %11 = memref.load %arg8[%arg10] : memref<?xi32>
        %12 = arith.cmpi sgt, %11, %c0_i32 : i32
        %13 = arith.select %12, %arg1, %arg11 : i32
        scf.if %12 {
          scf.for %arg12 = %c0 to %6 step %c1 {
            %14 = memref.load %arg7[%arg10, %arg12] : memref<?x?xf32>
            %15 = memref.load %arg8[%arg10] : memref<?xi32>
            %16 = arith.sitofp %15 : i32 to f32
            %17 = arith.divf %14, %16 : f32
            memref.store %17, %arg5[%arg10, %arg12] : memref<?x?xf32>
          }
        }
        scf.yield %13 : i32
      }
      %10 = arith.cmpf ogt, %8, %arg3 : f32
      scf.condition(%10) %9 : i32
    } do {
    ^bb0(%arg9: i32):
      scf.yield %arg9 : i32
    }
    return
  }
}
