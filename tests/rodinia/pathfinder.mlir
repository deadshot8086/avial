module {
  func.func @pathfinder(%arg0: i32, %arg1: i32, %arg2: memref<?x?xi32>, %arg3: memref<?xi32>, %arg4: memref<?xi32>) attributes {llvm.linkage = #llvm.linkage<external>} {
    %c-1_i32 = arith.constant -1 : i32
    %c1_i32 = arith.constant 1 : i32
    %c0 = arith.constant 0 : index
    %c1 = arith.constant 1 : index
    %c2_i32 = arith.constant 2 : i32
    %c0_i32 = arith.constant 0 : i32
    %0 = arith.addi %arg0, %c-1_i32 : i32
    %1 = arith.index_cast %arg1 : i32 to index
    %2 = arith.addi %arg1, %c-1_i32 : i32
    %3 = arith.addi %arg1, %c-1_i32 : i32
    %4:2 = scf.while (%arg5 = %c1_i32) : (i32) -> (i32, i32) {
      %8 = arith.addi %arg5, %c-1_i32 : i32
      %9 = arith.cmpi slt, %arg5, %0 : i32
      scf.condition(%9) %8, %arg5 : i32, i32
    } do {
    ^bb0(%arg5: i32, %arg6: i32):
      %8 = arith.index_cast %arg6 : i32 to index
      scf.for %arg7 = %c0 to %1 step %c1 {
        %12 = arith.index_cast %arg7 : index to i32
        %13 = memref.load %arg4[%arg7] : memref<?xi32>
        %14 = arith.cmpi sgt, %12, %c0_i32 : i32
        %15 = scf.if %14 -> (i32) {
          %20 = arith.addi %12, %c-1_i32 : i32
          %21 = arith.index_cast %20 : i32 to index
          %22 = memref.load %arg4[%21] : memref<?xi32>
          %23 = arith.cmpi slt, %22, %13 : i32
          %24 = scf.if %23 -> (i32) {
            %25 = memref.load %arg4[%21] : memref<?xi32>
            scf.yield %25 : i32
          } else {
            scf.yield %13 : i32
          }
          scf.yield %24 : i32
        } else {
          scf.yield %13 : i32
        }
        %16 = arith.cmpi slt, %12, %2 : i32
        %17 = scf.if %16 -> (i32) {
          %20 = arith.addi %12, %c1_i32 : i32
          %21 = arith.index_cast %20 : i32 to index
          %22 = memref.load %arg4[%21] : memref<?xi32>
          %23 = arith.cmpi slt, %22, %15 : i32
          %24 = scf.if %23 -> (i32) {
            %25 = memref.load %arg4[%21] : memref<?xi32>
            scf.yield %25 : i32
          } else {
            scf.yield %15 : i32
          }
          scf.yield %24 : i32
        } else {
          scf.yield %15 : i32
        }
        %18 = memref.load %arg2[%8, %arg7] : memref<?x?xi32>
        %19 = arith.addi %18, %17 : i32
        memref.store %19, %arg3[%arg7] : memref<?xi32>
      }
      %9 = arith.addi %arg6, %c1_i32 : i32
      %10 = arith.index_cast %9 : i32 to index
      scf.for %arg7 = %c0 to %1 step %c1 {
        %12 = arith.index_cast %arg7 : index to i32
        %13 = memref.load %arg3[%arg7] : memref<?xi32>
        %14 = arith.cmpi sgt, %12, %c0_i32 : i32
        %15 = scf.if %14 -> (i32) {
          %20 = arith.addi %12, %c-1_i32 : i32
          %21 = arith.index_cast %20 : i32 to index
          %22 = memref.load %arg3[%21] : memref<?xi32>
          %23 = arith.cmpi slt, %22, %13 : i32
          %24 = scf.if %23 -> (i32) {
            %25 = memref.load %arg3[%21] : memref<?xi32>
            scf.yield %25 : i32
          } else {
            scf.yield %13 : i32
          }
          scf.yield %24 : i32
        } else {
          scf.yield %13 : i32
        }
        %16 = arith.cmpi slt, %12, %3 : i32
        %17 = scf.if %16 -> (i32) {
          %20 = arith.addi %12, %c1_i32 : i32
          %21 = arith.index_cast %20 : i32 to index
          %22 = memref.load %arg3[%21] : memref<?xi32>
          %23 = arith.cmpi slt, %22, %15 : i32
          %24 = scf.if %23 -> (i32) {
            %25 = memref.load %arg3[%21] : memref<?xi32>
            scf.yield %25 : i32
          } else {
            scf.yield %15 : i32
          }
          scf.yield %24 : i32
        } else {
          scf.yield %15 : i32
        }
        %18 = memref.load %arg2[%10, %arg7] : memref<?x?xi32>
        %19 = arith.addi %18, %17 : i32
        memref.store %19, %arg4[%arg7] : memref<?xi32>
      }
      %11 = arith.addi %arg6, %c2_i32 : i32
      scf.yield %11 : i32
    }
    %5 = arith.cmpi slt, %4#0, %0 : i32
    scf.if %5 {
      %8 = arith.addi %arg1, %c-1_i32 : i32
      %9 = arith.index_cast %4#1 : i32 to index
      scf.for %arg5 = %c0 to %1 step %c1 {
        %10 = arith.index_cast %arg5 : index to i32
        %11 = memref.load %arg4[%arg5] : memref<?xi32>
        %12 = arith.cmpi sgt, %10, %c0_i32 : i32
        %13 = scf.if %12 -> (i32) {
          %18 = arith.addi %10, %c-1_i32 : i32
          %19 = arith.index_cast %18 : i32 to index
          %20 = memref.load %arg4[%19] : memref<?xi32>
          %21 = arith.cmpi slt, %20, %11 : i32
          %22 = scf.if %21 -> (i32) {
            %23 = memref.load %arg4[%19] : memref<?xi32>
            scf.yield %23 : i32
          } else {
            scf.yield %11 : i32
          }
          scf.yield %22 : i32
        } else {
          scf.yield %11 : i32
        }
        %14 = arith.cmpi slt, %10, %8 : i32
        %15 = scf.if %14 -> (i32) {
          %18 = arith.addi %10, %c1_i32 : i32
          %19 = arith.index_cast %18 : i32 to index
          %20 = memref.load %arg4[%19] : memref<?xi32>
          %21 = arith.cmpi slt, %20, %13 : i32
          %22 = scf.if %21 -> (i32) {
            %23 = memref.load %arg4[%19] : memref<?xi32>
            scf.yield %23 : i32
          } else {
            scf.yield %13 : i32
          }
          scf.yield %22 : i32
        } else {
          scf.yield %13 : i32
        }
        %16 = memref.load %arg2[%9, %arg5] : memref<?x?xi32>
        %17 = arith.addi %16, %15 : i32
        memref.store %17, %arg3[%arg5] : memref<?xi32>
      }
    }
    %6 = arith.remsi %0, %c2_i32 : i32
    %7 = arith.cmpi eq, %6, %c1_i32 : i32
    scf.if %7 {
      scf.for %arg5 = %c0 to %1 step %c1 {
        %8 = memref.load %arg4[%arg5] : memref<?xi32>
        %9 = memref.load %arg3[%arg5] : memref<?xi32>
        memref.store %9, %arg4[%arg5] : memref<?xi32>
        memref.store %8, %arg3[%arg5] : memref<?xi32>
      }
    }
    return
  }
}
