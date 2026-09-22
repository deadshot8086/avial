module {
  func.func @srad(%arg0: i32, %arg1: i32, %arg2: i32, %arg3: f32, %arg4: i32, %arg5: i32, %arg6: i32, %arg7: i32, %arg8: memref<?x?xf32>, %arg9: memref<?x?xf32>, %arg10: memref<?x?xf32>, %arg11: memref<?x?xf32>, %arg12: memref<?x?xf32>, %arg13: memref<?x?xf32>) attributes {llvm.linkage = #llvm.linkage<external>} {
    %c0 = arith.constant 0 : index
    %c1 = arith.constant 1 : index
    %c-1_i32 = arith.constant -1 : i32
    %cst = arith.constant 2.550000e+02 : f64
    %cst_0 = arith.constant 1.000000e+00 : f32
    %cst_1 = arith.constant 2.500000e-01 : f64
    %cst_2 = arith.constant 1.000000e+00 : f64
    %cst_3 = arith.constant 6.250000e-02 : f64
    %cst_4 = arith.constant 5.000000e-01 : f64
    %cst_5 = arith.constant 0.000000e+00 : f32
    %c0_i32 = arith.constant 0 : i32
    %c1_i32 = arith.constant 1 : i32
    %0 = arith.subi %arg5, %arg4 : i32
    %1 = arith.addi %0, %c1_i32 : i32
    %2 = arith.subi %arg7, %arg6 : i32
    %3 = arith.addi %2, %c1_i32 : i32
    %4 = arith.muli %1, %3 : i32
    %5 = arith.sitofp %4 : i32 to f32
    %6 = arith.index_cast %arg2 : i32 to index
    %7 = arith.addi %arg5, %c1_i32 : i32
    %8 = arith.index_cast %7 : i32 to index
    %9 = arith.index_cast %arg4 : i32 to index
    %10 = arith.addi %arg7, %c1_i32 : i32
    %11 = arith.index_cast %10 : i32 to index
    %12 = arith.index_cast %arg6 : i32 to index
    %13 = arith.index_cast %arg1 : i32 to index
    %14 = arith.index_cast %arg0 : i32 to index
    %15 = arith.addi %arg0, %c-1_i32 : i32
    %16 = arith.addi %arg1, %c-1_i32 : i32
    %17 = arith.index_cast %15 : i32 to index
    %18 = arith.index_cast %16 : i32 to index
    %19 = arith.index_cast %arg0 : i32 to index
    %20 = arith.addi %arg0, %c-1_i32 : i32
    %21 = arith.addi %arg1, %c-1_i32 : i32
    %22 = arith.extf %arg3 : f32 to f64
    %23 = arith.mulf %22, %cst_1 : f64
    %24 = arith.index_cast %20 : i32 to index
    %25 = arith.index_cast %21 : i32 to index
    scf.for %arg14 = %c0 to %6 step %c1 {
      %28:2 = scf.for %arg15 = %9 to %8 step %c1 iter_args(%arg16 = %cst_5, %arg17 = %cst_5) -> (f32, f32) {
        %36:2 = scf.for %arg18 = %12 to %11 step %c1 iter_args(%arg19 = %arg16, %arg20 = %arg17) -> (f32, f32) {
          %37 = memref.load %arg8[%arg18, %arg15] : memref<?x?xf32>
          %38 = arith.addf %arg20, %37 : f32
          %39 = arith.mulf %37, %37 : f32
          %40 = arith.addf %arg19, %39 : f32
          scf.yield %40, %38 : f32, f32
        }
        scf.yield %36#0, %36#1 : f32, f32
      }
      %29 = arith.divf %28#1, %5 : f32
      %30 = arith.divf %28#0, %5 : f32
      %31 = arith.mulf %29, %29 : f32
      %32 = arith.subf %30, %31 : f32
      %33 = arith.divf %32, %31 : f32
      %34 = arith.addf %33, %cst_0 : f32
      %35 = arith.mulf %33, %34 : f32
      scf.for %arg15 = %c0 to %13 step %c1 {
        %36 = arith.index_cast %arg15 : index to i32
        %37 = arith.cmpi sgt, %36, %c0_i32 : i32
        %38 = arith.cmpi slt, %36, %16 : i32
        %39 = arith.addi %36, %c-1_i32 : i32
        %40 = arith.index_cast %39 : i32 to index
        %41 = arith.addi %36, %c1_i32 : i32
        %42 = arith.index_cast %41 : i32 to index
        scf.for %arg16 = %c0 to %14 step %c1 {
          %43 = arith.index_cast %arg16 : index to i32
          %44 = memref.load %arg8[%arg15, %arg16] : memref<?x?xf32>
          %45 = arith.cmpi sgt, %43, %c0_i32 : i32
          %46 = scf.if %45 -> (f32) {
            %91 = arith.addi %43, %c-1_i32 : i32
            %92 = arith.index_cast %91 : i32 to index
            %93 = memref.load %arg8[%arg15, %92] : memref<?x?xf32>
            scf.yield %93 : f32
          } else {
            %91 = memref.load %arg8[%arg15, %c0] : memref<?x?xf32>
            scf.yield %91 : f32
          }
          %47 = arith.subf %46, %44 : f32
          %48 = arith.cmpi slt, %43, %15 : i32
          %49 = scf.if %48 -> (f32) {
            %91 = arith.addi %43, %c1_i32 : i32
            %92 = arith.index_cast %91 : i32 to index
            %93 = memref.load %arg8[%arg15, %92] : memref<?x?xf32>
            scf.yield %93 : f32
          } else {
            %91 = memref.load %arg8[%arg15, %17] : memref<?x?xf32>
            scf.yield %91 : f32
          }
          %50 = arith.subf %49, %44 : f32
          %51 = scf.if %37 -> (f32) {
            %91 = memref.load %arg8[%40, %arg16] : memref<?x?xf32>
            scf.yield %91 : f32
          } else {
            %91 = memref.load %arg8[%c0, %arg16] : memref<?x?xf32>
            scf.yield %91 : f32
          }
          %52 = arith.subf %51, %44 : f32
          %53 = scf.if %38 -> (f32) {
            %91 = memref.load %arg8[%42, %arg16] : memref<?x?xf32>
            scf.yield %91 : f32
          } else {
            %91 = memref.load %arg8[%18, %arg16] : memref<?x?xf32>
            scf.yield %91 : f32
          }
          %54 = arith.subf %53, %44 : f32
          memref.store %47, %arg10[%arg15, %arg16] : memref<?x?xf32>
          memref.store %50, %arg11[%arg15, %arg16] : memref<?x?xf32>
          memref.store %52, %arg12[%arg15, %arg16] : memref<?x?xf32>
          memref.store %54, %arg13[%arg15, %arg16] : memref<?x?xf32>
          %55 = arith.mulf %47, %47 : f32
          %56 = arith.mulf %50, %50 : f32
          %57 = arith.addf %55, %56 : f32
          %58 = arith.mulf %52, %52 : f32
          %59 = arith.addf %57, %58 : f32
          %60 = arith.mulf %54, %54 : f32
          %61 = arith.addf %59, %60 : f32
          %62 = arith.mulf %44, %44 : f32
          %63 = arith.divf %61, %62 : f32
          %64 = arith.addf %47, %50 : f32
          %65 = arith.addf %64, %52 : f32
          %66 = arith.addf %65, %54 : f32
          %67 = arith.divf %66, %44 : f32
          %68 = arith.extf %63 : f32 to f64
          %69 = arith.mulf %68, %cst_4 : f64
          %70 = arith.mulf %67, %67 : f32
          %71 = arith.extf %70 : f32 to f64
          %72 = arith.mulf %71, %cst_3 : f64
          %73 = arith.subf %69, %72 : f64
          %74 = arith.truncf %73 : f64 to f32
          %75 = arith.extf %67 : f32 to f64
          %76 = arith.mulf %75, %cst_1 : f64
          %77 = arith.addf %76, %cst_2 : f64
          %78 = arith.truncf %77 : f64 to f32
          %79 = arith.mulf %78, %78 : f32
          %80 = arith.divf %74, %79 : f32
          %81 = arith.subf %80, %33 : f32
          %82 = arith.divf %81, %35 : f32
          %83 = arith.extf %82 : f32 to f64
          %84 = arith.addf %83, %cst_2 : f64
          %85 = arith.divf %cst_2, %84 : f64
          %86 = arith.truncf %85 : f64 to f32
          %87 = arith.cmpf olt, %86, %cst_5 : f32
          %88 = arith.select %87, %cst_5, %86 : f32
          %89 = arith.cmpf ogt, %88, %cst_0 : f32
          %90 = arith.select %89, %cst_0, %88 : f32
          memref.store %90, %arg9[%arg15, %arg16] : memref<?x?xf32>
        }
      }
      scf.for %arg15 = %c0 to %13 step %c1 {
        %36 = arith.index_cast %arg15 : index to i32
        %37 = arith.cmpi slt, %36, %21 : i32
        %38 = arith.addi %36, %c1_i32 : i32
        %39 = arith.index_cast %38 : i32 to index
        scf.for %arg16 = %c0 to %19 step %c1 {
          %40 = arith.index_cast %arg16 : index to i32
          %41 = memref.load %arg9[%arg15, %arg16] : memref<?x?xf32>
          %42 = arith.cmpi slt, %40, %20 : i32
          %43 = scf.if %42 -> (f32) {
            %62 = arith.addi %40, %c1_i32 : i32
            %63 = arith.index_cast %62 : i32 to index
            %64 = memref.load %arg9[%arg15, %63] : memref<?x?xf32>
            scf.yield %64 : f32
          } else {
            %62 = memref.load %arg9[%arg15, %24] : memref<?x?xf32>
            scf.yield %62 : f32
          }
          %44 = scf.if %37 -> (f32) {
            %62 = memref.load %arg9[%39, %arg16] : memref<?x?xf32>
            scf.yield %62 : f32
          } else {
            %62 = memref.load %arg9[%25, %arg16] : memref<?x?xf32>
            scf.yield %62 : f32
          }
          %45 = memref.load %arg10[%arg15, %arg16] : memref<?x?xf32>
          %46 = arith.mulf %41, %45 : f32
          %47 = memref.load %arg11[%arg15, %arg16] : memref<?x?xf32>
          %48 = arith.mulf %43, %47 : f32
          %49 = arith.addf %46, %48 : f32
          %50 = memref.load %arg12[%arg15, %arg16] : memref<?x?xf32>
          %51 = arith.mulf %41, %50 : f32
          %52 = arith.addf %49, %51 : f32
          %53 = memref.load %arg13[%arg15, %arg16] : memref<?x?xf32>
          %54 = arith.mulf %44, %53 : f32
          %55 = arith.addf %52, %54 : f32
          %56 = memref.load %arg8[%arg15, %arg16] : memref<?x?xf32>
          %57 = arith.extf %56 : f32 to f64
          %58 = arith.extf %55 : f32 to f64
          %59 = arith.mulf %23, %58 : f64
          %60 = arith.addf %57, %59 : f64
          %61 = arith.truncf %60 : f64 to f32
          memref.store %61, %arg8[%arg15, %arg16] : memref<?x?xf32>
        }
      }
    }
    %26 = arith.index_cast %arg1 : i32 to index
    %27 = arith.index_cast %arg0 : i32 to index
    scf.for %arg14 = %c0 to %26 step %c1 {
      scf.for %arg15 = %c0 to %27 step %c1 {
        %28 = memref.load %arg8[%arg14, %arg15] : memref<?x?xf32>
        %29 = arith.extf %28 : f32 to f64
        %30 = math.log %29 : f64
        %31 = arith.mulf %30, %cst : f64
        %32 = arith.truncf %31 : f64 to f32
        memref.store %32, %arg8[%arg14, %arg15] : memref<?x?xf32>
      }
    }
    return
  }
}
