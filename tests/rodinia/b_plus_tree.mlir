module {
  func.func @b_plus_tree(%arg0: i32, %arg1: i32, %arg2: i32, %arg3: i64, %arg4: memref<?x?xi32>, %arg5: memref<?x?xi32>, %arg6: memref<?xi32>, %arg7: memref<?xi32>, %arg8: memref<?xi64>, %arg9: memref<?xi64>, %arg10: memref<?xi32>) attributes {llvm.linkage = #llvm.linkage<external>} {
    %c0 = arith.constant 0 : index
    %c1 = arith.constant 1 : index
    %c1_i32 = arith.constant 1 : i32
    %0 = arith.index_cast %arg0 : i32 to index
    %1 = arith.index_cast %arg1 : i32 to index
    %2 = arith.index_cast %arg2 : i32 to index
    %3 = arith.index_cast %arg2 : i32 to index
    scf.for %arg11 = %c0 to %0 step %c1 {
      %4 = memref.load %arg7[%arg11] : memref<?xi32>
      scf.for %arg12 = %c0 to %1 step %c1 {
        %8 = memref.load %arg8[%arg11] : memref<?xi64>
        %9 = arith.trunci %8 : i64 to i32
        %10 = memref.load %arg9[%arg11] : memref<?xi64>
        %11 = arith.trunci %10 : i64 to i32
        %12 = arith.index_cast %9 : i32 to index
        %13 = arith.index_cast %11 : i32 to index
        scf.for %arg13 = %c0 to %2 step %c1 {
          %15 = arith.index_cast %arg13 : index to i32
          %16 = memref.load %arg4[%12, %arg13] : memref<?x?xi32>
          %17 = arith.addi %15, %c1_i32 : i32
          %18 = arith.index_cast %17 : i32 to index
          %19 = memref.load %arg4[%12, %18] : memref<?x?xi32>
          %20 = arith.cmpi sle, %16, %4 : i32
          %21 = arith.cmpi sgt, %19, %4 : i32
          %22 = arith.andi %20, %21 : i1
          scf.if %22 {
            %23 = memref.load %arg5[%13, %arg13] : memref<?x?xi32>
            %24 = arith.extsi %23 : i32 to i64
            %25 = arith.cmpi slt, %24, %arg3 : i64
            scf.if %25 {
              memref.store %24, %arg9[%arg11] : memref<?xi64>
            }
          }
        }
        %14 = memref.load %arg9[%arg11] : memref<?xi64>
        memref.store %14, %arg8[%arg11] : memref<?xi64>
      }
      %5 = memref.load %arg8[%arg11] : memref<?xi64>
      %6 = arith.trunci %5 : i64 to i32
      %7 = arith.index_cast %6 : i32 to index
      scf.for %arg12 = %c0 to %3 step %c1 {
        %8 = memref.load %arg4[%7, %arg12] : memref<?x?xi32>
        %9 = arith.cmpi eq, %8, %4 : i32
        scf.if %9 {
          %10 = memref.load %arg5[%7, %arg12] : memref<?x?xi32>
          %11 = arith.index_cast %10 : i32 to index
          %12 = memref.load %arg6[%11] : memref<?xi32>
          memref.store %12, %arg10[%arg11] : memref<?xi32>
        }
      }
    }
    return
  }
}
