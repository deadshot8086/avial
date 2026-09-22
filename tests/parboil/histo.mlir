module {
  func.func @histo(%arg0: i32, %arg1: i32, %arg2: i32, %arg3: i32, %arg4: i32, %arg5: memref<?x?xi32>, %arg6: memref<?x?xi8>) attributes {llvm.linkage = #llvm.linkage<external>} {
    %c255_i32 = arith.constant 255 : i32
    %c1_i32 = arith.constant 1 : i32
    %c0_i8 = arith.constant 0 : i8
    %0 = arith.index_cast %arg4 : i32 to index
    %1 = arith.index_cast %arg3 : i32 to index
    %2 = arith.index_cast %arg2 : i32 to index
    %3 = arith.index_cast %arg1 : i32 to index
    %4 = arith.index_cast %arg0 : i32 to index
    affine.for %arg7 = 0 to %0 {
      affine.for %arg8 = 0 to %1 {
        affine.for %arg9 = 0 to %2 {
          affine.store %c0_i8, %arg6[%arg8, %arg9] : memref<?x?xi8>
        }
      }
      affine.for %arg8 = 0 to %3 {
        affine.for %arg9 = 0 to %4 {
          %5 = affine.load %arg5[%arg8, %arg9] : memref<?x?xi32>
          %6 = arith.divui %5, %arg2 : i32
          %7 = arith.remui %5, %arg2 : i32
          %8 = arith.cmpi ult, %6, %arg3 : i32
          scf.if %8 {
            %9 = arith.index_cast %6 : i32 to index
            %10 = arith.index_cast %7 : i32 to index
            %11 = memref.load %arg6[%9, %10] : memref<?x?xi8>
            %12 = arith.extui %11 : i8 to i32
            %13 = arith.cmpi slt, %12, %c255_i32 : i32
            scf.if %13 {
              %14 = memref.load %arg6[%9, %10] : memref<?x?xi8>
              %15 = arith.extui %14 : i8 to i32
              %16 = arith.addi %15, %c1_i32 : i32
              %17 = arith.trunci %16 : i32 to i8
              memref.store %17, %arg6[%9, %10] : memref<?x?xi8>
            }
          }
        }
      }
    }
    return
  }
}
