module {
  func.func @cutcp(%arg0: i32, %arg1: memref<?x4xf32>, %arg2: i32, %arg3: i32, %arg4: i32, %arg5: memref<?x?x?xf32>, %arg6: f32, %arg7: f32, %arg8: f32, %arg9: f32, %arg10: f32, %arg11: i32, %arg12: f32, %arg13: f32) attributes {llvm.linkage = #llvm.linkage<external>} {
    %c1 = arith.constant 1 : index
    %c-1_i32 = arith.constant -1 : i32
    %cst = arith.constant 1.000000e+00 : f32
    %c1_i32 = arith.constant 1 : i32
    %cst_0 = arith.constant 0.000000e+00 : f32
    %c0_i32 = arith.constant 0 : i32
    %0 = llvm.mlir.undef : i32
    %1 = arith.index_cast %arg0 : i32 to index
    %2 = arith.addi %arg2, %c-1_i32 : i32
    %3 = arith.addi %arg3, %c-1_i32 : i32
    %4 = arith.addi %arg4, %c-1_i32 : i32
    %5:2 = affine.for %arg14 = 0 to %1 iter_args(%arg15 = %0, %arg16 = %0) -> (i32, i32) {
      %6 = affine.load %arg1[%arg14, 3] : memref<?x4xf32>
      %7 = arith.cmpf une, %6, %cst_0 : f32
      %8:2 = scf.if %7 -> (i32, i32) {
        %9 = affine.load %arg1[%arg14, 0] : memref<?x4xf32>
        %10 = arith.subf %9, %arg6 : f32
        %11 = affine.load %arg1[%arg14, 1] : memref<?x4xf32>
        %12 = arith.subf %11, %arg7 : f32
        %13 = affine.load %arg1[%arg14, 2] : memref<?x4xf32>
        %14 = arith.subf %13, %arg8 : f32
        %15 = arith.mulf %10, %arg10 : f32
        %16 = arith.fptosi %15 : f32 to i32
        %17 = arith.mulf %12, %arg10 : f32
        %18 = arith.fptosi %17 : f32 to i32
        %19 = arith.mulf %14, %arg10 : f32
        %20 = arith.fptosi %19 : f32 to i32
        %21 = arith.subi %16, %arg11 : i32
        %22 = arith.addi %16, %arg11 : i32
        %23 = arith.addi %22, %c1_i32 : i32
        %24 = arith.subi %18, %arg11 : i32
        %25 = arith.addi %18, %arg11 : i32
        %26 = arith.addi %25, %c1_i32 : i32
        %27 = arith.subi %20, %arg11 : i32
        %28 = arith.addi %20, %arg11 : i32
        %29 = arith.addi %28, %c1_i32 : i32
        %30 = arith.cmpi slt, %21, %c0_i32 : i32
        %31 = arith.select %30, %c0_i32, %21 : i32
        %32 = arith.cmpi sge, %23, %arg2 : i32
        %33 = arith.select %32, %2, %23 : i32
        %34 = arith.cmpi slt, %24, %c0_i32 : i32
        %35 = arith.select %34, %c0_i32, %24 : i32
        %36 = arith.cmpi sge, %26, %arg3 : i32
        %37 = arith.select %36, %3, %26 : i32
        %38 = arith.cmpi slt, %27, %c0_i32 : i32
        %39 = arith.select %38, %c0_i32, %27 : i32
        %40 = arith.cmpi sge, %29, %arg4 : i32
        %41 = arith.select %40, %4, %29 : i32
        %42 = arith.sitofp %31 : i32 to f32
        %43 = arith.mulf %42, %arg9 : f32
        %44 = arith.subf %43, %10 : f32
        %45 = arith.sitofp %35 : i32 to f32
        %46 = arith.mulf %45, %arg9 : f32
        %47 = arith.subf %46, %12 : f32
        %48 = arith.sitofp %39 : i32 to f32
        %49 = arith.mulf %48, %arg9 : f32
        %50 = arith.subf %49, %14 : f32
        %51 = arith.addi %41, %c1_i32 : i32
        %52 = arith.index_cast %51 : i32 to index
        %53 = arith.index_cast %39 : i32 to index
        %54 = arith.addi %37, %c1_i32 : i32
        %55 = arith.index_cast %54 : i32 to index
        %56 = arith.index_cast %35 : i32 to index
        %57 = arith.addi %33, %c1_i32 : i32
        %58 = arith.index_cast %57 : i32 to index
        %59 = arith.index_cast %31 : i32 to index
        %60:3 = scf.for %arg17 = %53 to %52 step %c1 iter_args(%arg18 = %arg15, %arg19 = %50, %arg20 = %arg16) -> (i32, f32, i32) {
          %61 = arith.mulf %arg19, %arg19 : f32
          %62:2 = scf.for %arg21 = %56 to %55 step %c1 iter_args(%arg22 = %47, %arg23 = %arg18) -> (f32, i32) {
            %64 = arith.mulf %arg22, %arg22 : f32
            %65 = arith.addf %64, %61 : f32
            %66 = scf.for %arg24 = %59 to %58 step %c1 iter_args(%arg25 = %44) -> (f32) {
              %68 = arith.mulf %arg25, %arg25 : f32
              %69 = arith.addf %68, %65 : f32
              %70 = arith.cmpf olt, %69, %arg12 : f32
              scf.if %70 {
                %72 = arith.mulf %69, %arg13 : f32
                %73 = arith.subf %cst, %72 : f32
                %74 = math.sqrt %69 : f32
                %75 = arith.divf %cst, %74 : f32
                %76 = arith.mulf %6, %75 : f32
                %77 = arith.mulf %76, %73 : f32
                %78 = arith.mulf %77, %73 : f32
                %79 = memref.load %arg5[%arg17, %arg21, %arg24] : memref<?x?x?xf32>
                %80 = arith.addf %79, %78 : f32
                memref.store %80, %arg5[%arg17, %arg21, %arg24] : memref<?x?x?xf32>
              }
              %71 = arith.addf %arg25, %arg9 : f32
              scf.yield %71 : f32
            }
            %67 = arith.addf %arg22, %arg9 : f32
            scf.yield %67, %57 : f32, i32
          }
          %63 = arith.addf %arg19, %arg9 : f32
          scf.yield %62#1, %63, %54 : i32, f32, i32
        }
        scf.yield %60#0, %60#2 : i32, i32
      } else {
        scf.yield %arg15, %arg16 : i32, i32
      }
      affine.yield %8#0, %8#1 : i32, i32
    }
    return
  }
}
