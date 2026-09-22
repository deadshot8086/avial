module {
  func.func @backprop(%arg0: i32, %arg1: i32, %arg2: i32, %arg3: memref<?xf32>, %arg4: memref<?xf32>, %arg5: memref<?xf32>, %arg6: memref<?xf32>, %arg7: memref<?xf32>, %arg8: memref<?xf32>, %arg9: memref<?x?xf32>, %arg10: memref<?x?xf32>, %arg11: memref<?x?xf32>, %arg12: memref<?x?xf32>) attributes {llvm.linkage = #llvm.linkage<external>} {
    %c1 = arith.constant 1 : index
    %c1_i32 = arith.constant 1 : i32
    %c0 = arith.constant 0 : index
    %cst = arith.constant 0.000000e+00 : f32
    %cst_0 = arith.constant 1.000000e+00 : f32
    %cst_1 = arith.constant 3.000000e-01 : f32
    affine.store %cst_0, %arg3[0] : memref<?xf32>
    %0 = arith.addi %arg1, %c1_i32 : i32
    %1 = arith.index_cast %0 : i32 to index
    %2 = arith.addi %arg0, %c1_i32 : i32
    %3 = arith.index_cast %2 : i32 to index
    scf.for %arg13 = %c1 to %1 step %c1 {
      %8 = scf.for %arg14 = %c0 to %3 step %c1 iter_args(%arg15 = %cst) -> (f32) {
        %13 = memref.load %arg9[%arg14, %arg13] : memref<?x?xf32>
        %14 = memref.load %arg3[%arg14] : memref<?xf32>
        %15 = arith.mulf %13, %14 : f32
        %16 = arith.addf %arg15, %15 : f32
        scf.yield %16 : f32
      }
      %9 = arith.negf %8 : f32
      %10 = math.exp %9 : f32
      %11 = arith.addf %10, %cst_0 : f32
      %12 = arith.divf %cst_0, %11 : f32
      memref.store %12, %arg4[%arg13] : memref<?xf32>
    }
    affine.store %cst_0, %arg4[0] : memref<?xf32>
    %4 = arith.addi %arg2, %c1_i32 : i32
    %5 = arith.index_cast %4 : i32 to index
    scf.for %arg13 = %c1 to %5 step %c1 {
      %8 = scf.for %arg14 = %c0 to %1 step %c1 iter_args(%arg15 = %cst) -> (f32) {
        %13 = memref.load %arg11[%arg14, %arg13] : memref<?x?xf32>
        %14 = memref.load %arg4[%arg14] : memref<?xf32>
        %15 = arith.mulf %13, %14 : f32
        %16 = arith.addf %arg15, %15 : f32
        scf.yield %16 : f32
      }
      %9 = arith.negf %8 : f32
      %10 = math.exp %9 : f32
      %11 = arith.addf %10, %cst_0 : f32
      %12 = arith.divf %cst_0, %11 : f32
      memref.store %12, %arg5[%arg13] : memref<?xf32>
    }
    scf.for %arg13 = %c1 to %5 step %c1 {
      %8 = memref.load %arg5[%arg13] : memref<?xf32>
      %9 = memref.load %arg6[%arg13] : memref<?xf32>
      %10 = arith.subf %cst_0, %8 : f32
      %11 = arith.mulf %8, %10 : f32
      %12 = arith.subf %9, %8 : f32
      %13 = arith.mulf %11, %12 : f32
      memref.store %13, %arg7[%arg13] : memref<?xf32>
    }
    scf.for %arg13 = %c1 to %1 step %c1 {
      %8 = memref.load %arg4[%arg13] : memref<?xf32>
      %9 = scf.for %arg14 = %c1 to %5 step %c1 iter_args(%arg15 = %cst) -> (f32) {
        %13 = memref.load %arg7[%arg14] : memref<?xf32>
        %14 = memref.load %arg11[%arg13, %arg14] : memref<?x?xf32>
        %15 = arith.mulf %13, %14 : f32
        %16 = arith.addf %arg15, %15 : f32
        scf.yield %16 : f32
      }
      %10 = arith.subf %cst_0, %8 : f32
      %11 = arith.mulf %8, %10 : f32
      %12 = arith.mulf %11, %9 : f32
      memref.store %12, %arg8[%arg13] : memref<?xf32>
    }
    affine.store %cst_0, %arg4[0] : memref<?xf32>
    scf.for %arg13 = %c0 to %1 step %c1 {
      scf.for %arg14 = %c1 to %5 step %c1 {
        %8 = memref.load %arg7[%arg14] : memref<?xf32>
        %9 = arith.mulf %8, %cst_1 : f32
        %10 = memref.load %arg4[%arg13] : memref<?xf32>
        %11 = arith.mulf %9, %10 : f32
        %12 = memref.load %arg12[%arg13, %arg14] : memref<?x?xf32>
        %13 = arith.mulf %12, %cst_1 : f32
        %14 = arith.addf %11, %13 : f32
        %15 = memref.load %arg11[%arg13, %arg14] : memref<?x?xf32>
        %16 = arith.addf %15, %14 : f32
        memref.store %16, %arg11[%arg13, %arg14] : memref<?x?xf32>
        memref.store %14, %arg12[%arg13, %arg14] : memref<?x?xf32>
      }
    }
    affine.store %cst_0, %arg3[0] : memref<?xf32>
    %6 = arith.addi %arg0, %c1_i32 : i32
    %7 = arith.index_cast %6 : i32 to index
    scf.for %arg13 = %c0 to %7 step %c1 {
      scf.for %arg14 = %c1 to %1 step %c1 {
        %8 = memref.load %arg8[%arg14] : memref<?xf32>
        %9 = arith.mulf %8, %cst_1 : f32
        %10 = memref.load %arg3[%arg13] : memref<?xf32>
        %11 = arith.mulf %9, %10 : f32
        %12 = memref.load %arg10[%arg13, %arg14] : memref<?x?xf32>
        %13 = arith.mulf %12, %cst_1 : f32
        %14 = arith.addf %11, %13 : f32
        %15 = memref.load %arg9[%arg13, %arg14] : memref<?x?xf32>
        %16 = arith.addf %15, %14 : f32
        memref.store %16, %arg9[%arg13, %arg14] : memref<?x?xf32>
        memref.store %14, %arg10[%arg13, %arg14] : memref<?x?xf32>
      }
    }
    return
  }
}
