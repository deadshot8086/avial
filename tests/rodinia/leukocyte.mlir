module {
  func.func @leukocyte(%arg0: i32, %arg1: i32, %arg2: i32, %arg3: memref<?x?xf64>, %arg4: memref<?x?xf64>, %arg5: memref<?x150xi32>, %arg6: memref<?x150xi32>, %arg7: memref<?xf64>, %arg8: memref<?xf64>, %arg9: memref<?x25xi32>, %arg10: memref<?x?xf64>, %arg11: memref<?x?xf64>) attributes {llvm.linkage = #llvm.linkage<external>} {
    %c0 = arith.constant 0 : index
    %c1 = arith.constant 1 : index
    %c7 = arith.constant 7 : index
    %c150 = arith.constant 150 : index
    %c25 = arith.constant 25 : index
    %c-12_i32 = arith.constant -12 : i32
    %cst = arith.constant 1.490000e+02 : f64
    %c25_i32 = arith.constant 25 : i32
    %cst_0 = arith.constant 9.9999999999999998E-13 : f64
    %cst_1 = arith.constant 1.500000e+02 : f64
    %c0_i32 = arith.constant 0 : i32
    %cst_2 = arith.constant 0.000000e+00 : f64
    %0 = llvm.mlir.undef : i32
    %1 = arith.subi %arg0, %arg2 : i32
    %2 = arith.index_cast %1 : i32 to index
    %3 = arith.index_cast %arg2 : i32 to index
    %4 = arith.subi %arg1, %arg2 : i32
    %5 = arith.index_cast %4 : i32 to index
    scf.for %arg12 = %3 to %2 step %c1 {
      %9 = arith.index_cast %arg12 : index to i32
      scf.for %arg13 = %3 to %5 step %c1 {
        %10 = arith.index_cast %arg13 : index to i32
        %11 = scf.for %arg14 = %c0 to %c7 step %c1 iter_args(%arg15 = %cst_2) -> (f64) {
          %12 = scf.for %arg16 = %c0 to %c150 step %c1 iter_args(%arg17 = %cst_2) -> (f64) {
            %20 = memref.load %arg5[%arg14, %arg16] : memref<?x150xi32>
            %21 = arith.addi %9, %20 : i32
            %22 = memref.load %arg6[%arg14, %arg16] : memref<?x150xi32>
            %23 = arith.addi %10, %22 : i32
            %24 = arith.index_cast %23 : i32 to index
            %25 = arith.index_cast %21 : i32 to index
            %26 = memref.load %arg3[%24, %25] : memref<?x?xf64>
            %27 = memref.load %arg7[%arg16] : memref<?xf64>
            %28 = arith.mulf %26, %27 : f64
            %29 = memref.load %arg4[%24, %25] : memref<?x?xf64>
            %30 = memref.load %arg8[%arg16] : memref<?xf64>
            %31 = arith.mulf %29, %30 : f64
            %32 = arith.addf %28, %31 : f64
            %33 = arith.addf %arg17, %32 : f64
            scf.yield %33 : f64
          }
          %13 = arith.divf %12, %cst_1 : f64
          %14 = scf.for %arg16 = %c0 to %c150 step %c1 iter_args(%arg17 = %cst_2) -> (f64) {
            %20 = memref.load %arg5[%arg14, %arg16] : memref<?x150xi32>
            %21 = arith.addi %9, %20 : i32
            %22 = memref.load %arg6[%arg14, %arg16] : memref<?x150xi32>
            %23 = arith.addi %10, %22 : i32
            %24 = arith.index_cast %23 : i32 to index
            %25 = arith.index_cast %21 : i32 to index
            %26 = memref.load %arg3[%24, %25] : memref<?x?xf64>
            %27 = memref.load %arg7[%arg16] : memref<?xf64>
            %28 = arith.mulf %26, %27 : f64
            %29 = memref.load %arg4[%24, %25] : memref<?x?xf64>
            %30 = memref.load %arg8[%arg16] : memref<?xf64>
            %31 = arith.mulf %29, %30 : f64
            %32 = arith.addf %28, %31 : f64
            %33 = arith.subf %32, %13 : f64
            %34 = arith.mulf %33, %33 : f64
            %35 = arith.addf %arg17, %34 : f64
            scf.yield %35 : f64
          }
          %15 = arith.divf %14, %cst : f64
          %16 = arith.cmpf ogt, %15, %cst_0 : f64
          %17 = scf.if %16 -> (f64) {
            %20 = arith.mulf %13, %13 : f64
            %21 = arith.divf %20, %15 : f64
            scf.yield %21 : f64
          } else {
            scf.yield %cst_2 : f64
          }
          %18 = arith.cmpf ogt, %17, %arg15 : f64
          %19 = arith.select %18, %17, %arg15 : f64
          scf.yield %19 : f64
        }
        memref.store %11, %arg10[%arg13, %arg12] : memref<?x?xf64>
      }
    }
    %6 = arith.index_cast %arg1 : i32 to index
    %7 = arith.index_cast %arg0 : i32 to index
    %8 = scf.for %arg12 = %c0 to %6 step %c1 iter_args(%arg13 = %0) -> (i32) {
      %9 = arith.index_cast %arg12 : index to i32
      %10 = arith.addi %9, %c-12_i32 : i32
      %11 = scf.for %arg14 = %c0 to %7 step %c1 iter_args(%arg15 = %arg13) -> (i32) {
        %12 = arith.index_cast %arg14 : index to i32
        %13 = arith.addi %12, %c-12_i32 : i32
        %14:2 = scf.for %arg16 = %c0 to %c25 step %c1 iter_args(%arg17 = %arg15, %arg18 = %cst_2) -> (i32, f64) {
          %15 = arith.index_cast %arg16 : index to i32
          %16 = arith.addi %10, %15 : i32
          %17 = arith.cmpi sge, %16, %c0_i32 : i32
          %18 = arith.cmpi slt, %16, %arg1 : i32
          %19 = arith.andi %17, %18 : i1
          %20 = arith.select %19, %c25_i32, %arg17 : i32
          %21 = scf.if %19 -> (f64) {
            %22 = arith.index_cast %16 : i32 to index
            %23 = scf.for %arg19 = %c0 to %c25 step %c1 iter_args(%arg20 = %arg18) -> (f64) {
              %24 = arith.index_cast %arg19 : index to i32
              %25 = arith.addi %13, %24 : i32
              %26 = arith.cmpi sge, %25, %c0_i32 : i32
              %27 = arith.cmpi slt, %25, %arg0 : i32
              %28 = arith.andi %26, %27 : i1
              %29 = scf.if %28 -> (f64) {
                %30 = memref.load %arg9[%arg16, %arg19] : memref<?x25xi32>
                %31 = arith.cmpi ne, %30, %c0_i32 : i32
                %32 = scf.if %31 -> (f64) {
                  %33 = arith.index_cast %25 : i32 to index
                  %34 = memref.load %arg10[%22, %33] : memref<?x?xf64>
                  %35 = arith.cmpf ogt, %34, %arg20 : f64
                  %36 = arith.select %35, %34, %arg20 : f64
                  scf.yield %36 : f64
                } else {
                  scf.yield %arg20 : f64
                }
                scf.yield %32 : f64
              } else {
                scf.yield %arg20 : f64
              }
              scf.yield %29 : f64
            }
            scf.yield %23 : f64
          } else {
            scf.yield %arg18 : f64
          }
          scf.yield %20, %21 : i32, f64
        }
        memref.store %14#1, %arg11[%arg12, %arg14] : memref<?x?xf64>
        scf.yield %14#0 : i32
      }
      scf.yield %11 : i32
    }
    return
  }
}
