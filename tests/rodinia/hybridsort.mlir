module {
  func.func @hybridsort_bucket(%arg0: memref<?xf32>, %arg1: i32, %arg2: memref<?xf32>, %arg3: i32, %arg4: memref<?xi32>, %arg5: memref<?xi32>) attributes {llvm.linkage = #llvm.linkage<external>} {
    %c0 = arith.constant 0 : index
    %c1 = arith.constant 1 : index
    %c-1_i32 = arith.constant -1 : i32
    %c2_i32 = arith.constant 2 : i32
    %c1_i32 = arith.constant 1 : i32
    %c4_i32 = arith.constant 4 : i32
    %0 = arith.index_cast %arg3 : i32 to index
    %1 = arith.index_cast %arg1 : i32 to index
    %2 = arith.divsi %arg3, %c2_i32 : i32
    %3 = arith.addi %2, %c-1_i32 : i32
    %4 = arith.divsi %arg3, %c4_i32 : i32
    scf.for %arg6 = %c0 to %1 step %c1 {
      %5 = memref.load %arg0[%arg6] : memref<?xf32>
      %6 = affine.load %arg2[symbol(%0) floordiv 2 - 1] : memref<?xf32>
      %7:3 = scf.while (%arg7 = %6, %arg8 = %4, %arg9 = %3) : (f32, i32, i32) -> (f32, i32, i32) {
        %13 = arith.cmpi sge, %arg8, %c1_i32 : i32
        scf.condition(%13) %arg7, %arg9, %arg8 : f32, i32, i32
      } do {
      ^bb0(%arg7: f32, %arg8: i32, %arg9: i32):
        %13 = arith.cmpf olt, %5, %arg7 : f32
        %14 = scf.if %13 -> (i32) {
          %18 = arith.subi %arg8, %arg9 : i32
          scf.yield %18 : i32
        } else {
          %18 = arith.addi %arg8, %arg9 : i32
          scf.yield %18 : i32
        }
        %15 = arith.index_cast %14 : i32 to index
        %16 = memref.load %arg2[%15] : memref<?xf32>
        %17 = arith.divsi %arg9, %c2_i32 : i32
        scf.yield %16, %17, %14 : f32, i32, i32
      }
      %8 = arith.cmpf olt, %5, %7#0 : f32
      %9 = scf.if %8 -> (i32) {
        scf.yield %7#1 : i32
      } else {
        %13 = arith.addi %7#1, %c1_i32 : i32
        scf.yield %13 : i32
      }
      memref.store %9, %arg4[%arg6] : memref<?xi32>
      %10 = arith.index_cast %9 : i32 to index
      %11 = memref.load %arg5[%10] : memref<?xi32>
      %12 = arith.addi %11, %c1_i32 : i32
      memref.store %12, %arg5[%10] : memref<?xi32>
    }
    return
  }
}
