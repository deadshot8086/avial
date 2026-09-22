module {
  func.func @hotspot(%arg0: i32, %arg1: i32, %arg2: i32, %arg3: memref<?x?xf32>, %arg4: memref<?x?xf32>, %arg5: memref<?x?xf32>, %arg6: f32, %arg7: f32, %arg8: f32, %arg9: f32, %arg10: f32) attributes {llvm.linkage = #llvm.linkage<external>} {
    %c1_i32 = arith.constant 1 : i32
    %c-1_i32 = arith.constant -1 : i32
    %c0 = arith.constant 0 : index
    %c1 = arith.constant 1 : index
    %c2_i32 = arith.constant 2 : i32
    %cst = arith.constant 2.000000e+00 : f32
    %c0_i32 = arith.constant 0 : i32
    %0 = arith.index_cast %arg0 : i32 to index
    %1 = arith.index_cast %arg1 : i32 to index
    %2 = arith.addi %arg0, %c-1_i32 : i32
    %3 = arith.addi %arg1, %c-1_i32 : i32
    %4 = arith.index_cast %arg1 : i32 to index
    %5 = arith.addi %arg0, %c-1_i32 : i32
    %6 = arith.addi %arg1, %c-1_i32 : i32
    %7:2 = scf.while (%arg11 = %c1_i32) : (i32) -> (i32, i32) {
      %11 = arith.addi %arg11, %c-1_i32 : i32
      %12 = arith.cmpi slt, %arg11, %arg2 : i32
      scf.condition(%12) %11, %arg11 : i32, i32
    } do {
    ^bb0(%arg11: i32, %arg12: i32):
      scf.for %arg13 = %c0 to %0 step %c1 {
        %12 = arith.index_cast %arg13 : index to i32
        %13 = arith.cmpi sgt, %12, %c0_i32 : i32
        %14 = arith.cmpi slt, %12, %2 : i32
        %15 = arith.addi %12, %c-1_i32 : i32
        %16 = arith.index_cast %15 : i32 to index
        %17 = arith.addi %12, %c1_i32 : i32
        %18 = arith.index_cast %17 : i32 to index
        scf.for %arg14 = %c0 to %1 step %c1 {
          %19 = arith.index_cast %arg14 : index to i32
          %20 = memref.load %arg4[%arg13, %arg14] : memref<?x?xf32>
          %21 = scf.if %13 -> (f32) {
            %42 = memref.load %arg4[%16, %arg14] : memref<?x?xf32>
            scf.yield %42 : f32
          } else {
            scf.yield %20 : f32
          }
          %22 = scf.if %14 -> (f32) {
            %42 = memref.load %arg4[%18, %arg14] : memref<?x?xf32>
            scf.yield %42 : f32
          } else {
            scf.yield %20 : f32
          }
          %23 = arith.cmpi sgt, %19, %c0_i32 : i32
          %24 = scf.if %23 -> (f32) {
            %42 = arith.addi %19, %c-1_i32 : i32
            %43 = arith.index_cast %42 : i32 to index
            %44 = memref.load %arg4[%arg13, %43] : memref<?x?xf32>
            scf.yield %44 : f32
          } else {
            scf.yield %20 : f32
          }
          %25 = arith.cmpi slt, %19, %3 : i32
          %26 = scf.if %25 -> (f32) {
            %42 = arith.addi %19, %c1_i32 : i32
            %43 = arith.index_cast %42 : i32 to index
            %44 = memref.load %arg4[%arg13, %43] : memref<?x?xf32>
            scf.yield %44 : f32
          } else {
            scf.yield %20 : f32
          }
          %27 = memref.load %arg3[%arg13, %arg14] : memref<?x?xf32>
          %28 = arith.addf %22, %21 : f32
          %29 = arith.mulf %20, %cst : f32
          %30 = arith.subf %28, %29 : f32
          %31 = arith.mulf %30, %arg8 : f32
          %32 = arith.addf %27, %31 : f32
          %33 = arith.addf %26, %24 : f32
          %34 = arith.subf %33, %29 : f32
          %35 = arith.mulf %34, %arg7 : f32
          %36 = arith.addf %32, %35 : f32
          %37 = arith.subf %arg10, %20 : f32
          %38 = arith.mulf %37, %arg9 : f32
          %39 = arith.addf %36, %38 : f32
          %40 = arith.mulf %arg6, %39 : f32
          %41 = arith.addf %20, %40 : f32
          memref.store %41, %arg5[%arg13, %arg14] : memref<?x?xf32>
        }
      }
      scf.for %arg13 = %c0 to %0 step %c1 {
        %12 = arith.index_cast %arg13 : index to i32
        %13 = arith.cmpi sgt, %12, %c0_i32 : i32
        %14 = arith.cmpi slt, %12, %5 : i32
        %15 = arith.addi %12, %c-1_i32 : i32
        %16 = arith.index_cast %15 : i32 to index
        %17 = arith.addi %12, %c1_i32 : i32
        %18 = arith.index_cast %17 : i32 to index
        scf.for %arg14 = %c0 to %4 step %c1 {
          %19 = arith.index_cast %arg14 : index to i32
          %20 = memref.load %arg5[%arg13, %arg14] : memref<?x?xf32>
          %21 = scf.if %13 -> (f32) {
            %42 = memref.load %arg5[%16, %arg14] : memref<?x?xf32>
            scf.yield %42 : f32
          } else {
            scf.yield %20 : f32
          }
          %22 = scf.if %14 -> (f32) {
            %42 = memref.load %arg5[%18, %arg14] : memref<?x?xf32>
            scf.yield %42 : f32
          } else {
            scf.yield %20 : f32
          }
          %23 = arith.cmpi sgt, %19, %c0_i32 : i32
          %24 = scf.if %23 -> (f32) {
            %42 = arith.addi %19, %c-1_i32 : i32
            %43 = arith.index_cast %42 : i32 to index
            %44 = memref.load %arg5[%arg13, %43] : memref<?x?xf32>
            scf.yield %44 : f32
          } else {
            scf.yield %20 : f32
          }
          %25 = arith.cmpi slt, %19, %6 : i32
          %26 = scf.if %25 -> (f32) {
            %42 = arith.addi %19, %c1_i32 : i32
            %43 = arith.index_cast %42 : i32 to index
            %44 = memref.load %arg5[%arg13, %43] : memref<?x?xf32>
            scf.yield %44 : f32
          } else {
            scf.yield %20 : f32
          }
          %27 = memref.load %arg3[%arg13, %arg14] : memref<?x?xf32>
          %28 = arith.addf %22, %21 : f32
          %29 = arith.mulf %20, %cst : f32
          %30 = arith.subf %28, %29 : f32
          %31 = arith.mulf %30, %arg8 : f32
          %32 = arith.addf %27, %31 : f32
          %33 = arith.addf %26, %24 : f32
          %34 = arith.subf %33, %29 : f32
          %35 = arith.mulf %34, %arg7 : f32
          %36 = arith.addf %32, %35 : f32
          %37 = arith.subf %arg10, %20 : f32
          %38 = arith.mulf %37, %arg9 : f32
          %39 = arith.addf %36, %38 : f32
          %40 = arith.mulf %arg6, %39 : f32
          %41 = arith.addf %20, %40 : f32
          memref.store %41, %arg4[%arg13, %arg14] : memref<?x?xf32>
        }
      }
      %11 = arith.addi %arg12, %c2_i32 : i32
      scf.yield %11 : i32
    }
    %8 = arith.cmpi slt, %7#0, %arg2 : i32
    scf.if %8 {
      %11 = arith.index_cast %arg1 : i32 to index
      %12 = arith.addi %arg0, %c-1_i32 : i32
      %13 = arith.addi %arg1, %c-1_i32 : i32
      scf.for %arg11 = %c0 to %0 step %c1 {
        %14 = arith.index_cast %arg11 : index to i32
        %15 = arith.cmpi sgt, %14, %c0_i32 : i32
        %16 = arith.cmpi slt, %14, %12 : i32
        %17 = arith.addi %14, %c-1_i32 : i32
        %18 = arith.index_cast %17 : i32 to index
        %19 = arith.addi %14, %c1_i32 : i32
        %20 = arith.index_cast %19 : i32 to index
        scf.for %arg12 = %c0 to %11 step %c1 {
          %21 = arith.index_cast %arg12 : index to i32
          %22 = memref.load %arg4[%arg11, %arg12] : memref<?x?xf32>
          %23 = scf.if %15 -> (f32) {
            %44 = memref.load %arg4[%18, %arg12] : memref<?x?xf32>
            scf.yield %44 : f32
          } else {
            scf.yield %22 : f32
          }
          %24 = scf.if %16 -> (f32) {
            %44 = memref.load %arg4[%20, %arg12] : memref<?x?xf32>
            scf.yield %44 : f32
          } else {
            scf.yield %22 : f32
          }
          %25 = arith.cmpi sgt, %21, %c0_i32 : i32
          %26 = scf.if %25 -> (f32) {
            %44 = arith.addi %21, %c-1_i32 : i32
            %45 = arith.index_cast %44 : i32 to index
            %46 = memref.load %arg4[%arg11, %45] : memref<?x?xf32>
            scf.yield %46 : f32
          } else {
            scf.yield %22 : f32
          }
          %27 = arith.cmpi slt, %21, %13 : i32
          %28 = scf.if %27 -> (f32) {
            %44 = arith.addi %21, %c1_i32 : i32
            %45 = arith.index_cast %44 : i32 to index
            %46 = memref.load %arg4[%arg11, %45] : memref<?x?xf32>
            scf.yield %46 : f32
          } else {
            scf.yield %22 : f32
          }
          %29 = memref.load %arg3[%arg11, %arg12] : memref<?x?xf32>
          %30 = arith.addf %24, %23 : f32
          %31 = arith.mulf %22, %cst : f32
          %32 = arith.subf %30, %31 : f32
          %33 = arith.mulf %32, %arg8 : f32
          %34 = arith.addf %29, %33 : f32
          %35 = arith.addf %28, %26 : f32
          %36 = arith.subf %35, %31 : f32
          %37 = arith.mulf %36, %arg7 : f32
          %38 = arith.addf %34, %37 : f32
          %39 = arith.subf %arg10, %22 : f32
          %40 = arith.mulf %39, %arg9 : f32
          %41 = arith.addf %38, %40 : f32
          %42 = arith.mulf %arg6, %41 : f32
          %43 = arith.addf %22, %42 : f32
          memref.store %43, %arg5[%arg11, %arg12] : memref<?x?xf32>
        }
      }
    }
    %9 = arith.remsi %arg2, %c2_i32 : i32
    %10 = arith.cmpi eq, %9, %c0_i32 : i32
    scf.if %10 {
      %11 = arith.index_cast %arg1 : i32 to index
      scf.for %arg11 = %c0 to %0 step %c1 {
        scf.for %arg12 = %c0 to %11 step %c1 {
          %12 = memref.load %arg4[%arg11, %arg12] : memref<?x?xf32>
          memref.store %12, %arg5[%arg11, %arg12] : memref<?x?xf32>
        }
      }
    }
    return
  }
}
