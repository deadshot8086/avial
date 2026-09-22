module {
  func.func @lavaMD_box_interaction(%arg0: f64, %arg1: i32, %arg2: i32, %arg3: memref<?xi32>, %arg4: memref<?x?xi32>, %arg5: memref<?x100x4xf64>, %arg6: memref<?x100xf64>, %arg7: memref<?x100x4xf64>) attributes {llvm.linkage = #llvm.linkage<external>} {
    %c0 = arith.constant 0 : index
    %c1 = arith.constant 1 : index
    %c-1_i32 = arith.constant -1 : i32
    %cst = arith.constant 0.000000e+00 : f64
    %c1_i32 = arith.constant 1 : i32
    %c0_i32 = arith.constant 0 : i32
    %cst_0 = arith.constant 2.000000e+00 : f64
    %c2 = arith.constant 2 : index
    %c3 = arith.constant 3 : index
    %0 = arith.mulf %arg0, %cst_0 : f64
    %1 = arith.mulf %0, %arg0 : f64
    %2 = arith.index_cast %arg1 : i32 to index
    affine.for %arg8 = 0 to %2 {
      %3 = arith.index_cast %arg8 : index to i32
      %4 = affine.load %arg3[%arg8] : memref<?xi32>
      %5 = arith.addi %4, %c1_i32 : i32
      %6 = arith.index_cast %5 : i32 to index
      scf.for %arg9 = %c0 to %6 step %c1 {
        %7 = arith.index_cast %arg9 : index to i32
        %8 = arith.cmpi eq, %7, %c0_i32 : i32
        %9 = scf.if %8 -> (i32) {
          scf.yield %3 : i32
        } else {
          %11 = arith.addi %7, %c-1_i32 : i32
          %12 = arith.index_cast %11 : i32 to index
          %13 = memref.load %arg4[%arg8, %12] : memref<?x?xi32>
          scf.yield %13 : i32
        }
        %10 = arith.index_cast %9 : i32 to index
        affine.for %arg10 = 0 to 100 {
          %11 = affine.load %arg5[%arg8, %arg10, 0] : memref<?x100x4xf64>
          %12 = affine.load %arg5[%arg8, %arg10, 1] : memref<?x100x4xf64>
          %13 = affine.load %arg5[%arg8, %arg10, 2] : memref<?x100x4xf64>
          %14 = affine.load %arg5[%arg8, %arg10, 3] : memref<?x100x4xf64>
          %15:4 = affine.for %arg11 = 0 to 100 iter_args(%arg12 = %cst, %arg13 = %cst, %arg14 = %cst, %arg15 = %cst) -> (f64, f64, f64, f64) {
            %24 = memref.load %arg5[%10, %arg11, %c0] : memref<?x100x4xf64>
            %25 = memref.load %arg5[%10, %arg11, %c1] : memref<?x100x4xf64>
            %26 = memref.load %arg5[%10, %arg11, %c2] : memref<?x100x4xf64>
            %27 = memref.load %arg5[%10, %arg11, %c3] : memref<?x100x4xf64>
            %28 = memref.load %arg6[%10, %arg11] : memref<?x100xf64>
            %29 = arith.addf %11, %24 : f64
            %30 = arith.mulf %12, %25 : f64
            %31 = arith.mulf %13, %26 : f64
            %32 = arith.addf %30, %31 : f64
            %33 = arith.mulf %14, %27 : f64
            %34 = arith.addf %32, %33 : f64
            %35 = arith.subf %29, %34 : f64
            %36 = arith.mulf %1, %35 : f64
            %37 = arith.negf %36 : f64
            %38 = math.exp %37 : f64
            %39 = arith.mulf %38, %cst_0 : f64
            %40 = arith.subf %12, %25 : f64
            %41 = arith.subf %13, %26 : f64
            %42 = arith.subf %14, %27 : f64
            %43 = arith.mulf %28, %38 : f64
            %44 = arith.addf %arg15, %43 : f64
            %45 = arith.mulf %39, %40 : f64
            %46 = arith.mulf %28, %45 : f64
            %47 = arith.addf %arg14, %46 : f64
            %48 = arith.mulf %39, %41 : f64
            %49 = arith.mulf %28, %48 : f64
            %50 = arith.addf %arg13, %49 : f64
            %51 = arith.mulf %39, %42 : f64
            %52 = arith.mulf %28, %51 : f64
            %53 = arith.addf %arg12, %52 : f64
            affine.yield %53, %50, %47, %44 : f64, f64, f64, f64
          }
          %16 = affine.load %arg7[%arg8, %arg10, 0] : memref<?x100x4xf64>
          %17 = arith.addf %16, %15#3 : f64
          affine.store %17, %arg7[%arg8, %arg10, 0] : memref<?x100x4xf64>
          %18 = affine.load %arg7[%arg8, %arg10, 1] : memref<?x100x4xf64>
          %19 = arith.addf %18, %15#2 : f64
          affine.store %19, %arg7[%arg8, %arg10, 1] : memref<?x100x4xf64>
          %20 = affine.load %arg7[%arg8, %arg10, 2] : memref<?x100x4xf64>
          %21 = arith.addf %20, %15#1 : f64
          affine.store %21, %arg7[%arg8, %arg10, 2] : memref<?x100x4xf64>
          %22 = affine.load %arg7[%arg8, %arg10, 3] : memref<?x100x4xf64>
          %23 = arith.addf %22, %15#0 : f64
          affine.store %23, %arg7[%arg8, %arg10, 3] : memref<?x100x4xf64>
        }
      }
    }
    return
  }
}
