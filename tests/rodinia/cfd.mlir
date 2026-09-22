module {
  func.func @cfd(%arg0: i32, %arg1: memref<?x4xi32>, %arg2: memref<?x4x3xf32>, %arg3: memref<?xf32>, %arg4: memref<?x5xf32>, %arg5: memref<?x5xf32>, %arg6: memref<?xf32>, %arg7: memref<?x5xf32>, %arg8: memref<?xf32>, %arg9: memref<?xf32>, %arg10: memref<?xf32>, %arg11: memref<?xf32>, %arg12: memref<?xf32>) attributes {llvm.linkage = #llvm.linkage<external>} {
    %c2000 = arith.constant 1000 : index
    %c0 = arith.constant 0 : index
    %c1 = arith.constant 1 : index
    %c5 = arith.constant 5 : index
    %c-2_i32 = arith.constant -2 : i32
    %c-1_i32 = arith.constant -1 : i32
    %c4_i32 = arith.constant 4 : i32
    %cst = arith.constant 0.000000e+00 : f32
    %cst_0 = arith.constant 1.400000e+00 : f32
    %cst_1 = arith.constant 5.000000e-01 : f32
    %cst_2 = arith.constant 0.399999976 : f32
    %c0_i32 = arith.constant 0 : i32
    %cst_3 = arith.constant 2.000000e-01 : f32
    %c2 = arith.constant 2 : index
    %c3 = arith.constant 3 : index
    %c4 = arith.constant 4 : index
    %0 = arith.index_cast %arg0 : i32 to index
    scf.for %arg13 = %c0 to %c2000 step %c1 {
      scf.for %arg14 = %c0 to %0 step %c1 {
        scf.for %arg15 = %c0 to %c5 step %c1 {
          %1 = memref.load %arg5[%arg14, %arg15] : memref<?x5xf32>
          memref.store %1, %arg4[%arg14, %arg15] : memref<?x5xf32>
        }
      }
      scf.for %arg14 = %c0 to %0 step %c1 {
        %1 = memref.load %arg5[%arg14, %c0] : memref<?x5xf32>
        %2 = memref.load %arg5[%arg14, %c1] : memref<?x5xf32>
        %3 = memref.load %arg5[%arg14, %c2] : memref<?x5xf32>
        %4 = memref.load %arg5[%arg14, %c3] : memref<?x5xf32>
        %5 = memref.load %arg5[%arg14, %c4] : memref<?x5xf32>
        %6 = arith.divf %2, %1 : f32
        %7 = arith.divf %3, %1 : f32
        %8 = arith.divf %4, %1 : f32
        %9 = arith.mulf %6, %6 : f32
        %10 = arith.mulf %7, %7 : f32
        %11 = arith.addf %9, %10 : f32
        %12 = arith.mulf %8, %8 : f32
        %13 = arith.addf %11, %12 : f32
        %14 = arith.mulf %1, %cst_1 : f32
        %15 = arith.mulf %14, %13 : f32
        %16 = arith.subf %5, %15 : f32
        %17 = arith.mulf %16, %cst_2 : f32
        %18 = arith.mulf %17, %cst_0 : f32
        %19 = arith.divf %18, %1 : f32
        %20 = math.sqrt %19 : f32
        %21 = memref.load %arg3[%arg14] : memref<?xf32>
        %22 = math.sqrt %21 : f32
        %23 = math.sqrt %13 : f32
        %24 = arith.addf %23, %20 : f32
        %25 = arith.mulf %22, %24 : f32
        %26 = arith.divf %cst_1, %25 : f32
        memref.store %26, %arg6[%arg14] : memref<?xf32>
      }
      scf.for %arg14 = %c0 to %c3 step %c1 {
        %1 = arith.index_cast %arg14 : index to i32
        scf.for %arg15 = %c0 to %0 step %c1 {
          %4 = memref.load %arg5[%arg15, %c0] : memref<?x5xf32>
          %5 = memref.load %arg5[%arg15, %c1] : memref<?x5xf32>
          %6 = memref.load %arg5[%arg15, %c2] : memref<?x5xf32>
          %7 = memref.load %arg5[%arg15, %c3] : memref<?x5xf32>
          %8 = memref.load %arg5[%arg15, %c4] : memref<?x5xf32>
          %9 = arith.divf %5, %4 : f32
          %10 = arith.divf %6, %4 : f32
          %11 = arith.divf %7, %4 : f32
          %12 = arith.mulf %9, %9 : f32
          %13 = arith.mulf %10, %10 : f32
          %14 = arith.addf %12, %13 : f32
          %15 = arith.mulf %11, %11 : f32
          %16 = arith.addf %14, %15 : f32
          %17 = math.sqrt %16 : f32
          %18 = arith.mulf %4, %cst_1 : f32
          %19 = arith.mulf %18, %16 : f32
          %20 = arith.subf %8, %19 : f32
          %21 = arith.mulf %20, %cst_2 : f32
          %22 = arith.mulf %21, %cst_0 : f32
          %23 = arith.divf %22, %4 : f32
          %24 = math.sqrt %23 : f32
          %25 = arith.mulf %9, %5 : f32
          %26 = arith.addf %25, %21 : f32
          %27 = arith.mulf %9, %6 : f32
          %28 = arith.mulf %9, %7 : f32
          %29 = arith.mulf %10, %6 : f32
          %30 = arith.addf %29, %21 : f32
          %31 = arith.mulf %10, %7 : f32
          %32 = arith.mulf %11, %7 : f32
          %33 = arith.addf %32, %21 : f32
          %34 = arith.addf %8, %21 : f32
          %35 = arith.mulf %9, %34 : f32
          %36 = arith.mulf %10, %34 : f32
          %37 = arith.mulf %11, %34 : f32
          %38:5 = scf.for %arg16 = %c0 to %c4 step %c1 iter_args(%arg17 = %cst, %arg18 = %cst, %arg19 = %cst, %arg20 = %cst, %arg21 = %cst) -> (f32, f32, f32, f32, f32) {
            %39 = memref.load %arg1[%arg15, %arg16] : memref<?x4xi32>
            %40 = memref.load %arg2[%arg15, %arg16, %c0] : memref<?x4x3xf32>
            %41 = memref.load %arg2[%arg15, %arg16, %c1] : memref<?x4x3xf32>
            %42 = memref.load %arg2[%arg15, %arg16, %c2] : memref<?x4x3xf32>
            %43 = arith.mulf %40, %40 : f32
            %44 = arith.mulf %41, %41 : f32
            %45 = arith.addf %43, %44 : f32
            %46 = arith.mulf %42, %42 : f32
            %47 = arith.addf %45, %46 : f32
            %48 = math.sqrt %47 : f32
            %49 = arith.cmpi sge, %39, %c0_i32 : i32
            %50:5 = scf.if %49 -> (f32, f32, f32, f32, f32) {
              %51 = arith.index_cast %39 : i32 to index
              %52 = memref.load %arg5[%51, %c0] : memref<?x5xf32>
              %53 = memref.load %arg5[%51, %c1] : memref<?x5xf32>
              %54 = memref.load %arg5[%51, %c2] : memref<?x5xf32>
              %55 = memref.load %arg5[%51, %c3] : memref<?x5xf32>
              %56 = memref.load %arg5[%51, %c4] : memref<?x5xf32>
              %57 = arith.divf %53, %52 : f32
              %58 = arith.divf %54, %52 : f32
              %59 = arith.divf %55, %52 : f32
              %60 = arith.mulf %57, %57 : f32
              %61 = arith.mulf %58, %58 : f32
              %62 = arith.addf %60, %61 : f32
              %63 = arith.mulf %59, %59 : f32
              %64 = arith.addf %62, %63 : f32
              %65 = arith.mulf %52, %cst_1 : f32
              %66 = arith.mulf %65, %64 : f32
              %67 = arith.subf %56, %66 : f32
              %68 = arith.mulf %67, %cst_2 : f32
              %69 = arith.mulf %68, %cst_0 : f32
              %70 = arith.divf %69, %52 : f32
              %71 = math.sqrt %70 : f32
              %72 = arith.mulf %57, %53 : f32
              %73 = arith.addf %72, %68 : f32
              %74 = arith.mulf %57, %54 : f32
              %75 = arith.mulf %57, %55 : f32
              %76 = arith.mulf %58, %54 : f32
              %77 = arith.addf %76, %68 : f32
              %78 = arith.mulf %58, %55 : f32
              %79 = arith.mulf %59, %55 : f32
              %80 = arith.addf %79, %68 : f32
              %81 = arith.addf %56, %68 : f32
              %82 = arith.mulf %57, %81 : f32
              %83 = arith.mulf %58, %81 : f32
              %84 = arith.mulf %59, %81 : f32
              %85 = arith.negf %48 : f32
              %86 = arith.mulf %85, %cst_3 : f32
              %87 = arith.mulf %86, %cst_1 : f32
              %88 = math.sqrt %64 : f32
              %89 = arith.addf %17, %88 : f32
              %90 = arith.addf %89, %24 : f32
              %91 = arith.addf %90, %71 : f32
              %92 = arith.mulf %87, %91 : f32
              %93 = arith.subf %4, %52 : f32
              %94 = arith.mulf %92, %93 : f32
              %95 = arith.addf %arg21, %94 : f32
              %96 = arith.subf %8, %56 : f32
              %97 = arith.mulf %92, %96 : f32
              %98 = arith.addf %arg17, %97 : f32
              %99 = arith.subf %5, %53 : f32
              %100 = arith.mulf %92, %99 : f32
              %101 = arith.addf %arg20, %100 : f32
              %102 = arith.subf %6, %54 : f32
              %103 = arith.mulf %92, %102 : f32
              %104 = arith.addf %arg19, %103 : f32
              %105 = arith.subf %7, %55 : f32
              %106 = arith.mulf %92, %105 : f32
              %107 = arith.addf %arg18, %106 : f32
              %108 = arith.mulf %40, %cst_1 : f32
              %109 = arith.addf %53, %5 : f32
              %110 = arith.mulf %108, %109 : f32
              %111 = arith.addf %95, %110 : f32
              %112 = arith.addf %82, %35 : f32
              %113 = arith.mulf %108, %112 : f32
              %114 = arith.addf %98, %113 : f32
              %115 = arith.addf %73, %26 : f32
              %116 = arith.mulf %108, %115 : f32
              %117 = arith.addf %101, %116 : f32
              %118 = arith.addf %74, %27 : f32
              %119 = arith.mulf %108, %118 : f32
              %120 = arith.addf %104, %119 : f32
              %121 = arith.addf %75, %28 : f32
              %122 = arith.mulf %108, %121 : f32
              %123 = arith.addf %107, %122 : f32
              %124 = arith.mulf %41, %cst_1 : f32
              %125 = arith.addf %54, %6 : f32
              %126 = arith.mulf %124, %125 : f32
              %127 = arith.addf %111, %126 : f32
              %128 = arith.addf %83, %36 : f32
              %129 = arith.mulf %124, %128 : f32
              %130 = arith.addf %114, %129 : f32
              %131 = arith.mulf %124, %118 : f32
              %132 = arith.addf %117, %131 : f32
              %133 = arith.addf %77, %30 : f32
              %134 = arith.mulf %124, %133 : f32
              %135 = arith.addf %120, %134 : f32
              %136 = arith.addf %78, %31 : f32
              %137 = arith.mulf %124, %136 : f32
              %138 = arith.addf %123, %137 : f32
              %139 = arith.mulf %42, %cst_1 : f32
              %140 = arith.addf %55, %7 : f32
              %141 = arith.mulf %139, %140 : f32
              %142 = arith.addf %127, %141 : f32
              %143 = arith.addf %84, %37 : f32
              %144 = arith.mulf %139, %143 : f32
              %145 = arith.addf %130, %144 : f32
              %146 = arith.mulf %139, %121 : f32
              %147 = arith.addf %132, %146 : f32
              %148 = arith.mulf %139, %136 : f32
              %149 = arith.addf %135, %148 : f32
              %150 = arith.addf %80, %33 : f32
              %151 = arith.mulf %139, %150 : f32
              %152 = arith.addf %138, %151 : f32
              scf.yield %145, %152, %149, %147, %142 : f32, f32, f32, f32, f32
            } else {
              %51 = arith.cmpi eq, %39, %c-1_i32 : i32
              %52:5 = scf.if %51 -> (f32, f32, f32, f32, f32) {
                %53 = arith.mulf %40, %21 : f32
                %54 = arith.addf %arg20, %53 : f32
                %55 = arith.mulf %41, %21 : f32
                %56 = arith.addf %arg19, %55 : f32
                %57 = arith.mulf %42, %21 : f32
                %58 = arith.addf %arg18, %57 : f32
                scf.yield %arg17, %58, %56, %54, %arg21 : f32, f32, f32, f32, f32
              } else {
                %53 = arith.cmpi eq, %39, %c-2_i32 : i32
                %54:5 = scf.if %53 -> (f32, f32, f32, f32, f32) {
                  %55 = arith.mulf %40, %cst_1 : f32
                  %56 = affine.load %arg8[1] : memref<?xf32>
                  %57 = arith.addf %56, %5 : f32
                  %58 = arith.mulf %55, %57 : f32
                  %59 = arith.addf %arg21, %58 : f32
                  %60 = affine.load %arg9[0] : memref<?xf32>
                  %61 = arith.addf %60, %35 : f32
                  %62 = arith.mulf %55, %61 : f32
                  %63 = arith.addf %arg17, %62 : f32
                  %64 = affine.load %arg10[0] : memref<?xf32>
                  %65 = arith.addf %64, %26 : f32
                  %66 = arith.mulf %55, %65 : f32
                  %67 = arith.addf %arg20, %66 : f32
                  %68 = affine.load %arg11[0] : memref<?xf32>
                  %69 = arith.addf %68, %27 : f32
                  %70 = arith.mulf %55, %69 : f32
                  %71 = arith.addf %arg19, %70 : f32
                  %72 = affine.load %arg12[0] : memref<?xf32>
                  %73 = arith.addf %72, %28 : f32
                  %74 = arith.mulf %55, %73 : f32
                  %75 = arith.addf %arg18, %74 : f32
                  %76 = arith.mulf %41, %cst_1 : f32
                  %77 = affine.load %arg8[2] : memref<?xf32>
                  %78 = arith.addf %77, %6 : f32
                  %79 = arith.mulf %76, %78 : f32
                  %80 = arith.addf %59, %79 : f32
                  %81 = affine.load %arg9[1] : memref<?xf32>
                  %82 = arith.addf %81, %36 : f32
                  %83 = arith.mulf %76, %82 : f32
                  %84 = arith.addf %63, %83 : f32
                  %85 = affine.load %arg10[1] : memref<?xf32>
                  %86 = arith.addf %85, %27 : f32
                  %87 = arith.mulf %76, %86 : f32
                  %88 = arith.addf %67, %87 : f32
                  %89 = affine.load %arg11[1] : memref<?xf32>
                  %90 = arith.addf %89, %30 : f32
                  %91 = arith.mulf %76, %90 : f32
                  %92 = arith.addf %71, %91 : f32
                  %93 = affine.load %arg12[1] : memref<?xf32>
                  %94 = arith.addf %93, %31 : f32
                  %95 = arith.mulf %76, %94 : f32
                  %96 = arith.addf %75, %95 : f32
                  %97 = arith.mulf %42, %cst_1 : f32
                  %98 = affine.load %arg8[3] : memref<?xf32>
                  %99 = arith.addf %98, %7 : f32
                  %100 = arith.mulf %97, %99 : f32
                  %101 = arith.addf %80, %100 : f32
                  %102 = affine.load %arg9[2] : memref<?xf32>
                  %103 = arith.addf %102, %37 : f32
                  %104 = arith.mulf %97, %103 : f32
                  %105 = arith.addf %84, %104 : f32
                  %106 = affine.load %arg10[2] : memref<?xf32>
                  %107 = arith.addf %106, %28 : f32
                  %108 = arith.mulf %97, %107 : f32
                  %109 = arith.addf %88, %108 : f32
                  %110 = affine.load %arg11[2] : memref<?xf32>
                  %111 = arith.addf %110, %31 : f32
                  %112 = arith.mulf %97, %111 : f32
                  %113 = arith.addf %92, %112 : f32
                  %114 = affine.load %arg12[2] : memref<?xf32>
                  %115 = arith.addf %114, %33 : f32
                  %116 = arith.mulf %97, %115 : f32
                  %117 = arith.addf %96, %116 : f32
                  scf.yield %105, %117, %113, %109, %101 : f32, f32, f32, f32, f32
                } else {
                  scf.yield %arg17, %arg18, %arg19, %arg20, %arg21 : f32, f32, f32, f32, f32
                }
                scf.yield %54#0, %54#1, %54#2, %54#3, %54#4 : f32, f32, f32, f32, f32
              }
              scf.yield %52#0, %52#1, %52#2, %52#3, %52#4 : f32, f32, f32, f32, f32
            }
            scf.yield %50#0, %50#1, %50#2, %50#3, %50#4 : f32, f32, f32, f32, f32
          }
          memref.store %38#4, %arg7[%arg15, %c0] : memref<?x5xf32>
          memref.store %38#3, %arg7[%arg15, %c1] : memref<?x5xf32>
          memref.store %38#2, %arg7[%arg15, %c2] : memref<?x5xf32>
          memref.store %38#1, %arg7[%arg15, %c3] : memref<?x5xf32>
          memref.store %38#0, %arg7[%arg15, %c4] : memref<?x5xf32>
        }
        %2 = arith.subi %c4_i32, %1 : i32
        %3 = arith.sitofp %2 : i32 to f32
        scf.for %arg15 = %c0 to %0 step %c1 {
          %4 = memref.load %arg6[%arg15] : memref<?xf32>
          %5 = arith.divf %4, %3 : f32
          %6 = memref.load %arg4[%arg15, %c0] : memref<?x5xf32>
          %7 = memref.load %arg7[%arg15, %c0] : memref<?x5xf32>
          %8 = arith.mulf %5, %7 : f32
          %9 = arith.addf %6, %8 : f32
          memref.store %9, %arg5[%arg15, %c0] : memref<?x5xf32>
          %10 = memref.load %arg4[%arg15, %c1] : memref<?x5xf32>
          %11 = memref.load %arg7[%arg15, %c1] : memref<?x5xf32>
          %12 = arith.mulf %5, %11 : f32
          %13 = arith.addf %10, %12 : f32
          memref.store %13, %arg5[%arg15, %c1] : memref<?x5xf32>
          %14 = memref.load %arg4[%arg15, %c2] : memref<?x5xf32>
          %15 = memref.load %arg7[%arg15, %c2] : memref<?x5xf32>
          %16 = arith.mulf %5, %15 : f32
          %17 = arith.addf %14, %16 : f32
          memref.store %17, %arg5[%arg15, %c2] : memref<?x5xf32>
          %18 = memref.load %arg4[%arg15, %c3] : memref<?x5xf32>
          %19 = memref.load %arg7[%arg15, %c3] : memref<?x5xf32>
          %20 = arith.mulf %5, %19 : f32
          %21 = arith.addf %18, %20 : f32
          memref.store %21, %arg5[%arg15, %c3] : memref<?x5xf32>
          %22 = memref.load %arg4[%arg15, %c4] : memref<?x5xf32>
          %23 = memref.load %arg7[%arg15, %c4] : memref<?x5xf32>
          %24 = arith.mulf %5, %23 : f32
          %25 = arith.addf %22, %24 : f32
          memref.store %25, %arg5[%arg15, %c4] : memref<?x5xf32>
        }
      }
    }
    return
  }
}
