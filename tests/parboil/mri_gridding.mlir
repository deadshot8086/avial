module {
  func.func @mri_gridding(%arg0: i32, %arg1: i32, %arg2: i32, %arg3: i32, %arg4: f32, %arg5: f32, %arg6: f32, %arg7: f32, %arg8: i32, %arg9: i32, %arg10: memref<?x6xf32>, %arg11: memref<?xf32>, %arg12: memref<?x?x?x2xf32>, %arg13: memref<?x?x?xf32>) attributes {llvm.linkage = #llvm.linkage<external>} {
    %c0 = arith.constant 0 : index
    %c1 = arith.constant 1 : index
    %c-1_i32 = arith.constant -1 : i32
    %true = arith.constant true
    %cst = arith.constant 1.000000e+00 : f32
    %cst_0 = arith.constant 3476263.25 : f32
    %cst_1 = arith.constant 3076.46924 : f32
    %cst_2 = arith.constant 1.44048294E+9 : f32
    %cst_3 = arith.constant 0x4DAA0FB4 : f32
    %cst_4 = arith.constant 0x4BA51CAB : f32
    %cst_5 = arith.constant 571661.125 : f32
    %cst_6 = arith.constant 8307.92578 : f32
    %cst_7 = arith.constant 75.433731 : f32
    %cst_8 = arith.constant 0.463076293 : f32
    %cst_9 = arith.constant 0.00202591089 : f32
    %cst_10 = arith.constant 6.54858376E-6 : f32
    %cst_11 = arith.constant 1.60224687E-8 : f32
    %cst_12 = arith.constant 3.0093112E-11 : f32
    %cst_13 = arith.constant 4.35125975E-14 : f32
    %cst_14 = arith.constant 4.79440269E-17 : f32
    %cst_15 = arith.constant 3.80715241E-20 : f32
    %cst_16 = arith.constant 2.10580723E-23 : f32
    %c1_i32 = arith.constant 1 : i32
    %cst_17 = arith.constant 1.000000e+00 : f64
    %cst_18 = arith.constant 0.000000e+00 : f32
    %c0_i32 = arith.constant 0 : i32
    %0 = llvm.mlir.undef : i32
    %1 = arith.index_cast %arg0 : i32 to index
    %2 = arith.sitofp %arg1 : i32 to f64
    %3 = arith.subf %2, %cst_17 : f64
    %4 = arith.sitofp %arg2 : i32 to f64
    %5 = arith.subf %4, %cst_17 : f64
    %6 = arith.sitofp %arg3 : i32 to f64
    %7 = arith.subf %6, %cst_17 : f64
    %8 = arith.addi %arg1, %c-1_i32 : i32
    %9 = arith.addi %arg2, %c-1_i32 : i32
    %10 = arith.addi %arg3, %c-1_i32 : i32
    %11 = arith.cmpi ne, %arg9, %c0_i32 : i32
    %12 = arith.sitofp %arg8 : i32 to f32
    %13 = arith.extf %arg7 : f32 to f64
    %14:2 = affine.for %arg14 = 0 to %1 iter_args(%arg15 = %0, %arg16 = %0) -> (i32, i32) {
      %15 = affine.load %arg10[%arg14, 0] : memref<?x6xf32>
      %16 = affine.load %arg10[%arg14, 1] : memref<?x6xf32>
      %17 = affine.load %arg10[%arg14, 2] : memref<?x6xf32>
      %18 = affine.load %arg10[%arg14, 3] : memref<?x6xf32>
      %19 = affine.load %arg10[%arg14, 4] : memref<?x6xf32>
      %20 = affine.load %arg10[%arg14, 5] : memref<?x6xf32>
      %21 = arith.subf %17, %arg4 : f32
      %22 = arith.addf %17, %arg4 : f32
      %23 = arith.subf %18, %arg4 : f32
      %24 = arith.addf %18, %arg4 : f32
      %25 = arith.subf %19, %arg4 : f32
      %26 = arith.addf %19, %arg4 : f32
      %27 = arith.cmpf olt, %21, %cst_18 : f32
      %28 = scf.if %27 -> (i32) {
        scf.yield %c0_i32 : i32
      } else {
        %47 = arith.fptosi %21 : f32 to i32
        scf.yield %47 : i32
      }
      %29 = arith.extf %22 : f32 to f64
      %30 = arith.cmpf ogt, %29, %3 : f64
      %31 = scf.if %30 -> (i32) {
        scf.yield %8 : i32
      } else {
        %47 = arith.fptosi %22 : f32 to i32
        scf.yield %47 : i32
      }
      %32 = arith.cmpf olt, %23, %cst_18 : f32
      %33 = scf.if %32 -> (i32) {
        scf.yield %c0_i32 : i32
      } else {
        %47 = arith.fptosi %23 : f32 to i32
        scf.yield %47 : i32
      }
      %34 = arith.extf %24 : f32 to f64
      %35 = arith.cmpf ogt, %34, %5 : f64
      %36 = scf.if %35 -> (i32) {
        scf.yield %9 : i32
      } else {
        %47 = arith.fptosi %24 : f32 to i32
        scf.yield %47 : i32
      }
      %37 = arith.cmpf olt, %25, %cst_18 : f32
      %38 = scf.if %37 -> (i32) {
        scf.yield %c0_i32 : i32
      } else {
        %47 = arith.fptosi %25 : f32 to i32
        scf.yield %47 : i32
      }
      %39 = arith.extf %26 : f32 to f64
      %40 = arith.cmpf ogt, %39, %7 : f64
      %41 = scf.if %40 -> (i32) {
        scf.yield %10 : i32
      } else {
        %47 = arith.fptosi %26 : f32 to i32
        scf.yield %47 : i32
      }
      %42 = arith.cmpf une, %15, %cst_18 : f32
      %43 = scf.if %42 -> (i1) {
        scf.yield %true : i1
      } else {
        %47 = arith.cmpf une, %16, %cst_18 : f32
        scf.yield %47 : i1
      }
      %44 = arith.cmpf une, %20, %cst_18 : f32
      %45 = arith.andi %43, %44 : i1
      %46:2 = scf.if %45 -> (i32, i32) {
        %47 = arith.addi %41, %c1_i32 : i32
        %48 = arith.index_cast %47 : i32 to index
        %49 = arith.index_cast %38 : i32 to index
        %50 = arith.addi %36, %c1_i32 : i32
        %51 = arith.index_cast %50 : i32 to index
        %52 = arith.index_cast %33 : i32 to index
        %53 = arith.addi %31, %c1_i32 : i32
        %54 = arith.index_cast %53 : i32 to index
        %55 = arith.index_cast %28 : i32 to index
        %56:2 = scf.for %arg17 = %49 to %48 step %c1 iter_args(%arg18 = %arg15, %arg19 = %arg16) -> (i32, i32) {
          %57 = arith.index_cast %arg17 : index to i32
          %58 = arith.sitofp %57 : i32 to f32
          %59 = arith.subf %19, %58 : f32
          %60 = arith.mulf %59, %59 : f32
          %61 = arith.cmpf olt, %60, %arg5 : f32
          %62 = arith.select %61, %50, %arg19 : i32
          %63 = scf.if %61 -> (i32) {
            %64 = scf.for %arg20 = %52 to %51 step %c1 iter_args(%arg21 = %arg18) -> (i32) {
              %65 = arith.index_cast %arg20 : index to i32
              %66 = arith.sitofp %65 : i32 to f32
              %67 = arith.subf %18, %66 : f32
              %68 = arith.mulf %67, %67 : f32
              %69 = arith.addf %60, %68 : f32
              %70 = arith.cmpf olt, %69, %arg5 : f32
              %71 = arith.select %70, %53, %arg21 : i32
              scf.if %70 {
                scf.for %arg22 = %55 to %54 step %c1 {
                  %72 = arith.index_cast %arg22 : index to i32
                  %73 = arith.sitofp %72 : i32 to f32
                  %74 = arith.subf %17, %73 : f32
                  %75 = arith.mulf %74, %74 : f32
                  %76 = arith.addf %69, %75 : f32
                  %77 = arith.cmpf olt, %76, %arg5 : f32
                  scf.if %77 {
                    %78 = scf.if %11 -> (f32) {
                      %87 = arith.mulf %76, %12 : f32
                      %88 = arith.mulf %87, %arg6 : f32
                      %89 = arith.fptosi %88 : f32 to i32
                      %90 = arith.sitofp %89 : i32 to f32
                      %91 = arith.divf %90, %arg6 : f32
                      %92 = arith.index_cast %89 : i32 to index
                      %93 = memref.load %arg11[%92] : memref<?xf32>
                      %94 = arith.subf %87, %91 : f32
                      %95 = arith.addi %89, %c1_i32 : i32
                      %96 = arith.index_cast %95 : i32 to index
                      %97 = memref.load %arg11[%96] : memref<?xf32>
                      %98 = arith.subf %97, %93 : f32
                      %99 = arith.mulf %94, %98 : f32
                      %100 = arith.divf %99, %arg6 : f32
                      %101 = arith.addf %93, %100 : f32
                      %102 = arith.mulf %101, %20 : f32
                      scf.yield %102 : f32
                    } else {
                      %87 = arith.mulf %76, %arg6 : f32
                      %88 = arith.extf %87 : f32 to f64
                      %89 = arith.subf %cst_17, %88 : f64
                      %90 = math.sqrt %89 : f64
                      %91 = arith.mulf %13, %90 : f64
                      %92 = arith.truncf %91 : f64 to f32
                      %93 = arith.mulf %92, %92 : f32
                      %94 = arith.mulf %93, %cst_16 : f32
                      %95 = arith.addf %94, %cst_15 : f32
                      %96 = arith.mulf %93, %95 : f32
                      %97 = arith.addf %96, %cst_14 : f32
                      %98 = arith.mulf %93, %97 : f32
                      %99 = arith.addf %98, %cst_13 : f32
                      %100 = arith.mulf %93, %99 : f32
                      %101 = arith.addf %100, %cst_12 : f32
                      %102 = arith.mulf %93, %101 : f32
                      %103 = arith.addf %102, %cst_11 : f32
                      %104 = arith.mulf %93, %103 : f32
                      %105 = arith.addf %104, %cst_10 : f32
                      %106 = arith.mulf %93, %105 : f32
                      %107 = arith.addf %106, %cst_9 : f32
                      %108 = arith.mulf %93, %107 : f32
                      %109 = arith.addf %108, %cst_8 : f32
                      %110 = arith.mulf %93, %109 : f32
                      %111 = arith.addf %110, %cst_7 : f32
                      %112 = arith.mulf %93, %111 : f32
                      %113 = arith.addf %112, %cst_6 : f32
                      %114 = arith.mulf %93, %113 : f32
                      %115 = arith.addf %114, %cst_5 : f32
                      %116 = arith.mulf %93, %115 : f32
                      %117 = arith.addf %116, %cst_4 : f32
                      %118 = arith.mulf %93, %117 : f32
                      %119 = arith.addf %118, %cst_3 : f32
                      %120 = arith.mulf %93, %119 : f32
                      %121 = arith.addf %120, %cst_2 : f32
                      %122 = arith.subf %93, %cst_1 : f32
                      %123 = arith.mulf %93, %122 : f32
                      %124 = arith.addf %123, %cst_0 : f32
                      %125 = arith.mulf %93, %124 : f32
                      %126 = arith.subf %125, %cst_2 : f32
                      %127 = arith.negf %121 : f32
                      %128 = arith.divf %127, %126 : f32
                      %129 = arith.mulf %128, %20 : f32
                      scf.yield %129 : f32
                    }
                    %79 = arith.mulf %78, %15 : f32
                    %80 = memref.load %arg12[%arg17, %arg20, %arg22, %c0] : memref<?x?x?x2xf32>
                    %81 = arith.addf %80, %79 : f32
                    memref.store %81, %arg12[%arg17, %arg20, %arg22, %c0] : memref<?x?x?x2xf32>
                    %82 = arith.mulf %78, %16 : f32
                    %83 = memref.load %arg12[%arg17, %arg20, %arg22, %c1] : memref<?x?x?x2xf32>
                    %84 = arith.addf %83, %82 : f32
                    memref.store %84, %arg12[%arg17, %arg20, %arg22, %c1] : memref<?x?x?x2xf32>
                    %85 = memref.load %arg13[%arg17, %arg20, %arg22] : memref<?x?x?xf32>
                    %86 = arith.addf %85, %cst : f32
                    memref.store %86, %arg13[%arg17, %arg20, %arg22] : memref<?x?x?xf32>
                  }
                }
              }
              scf.yield %71 : i32
            }
            scf.yield %64 : i32
          } else {
            scf.yield %arg18 : i32
          }
          scf.yield %63, %62 : i32, i32
        }
        scf.yield %56#0, %56#1 : i32, i32
      } else {
        scf.yield %arg15, %arg16 : i32, i32
      }
      affine.yield %46#0, %46#1 : i32, i32
    }
    return
  }
}
