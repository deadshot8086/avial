module {
  func.func @nn(%arg0: i32, %arg1: i32, %arg2: memref<?xf32>, %arg3: memref<?xf32>, %arg4: f32, %arg5: f32, %arg6: memref<?xi32>, %arg7: memref<?xf32>) attributes {llvm.linkage = #llvm.linkage<external>} {
    %c0 = arith.constant 0 : index
    %c1 = arith.constant 1 : index
    %cst = arith.constant -1.000000e+00 : f32
    %c-1_i32 = arith.constant -1 : i32
    %cst_0 = arith.constant 1.000000e+04 : f32
    %c0_i32 = arith.constant 0 : i32
    %0 = arith.index_cast %arg1 : i32 to index
    scf.for %arg8 = %c0 to %0 step %c1 {
      memref.store %cst_0, %arg7[%arg8] : memref<?xf32>
      memref.store %c-1_i32, %arg6[%arg8] : memref<?xi32>
    }
    %1 = arith.index_cast %arg0 : i32 to index
    scf.for %arg8 = %c0 to %1 step %c1 {
      %2 = arith.index_cast %arg8 : index to i32
      %3 = memref.load %arg2[%arg8] : memref<?xf32>
      %4 = arith.subf %3, %arg4 : f32
      %5 = memref.load %arg3[%arg8] : memref<?xf32>
      %6 = arith.subf %5, %arg5 : f32
      %7 = arith.mulf %4, %4 : f32
      %8 = arith.mulf %6, %6 : f32
      %9 = arith.addf %7, %8 : f32
      %10 = math.sqrt %9 : f32
      %11:2 = scf.for %arg9 = %c0 to %0 step %c1 iter_args(%arg10 = %c0_i32, %arg11 = %cst) -> (i32, f32) {
        %15 = arith.index_cast %arg9 : index to i32
        %16 = memref.load %arg7[%arg9] : memref<?xf32>
        %17 = arith.cmpf ogt, %16, %arg11 : f32
        %18 = arith.select %17, %15, %arg10 : i32
        %19 = scf.if %17 -> (f32) {
          %20 = memref.load %arg7[%arg9] : memref<?xf32>
          scf.yield %20 : f32
        } else {
          scf.yield %arg11 : f32
        }
        scf.yield %18, %19 : i32, f32
      }
      %12 = arith.index_cast %11#0 : i32 to index
      %13 = memref.load %arg7[%12] : memref<?xf32>
      %14 = arith.cmpf olt, %10, %13 : f32
      scf.if %14 {
        memref.store %10, %arg7[%12] : memref<?xf32>
        memref.store %2, %arg6[%12] : memref<?xi32>
      }
    }
    return
  }
}
