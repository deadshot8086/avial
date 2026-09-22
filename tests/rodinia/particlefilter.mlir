module {
  func.func @particle_filter(%arg0: i32, %arg1: i32, %arg2: i32, %arg3: i32, %arg4: i32, %arg5: i32, %arg6: memref<?xi32>, %arg7: memref<?x2xi32>, %arg8: memref<?xi32>, %arg9: memref<?xf64>, %arg10: memref<?xf64>, %arg11: memref<?xf64>, %arg12: memref<?xf64>, %arg13: memref<?xf64>, %arg14: memref<?xf64>, %arg15: memref<?xf64>, %arg16: memref<?xf64>, %arg17: memref<?xf64>, %arg18: memref<?xf64>) attributes {llvm.linkage = #llvm.linkage<external>} {
    %false = arith.constant false
    %c1 = arith.constant 1 : index
    %c-100_i32 = arith.constant -100 : i32
    %c-228_i32 = arith.constant -228 : i32
    %cst = arith.constant 0x41DFFFFFFFC00000 : f64
    %true = arith.constant true
    %c-1_i32 = arith.constant -1 : i32
    %cst_0 = arith.constant 5.000000e+01 : f64
    %cst_1 = arith.constant 2.000000e+00 : f64
    %cst_2 = arith.constant 5.000000e+00 : f64
    %cst_3 = arith.constant 1.000000e+00 : f64
    %cst_4 = arith.constant -2.000000e+00 : f64
    %cst_5 = arith.constant 6.2831853071795862 : f64
    %cst_6 = arith.constant 0.000000e+00 : f64
    %c0_i32 = arith.constant 0 : i32
    %c12345_i32 = arith.constant 12345 : i32
    %c1103515245_i32 = arith.constant 1103515245 : i32
    %c2147483647_i64 = arith.constant 2147483647 : i64
    %c0 = arith.constant 0 : index
    %0 = arith.index_cast %arg5 : i32 to index
    %1 = arith.index_cast %arg0 : i32 to index
    %2 = arith.index_cast %arg1 : i32 to index
    %3 = arith.sitofp %arg1 : i32 to f64
    %4 = arith.sitofp %arg0 : i32 to f64
    %5 = arith.divf %cst_3, %4 : f64
    %6 = arith.cmpi sgt, %arg0, %c0_i32 : i32
    %7 = arith.addi %arg0, %c-1_i32 : i32
    scf.for %arg19 = %c1 to %0 step %c1 {
      %8 = arith.index_cast %arg19 : index to i32
      scf.for %arg20 = %c0 to %1 step %c1 {
        %23 = memref.load %arg8[%arg20] : memref<?xi32>
        %24 = arith.muli %23, %c1103515245_i32 : i32
        %25 = arith.addi %24, %c12345_i32 : i32
        %26 = arith.extsi %25 : i32 to i64
        %27 = arith.remsi %26, %c2147483647_i64 : i64
        %28 = arith.trunci %27 : i64 to i32
        memref.store %28, %arg8[%arg20] : memref<?xi32>
        %29 = memref.load %arg8[%arg20] : memref<?xi32>
        %30 = arith.sitofp %29 : i32 to f64
        %31 = arith.divf %30, %cst : f64
        %32 = arith.cmpf olt, %31, %cst_6 : f64
        %33 = scf.if %32 -> (f64) {
          %84 = arith.negf %31 : f64
          scf.yield %84 : f64
        } else {
          scf.yield %31 : f64
        }
        %34 = arith.muli %29, %c1103515245_i32 : i32
        %35 = arith.addi %34, %c12345_i32 : i32
        %36 = arith.extsi %35 : i32 to i64
        %37 = arith.remsi %36, %c2147483647_i64 : i64
        %38 = arith.trunci %37 : i64 to i32
        memref.store %38, %arg8[%arg20] : memref<?xi32>
        %39 = memref.load %arg8[%arg20] : memref<?xi32>
        %40 = arith.sitofp %39 : i32 to f64
        %41 = arith.divf %40, %cst : f64
        %42 = arith.cmpf olt, %41, %cst_6 : f64
        %43 = scf.if %42 -> (f64) {
          %84 = arith.negf %41 : f64
          scf.yield %84 : f64
        } else {
          scf.yield %41 : f64
        }
        %44 = arith.mulf %43, %cst_5 : f64
        %45 = math.cos %44 : f64
        %46 = math.log %33 : f64
        %47 = arith.mulf %46, %cst_4 : f64
        %48 = math.sqrt %47 : f64
        %49 = arith.mulf %48, %45 : f64
        %50 = arith.muli %39, %c1103515245_i32 : i32
        %51 = arith.addi %50, %c12345_i32 : i32
        %52 = arith.extsi %51 : i32 to i64
        %53 = arith.remsi %52, %c2147483647_i64 : i64
        %54 = arith.trunci %53 : i64 to i32
        memref.store %54, %arg8[%arg20] : memref<?xi32>
        %55 = memref.load %arg8[%arg20] : memref<?xi32>
        %56 = arith.sitofp %55 : i32 to f64
        %57 = arith.divf %56, %cst : f64
        %58 = arith.cmpf olt, %57, %cst_6 : f64
        %59 = scf.if %58 -> (f64) {
          %84 = arith.negf %57 : f64
          scf.yield %84 : f64
        } else {
          scf.yield %57 : f64
        }
        %60 = arith.muli %55, %c1103515245_i32 : i32
        %61 = arith.addi %60, %c12345_i32 : i32
        %62 = arith.extsi %61 : i32 to i64
        %63 = arith.remsi %62, %c2147483647_i64 : i64
        %64 = arith.trunci %63 : i64 to i32
        memref.store %64, %arg8[%arg20] : memref<?xi32>
        %65 = memref.load %arg8[%arg20] : memref<?xi32>
        %66 = arith.sitofp %65 : i32 to f64
        %67 = arith.divf %66, %cst : f64
        %68 = arith.cmpf olt, %67, %cst_6 : f64
        %69 = scf.if %68 -> (f64) {
          %84 = arith.negf %67 : f64
          scf.yield %84 : f64
        } else {
          scf.yield %67 : f64
        }
        %70 = arith.mulf %69, %cst_5 : f64
        %71 = math.cos %70 : f64
        %72 = math.log %59 : f64
        %73 = arith.mulf %72, %cst_4 : f64
        %74 = math.sqrt %73 : f64
        %75 = arith.mulf %74, %71 : f64
        %76 = arith.mulf %49, %cst_2 : f64
        %77 = arith.addf %76, %cst_3 : f64
        %78 = memref.load %arg9[%arg20] : memref<?xf64>
        %79 = arith.addf %78, %77 : f64
        memref.store %79, %arg9[%arg20] : memref<?xf64>
        %80 = arith.mulf %75, %cst_1 : f64
        %81 = arith.addf %80, %cst_4 : f64
        %82 = memref.load %arg10[%arg20] : memref<?xf64>
        %83 = arith.addf %82, %81 : f64
        memref.store %83, %arg10[%arg20] : memref<?xf64>
      }
      scf.for %arg20 = %c0 to %1 step %c1 {
        %23 = memref.load %arg9[%arg20] : memref<?xf64>
        %24 = arith.fptosi %23 : f64 to i32
        %25 = arith.sitofp %24 : i32 to f64
        %26 = arith.fptosi %25 : f64 to i32
        %27 = memref.load %arg10[%arg20] : memref<?xf64>
        %28 = arith.fptosi %27 : f64 to i32
        %29 = arith.sitofp %28 : i32 to f64
        %30 = arith.fptosi %29 : f64 to i32
        %31 = scf.for %arg21 = %c0 to %2 step %c1 iter_args(%arg22 = %cst_6) -> (f64) {
          %36 = memref.load %arg7[%arg21, %c1] : memref<?x2xi32>
          %37 = arith.addi %26, %36 : i32
          %38 = memref.load %arg7[%arg21, %c0] : memref<?x2xi32>
          %39 = arith.addi %30, %38 : i32
          %40 = arith.muli %37, %arg4 : i32
          %41 = arith.muli %40, %arg5 : i32
          %42 = arith.muli %39, %arg5 : i32
          %43 = arith.addi %41, %42 : i32
          %44 = arith.addi %43, %8 : i32
          %45 = arith.cmpi slt, %44, %c0_i32 : i32
          %46 = scf.if %45 -> (i32) {
            %60 = arith.subi %c0_i32, %44 : i32
            scf.yield %60 : i32
          } else {
            scf.yield %44 : i32
          }
          %47 = arith.cmpi sge, %46, %arg2 : i32
          %48 = arith.select %47, %c0_i32, %46 : i32
          %49 = arith.index_cast %48 : i32 to index
          %50 = memref.load %arg6[%49] : memref<?xi32>
          %51 = arith.addi %50, %c-100_i32 : i32
          %52 = arith.sitofp %51 : i32 to f64
          %53 = arith.addi %50, %c-228_i32 : i32
          %54 = arith.sitofp %53 : i32 to f64
          %55 = arith.mulf %52, %52 : f64
          %56 = arith.mulf %54, %54 : f64
          %57 = arith.subf %55, %56 : f64
          %58 = arith.divf %57, %cst_0 : f64
          %59 = arith.addf %arg22, %58 : f64
          scf.yield %59 : f64
        }
        %32 = arith.divf %31, %3 : f64
        memref.store %32, %arg12[%arg20] : memref<?xf64>
        %33 = memref.load %arg11[%arg20] : memref<?xf64>
        %34 = math.exp %32 : f64
        %35 = arith.mulf %33, %34 : f64
        memref.store %35, %arg11[%arg20] : memref<?xf64>
      }
      %9 = scf.for %arg20 = %c0 to %1 step %c1 iter_args(%arg21 = %cst_6) -> (f64) {
        %23 = memref.load %arg11[%arg20] : memref<?xf64>
        %24 = arith.addf %arg21, %23 : f64
        scf.yield %24 : f64
      }
      scf.for %arg20 = %c0 to %1 step %c1 {
        %23 = memref.load %arg11[%arg20] : memref<?xf64>
        %24 = arith.divf %23, %9 : f64
        memref.store %24, %arg11[%arg20] : memref<?xf64>
      }
      %10:2 = scf.for %arg20 = %c0 to %1 step %c1 iter_args(%arg21 = %cst_6, %arg22 = %cst_6) -> (f64, f64) {
        %23 = memref.load %arg9[%arg20] : memref<?xf64>
        %24 = memref.load %arg11[%arg20] : memref<?xf64>
        %25 = arith.mulf %23, %24 : f64
        %26 = arith.addf %arg22, %25 : f64
        %27 = memref.load %arg10[%arg20] : memref<?xf64>
        %28 = arith.mulf %27, %24 : f64
        %29 = arith.addf %arg21, %28 : f64
        scf.yield %29, %26 : f64, f64
      }
      memref.store %10#1, %arg17[%arg19] : memref<?xf64>
      memref.store %10#0, %arg18[%arg19] : memref<?xf64>
      %11 = affine.load %arg11[0] : memref<?xf64>
      affine.store %11, %arg13[0] : memref<?xf64>
      scf.for %arg20 = %c1 to %1 step %c1 {
        %23 = arith.index_cast %arg20 : index to i32
        %24 = arith.addi %23, %c-1_i32 : i32
        %25 = arith.index_cast %24 : i32 to index
        %26 = memref.load %arg13[%25] : memref<?xf64>
        %27 = memref.load %arg11[%arg20] : memref<?xf64>
        %28 = arith.addf %26, %27 : f64
        memref.store %28, %arg13[%arg20] : memref<?xf64>
      }
      %12 = affine.load %arg8[0] : memref<?xi32>
      %13 = arith.muli %12, %c1103515245_i32 : i32
      %14 = arith.addi %13, %c12345_i32 : i32
      %15 = arith.extsi %14 : i32 to i64
      %16 = arith.remsi %15, %c2147483647_i64 : i64
      %17 = arith.trunci %16 : i64 to i32
      affine.store %17, %arg8[0] : memref<?xi32>
      %18 = arith.sitofp %17 : i32 to f64
      %19 = arith.divf %18, %cst : f64
      %20 = arith.cmpf olt, %19, %cst_6 : f64
      %21 = scf.if %20 -> (f64) {
        %23 = arith.negf %19 : f64
        scf.yield %23 : f64
      } else {
        scf.yield %19 : f64
      }
      %22 = arith.mulf %5, %21 : f64
      scf.for %arg20 = %c0 to %1 step %c1 {
        %23 = arith.index_cast %arg20 : index to i32
        %24 = arith.sitofp %23 : i32 to f64
        %25 = arith.divf %24, %4 : f64
        %26 = arith.addf %22, %25 : f64
        memref.store %26, %arg14[%arg20] : memref<?xf64>
      }
      scf.for %arg20 = %c0 to %1 step %c1 {
        %23:2 = scf.if %6 -> (i32, i1) {
          %28 = memref.load %arg14[%arg20] : memref<?xf64>
          %29:2 = scf.for %arg21 = %c0 to %1 step %c1 iter_args(%arg22 = %c-1_i32, %arg23 = %true) -> (i32, i1) {
            %31:2 = scf.if %arg23 -> (i32, i1) {
              %32 = arith.index_cast %arg21 : index to i32
              %33 = memref.load %arg13[%arg21] : memref<?xf64>
              %34 = arith.cmpf oge, %33, %28 : f64
              %35 = arith.select %34, %32, %c-1_i32 : i32
              %36 = arith.xori %34, %true : i1
              scf.yield %35, %36 : i32, i1
            } else {
              scf.yield %arg22, %false : i32, i1
            }
            scf.yield %31#0, %31#1 : i32, i1
          }
          %30 = arith.cmpi eq, %29#0, %c-1_i32 : i32
          scf.yield %29#0, %30 : i32, i1
        } else {
          scf.yield %c-1_i32, %true : i32, i1
        }
        %24 = arith.select %23#1, %7, %23#0 : i32
        %25 = arith.index_cast %24 : i32 to index
        %26 = memref.load %arg9[%25] : memref<?xf64>
        memref.store %26, %arg15[%arg20] : memref<?xf64>
        %27 = memref.load %arg10[%25] : memref<?xf64>
        memref.store %27, %arg16[%arg20] : memref<?xf64>
      }
      scf.for %arg20 = %c0 to %1 step %c1 {
        %23 = memref.load %arg15[%arg20] : memref<?xf64>
        memref.store %23, %arg9[%arg20] : memref<?xf64>
        %24 = memref.load %arg16[%arg20] : memref<?xf64>
        memref.store %24, %arg10[%arg20] : memref<?xf64>
        memref.store %5, %arg11[%arg20] : memref<?xf64>
      }
    }
    return
  }
}
