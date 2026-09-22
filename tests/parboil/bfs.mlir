module {
  func.func @bfs(%arg0: i32, %arg1: i32, %arg2: memref<?x2xi32>, %arg3: memref<?xi32>, %arg4: memref<?xi32>, %arg5: memref<?xi32>, %arg6: memref<?xi32>) attributes {llvm.linkage = #llvm.linkage<external>} {
    %c0 = arith.constant 0 : index
    %c1 = arith.constant 1 : index
    %c16677221_i32 = arith.constant 16677221 : i32
    %c16677218_i32 = arith.constant 16677218 : i32
    %c1_i32 = arith.constant 1 : i32
    %c2147483647_i32 = arith.constant 2147483647 : i32
    %c16677217_i32 = arith.constant 16677217 : i32
    %c0_i32 = arith.constant 0 : i32
    %0 = arith.index_cast %arg0 : i32 to index
    affine.for %arg7 = 0 to %0 {
      affine.store %c16677217_i32, %arg4[%arg7] : memref<?xi32>
      affine.store %c2147483647_i32, %arg5[%arg7] : memref<?xi32>
    }
    %1 = arith.index_cast %arg1 : i32 to index
    affine.store %c0_i32, %arg5[symbol(%1)] : memref<?xi32>
    affine.store %arg1, %arg6[0] : memref<?xi32>
    affine.store %c16677218_i32, %arg4[symbol(%1)] : memref<?xi32>
    %2:2 = scf.while (%arg7 = %c1_i32, %arg8 = %c0_i32) : (i32, i32) -> (i32, i32) {
      %3 = arith.cmpi slt, %arg8, %arg7 : i32
      scf.condition(%3) %arg7, %arg8 : i32, i32
    } do {
    ^bb0(%arg7: i32, %arg8: i32):
      %3 = arith.index_cast %arg8 : i32 to index
      %4 = memref.load %arg6[%3] : memref<?xi32>
      %5 = arith.addi %arg8, %c1_i32 : i32
      %6 = arith.index_cast %4 : i32 to index
      %7 = memref.load %arg2[%6, %c0] : memref<?x2xi32>
      %8 = memref.load %arg2[%6, %c1] : memref<?x2xi32>
      %9 = arith.addi %8, %7 : i32
      %10 = arith.index_cast %9 : i32 to index
      %11 = arith.index_cast %7 : i32 to index
      %12 = scf.for %arg9 = %11 to %10 step %c1 iter_args(%arg10 = %arg7) -> (i32) {
        %13 = memref.load %arg3[%arg9] : memref<?xi32>
        %14 = arith.index_cast %13 : i32 to index
        %15 = memref.load %arg4[%14] : memref<?xi32>
        %16 = arith.cmpi eq, %15, %c16677217_i32 : i32
        %17 = scf.if %16 -> (i32) {
          %18 = memref.load %arg5[%6] : memref<?xi32>
          %19 = arith.addi %18, %c1_i32 : i32
          memref.store %19, %arg5[%14] : memref<?xi32>
          %20 = arith.index_cast %arg10 : i32 to index
          memref.store %13, %arg6[%20] : memref<?xi32>
          %21 = arith.addi %arg10, %c1_i32 : i32
          memref.store %c16677218_i32, %arg4[%14] : memref<?xi32>
          scf.yield %21 : i32
        } else {
          scf.yield %arg10 : i32
        }
        scf.yield %17 : i32
      }
      memref.store %c16677221_i32, %arg4[%6] : memref<?xi32>
      scf.yield %12, %5 : i32, i32
    }
    return
  }
}
