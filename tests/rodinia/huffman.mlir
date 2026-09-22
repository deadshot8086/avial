module {
  func.func @cpu_vlc_encode(%arg0: memref<?xi32>, %arg1: i32, %arg2: memref<?xi32>, %arg3: memref<?xi32>, %arg4: memref<?x2xi32>) attributes {llvm.linkage = #llvm.linkage<external>} {
    %c0 = arith.constant 0 : index
    %c1 = arith.constant 1 : index
    %c4 = arith.constant 4 : index
    %c-1_i32 = arith.constant -1 : i32
    %c1_i32 = arith.constant 1 : i32
    %c32_i32 = arith.constant 32 : i32
    %c255_i32 = arith.constant 255 : i32
    %c3_i32 = arith.constant 3 : i32
    %c8_i32 = arith.constant 8 : i32
    %c4_i32 = arith.constant 4 : i32
    %c0_i32 = arith.constant 0 : i32
    affine.store %c0_i32, %arg3[0] : memref<?xi32>
    affine.store %c0_i32, %arg3[1] : memref<?xi32>
    affine.store %c0_i32, %arg3[2] : memref<?xi32>
    affine.store %c0_i32, %arg2[0] : memref<?xi32>
    %0 = arith.index_cast %arg1 : i32 to index
    scf.for %arg5 = %c0 to %0 step %c1 {
      %9 = memref.load %arg0[%arg5] : memref<?xi32>
      scf.for %arg6 = %c0 to %c4 step %c1 {
        %10 = arith.index_cast %arg6 : index to i32
        %11 = arith.subi %c3_i32, %10 : i32
        %12 = arith.muli %11, %c8_i32 : i32
        %13 = arith.shrui %9, %12 : i32
        %14 = arith.andi %13, %c255_i32 : i32
        %15 = arith.index_cast %14 : i32 to index
        %16 = memref.load %arg4[%15, %c0] : memref<?x2xi32>
        %17 = memref.load %arg4[%15, %c1] : memref<?x2xi32>
        %18 = scf.while (%arg7 = %17) : (i32) -> i32 {
          %19 = arith.cmpi ugt, %arg7, %c0_i32 : i32
          scf.condition(%19) %arg7 : i32
        } do {
        ^bb0(%arg7: i32):
          %19 = affine.load %arg3[1] : memref<?xi32>
          %20 = arith.subi %c32_i32, %19 : i32
          %21 = arith.cmpi slt, %20, %arg7 : i32
          %22 = arith.select %21, %20, %arg7 : i32
          %23 = arith.cmpi eq, %arg7, %22 : i32
          %24 = scf.if %23 -> (i32) {
            %33 = arith.shli %c1_i32, %arg7 : i32
            %34 = arith.addi %33, %c-1_i32 : i32
            %35 = arith.andi %16, %34 : i32
            %36 = arith.subi %20, %arg7 : i32
            %37 = arith.shli %35, %36 : i32
            scf.yield %37 : i32
          } else {
            %33 = arith.subi %arg7, %22 : i32
            %34 = arith.shrui %16, %33 : i32
            scf.yield %34 : i32
          }
          %25 = affine.load %arg3[2] : memref<?xi32>
          %26 = arith.index_cast %25 : i32 to index
          %27 = memref.load %arg2[%26] : memref<?xi32>
          %28 = arith.ori %27, %24 : i32
          memref.store %28, %arg2[%26] : memref<?xi32>
          %29 = arith.subi %arg7, %22 : i32
          %30 = arith.addi %19, %22 : i32
          %31 = arith.remui %30, %c32_i32 : i32
          affine.store %31, %arg3[1] : memref<?xi32>
          %32 = arith.cmpi eq, %31, %c0_i32 : i32
          scf.if %32 {
            %33 = arith.addi %25, %c1_i32 : i32
            affine.store %33, %arg3[2] : memref<?xi32>
            %34 = arith.index_cast %33 : i32 to index
            memref.store %c0_i32, %arg2[%34] : memref<?xi32>
            %35 = affine.load %arg3[0] : memref<?xi32>
            %36 = arith.addi %35, %c4_i32 : i32
            affine.store %36, %arg3[0] : memref<?xi32>
          }
          scf.yield %29 : i32
        }
      }
    }
    %1 = affine.load %arg3[1] : memref<?xi32>
    %2 = arith.divui %1, %c8_i32 : i32
    %3 = arith.remui %1, %c8_i32 : i32
    %4 = arith.cmpi ne, %3, %c0_i32 : i32
    %5 = arith.extui %4 : i1 to i32
    %6 = arith.addi %2, %5 : i32
    %7 = affine.load %arg3[0] : memref<?xi32>
    %8 = arith.addi %7, %6 : i32
    affine.store %8, %arg3[0] : memref<?xi32>
    return
  }
}
