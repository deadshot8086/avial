module {
  func.func @sad(%arg0: i32, %arg1: i32, %arg2: i32, %arg3: i32, %arg4: memref<?x?xi16>, %arg5: memref<?x?xi16>, %arg6: memref<?x44772xi16>) attributes {llvm.linkage = #llvm.linkage<external>} {
    %c0 = arith.constant 0 : index
    %c1 = arith.constant 1 : index
    %c33 = arith.constant 33 : index
    %c-16_i32 = arith.constant -16 : i32
    %c-1_i32 = arith.constant -1 : i32
    %c4 = arith.constant 4 : index
    %c2 = arith.constant 2 : index
    %c1089 = arith.constant 1089 : index
    %c28392_i32 = arith.constant 28392 : i32
    %c37128_i32 = arith.constant 37128 : i32
    %c36036_i32 = arith.constant 36036 : i32
    %c42588_i32 = arith.constant 42588 : i32
    %c33_i32 = arith.constant 33 : i32
    %c43680_i32 = arith.constant 43680 : i32
    %c41496_i32 = arith.constant 41496 : i32
    %c39312_i32 = arith.constant 39312 : i32
    %c34944_i32 = arith.constant 34944 : i32
    %c2184_i32 = arith.constant 2184 : i32
    %c26208_i32 = arith.constant 26208 : i32
    %c17472_i32 = arith.constant 17472 : i32
    %c4368_i32 = arith.constant 4368 : i32
    %c8_i32 = arith.constant 8 : i32
    %c2_i32 = arith.constant 2 : i32
    %c1092_i32 = arith.constant 1092 : i32
    %c16_i32 = arith.constant 16 : i32
    %c0_i16 = arith.constant 0 : i16
    %c4_i32 = arith.constant 4 : i32
    %c0_i32 = arith.constant 0 : i32
    %0 = arith.muli %arg0, %arg1 : i32
    %1 = arith.index_cast %arg1 : i32 to index
    %2 = arith.index_cast %arg0 : i32 to index
    %3 = arith.addi %arg3, %c-1_i32 : i32
    %4 = arith.addi %arg2, %c-1_i32 : i32
    scf.for %arg7 = %c0 to %1 step %c1 {
      %6 = arith.index_cast %arg7 : index to i32
      %7 = arith.muli %6, %arg0 : i32
      %8 = arith.muli %6, %c16_i32 : i32
      scf.for %arg8 = %c0 to %2 step %c1 {
        %9 = arith.index_cast %arg8 : index to i32
        %10 = arith.addi %7, %9 : i32
        %11 = arith.muli %9, %c16_i32 : i32
        %12 = arith.index_cast %10 : i32 to index
        scf.for %arg9 = %c0 to %c33 step %c1 {
          %13 = arith.index_cast %arg9 : index to i32
          %14 = arith.muli %13, %c33_i32 : i32
          %15 = arith.addi %13, %c-16_i32 : i32
          %16 = arith.addi %8, %15 : i32
          scf.for %arg10 = %c0 to %c33 step %c1 {
            %17 = arith.index_cast %arg10 : index to i32
            %18 = arith.addi %14, %17 : i32
            %19 = arith.addi %17, %c-16_i32 : i32
            %20 = arith.addi %11, %19 : i32
            scf.for %arg11 = %c0 to %c4 step %c1 {
              %21 = arith.index_cast %arg11 : index to i32
              %22 = arith.muli %21, %c4_i32 : i32
              %23 = arith.addi %16, %22 : i32
              %24 = arith.addi %8, %22 : i32
              %25 = arith.muli %21, %c4_i32 : i32
              scf.for %arg12 = %c0 to %c4 step %c1 {
                %26 = arith.index_cast %arg12 : index to i32
                %27 = arith.muli %26, %c4_i32 : i32
                %28 = arith.addi %20, %27 : i32
                %29 = arith.addi %11, %27 : i32
                %30 = scf.for %arg13 = %c0 to %c4 step %c1 iter_args(%arg14 = %c0_i16) -> (i16) {
                  %35 = arith.index_cast %arg13 : index to i32
                  %36 = arith.addi %23, %35 : i32
                  %37 = arith.cmpi slt, %36, %c0_i32 : i32
                  %38 = arith.select %37, %c0_i32, %36 : i32
                  %39 = arith.cmpi sge, %38, %arg2 : i32
                  %40 = arith.addi %24, %35 : i32
                  %41 = arith.index_cast %40 : i32 to index
                  %42 = arith.select %39, %4, %38 : i32
                  %43 = arith.index_cast %42 : i32 to index
                  %44 = scf.for %arg15 = %c0 to %c4 step %c1 iter_args(%arg16 = %arg14) -> (i16) {
                    %45 = arith.index_cast %arg15 : index to i32
                    %46 = arith.addi %28, %45 : i32
                    %47 = arith.cmpi slt, %46, %c0_i32 : i32
                    %48 = arith.select %47, %c0_i32, %46 : i32
                    %49 = arith.cmpi sge, %48, %arg3 : i32
                    %50 = arith.select %49, %3, %48 : i32
                    %51 = arith.index_cast %50 : i32 to index
                    %52 = memref.load %arg5[%43, %51] : memref<?x?xi16>
                    %53 = arith.extui %52 : i16 to i32
                    %54 = arith.addi %29, %45 : i32
                    %55 = arith.index_cast %54 : i32 to index
                    %56 = memref.load %arg4[%41, %55] : memref<?x?xi16>
                    %57 = arith.extui %56 : i16 to i32
                    %58 = arith.cmpi ugt, %57, %53 : i32
                    %59 = scf.if %58 -> (i32) {
                      %63 = arith.subi %57, %53 : i32
                      scf.yield %63 : i32
                    } else {
                      %63 = arith.subi %53, %57 : i32
                      scf.yield %63 : i32
                    }
                    %60 = arith.extui %arg16 : i16 to i32
                    %61 = arith.addi %60, %59 : i32
                    %62 = arith.trunci %61 : i32 to i16
                    scf.yield %62 : i16
                  }
                  scf.yield %44 : i16
                }
                %31 = arith.addi %25, %26 : i32
                %32 = arith.muli %31, %c1092_i32 : i32
                %33 = arith.addi %32, %18 : i32
                %34 = arith.index_cast %33 : i32 to index
                memref.store %30, %arg6[%12, %34] : memref<?x44772xi16>
              }
            }
          }
        }
      }
    }
    %5 = arith.index_cast %0 : i32 to index
    scf.for %arg7 = %c0 to %5 step %c1 {
      scf.for %arg8 = %c0 to %c2 step %c1 {
        %6 = arith.index_cast %arg8 : index to i32
        %7 = arith.muli %6, %c8_i32 : i32
        %8 = arith.muli %6, %c4_i32 : i32
        scf.for %arg9 = %c0 to %c4 step %c1 {
          %9 = arith.index_cast %arg9 : index to i32
          %10 = arith.addi %7, %9 : i32
          %11 = arith.muli %10, %c1092_i32 : i32
          %12 = arith.addi %11, %c4368_i32 : i32
          %13 = arith.addi %8, %9 : i32
          %14 = arith.muli %13, %c1092_i32 : i32
          %15 = arith.addi %14, %c17472_i32 : i32
          scf.for %arg10 = %c0 to %c1089 step %c1 {
            %16 = arith.index_cast %arg10 : index to i32
            %17 = arith.addi %15, %16 : i32
            %18 = arith.index_cast %17 : i32 to index
            %19 = arith.addi %11, %16 : i32
            %20 = arith.index_cast %19 : i32 to index
            %21 = memref.load %arg6[%arg7, %20] : memref<?x44772xi16>
            %22 = arith.extui %21 : i16 to i32
            %23 = arith.addi %12, %16 : i32
            %24 = arith.index_cast %23 : i32 to index
            %25 = memref.load %arg6[%arg7, %24] : memref<?x44772xi16>
            %26 = arith.extui %25 : i16 to i32
            %27 = arith.addi %22, %26 : i32
            %28 = arith.trunci %27 : i32 to i16
            memref.store %28, %arg6[%arg7, %18] : memref<?x44772xi16>
          }
        }
      }
      scf.for %arg8 = %c0 to %c4 step %c1 {
        %6 = arith.index_cast %arg8 : index to i32
        %7 = arith.muli %6, %c4_i32 : i32
        %8 = arith.muli %6, %c2_i32 : i32
        scf.for %arg9 = %c0 to %c2 step %c1 {
          %9 = arith.index_cast %arg9 : index to i32
          %10 = arith.muli %9, %c2_i32 : i32
          %11 = arith.addi %7, %10 : i32
          %12 = arith.muli %11, %c1092_i32 : i32
          %13 = arith.addi %12, %c1092_i32 : i32
          %14 = arith.addi %8, %9 : i32
          %15 = arith.muli %14, %c1092_i32 : i32
          %16 = arith.addi %15, %c26208_i32 : i32
          scf.for %arg10 = %c0 to %c1089 step %c1 {
            %17 = arith.index_cast %arg10 : index to i32
            %18 = arith.addi %16, %17 : i32
            %19 = arith.index_cast %18 : i32 to index
            %20 = arith.addi %12, %17 : i32
            %21 = arith.index_cast %20 : i32 to index
            %22 = memref.load %arg6[%arg7, %21] : memref<?x44772xi16>
            %23 = arith.extui %22 : i16 to i32
            %24 = arith.addi %13, %17 : i32
            %25 = arith.index_cast %24 : i32 to index
            %26 = memref.load %arg6[%arg7, %25] : memref<?x44772xi16>
            %27 = arith.extui %26 : i16 to i32
            %28 = arith.addi %23, %27 : i32
            %29 = arith.trunci %28 : i32 to i16
            memref.store %29, %arg6[%arg7, %19] : memref<?x44772xi16>
          }
        }
      }
      scf.for %arg8 = %c0 to %c2 step %c1 {
        %6 = arith.index_cast %arg8 : index to i32
        %7 = arith.muli %6, %c4_i32 : i32
        %8 = arith.muli %6, %c2_i32 : i32
        scf.for %arg9 = %c0 to %c2 step %c1 {
          %9 = arith.index_cast %arg9 : index to i32
          %10 = arith.addi %7, %9 : i32
          %11 = arith.muli %10, %c1092_i32 : i32
          %12 = arith.addi %11, %c26208_i32 : i32
          %13 = arith.addi %11, %c28392_i32 : i32
          %14 = arith.addi %8, %9 : i32
          %15 = arith.muli %14, %c1092_i32 : i32
          %16 = arith.addi %15, %c34944_i32 : i32
          scf.for %arg10 = %c0 to %c1089 step %c1 {
            %17 = arith.index_cast %arg10 : index to i32
            %18 = arith.addi %16, %17 : i32
            %19 = arith.index_cast %18 : i32 to index
            %20 = arith.addi %12, %17 : i32
            %21 = arith.index_cast %20 : i32 to index
            %22 = memref.load %arg6[%arg7, %21] : memref<?x44772xi16>
            %23 = arith.extui %22 : i16 to i32
            %24 = arith.addi %13, %17 : i32
            %25 = arith.index_cast %24 : i32 to index
            %26 = memref.load %arg6[%arg7, %25] : memref<?x44772xi16>
            %27 = arith.extui %26 : i16 to i32
            %28 = arith.addi %23, %27 : i32
            %29 = arith.trunci %28 : i32 to i16
            memref.store %29, %arg6[%arg7, %19] : memref<?x44772xi16>
          }
        }
      }
      scf.for %arg8 = %c0 to %c2 step %c1 {
        %6 = arith.index_cast %arg8 : index to i32
        %7 = arith.muli %6, %c1092_i32 : i32
        %8 = arith.addi %7, %c34944_i32 : i32
        %9 = arith.addi %7, %c37128_i32 : i32
        %10 = arith.addi %7, %c39312_i32 : i32
        scf.for %arg9 = %c0 to %c1089 step %c1 {
          %11 = arith.index_cast %arg9 : index to i32
          %12 = arith.addi %10, %11 : i32
          %13 = arith.index_cast %12 : i32 to index
          %14 = arith.addi %8, %11 : i32
          %15 = arith.index_cast %14 : i32 to index
          %16 = memref.load %arg6[%arg7, %15] : memref<?x44772xi16>
          %17 = arith.extui %16 : i16 to i32
          %18 = arith.addi %9, %11 : i32
          %19 = arith.index_cast %18 : i32 to index
          %20 = memref.load %arg6[%arg7, %19] : memref<?x44772xi16>
          %21 = arith.extui %20 : i16 to i32
          %22 = arith.addi %17, %21 : i32
          %23 = arith.trunci %22 : i32 to i16
          memref.store %23, %arg6[%arg7, %13] : memref<?x44772xi16>
        }
      }
      scf.for %arg8 = %c0 to %c2 step %c1 {
        %6 = arith.index_cast %arg8 : index to i32
        %7 = arith.muli %6, %c2184_i32 : i32
        %8 = arith.addi %7, %c34944_i32 : i32
        %9 = arith.addi %7, %c36036_i32 : i32
        %10 = arith.muli %6, %c1092_i32 : i32
        %11 = arith.addi %10, %c41496_i32 : i32
        scf.for %arg9 = %c0 to %c1089 step %c1 {
          %12 = arith.index_cast %arg9 : index to i32
          %13 = arith.addi %11, %12 : i32
          %14 = arith.index_cast %13 : i32 to index
          %15 = arith.addi %8, %12 : i32
          %16 = arith.index_cast %15 : i32 to index
          %17 = memref.load %arg6[%arg7, %16] : memref<?x44772xi16>
          %18 = arith.extui %17 : i16 to i32
          %19 = arith.addi %9, %12 : i32
          %20 = arith.index_cast %19 : i32 to index
          %21 = memref.load %arg6[%arg7, %20] : memref<?x44772xi16>
          %22 = arith.extui %21 : i16 to i32
          %23 = arith.addi %18, %22 : i32
          %24 = arith.trunci %23 : i32 to i16
          memref.store %24, %arg6[%arg7, %14] : memref<?x44772xi16>
        }
      }
      scf.for %arg8 = %c0 to %c1089 step %c1 {
        %6 = arith.index_cast %arg8 : index to i32
        %7 = arith.addi %6, %c43680_i32 : i32
        %8 = arith.index_cast %7 : i32 to index
        %9 = arith.addi %6, %c41496_i32 : i32
        %10 = arith.index_cast %9 : i32 to index
        %11 = memref.load %arg6[%arg7, %10] : memref<?x44772xi16>
        %12 = arith.extui %11 : i16 to i32
        %13 = arith.addi %6, %c42588_i32 : i32
        %14 = arith.index_cast %13 : i32 to index
        %15 = memref.load %arg6[%arg7, %14] : memref<?x44772xi16>
        %16 = arith.extui %15 : i16 to i32
        %17 = arith.addi %12, %16 : i32
        %18 = arith.trunci %17 : i32 to i16
        memref.store %18, %arg6[%arg7, %8] : memref<?x44772xi16>
      }
    }
    return
  }
}
