module {
  func.func @bfs_kernel(%arg0: i32, %arg1: memref<?x2xi32>, %arg2: memref<?xi32>, %arg3: memref<?xi32>, %arg4: memref<?xi32>, %arg5: memref<?xi32>, %arg6: memref<?xi32>) attributes {llvm.linkage = #llvm.linkage<external>} {
    %c0_i32 = arith.constant 0 : i32
    %c1_i32 = arith.constant 1 : i32
    %c1 = arith.constant 1 : index
    %c0 = arith.constant 0 : index
    
    %1 = arith.index_cast %arg0 : i32 to index
    %2:3 = scf.while (%arg7 = %c0_i32, %arg8 = %c0_i32, %arg9 = %c0_i32, %arg10 = %c1_i32) : (i32, i32, i32, i32) -> (i32, i32, i32) {
      %3 = arith.cmpi ne, %arg10, %c0_i32 : i32
      scf.condition(%3) %arg7, %arg8, %arg9 : i32, i32, i32
    } do {
    ^bb0(%arg7: i32, %arg8: i32, %arg9: i32):
      %3:3 = scf.for %arg10 = %c0 to %1 step %c1 iter_args(%arg11 = %arg7, %arg12 = %arg8, %arg13 = %arg9) -> (i32, i32, i32) {
        %5 = memref.load %arg3[%arg10] : memref<?xi32>
        %6 = arith.cmpi ne, %5, %c0_i32 : i32
        %7:3 = scf.if %6 -> (i32, i32, i32) {
          memref.store %c0_i32, %arg3[%arg10] : memref<?xi32>
          %8 = memref.load %arg1[%arg10, %c0] : memref<?x2xi32>
          %9 = memref.load %arg1[%arg10, %c1] : memref<?x2xi32>
          %10 = arith.addi %8, %9 : i32
          %11 = arith.index_cast %10 : i32 to index
          %12 = arith.index_cast %8 : i32 to index
          scf.for %arg14 = %12 to %11 step %c1 {
            %13 = memref.load %arg2[%arg14] : memref<?xi32>
            %14 = arith.index_cast %13 : i32 to index
            %15 = memref.load %arg5[%14] : memref<?xi32>
            %16 = arith.cmpi eq, %15, %c0_i32 : i32
            scf.if %16 {
              %17 = memref.load %arg6[%arg10] : memref<?xi32>
              %18 = arith.addi %17, %c1_i32 : i32
              memref.store %18, %arg6[%14] : memref<?xi32>
              memref.store %c1_i32, %arg4[%14] : memref<?xi32>
            }
          }
          scf.yield %10, %9, %8 : i32, i32, i32
        } else {
          scf.yield %arg11, %arg12, %arg13 : i32, i32, i32
        }
        scf.yield %7#0, %7#1, %7#2 : i32, i32, i32
      }
      %4:3 = scf.for %arg10 = %c0 to %1 step %c1 iter_args(%arg11 = %c0_i32, %arg12 = %c0_i32, %arg13 = %c0_i32) -> (i32, i32, i32) {
        %5 = arith.index_cast %arg11 : i32 to index
        %6 = memref.load %arg4[%5] : memref<?xi32>
        %7 = arith.cmpi ne, %6, %c0_i32 : i32
        %8 = arith.select %7, %c1_i32, %arg12 : i32
        scf.if %7 {
          memref.store %c1_i32, %arg3[%5] : memref<?xi32>
          memref.store %c1_i32, %arg5[%5] : memref<?xi32>
          memref.store %c0_i32, %arg4[%5] : memref<?xi32>
        }
        %9 = arith.addi %arg11, %c1_i32 : i32
        scf.yield %9, %8, %8 : i32, i32, i32
      }
      scf.yield %3#0, %3#1, %3#2, %4#2 : i32, i32, i32, i32
    }
    return
  }
}
