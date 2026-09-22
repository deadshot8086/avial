module {
  func.func @hotspot3D(%arg0: i32, %arg1: i32, %arg2: i32, %arg3: i32, %arg4: memref<?x?x?xf32>, %arg5: memref<?x?x?xf32>, %arg6: memref<?x?x?xf32>, %arg7: f32, %arg8: f32, %arg9: f32, %arg10: f32, %arg11: f32, %arg12: f32, %arg13: f32, %arg14: f32, %arg15: f32) attributes {llvm.linkage = #llvm.linkage<external>} {
    %c1_i32 = arith.constant 1 : i32
    %c-1_i32 = arith.constant -1 : i32
    %c0 = arith.constant 0 : index
    %c1 = arith.constant 1 : index
    %c2_i32 = arith.constant 2 : i32
    %c0_i32 = arith.constant 0 : i32
    %0 = arith.index_cast %arg2 : i32 to index
    %1 = arith.index_cast %arg1 : i32 to index
    %2 = arith.index_cast %arg0 : i32 to index
    %3 = arith.addi %arg0, %c-1_i32 : i32
    %4 = arith.addi %arg1, %c-1_i32 : i32
    %5 = arith.addi %arg2, %c-1_i32 : i32
    %6 = arith.mulf %arg11, %arg15 : f32
    %7 = arith.index_cast %arg1 : i32 to index
    %8 = arith.index_cast %arg0 : i32 to index
    %9 = arith.addi %arg0, %c-1_i32 : i32
    %10 = arith.addi %arg1, %c-1_i32 : i32
    %11 = arith.addi %arg2, %c-1_i32 : i32
    %12 = arith.mulf %arg11, %arg15 : f32
    %13:2 = scf.while (%arg16 = %c1_i32) : (i32) -> (i32, i32) {
      %15 = arith.addi %arg16, %c-1_i32 : i32
      %16 = arith.cmpi slt, %arg16, %arg3 : i32
      scf.condition(%16) %15, %arg16 : i32, i32
    } do {
    ^bb0(%arg16: i32, %arg17: i32):
      scf.for %arg18 = %c0 to %0 step %c1 {
        %16 = arith.index_cast %arg18 : index to i32
        %17 = arith.cmpi sgt, %16, %c0_i32 : i32
        %18 = arith.cmpi slt, %16, %5 : i32
        %19 = arith.addi %16, %c-1_i32 : i32
        %20 = arith.index_cast %19 : i32 to index
        %21 = arith.addi %16, %c1_i32 : i32
        %22 = arith.index_cast %21 : i32 to index
        scf.for %arg19 = %c0 to %1 step %c1 {
          %23 = arith.index_cast %arg19 : index to i32
          %24 = arith.cmpi sgt, %23, %c0_i32 : i32
          %25 = arith.cmpi slt, %23, %4 : i32
          %26 = arith.addi %23, %c-1_i32 : i32
          %27 = arith.index_cast %26 : i32 to index
          %28 = arith.addi %23, %c1_i32 : i32
          %29 = arith.index_cast %28 : i32 to index
          scf.for %arg20 = %c0 to %2 step %c1 {
            %30 = arith.index_cast %arg20 : index to i32
            %31 = memref.load %arg5[%arg18, %arg19, %arg20] : memref<?x?x?xf32>
            %32 = arith.cmpi sgt, %30, %c0_i32 : i32
            %33 = scf.if %32 -> (f32) {
              %57 = arith.addi %30, %c-1_i32 : i32
              %58 = arith.index_cast %57 : i32 to index
              %59 = memref.load %arg5[%arg18, %arg19, %58] : memref<?x?x?xf32>
              scf.yield %59 : f32
            } else {
              scf.yield %31 : f32
            }
            %34 = arith.cmpi slt, %30, %3 : i32
            %35 = scf.if %34 -> (f32) {
              %57 = arith.addi %30, %c1_i32 : i32
              %58 = arith.index_cast %57 : i32 to index
              %59 = memref.load %arg5[%arg18, %arg19, %58] : memref<?x?x?xf32>
              scf.yield %59 : f32
            } else {
              scf.yield %31 : f32
            }
            %36 = scf.if %24 -> (f32) {
              %57 = memref.load %arg5[%arg18, %27, %arg20] : memref<?x?x?xf32>
              scf.yield %57 : f32
            } else {
              scf.yield %31 : f32
            }
            %37 = scf.if %25 -> (f32) {
              %57 = memref.load %arg5[%arg18, %29, %arg20] : memref<?x?x?xf32>
              scf.yield %57 : f32
            } else {
              scf.yield %31 : f32
            }
            %38 = scf.if %17 -> (f32) {
              %57 = memref.load %arg5[%20, %arg19, %arg20] : memref<?x?x?xf32>
              scf.yield %57 : f32
            } else {
              scf.yield %31 : f32
            }
            %39 = scf.if %18 -> (f32) {
              %57 = memref.load %arg5[%22, %arg19, %arg20] : memref<?x?x?xf32>
              scf.yield %57 : f32
            } else {
              scf.yield %31 : f32
            }
            %40 = arith.mulf %31, %arg13 : f32
            %41 = arith.mulf %36, %arg9 : f32
            %42 = arith.addf %40, %41 : f32
            %43 = arith.mulf %37, %arg10 : f32
            %44 = arith.addf %42, %43 : f32
            %45 = arith.mulf %35, %arg7 : f32
            %46 = arith.addf %44, %45 : f32
            %47 = arith.mulf %33, %arg8 : f32
            %48 = arith.addf %46, %47 : f32
            %49 = arith.mulf %39, %arg11 : f32
            %50 = arith.addf %48, %49 : f32
            %51 = arith.mulf %38, %arg12 : f32
            %52 = arith.addf %50, %51 : f32
            %53 = memref.load %arg4[%arg18, %arg19, %arg20] : memref<?x?x?xf32>
            %54 = arith.mulf %arg14, %53 : f32
            %55 = arith.addf %52, %54 : f32
            %56 = arith.addf %55, %6 : f32
            memref.store %56, %arg6[%arg18, %arg19, %arg20] : memref<?x?x?xf32>
          }
        }
      }
      scf.for %arg18 = %c0 to %0 step %c1 {
        %16 = arith.index_cast %arg18 : index to i32
        %17 = arith.cmpi sgt, %16, %c0_i32 : i32
        %18 = arith.cmpi slt, %16, %11 : i32
        %19 = arith.addi %16, %c-1_i32 : i32
        %20 = arith.index_cast %19 : i32 to index
        %21 = arith.addi %16, %c1_i32 : i32
        %22 = arith.index_cast %21 : i32 to index
        scf.for %arg19 = %c0 to %7 step %c1 {
          %23 = arith.index_cast %arg19 : index to i32
          %24 = arith.cmpi sgt, %23, %c0_i32 : i32
          %25 = arith.cmpi slt, %23, %10 : i32
          %26 = arith.addi %23, %c-1_i32 : i32
          %27 = arith.index_cast %26 : i32 to index
          %28 = arith.addi %23, %c1_i32 : i32
          %29 = arith.index_cast %28 : i32 to index
          scf.for %arg20 = %c0 to %8 step %c1 {
            %30 = arith.index_cast %arg20 : index to i32
            %31 = memref.load %arg6[%arg18, %arg19, %arg20] : memref<?x?x?xf32>
            %32 = arith.cmpi sgt, %30, %c0_i32 : i32
            %33 = scf.if %32 -> (f32) {
              %57 = arith.addi %30, %c-1_i32 : i32
              %58 = arith.index_cast %57 : i32 to index
              %59 = memref.load %arg6[%arg18, %arg19, %58] : memref<?x?x?xf32>
              scf.yield %59 : f32
            } else {
              scf.yield %31 : f32
            }
            %34 = arith.cmpi slt, %30, %9 : i32
            %35 = scf.if %34 -> (f32) {
              %57 = arith.addi %30, %c1_i32 : i32
              %58 = arith.index_cast %57 : i32 to index
              %59 = memref.load %arg6[%arg18, %arg19, %58] : memref<?x?x?xf32>
              scf.yield %59 : f32
            } else {
              scf.yield %31 : f32
            }
            %36 = scf.if %24 -> (f32) {
              %57 = memref.load %arg6[%arg18, %27, %arg20] : memref<?x?x?xf32>
              scf.yield %57 : f32
            } else {
              scf.yield %31 : f32
            }
            %37 = scf.if %25 -> (f32) {
              %57 = memref.load %arg6[%arg18, %29, %arg20] : memref<?x?x?xf32>
              scf.yield %57 : f32
            } else {
              scf.yield %31 : f32
            }
            %38 = scf.if %17 -> (f32) {
              %57 = memref.load %arg6[%20, %arg19, %arg20] : memref<?x?x?xf32>
              scf.yield %57 : f32
            } else {
              scf.yield %31 : f32
            }
            %39 = scf.if %18 -> (f32) {
              %57 = memref.load %arg6[%22, %arg19, %arg20] : memref<?x?x?xf32>
              scf.yield %57 : f32
            } else {
              scf.yield %31 : f32
            }
            %40 = arith.mulf %31, %arg13 : f32
            %41 = arith.mulf %36, %arg9 : f32
            %42 = arith.addf %40, %41 : f32
            %43 = arith.mulf %37, %arg10 : f32
            %44 = arith.addf %42, %43 : f32
            %45 = arith.mulf %35, %arg7 : f32
            %46 = arith.addf %44, %45 : f32
            %47 = arith.mulf %33, %arg8 : f32
            %48 = arith.addf %46, %47 : f32
            %49 = arith.mulf %39, %arg11 : f32
            %50 = arith.addf %48, %49 : f32
            %51 = arith.mulf %38, %arg12 : f32
            %52 = arith.addf %50, %51 : f32
            %53 = memref.load %arg4[%arg18, %arg19, %arg20] : memref<?x?x?xf32>
            %54 = arith.mulf %arg14, %53 : f32
            %55 = arith.addf %52, %54 : f32
            %56 = arith.addf %55, %12 : f32
            memref.store %56, %arg5[%arg18, %arg19, %arg20] : memref<?x?x?xf32>
          }
        }
      }
      %15 = arith.addi %arg17, %c2_i32 : i32
      scf.yield %15 : i32
    }
    %14 = arith.cmpi slt, %13#0, %arg3 : i32
    scf.if %14 {
      %15 = arith.index_cast %arg1 : i32 to index
      %16 = arith.index_cast %arg0 : i32 to index
      %17 = arith.addi %arg0, %c-1_i32 : i32
      %18 = arith.addi %arg1, %c-1_i32 : i32
      %19 = arith.addi %arg2, %c-1_i32 : i32
      %20 = arith.mulf %arg11, %arg15 : f32
      scf.for %arg16 = %c0 to %0 step %c1 {
        %21 = arith.index_cast %arg16 : index to i32
        %22 = arith.cmpi sgt, %21, %c0_i32 : i32
        %23 = arith.cmpi slt, %21, %19 : i32
        %24 = arith.addi %21, %c-1_i32 : i32
        %25 = arith.index_cast %24 : i32 to index
        %26 = arith.addi %21, %c1_i32 : i32
        %27 = arith.index_cast %26 : i32 to index
        scf.for %arg17 = %c0 to %15 step %c1 {
          %28 = arith.index_cast %arg17 : index to i32
          %29 = arith.cmpi sgt, %28, %c0_i32 : i32
          %30 = arith.cmpi slt, %28, %18 : i32
          %31 = arith.addi %28, %c-1_i32 : i32
          %32 = arith.index_cast %31 : i32 to index
          %33 = arith.addi %28, %c1_i32 : i32
          %34 = arith.index_cast %33 : i32 to index
          scf.for %arg18 = %c0 to %16 step %c1 {
            %35 = arith.index_cast %arg18 : index to i32
            %36 = memref.load %arg5[%arg16, %arg17, %arg18] : memref<?x?x?xf32>
            %37 = arith.cmpi sgt, %35, %c0_i32 : i32
            %38 = scf.if %37 -> (f32) {
              %62 = arith.addi %35, %c-1_i32 : i32
              %63 = arith.index_cast %62 : i32 to index
              %64 = memref.load %arg5[%arg16, %arg17, %63] : memref<?x?x?xf32>
              scf.yield %64 : f32
            } else {
              scf.yield %36 : f32
            }
            %39 = arith.cmpi slt, %35, %17 : i32
            %40 = scf.if %39 -> (f32) {
              %62 = arith.addi %35, %c1_i32 : i32
              %63 = arith.index_cast %62 : i32 to index
              %64 = memref.load %arg5[%arg16, %arg17, %63] : memref<?x?x?xf32>
              scf.yield %64 : f32
            } else {
              scf.yield %36 : f32
            }
            %41 = scf.if %29 -> (f32) {
              %62 = memref.load %arg5[%arg16, %32, %arg18] : memref<?x?x?xf32>
              scf.yield %62 : f32
            } else {
              scf.yield %36 : f32
            }
            %42 = scf.if %30 -> (f32) {
              %62 = memref.load %arg5[%arg16, %34, %arg18] : memref<?x?x?xf32>
              scf.yield %62 : f32
            } else {
              scf.yield %36 : f32
            }
            %43 = scf.if %22 -> (f32) {
              %62 = memref.load %arg5[%25, %arg17, %arg18] : memref<?x?x?xf32>
              scf.yield %62 : f32
            } else {
              scf.yield %36 : f32
            }
            %44 = scf.if %23 -> (f32) {
              %62 = memref.load %arg5[%27, %arg17, %arg18] : memref<?x?x?xf32>
              scf.yield %62 : f32
            } else {
              scf.yield %36 : f32
            }
            %45 = arith.mulf %36, %arg13 : f32
            %46 = arith.mulf %41, %arg9 : f32
            %47 = arith.addf %45, %46 : f32
            %48 = arith.mulf %42, %arg10 : f32
            %49 = arith.addf %47, %48 : f32
            %50 = arith.mulf %40, %arg7 : f32
            %51 = arith.addf %49, %50 : f32
            %52 = arith.mulf %38, %arg8 : f32
            %53 = arith.addf %51, %52 : f32
            %54 = arith.mulf %44, %arg11 : f32
            %55 = arith.addf %53, %54 : f32
            %56 = arith.mulf %43, %arg12 : f32
            %57 = arith.addf %55, %56 : f32
            %58 = memref.load %arg4[%arg16, %arg17, %arg18] : memref<?x?x?xf32>
            %59 = arith.mulf %arg14, %58 : f32
            %60 = arith.addf %57, %59 : f32
            %61 = arith.addf %60, %20 : f32
            memref.store %61, %arg6[%arg16, %arg17, %arg18] : memref<?x?x?xf32>
          }
        }
      }
    }
    return
  }
}
