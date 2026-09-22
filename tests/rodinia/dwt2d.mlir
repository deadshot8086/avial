module {
  func.func @dwt2d(%arg0: i32, %arg1: i32, %arg2: i32, %arg3: i32, %arg4: memref<?x?x?xi32>, %arg5: memref<?x?xi32>, %arg6: memref<?x?x?xi32>) attributes {llvm.linkage = #llvm.linkage<external>} {
    %false = arith.constant false
    %c0_i32 = arith.constant 0 : i32
    %c-2_i32 = arith.constant -2 : i32
    %c0 = arith.constant 0 : index
    %c1 = arith.constant 1 : index
    %c2 = arith.constant 2 : index
    %c-1_i32 = arith.constant -1 : i32
    %c4_i32 = arith.constant 4 : i32
    %c2_i32 = arith.constant 2 : i32
    %c1_i32 = arith.constant 1 : i32
    %0 = arith.index_cast %arg3 : i32 to index
    %1 = arith.index_cast %arg1 : i32 to index
    %2 = arith.index_cast %arg0 : i32 to index
    %3 = arith.index_cast %arg2 : i32 to index
    %4 = llvm.mlir.undef : i32
    %5 = llvm.mlir.undef : i32
    %6 = llvm.mlir.undef : i32
    %7 = llvm.mlir.undef : i32
    scf.for %arg7 = %c0 to %0 step %c1 {
      scf.for %arg8 = %c0 to %1 step %c1 {
        scf.for %arg9 = %c0 to %2 step %c1 {
          %9 = memref.load %arg4[%arg7, %arg8, %arg9] : memref<?x?x?xi32>
          memref.store %9, %arg6[%arg7, %arg8, %arg9] : memref<?x?x?xi32>
        }
      }
      %8:2 = scf.for %arg8 = %c0 to %3 step %c1 iter_args(%arg9 = %arg1, %arg10 = %arg0) -> (i32, i32) {
        %9 = arith.divsi %arg10, %c2_i32 : i32
        %10 = arith.divsi %arg9, %c2_i32 : i32
        %11 = arith.index_cast %arg9 : i32 to index
        %12 = arith.index_cast %arg10 : i32 to index
        scf.for %arg11 = %c0 to %11 step %c1 {
          scf.for %arg12 = %c0 to %12 step %c1 {
            %28 = memref.load %arg6[%arg7, %arg11, %arg12] : memref<?x?x?xi32>
            memref.store %28, %arg5[%arg11, %arg12] : memref<?x?xi32>
          }
        }
        %13 = arith.index_cast %arg10 : i32 to index
        %14 = arith.muli %arg9, %c2_i32 : i32
        %15 = arith.addi %14, %c-2_i32 : i32
        %16 = arith.muli %arg9, %c2_i32 : i32
        %17 = arith.addi %16, %c-2_i32 : i32
        %18 = arith.muli %arg9, %c2_i32 : i32
        %19 = arith.addi %18, %c-2_i32 : i32
        scf.for %arg11 = %c0 to %13 step %c1 {
          scf.for %arg12 = %c1 to %11 step %c2 {
            %28 = arith.index_cast %arg12 : index to i32
            %29 = arith.addi %28, %c-1_i32 : i32
            %30 = arith.index_cast %29 : i32 to index
            %31 = memref.load %arg5[%30, %arg11] : memref<?x?xi32>
            %32 = arith.addi %28, %c1_i32 : i32
            %33 = arith.cmpi sge, %32, %arg9 : i32
            %34:2 = scf.if %33 -> (i1, i32) {
              %42 = arith.subi %15, %32 : i32
              scf.yield %false, %42 : i1, i32
            } else {
              %42 = arith.cmpi slt, %32, %c0_i32 : i32
              %43 = arith.cmpi sge, %32, %c0_i32 : i32
              %44 = scf.if %42 -> (i32) {
                %45 = arith.subi %c-1_i32, %28 : i32
                scf.yield %45 : i32
              } else {
                scf.yield %4 : i32
              }
              scf.yield %43, %44 : i1, i32
            }
            %35 = arith.select %34#0, %32, %34#1 : i32
            %36 = arith.index_cast %35 : i32 to index
            %37 = memref.load %arg5[%36, %arg11] : memref<?x?xi32>
            %38 = arith.addi %31, %37 : i32
            %39 = arith.divsi %38, %c2_i32 : i32
            %40 = memref.load %arg5[%arg12, %arg11] : memref<?x?xi32>
            %41 = arith.subi %40, %39 : i32
            memref.store %41, %arg5[%arg12, %arg11] : memref<?x?xi32>
          }
          scf.for %arg12 = %c0 to %11 step %c2 {
            %28 = arith.index_cast %arg12 : index to i32
            %29 = arith.addi %28, %c-1_i32 : i32
            %30 = arith.cmpi sge, %29, %arg9 : i32
            %31:2 = scf.if %30 -> (i1, i32) {
              %46 = arith.subi %17, %29 : i32
              scf.yield %false, %46 : i1, i32
            } else {
              %46 = arith.cmpi slt, %29, %c0_i32 : i32
              %47 = arith.cmpi sge, %29, %c0_i32 : i32
              %48 = scf.if %46 -> (i32) {
                %49 = arith.subi %c1_i32, %28 : i32
                scf.yield %49 : i32
              } else {
                scf.yield %5 : i32
              }
              scf.yield %47, %48 : i1, i32
            }
            %32 = arith.select %31#0, %29, %31#1 : i32
            %33 = arith.index_cast %32 : i32 to index
            %34 = memref.load %arg5[%33, %arg11] : memref<?x?xi32>
            %35 = arith.addi %28, %c1_i32 : i32
            %36 = arith.cmpi sge, %35, %arg9 : i32
            %37:2 = scf.if %36 -> (i1, i32) {
              %46 = arith.subi %19, %35 : i32
              scf.yield %false, %46 : i1, i32
            } else {
              %46 = arith.cmpi slt, %35, %c0_i32 : i32
              %47 = arith.cmpi sge, %35, %c0_i32 : i32
              %48 = scf.if %46 -> (i32) {
                %49 = arith.subi %c-1_i32, %28 : i32
                scf.yield %49 : i32
              } else {
                scf.yield %5 : i32
              }
              scf.yield %47, %48 : i1, i32
            }
            %38 = arith.select %37#0, %35, %37#1 : i32
            %39 = arith.index_cast %38 : i32 to index
            %40 = memref.load %arg5[%39, %arg11] : memref<?x?xi32>
            %41 = arith.addi %34, %40 : i32
            %42 = arith.addi %41, %c2_i32 : i32
            %43 = arith.divsi %42, %c4_i32 : i32
            %44 = memref.load %arg5[%arg12, %arg11] : memref<?x?xi32>
            %45 = arith.addi %44, %43 : i32
            memref.store %45, %arg5[%arg12, %arg11] : memref<?x?xi32>
          }
        }
        %20 = arith.muli %arg10, %c2_i32 : i32
        %21 = arith.addi %20, %c-2_i32 : i32
        %22 = arith.muli %arg10, %c2_i32 : i32
        %23 = arith.addi %22, %c-2_i32 : i32
        %24 = arith.muli %arg10, %c2_i32 : i32
        %25 = arith.addi %24, %c-2_i32 : i32
        scf.for %arg11 = %c0 to %11 step %c1 {
          scf.for %arg12 = %c1 to %13 step %c2 {
            %28 = arith.index_cast %arg12 : index to i32
            %29 = arith.addi %28, %c-1_i32 : i32
            %30 = arith.index_cast %29 : i32 to index
            %31 = memref.load %arg5[%arg11, %30] : memref<?x?xi32>
            %32 = arith.addi %28, %c1_i32 : i32
            %33 = arith.cmpi sge, %32, %arg10 : i32
            %34:2 = scf.if %33 -> (i1, i32) {
              %42 = arith.subi %21, %32 : i32
              scf.yield %false, %42 : i1, i32
            } else {
              %42 = arith.cmpi slt, %32, %c0_i32 : i32
              %43 = arith.cmpi sge, %32, %c0_i32 : i32
              %44 = scf.if %42 -> (i32) {
                %45 = arith.subi %c-1_i32, %28 : i32
                scf.yield %45 : i32
              } else {
                scf.yield %6 : i32
              }
              scf.yield %43, %44 : i1, i32
            }
            %35 = arith.select %34#0, %32, %34#1 : i32
            %36 = arith.index_cast %35 : i32 to index
            %37 = memref.load %arg5[%arg11, %36] : memref<?x?xi32>
            %38 = arith.addi %31, %37 : i32
            %39 = arith.divsi %38, %c2_i32 : i32
            %40 = memref.load %arg5[%arg11, %arg12] : memref<?x?xi32>
            %41 = arith.subi %40, %39 : i32
            memref.store %41, %arg5[%arg11, %arg12] : memref<?x?xi32>
          }
          scf.for %arg12 = %c0 to %13 step %c2 {
            %28 = arith.index_cast %arg12 : index to i32
            %29 = arith.addi %28, %c-1_i32 : i32
            %30 = arith.cmpi sge, %29, %arg10 : i32
            %31:2 = scf.if %30 -> (i1, i32) {
              %46 = arith.subi %23, %29 : i32
              scf.yield %false, %46 : i1, i32
            } else {
              %46 = arith.cmpi slt, %29, %c0_i32 : i32
              %47 = arith.cmpi sge, %29, %c0_i32 : i32
              %48 = scf.if %46 -> (i32) {
                %49 = arith.subi %c1_i32, %28 : i32
                scf.yield %49 : i32
              } else {
                scf.yield %7 : i32
              }
              scf.yield %47, %48 : i1, i32
            }
            %32 = arith.select %31#0, %29, %31#1 : i32
            %33 = arith.index_cast %32 : i32 to index
            %34 = memref.load %arg5[%arg11, %33] : memref<?x?xi32>
            %35 = arith.addi %28, %c1_i32 : i32
            %36 = arith.cmpi sge, %35, %arg10 : i32
            %37:2 = scf.if %36 -> (i1, i32) {
              %46 = arith.subi %25, %35 : i32
              scf.yield %false, %46 : i1, i32
            } else {
              %46 = arith.cmpi slt, %35, %c0_i32 : i32
              %47 = arith.cmpi sge, %35, %c0_i32 : i32
              %48 = scf.if %46 -> (i32) {
                %49 = arith.subi %c-1_i32, %28 : i32
                scf.yield %49 : i32
              } else {
                scf.yield %7 : i32
              }
              scf.yield %47, %48 : i1, i32
            }
            %38 = arith.select %37#0, %35, %37#1 : i32
            %39 = arith.index_cast %38 : i32 to index
            %40 = memref.load %arg5[%arg11, %39] : memref<?x?xi32>
            %41 = arith.addi %34, %40 : i32
            %42 = arith.addi %41, %c2_i32 : i32
            %43 = arith.divsi %42, %c4_i32 : i32
            %44 = memref.load %arg5[%arg11, %arg12] : memref<?x?xi32>
            %45 = arith.addi %44, %43 : i32
            memref.store %45, %arg5[%arg11, %arg12] : memref<?x?xi32>
          }
        }
        %26 = arith.index_cast %10 : i32 to index
        %27 = arith.index_cast %9 : i32 to index
        scf.for %arg11 = %c0 to %26 step %c1 {
          %28 = arith.index_cast %arg11 : index to i32
          %29 = arith.muli %28, %c2_i32 : i32
          %30 = arith.index_cast %29 : i32 to index
          %31 = arith.addi %10, %28 : i32
          %32 = arith.index_cast %31 : i32 to index
          %33 = arith.addi %29, %c1_i32 : i32
          %34 = arith.index_cast %33 : i32 to index
          scf.for %arg12 = %c0 to %27 step %c1 {
            %35 = arith.index_cast %arg12 : index to i32
            %36 = arith.muli %35, %c2_i32 : i32
            %37 = arith.index_cast %36 : i32 to index
            %38 = memref.load %arg5[%30, %37] : memref<?x?xi32>
            memref.store %38, %arg6[%arg7, %arg11, %arg12] : memref<?x?x?xi32>
            %39 = arith.addi %9, %35 : i32
            %40 = arith.index_cast %39 : i32 to index
            %41 = arith.addi %36, %c1_i32 : i32
            %42 = arith.index_cast %41 : i32 to index
            %43 = memref.load %arg5[%30, %42] : memref<?x?xi32>
            memref.store %43, %arg6[%arg7, %arg11, %40] : memref<?x?x?xi32>
            %44 = memref.load %arg5[%34, %37] : memref<?x?xi32>
            memref.store %44, %arg6[%arg7, %32, %arg12] : memref<?x?x?xi32>
            %45 = memref.load %arg5[%34, %42] : memref<?x?xi32>
            memref.store %45, %arg6[%arg7, %32, %40] : memref<?x?x?xi32>
          }
        }
        scf.yield %10, %9 : i32, i32
      }
    }
    return
  }
}
