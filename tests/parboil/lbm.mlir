#map = affine_map<()[s0] -> (s0 - 2)>
module {
  func.func @lbm(%arg0: i32, %arg1: i32, %arg2: i32, %arg3: i32, %arg4: memref<?x?x?x20xf64>, %arg5: memref<?x?x?x20xf64>, %arg6: memref<?x?x?xi32>) attributes {llvm.linkage = #llvm.linkage<external>} {
    %c0_i32 = arith.constant 0 : i32
    %c2_i32 = arith.constant 2 : i32
    %c1_i32 = arith.constant 1 : i32
    %cst = arith.constant 5.000000e-03 : f64
    %cst_0 = arith.constant 2.000000e-03 : f64
    %cst_1 = arith.constant 0.000000e+00 : f64
    %cst_2 = arith.constant 1.500000e+00 : f64
    %cst_3 = arith.constant -0.94999999999999996 : f64
    %cst_4 = arith.constant 1.000000e+00 : f64
    %cst_5 = arith.constant 4.500000e+00 : f64
    %cst_6 = arith.constant 3.000000e+00 : f64
    %cst_7 = arith.constant 0.054166666666666662 : f64
    %cst_8 = arith.constant 0.10833333333333332 : f64
    %cst_9 = arith.constant 0.64999999999999991 : f64
    %0 = arith.index_cast %arg0 : i32 to index
    %1 = arith.divsi %arg3, %c2_i32 : i32
    %2 = arith.index_cast %1 : i32 to index
    %3 = arith.index_cast %arg1 : i32 to index
    %4 = arith.index_cast %arg2 : i32 to index
    affine.for %arg7 = 0 to %2 {
      affine.for %arg8 = 2 to #map()[%0] {
        affine.for %arg9 = 0 to %3 {
          affine.for %arg10 = 0 to %4 {
            %7 = affine.load %arg4[%arg8, %arg9, %arg10, 0] : memref<?x?x?x20xf64>
            %8 = affine.load %arg4[%arg8, %arg9, %arg10, 1] : memref<?x?x?x20xf64>
            %9 = affine.load %arg4[%arg8, %arg9, %arg10, 2] : memref<?x?x?x20xf64>
            %10 = affine.load %arg4[%arg8, %arg9, %arg10, 3] : memref<?x?x?x20xf64>
            %11 = affine.load %arg4[%arg8, %arg9, %arg10, 4] : memref<?x?x?x20xf64>
            %12 = affine.load %arg4[%arg8, %arg9, %arg10, 5] : memref<?x?x?x20xf64>
            %13 = affine.load %arg4[%arg8, %arg9, %arg10, 6] : memref<?x?x?x20xf64>
            %14 = affine.load %arg4[%arg8, %arg9, %arg10, 7] : memref<?x?x?x20xf64>
            %15 = affine.load %arg4[%arg8, %arg9, %arg10, 8] : memref<?x?x?x20xf64>
            %16 = affine.load %arg4[%arg8, %arg9, %arg10, 9] : memref<?x?x?x20xf64>
            %17 = affine.load %arg4[%arg8, %arg9, %arg10, 10] : memref<?x?x?x20xf64>
            %18 = affine.load %arg4[%arg8, %arg9, %arg10, 11] : memref<?x?x?x20xf64>
            %19 = affine.load %arg4[%arg8, %arg9, %arg10, 12] : memref<?x?x?x20xf64>
            %20 = affine.load %arg4[%arg8, %arg9, %arg10, 13] : memref<?x?x?x20xf64>
            %21 = affine.load %arg4[%arg8, %arg9, %arg10, 14] : memref<?x?x?x20xf64>
            %22 = affine.load %arg4[%arg8, %arg9, %arg10, 15] : memref<?x?x?x20xf64>
            %23 = affine.load %arg4[%arg8, %arg9, %arg10, 16] : memref<?x?x?x20xf64>
            %24 = affine.load %arg4[%arg8, %arg9, %arg10, 17] : memref<?x?x?x20xf64>
            %25 = affine.load %arg4[%arg8, %arg9, %arg10, 18] : memref<?x?x?x20xf64>
            %26 = affine.load %arg6[%arg8, %arg9, %arg10] : memref<?x?x?xi32>
            %27 = arith.andi %26, %c1_i32 : i32
            %28 = arith.cmpi ne, %27, %c0_i32 : i32
            scf.if %28 {
              affine.store %7, %arg5[%arg8, %arg9, %arg10, 0] : memref<?x?x?x20xf64>
              affine.store %8, %arg5[%arg8, %arg9 - 1, %arg10, 2] : memref<?x?x?x20xf64>
              affine.store %9, %arg5[%arg8, %arg9 + 1, %arg10, 1] : memref<?x?x?x20xf64>
              affine.store %10, %arg5[%arg8, %arg9, %arg10 - 1, 4] : memref<?x?x?x20xf64>
              affine.store %11, %arg5[%arg8, %arg9, %arg10 + 1, 3] : memref<?x?x?x20xf64>
              affine.store %12, %arg5[%arg8 - 1, %arg9, %arg10, 6] : memref<?x?x?x20xf64>
              affine.store %13, %arg5[%arg8 + 1, %arg9, %arg10, 5] : memref<?x?x?x20xf64>
              affine.store %14, %arg5[%arg8, %arg9 - 1, %arg10 - 1, 10] : memref<?x?x?x20xf64>
              affine.store %15, %arg5[%arg8, %arg9 - 1, %arg10 + 1, 9] : memref<?x?x?x20xf64>
              affine.store %16, %arg5[%arg8, %arg9 + 1, %arg10 - 1, 8] : memref<?x?x?x20xf64>
              affine.store %17, %arg5[%arg8, %arg9 + 1, %arg10 + 1, 7] : memref<?x?x?x20xf64>
              affine.store %18, %arg5[%arg8 - 1, %arg9 - 1, %arg10, 14] : memref<?x?x?x20xf64>
              affine.store %19, %arg5[%arg8 + 1, %arg9 - 1, %arg10, 13] : memref<?x?x?x20xf64>
              affine.store %20, %arg5[%arg8 - 1, %arg9 + 1, %arg10, 12] : memref<?x?x?x20xf64>
              affine.store %21, %arg5[%arg8 + 1, %arg9 + 1, %arg10, 11] : memref<?x?x?x20xf64>
              affine.store %22, %arg5[%arg8 - 1, %arg9, %arg10 - 1, 18] : memref<?x?x?x20xf64>
              affine.store %23, %arg5[%arg8 + 1, %arg9, %arg10 - 1, 17] : memref<?x?x?x20xf64>
              affine.store %24, %arg5[%arg8 - 1, %arg9, %arg10 + 1, 16] : memref<?x?x?x20xf64>
              affine.store %25, %arg5[%arg8 + 1, %arg9, %arg10 + 1, 15] : memref<?x?x?x20xf64>
            } else {
              %29 = arith.addf %7, %8 : f64
              %30 = arith.addf %29, %9 : f64
              %31 = arith.addf %30, %10 : f64
              %32 = arith.addf %31, %11 : f64
              %33 = arith.addf %32, %12 : f64
              %34 = arith.addf %33, %13 : f64
              %35 = arith.addf %34, %14 : f64
              %36 = arith.addf %35, %15 : f64
              %37 = arith.addf %36, %16 : f64
              %38 = arith.addf %37, %17 : f64
              %39 = arith.addf %38, %18 : f64
              %40 = arith.addf %39, %19 : f64
              %41 = arith.addf %40, %20 : f64
              %42 = arith.addf %41, %21 : f64
              %43 = arith.addf %42, %22 : f64
              %44 = arith.addf %43, %23 : f64
              %45 = arith.addf %44, %24 : f64
              %46 = arith.addf %45, %25 : f64
              %47 = arith.subf %10, %11 : f64
              %48 = arith.addf %47, %14 : f64
              %49 = arith.subf %48, %15 : f64
              %50 = arith.addf %49, %16 : f64
              %51 = arith.subf %50, %17 : f64
              %52 = arith.addf %51, %22 : f64
              %53 = arith.addf %52, %23 : f64
              %54 = arith.subf %53, %24 : f64
              %55 = arith.subf %54, %25 : f64
              %56 = arith.subf %8, %9 : f64
              %57 = arith.addf %56, %14 : f64
              %58 = arith.addf %57, %15 : f64
              %59 = arith.subf %58, %16 : f64
              %60 = arith.subf %59, %17 : f64
              %61 = arith.addf %60, %18 : f64
              %62 = arith.addf %61, %19 : f64
              %63 = arith.subf %62, %20 : f64
              %64 = arith.subf %63, %21 : f64
              %65 = arith.subf %12, %13 : f64
              %66 = arith.addf %65, %18 : f64
              %67 = arith.subf %66, %19 : f64
              %68 = arith.addf %67, %20 : f64
              %69 = arith.subf %68, %21 : f64
              %70 = arith.addf %69, %22 : f64
              %71 = arith.subf %70, %23 : f64
              %72 = arith.addf %71, %24 : f64
              %73 = arith.subf %72, %25 : f64
              %74 = arith.divf %55, %46 : f64
              %75 = arith.divf %64, %46 : f64
              %76 = arith.divf %73, %46 : f64
              %77 = arith.andi %26, %c2_i32 : i32
              %78 = arith.cmpi ne, %77, %c0_i32 : i32
              %79 = arith.select %78, %cst_1, %76 : f64
              %80 = arith.select %78, %cst_0, %75 : f64
              %81 = arith.select %78, %cst, %74 : f64
              %82 = arith.mulf %81, %81 : f64
              %83 = arith.mulf %80, %80 : f64
              %84 = arith.addf %82, %83 : f64
              %85 = arith.mulf %79, %79 : f64
              %86 = arith.addf %84, %85 : f64
              %87 = arith.mulf %86, %cst_2 : f64
              %88 = arith.mulf %7, %cst_3 : f64
              %89 = arith.mulf %46, %cst_9 : f64
              %90 = arith.subf %cst_4, %87 : f64
              %91 = arith.mulf %89, %90 : f64
              %92 = arith.addf %88, %91 : f64
              affine.store %92, %arg5[%arg8, %arg9, %arg10, 0] : memref<?x?x?x20xf64>
              %93 = arith.mulf %8, %cst_3 : f64
              %94 = arith.mulf %46, %cst_8 : f64
              %95 = arith.mulf %80, %cst_5 : f64
              %96 = arith.addf %95, %cst_6 : f64
              %97 = arith.mulf %80, %96 : f64
              %98 = arith.addf %97, %cst_4 : f64
              %99 = arith.subf %98, %87 : f64
              %100 = arith.mulf %94, %99 : f64
              %101 = arith.addf %93, %100 : f64
              affine.store %101, %arg5[%arg8, %arg9 + 1, %arg10, 1] : memref<?x?x?x20xf64>
              %102 = arith.mulf %9, %cst_3 : f64
              %103 = arith.subf %95, %cst_6 : f64
              %104 = arith.mulf %80, %103 : f64
              %105 = arith.addf %104, %cst_4 : f64
              %106 = arith.subf %105, %87 : f64
              %107 = arith.mulf %94, %106 : f64
              %108 = arith.addf %102, %107 : f64
              affine.store %108, %arg5[%arg8, %arg9 - 1, %arg10, 2] : memref<?x?x?x20xf64>
              %109 = arith.mulf %10, %cst_3 : f64
              %110 = arith.mulf %81, %cst_5 : f64
              %111 = arith.addf %110, %cst_6 : f64
              %112 = arith.mulf %81, %111 : f64
              %113 = arith.addf %112, %cst_4 : f64
              %114 = arith.subf %113, %87 : f64
              %115 = arith.mulf %94, %114 : f64
              %116 = arith.addf %109, %115 : f64
              affine.store %116, %arg5[%arg8, %arg9, %arg10 + 1, 3] : memref<?x?x?x20xf64>
              %117 = arith.mulf %11, %cst_3 : f64
              %118 = arith.subf %110, %cst_6 : f64
              %119 = arith.mulf %81, %118 : f64
              %120 = arith.addf %119, %cst_4 : f64
              %121 = arith.subf %120, %87 : f64
              %122 = arith.mulf %94, %121 : f64
              %123 = arith.addf %117, %122 : f64
              affine.store %123, %arg5[%arg8, %arg9, %arg10 - 1, 4] : memref<?x?x?x20xf64>
              %124 = arith.mulf %12, %cst_3 : f64
              %125 = arith.mulf %79, %cst_5 : f64
              %126 = arith.addf %125, %cst_6 : f64
              %127 = arith.mulf %79, %126 : f64
              %128 = arith.addf %127, %cst_4 : f64
              %129 = arith.subf %128, %87 : f64
              %130 = arith.mulf %94, %129 : f64
              %131 = arith.addf %124, %130 : f64
              affine.store %131, %arg5[%arg8 + 1, %arg9, %arg10, 5] : memref<?x?x?x20xf64>
              %132 = arith.mulf %13, %cst_3 : f64
              %133 = arith.subf %125, %cst_6 : f64
              %134 = arith.mulf %79, %133 : f64
              %135 = arith.addf %134, %cst_4 : f64
              %136 = arith.subf %135, %87 : f64
              %137 = arith.mulf %94, %136 : f64
              %138 = arith.addf %132, %137 : f64
              affine.store %138, %arg5[%arg8 - 1, %arg9, %arg10, 6] : memref<?x?x?x20xf64>
              %139 = arith.mulf %14, %cst_3 : f64
              %140 = arith.mulf %46, %cst_7 : f64
              %141 = arith.addf %81, %80 : f64
              %142 = arith.mulf %141, %cst_5 : f64
              %143 = arith.addf %142, %cst_6 : f64
              %144 = arith.mulf %141, %143 : f64
              %145 = arith.addf %144, %cst_4 : f64
              %146 = arith.subf %145, %87 : f64
              %147 = arith.mulf %140, %146 : f64
              %148 = arith.addf %139, %147 : f64
              affine.store %148, %arg5[%arg8, %arg9 + 1, %arg10 + 1, 7] : memref<?x?x?x20xf64>
              %149 = arith.mulf %15, %cst_3 : f64
              %150 = arith.negf %81 : f64
              %151 = arith.addf %150, %80 : f64
              %152 = arith.mulf %151, %cst_5 : f64
              %153 = arith.addf %152, %cst_6 : f64
              %154 = arith.mulf %151, %153 : f64
              %155 = arith.addf %154, %cst_4 : f64
              %156 = arith.subf %155, %87 : f64
              %157 = arith.mulf %140, %156 : f64
              %158 = arith.addf %149, %157 : f64
              affine.store %158, %arg5[%arg8, %arg9 + 1, %arg10 - 1, 8] : memref<?x?x?x20xf64>
              %159 = arith.mulf %16, %cst_3 : f64
              %160 = arith.subf %81, %80 : f64
              %161 = arith.mulf %160, %cst_5 : f64
              %162 = arith.addf %161, %cst_6 : f64
              %163 = arith.mulf %160, %162 : f64
              %164 = arith.addf %163, %cst_4 : f64
              %165 = arith.subf %164, %87 : f64
              %166 = arith.mulf %140, %165 : f64
              %167 = arith.addf %159, %166 : f64
              affine.store %167, %arg5[%arg8, %arg9 - 1, %arg10 + 1, 9] : memref<?x?x?x20xf64>
              %168 = arith.mulf %17, %cst_3 : f64
              %169 = arith.subf %150, %80 : f64
              %170 = arith.mulf %169, %cst_5 : f64
              %171 = arith.addf %170, %cst_6 : f64
              %172 = arith.mulf %169, %171 : f64
              %173 = arith.addf %172, %cst_4 : f64
              %174 = arith.subf %173, %87 : f64
              %175 = arith.mulf %140, %174 : f64
              %176 = arith.addf %168, %175 : f64
              affine.store %176, %arg5[%arg8, %arg9 - 1, %arg10 - 1, 10] : memref<?x?x?x20xf64>
              %177 = arith.mulf %18, %cst_3 : f64
              %178 = arith.addf %80, %79 : f64
              %179 = arith.mulf %178, %cst_5 : f64
              %180 = arith.addf %179, %cst_6 : f64
              %181 = arith.mulf %178, %180 : f64
              %182 = arith.addf %181, %cst_4 : f64
              %183 = arith.subf %182, %87 : f64
              %184 = arith.mulf %140, %183 : f64
              %185 = arith.addf %177, %184 : f64
              affine.store %185, %arg5[%arg8 + 1, %arg9 + 1, %arg10, 11] : memref<?x?x?x20xf64>
              %186 = arith.mulf %19, %cst_3 : f64
              %187 = arith.subf %80, %79 : f64
              %188 = arith.mulf %187, %cst_5 : f64
              %189 = arith.addf %188, %cst_6 : f64
              %190 = arith.mulf %187, %189 : f64
              %191 = arith.addf %190, %cst_4 : f64
              %192 = arith.subf %191, %87 : f64
              %193 = arith.mulf %140, %192 : f64
              %194 = arith.addf %186, %193 : f64
              affine.store %194, %arg5[%arg8 - 1, %arg9 + 1, %arg10, 12] : memref<?x?x?x20xf64>
              %195 = arith.mulf %20, %cst_3 : f64
              %196 = arith.negf %80 : f64
              %197 = arith.addf %196, %79 : f64
              %198 = arith.mulf %197, %cst_5 : f64
              %199 = arith.addf %198, %cst_6 : f64
              %200 = arith.mulf %197, %199 : f64
              %201 = arith.addf %200, %cst_4 : f64
              %202 = arith.subf %201, %87 : f64
              %203 = arith.mulf %140, %202 : f64
              %204 = arith.addf %195, %203 : f64
              affine.store %204, %arg5[%arg8 + 1, %arg9 - 1, %arg10, 13] : memref<?x?x?x20xf64>
              %205 = arith.mulf %21, %cst_3 : f64
              %206 = arith.subf %196, %79 : f64
              %207 = arith.mulf %206, %cst_5 : f64
              %208 = arith.addf %207, %cst_6 : f64
              %209 = arith.mulf %206, %208 : f64
              %210 = arith.addf %209, %cst_4 : f64
              %211 = arith.subf %210, %87 : f64
              %212 = arith.mulf %140, %211 : f64
              %213 = arith.addf %205, %212 : f64
              affine.store %213, %arg5[%arg8 - 1, %arg9 - 1, %arg10, 14] : memref<?x?x?x20xf64>
              %214 = arith.mulf %22, %cst_3 : f64
              %215 = arith.addf %81, %79 : f64
              %216 = arith.mulf %215, %cst_5 : f64
              %217 = arith.addf %216, %cst_6 : f64
              %218 = arith.mulf %215, %217 : f64
              %219 = arith.addf %218, %cst_4 : f64
              %220 = arith.subf %219, %87 : f64
              %221 = arith.mulf %140, %220 : f64
              %222 = arith.addf %214, %221 : f64
              affine.store %222, %arg5[%arg8 + 1, %arg9, %arg10 + 1, 15] : memref<?x?x?x20xf64>
              %223 = arith.mulf %23, %cst_3 : f64
              %224 = arith.subf %81, %79 : f64
              %225 = arith.mulf %224, %cst_5 : f64
              %226 = arith.addf %225, %cst_6 : f64
              %227 = arith.mulf %224, %226 : f64
              %228 = arith.addf %227, %cst_4 : f64
              %229 = arith.subf %228, %87 : f64
              %230 = arith.mulf %140, %229 : f64
              %231 = arith.addf %223, %230 : f64
              affine.store %231, %arg5[%arg8 - 1, %arg9, %arg10 + 1, 16] : memref<?x?x?x20xf64>
              %232 = arith.mulf %24, %cst_3 : f64
              %233 = arith.addf %150, %79 : f64
              %234 = arith.mulf %233, %cst_5 : f64
              %235 = arith.addf %234, %cst_6 : f64
              %236 = arith.mulf %233, %235 : f64
              %237 = arith.addf %236, %cst_4 : f64
              %238 = arith.subf %237, %87 : f64
              %239 = arith.mulf %140, %238 : f64
              %240 = arith.addf %232, %239 : f64
              affine.store %240, %arg5[%arg8 + 1, %arg9, %arg10 - 1, 17] : memref<?x?x?x20xf64>
              %241 = arith.mulf %25, %cst_3 : f64
              %242 = arith.subf %150, %79 : f64
              %243 = arith.mulf %242, %cst_5 : f64
              %244 = arith.addf %243, %cst_6 : f64
              %245 = arith.mulf %242, %244 : f64
              %246 = arith.addf %245, %cst_4 : f64
              %247 = arith.subf %246, %87 : f64
              %248 = arith.mulf %140, %247 : f64
              %249 = arith.addf %241, %248 : f64
              affine.store %249, %arg5[%arg8 - 1, %arg9, %arg10 - 1, 18] : memref<?x?x?x20xf64>
            }
          }
        }
      }
      affine.for %arg8 = 2 to #map()[%0] {
        affine.for %arg9 = 0 to %3 {
          affine.for %arg10 = 0 to %4 {
            %7 = affine.load %arg5[%arg8, %arg9, %arg10, 0] : memref<?x?x?x20xf64>
            %8 = affine.load %arg5[%arg8, %arg9, %arg10, 1] : memref<?x?x?x20xf64>
            %9 = affine.load %arg5[%arg8, %arg9, %arg10, 2] : memref<?x?x?x20xf64>
            %10 = affine.load %arg5[%arg8, %arg9, %arg10, 3] : memref<?x?x?x20xf64>
            %11 = affine.load %arg5[%arg8, %arg9, %arg10, 4] : memref<?x?x?x20xf64>
            %12 = affine.load %arg5[%arg8, %arg9, %arg10, 5] : memref<?x?x?x20xf64>
            %13 = affine.load %arg5[%arg8, %arg9, %arg10, 6] : memref<?x?x?x20xf64>
            %14 = affine.load %arg5[%arg8, %arg9, %arg10, 7] : memref<?x?x?x20xf64>
            %15 = affine.load %arg5[%arg8, %arg9, %arg10, 8] : memref<?x?x?x20xf64>
            %16 = affine.load %arg5[%arg8, %arg9, %arg10, 9] : memref<?x?x?x20xf64>
            %17 = affine.load %arg5[%arg8, %arg9, %arg10, 10] : memref<?x?x?x20xf64>
            %18 = affine.load %arg5[%arg8, %arg9, %arg10, 11] : memref<?x?x?x20xf64>
            %19 = affine.load %arg5[%arg8, %arg9, %arg10, 12] : memref<?x?x?x20xf64>
            %20 = affine.load %arg5[%arg8, %arg9, %arg10, 13] : memref<?x?x?x20xf64>
            %21 = affine.load %arg5[%arg8, %arg9, %arg10, 14] : memref<?x?x?x20xf64>
            %22 = affine.load %arg5[%arg8, %arg9, %arg10, 15] : memref<?x?x?x20xf64>
            %23 = affine.load %arg5[%arg8, %arg9, %arg10, 16] : memref<?x?x?x20xf64>
            %24 = affine.load %arg5[%arg8, %arg9, %arg10, 17] : memref<?x?x?x20xf64>
            %25 = affine.load %arg5[%arg8, %arg9, %arg10, 18] : memref<?x?x?x20xf64>
            %26 = affine.load %arg6[%arg8, %arg9, %arg10] : memref<?x?x?xi32>
            %27 = arith.andi %26, %c1_i32 : i32
            %28 = arith.cmpi ne, %27, %c0_i32 : i32
            scf.if %28 {
              affine.store %7, %arg4[%arg8, %arg9, %arg10, 0] : memref<?x?x?x20xf64>
              affine.store %8, %arg4[%arg8, %arg9 - 1, %arg10, 2] : memref<?x?x?x20xf64>
              affine.store %9, %arg4[%arg8, %arg9 + 1, %arg10, 1] : memref<?x?x?x20xf64>
              affine.store %10, %arg4[%arg8, %arg9, %arg10 - 1, 4] : memref<?x?x?x20xf64>
              affine.store %11, %arg4[%arg8, %arg9, %arg10 + 1, 3] : memref<?x?x?x20xf64>
              affine.store %12, %arg4[%arg8 - 1, %arg9, %arg10, 6] : memref<?x?x?x20xf64>
              affine.store %13, %arg4[%arg8 + 1, %arg9, %arg10, 5] : memref<?x?x?x20xf64>
              affine.store %14, %arg4[%arg8, %arg9 - 1, %arg10 - 1, 10] : memref<?x?x?x20xf64>
              affine.store %15, %arg4[%arg8, %arg9 - 1, %arg10 + 1, 9] : memref<?x?x?x20xf64>
              affine.store %16, %arg4[%arg8, %arg9 + 1, %arg10 - 1, 8] : memref<?x?x?x20xf64>
              affine.store %17, %arg4[%arg8, %arg9 + 1, %arg10 + 1, 7] : memref<?x?x?x20xf64>
              affine.store %18, %arg4[%arg8 - 1, %arg9 - 1, %arg10, 14] : memref<?x?x?x20xf64>
              affine.store %19, %arg4[%arg8 + 1, %arg9 - 1, %arg10, 13] : memref<?x?x?x20xf64>
              affine.store %20, %arg4[%arg8 - 1, %arg9 + 1, %arg10, 12] : memref<?x?x?x20xf64>
              affine.store %21, %arg4[%arg8 + 1, %arg9 + 1, %arg10, 11] : memref<?x?x?x20xf64>
              affine.store %22, %arg4[%arg8 - 1, %arg9, %arg10 - 1, 18] : memref<?x?x?x20xf64>
              affine.store %23, %arg4[%arg8 + 1, %arg9, %arg10 - 1, 17] : memref<?x?x?x20xf64>
              affine.store %24, %arg4[%arg8 - 1, %arg9, %arg10 + 1, 16] : memref<?x?x?x20xf64>
              affine.store %25, %arg4[%arg8 + 1, %arg9, %arg10 + 1, 15] : memref<?x?x?x20xf64>
            } else {
              %29 = arith.addf %7, %8 : f64
              %30 = arith.addf %29, %9 : f64
              %31 = arith.addf %30, %10 : f64
              %32 = arith.addf %31, %11 : f64
              %33 = arith.addf %32, %12 : f64
              %34 = arith.addf %33, %13 : f64
              %35 = arith.addf %34, %14 : f64
              %36 = arith.addf %35, %15 : f64
              %37 = arith.addf %36, %16 : f64
              %38 = arith.addf %37, %17 : f64
              %39 = arith.addf %38, %18 : f64
              %40 = arith.addf %39, %19 : f64
              %41 = arith.addf %40, %20 : f64
              %42 = arith.addf %41, %21 : f64
              %43 = arith.addf %42, %22 : f64
              %44 = arith.addf %43, %23 : f64
              %45 = arith.addf %44, %24 : f64
              %46 = arith.addf %45, %25 : f64
              %47 = arith.subf %10, %11 : f64
              %48 = arith.addf %47, %14 : f64
              %49 = arith.subf %48, %15 : f64
              %50 = arith.addf %49, %16 : f64
              %51 = arith.subf %50, %17 : f64
              %52 = arith.addf %51, %22 : f64
              %53 = arith.addf %52, %23 : f64
              %54 = arith.subf %53, %24 : f64
              %55 = arith.subf %54, %25 : f64
              %56 = arith.subf %8, %9 : f64
              %57 = arith.addf %56, %14 : f64
              %58 = arith.addf %57, %15 : f64
              %59 = arith.subf %58, %16 : f64
              %60 = arith.subf %59, %17 : f64
              %61 = arith.addf %60, %18 : f64
              %62 = arith.addf %61, %19 : f64
              %63 = arith.subf %62, %20 : f64
              %64 = arith.subf %63, %21 : f64
              %65 = arith.subf %12, %13 : f64
              %66 = arith.addf %65, %18 : f64
              %67 = arith.subf %66, %19 : f64
              %68 = arith.addf %67, %20 : f64
              %69 = arith.subf %68, %21 : f64
              %70 = arith.addf %69, %22 : f64
              %71 = arith.subf %70, %23 : f64
              %72 = arith.addf %71, %24 : f64
              %73 = arith.subf %72, %25 : f64
              %74 = arith.divf %55, %46 : f64
              %75 = arith.divf %64, %46 : f64
              %76 = arith.divf %73, %46 : f64
              %77 = arith.andi %26, %c2_i32 : i32
              %78 = arith.cmpi ne, %77, %c0_i32 : i32
              %79 = arith.select %78, %cst_1, %76 : f64
              %80 = arith.select %78, %cst_0, %75 : f64
              %81 = arith.select %78, %cst, %74 : f64
              %82 = arith.mulf %81, %81 : f64
              %83 = arith.mulf %80, %80 : f64
              %84 = arith.addf %82, %83 : f64
              %85 = arith.mulf %79, %79 : f64
              %86 = arith.addf %84, %85 : f64
              %87 = arith.mulf %86, %cst_2 : f64
              %88 = arith.mulf %7, %cst_3 : f64
              %89 = arith.mulf %46, %cst_9 : f64
              %90 = arith.subf %cst_4, %87 : f64
              %91 = arith.mulf %89, %90 : f64
              %92 = arith.addf %88, %91 : f64
              affine.store %92, %arg4[%arg8, %arg9, %arg10, 0] : memref<?x?x?x20xf64>
              %93 = arith.mulf %8, %cst_3 : f64
              %94 = arith.mulf %46, %cst_8 : f64
              %95 = arith.mulf %80, %cst_5 : f64
              %96 = arith.addf %95, %cst_6 : f64
              %97 = arith.mulf %80, %96 : f64
              %98 = arith.addf %97, %cst_4 : f64
              %99 = arith.subf %98, %87 : f64
              %100 = arith.mulf %94, %99 : f64
              %101 = arith.addf %93, %100 : f64
              affine.store %101, %arg4[%arg8, %arg9 + 1, %arg10, 1] : memref<?x?x?x20xf64>
              %102 = arith.mulf %9, %cst_3 : f64
              %103 = arith.subf %95, %cst_6 : f64
              %104 = arith.mulf %80, %103 : f64
              %105 = arith.addf %104, %cst_4 : f64
              %106 = arith.subf %105, %87 : f64
              %107 = arith.mulf %94, %106 : f64
              %108 = arith.addf %102, %107 : f64
              affine.store %108, %arg4[%arg8, %arg9 - 1, %arg10, 2] : memref<?x?x?x20xf64>
              %109 = arith.mulf %10, %cst_3 : f64
              %110 = arith.mulf %81, %cst_5 : f64
              %111 = arith.addf %110, %cst_6 : f64
              %112 = arith.mulf %81, %111 : f64
              %113 = arith.addf %112, %cst_4 : f64
              %114 = arith.subf %113, %87 : f64
              %115 = arith.mulf %94, %114 : f64
              %116 = arith.addf %109, %115 : f64
              affine.store %116, %arg4[%arg8, %arg9, %arg10 + 1, 3] : memref<?x?x?x20xf64>
              %117 = arith.mulf %11, %cst_3 : f64
              %118 = arith.subf %110, %cst_6 : f64
              %119 = arith.mulf %81, %118 : f64
              %120 = arith.addf %119, %cst_4 : f64
              %121 = arith.subf %120, %87 : f64
              %122 = arith.mulf %94, %121 : f64
              %123 = arith.addf %117, %122 : f64
              affine.store %123, %arg4[%arg8, %arg9, %arg10 - 1, 4] : memref<?x?x?x20xf64>
              %124 = arith.mulf %12, %cst_3 : f64
              %125 = arith.mulf %79, %cst_5 : f64
              %126 = arith.addf %125, %cst_6 : f64
              %127 = arith.mulf %79, %126 : f64
              %128 = arith.addf %127, %cst_4 : f64
              %129 = arith.subf %128, %87 : f64
              %130 = arith.mulf %94, %129 : f64
              %131 = arith.addf %124, %130 : f64
              affine.store %131, %arg4[%arg8 + 1, %arg9, %arg10, 5] : memref<?x?x?x20xf64>
              %132 = arith.mulf %13, %cst_3 : f64
              %133 = arith.subf %125, %cst_6 : f64
              %134 = arith.mulf %79, %133 : f64
              %135 = arith.addf %134, %cst_4 : f64
              %136 = arith.subf %135, %87 : f64
              %137 = arith.mulf %94, %136 : f64
              %138 = arith.addf %132, %137 : f64
              affine.store %138, %arg4[%arg8 - 1, %arg9, %arg10, 6] : memref<?x?x?x20xf64>
              %139 = arith.mulf %14, %cst_3 : f64
              %140 = arith.mulf %46, %cst_7 : f64
              %141 = arith.addf %81, %80 : f64
              %142 = arith.mulf %141, %cst_5 : f64
              %143 = arith.addf %142, %cst_6 : f64
              %144 = arith.mulf %141, %143 : f64
              %145 = arith.addf %144, %cst_4 : f64
              %146 = arith.subf %145, %87 : f64
              %147 = arith.mulf %140, %146 : f64
              %148 = arith.addf %139, %147 : f64
              affine.store %148, %arg4[%arg8, %arg9 + 1, %arg10 + 1, 7] : memref<?x?x?x20xf64>
              %149 = arith.mulf %15, %cst_3 : f64
              %150 = arith.negf %81 : f64
              %151 = arith.addf %150, %80 : f64
              %152 = arith.mulf %151, %cst_5 : f64
              %153 = arith.addf %152, %cst_6 : f64
              %154 = arith.mulf %151, %153 : f64
              %155 = arith.addf %154, %cst_4 : f64
              %156 = arith.subf %155, %87 : f64
              %157 = arith.mulf %140, %156 : f64
              %158 = arith.addf %149, %157 : f64
              affine.store %158, %arg4[%arg8, %arg9 + 1, %arg10 - 1, 8] : memref<?x?x?x20xf64>
              %159 = arith.mulf %16, %cst_3 : f64
              %160 = arith.subf %81, %80 : f64
              %161 = arith.mulf %160, %cst_5 : f64
              %162 = arith.addf %161, %cst_6 : f64
              %163 = arith.mulf %160, %162 : f64
              %164 = arith.addf %163, %cst_4 : f64
              %165 = arith.subf %164, %87 : f64
              %166 = arith.mulf %140, %165 : f64
              %167 = arith.addf %159, %166 : f64
              affine.store %167, %arg4[%arg8, %arg9 - 1, %arg10 + 1, 9] : memref<?x?x?x20xf64>
              %168 = arith.mulf %17, %cst_3 : f64
              %169 = arith.subf %150, %80 : f64
              %170 = arith.mulf %169, %cst_5 : f64
              %171 = arith.addf %170, %cst_6 : f64
              %172 = arith.mulf %169, %171 : f64
              %173 = arith.addf %172, %cst_4 : f64
              %174 = arith.subf %173, %87 : f64
              %175 = arith.mulf %140, %174 : f64
              %176 = arith.addf %168, %175 : f64
              affine.store %176, %arg4[%arg8, %arg9 - 1, %arg10 - 1, 10] : memref<?x?x?x20xf64>
              %177 = arith.mulf %18, %cst_3 : f64
              %178 = arith.addf %80, %79 : f64
              %179 = arith.mulf %178, %cst_5 : f64
              %180 = arith.addf %179, %cst_6 : f64
              %181 = arith.mulf %178, %180 : f64
              %182 = arith.addf %181, %cst_4 : f64
              %183 = arith.subf %182, %87 : f64
              %184 = arith.mulf %140, %183 : f64
              %185 = arith.addf %177, %184 : f64
              affine.store %185, %arg4[%arg8 + 1, %arg9 + 1, %arg10, 11] : memref<?x?x?x20xf64>
              %186 = arith.mulf %19, %cst_3 : f64
              %187 = arith.subf %80, %79 : f64
              %188 = arith.mulf %187, %cst_5 : f64
              %189 = arith.addf %188, %cst_6 : f64
              %190 = arith.mulf %187, %189 : f64
              %191 = arith.addf %190, %cst_4 : f64
              %192 = arith.subf %191, %87 : f64
              %193 = arith.mulf %140, %192 : f64
              %194 = arith.addf %186, %193 : f64
              affine.store %194, %arg4[%arg8 - 1, %arg9 + 1, %arg10, 12] : memref<?x?x?x20xf64>
              %195 = arith.mulf %20, %cst_3 : f64
              %196 = arith.negf %80 : f64
              %197 = arith.addf %196, %79 : f64
              %198 = arith.mulf %197, %cst_5 : f64
              %199 = arith.addf %198, %cst_6 : f64
              %200 = arith.mulf %197, %199 : f64
              %201 = arith.addf %200, %cst_4 : f64
              %202 = arith.subf %201, %87 : f64
              %203 = arith.mulf %140, %202 : f64
              %204 = arith.addf %195, %203 : f64
              affine.store %204, %arg4[%arg8 + 1, %arg9 - 1, %arg10, 13] : memref<?x?x?x20xf64>
              %205 = arith.mulf %21, %cst_3 : f64
              %206 = arith.subf %196, %79 : f64
              %207 = arith.mulf %206, %cst_5 : f64
              %208 = arith.addf %207, %cst_6 : f64
              %209 = arith.mulf %206, %208 : f64
              %210 = arith.addf %209, %cst_4 : f64
              %211 = arith.subf %210, %87 : f64
              %212 = arith.mulf %140, %211 : f64
              %213 = arith.addf %205, %212 : f64
              affine.store %213, %arg4[%arg8 - 1, %arg9 - 1, %arg10, 14] : memref<?x?x?x20xf64>
              %214 = arith.mulf %22, %cst_3 : f64
              %215 = arith.addf %81, %79 : f64
              %216 = arith.mulf %215, %cst_5 : f64
              %217 = arith.addf %216, %cst_6 : f64
              %218 = arith.mulf %215, %217 : f64
              %219 = arith.addf %218, %cst_4 : f64
              %220 = arith.subf %219, %87 : f64
              %221 = arith.mulf %140, %220 : f64
              %222 = arith.addf %214, %221 : f64
              affine.store %222, %arg4[%arg8 + 1, %arg9, %arg10 + 1, 15] : memref<?x?x?x20xf64>
              %223 = arith.mulf %23, %cst_3 : f64
              %224 = arith.subf %81, %79 : f64
              %225 = arith.mulf %224, %cst_5 : f64
              %226 = arith.addf %225, %cst_6 : f64
              %227 = arith.mulf %224, %226 : f64
              %228 = arith.addf %227, %cst_4 : f64
              %229 = arith.subf %228, %87 : f64
              %230 = arith.mulf %140, %229 : f64
              %231 = arith.addf %223, %230 : f64
              affine.store %231, %arg4[%arg8 - 1, %arg9, %arg10 + 1, 16] : memref<?x?x?x20xf64>
              %232 = arith.mulf %24, %cst_3 : f64
              %233 = arith.addf %150, %79 : f64
              %234 = arith.mulf %233, %cst_5 : f64
              %235 = arith.addf %234, %cst_6 : f64
              %236 = arith.mulf %233, %235 : f64
              %237 = arith.addf %236, %cst_4 : f64
              %238 = arith.subf %237, %87 : f64
              %239 = arith.mulf %140, %238 : f64
              %240 = arith.addf %232, %239 : f64
              affine.store %240, %arg4[%arg8 + 1, %arg9, %arg10 - 1, 17] : memref<?x?x?x20xf64>
              %241 = arith.mulf %25, %cst_3 : f64
              %242 = arith.subf %150, %79 : f64
              %243 = arith.mulf %242, %cst_5 : f64
              %244 = arith.addf %243, %cst_6 : f64
              %245 = arith.mulf %242, %244 : f64
              %246 = arith.addf %245, %cst_4 : f64
              %247 = arith.subf %246, %87 : f64
              %248 = arith.mulf %140, %247 : f64
              %249 = arith.addf %241, %248 : f64
              affine.store %249, %arg4[%arg8 - 1, %arg9, %arg10 - 1, 18] : memref<?x?x?x20xf64>
            }
          }
        }
      }
    }
    %5 = arith.remsi %arg3, %c2_i32 : i32
    %6 = arith.cmpi eq, %5, %c1_i32 : i32
    scf.if %6 {
      affine.for %arg7 = 2 to #map()[%0] {
        affine.for %arg8 = 0 to %3 {
          affine.for %arg9 = 0 to %4 {
            %7 = affine.load %arg4[%arg7, %arg8, %arg9, 0] : memref<?x?x?x20xf64>
            %8 = affine.load %arg4[%arg7, %arg8, %arg9, 1] : memref<?x?x?x20xf64>
            %9 = affine.load %arg4[%arg7, %arg8, %arg9, 2] : memref<?x?x?x20xf64>
            %10 = affine.load %arg4[%arg7, %arg8, %arg9, 3] : memref<?x?x?x20xf64>
            %11 = affine.load %arg4[%arg7, %arg8, %arg9, 4] : memref<?x?x?x20xf64>
            %12 = affine.load %arg4[%arg7, %arg8, %arg9, 5] : memref<?x?x?x20xf64>
            %13 = affine.load %arg4[%arg7, %arg8, %arg9, 6] : memref<?x?x?x20xf64>
            %14 = affine.load %arg4[%arg7, %arg8, %arg9, 7] : memref<?x?x?x20xf64>
            %15 = affine.load %arg4[%arg7, %arg8, %arg9, 8] : memref<?x?x?x20xf64>
            %16 = affine.load %arg4[%arg7, %arg8, %arg9, 9] : memref<?x?x?x20xf64>
            %17 = affine.load %arg4[%arg7, %arg8, %arg9, 10] : memref<?x?x?x20xf64>
            %18 = affine.load %arg4[%arg7, %arg8, %arg9, 11] : memref<?x?x?x20xf64>
            %19 = affine.load %arg4[%arg7, %arg8, %arg9, 12] : memref<?x?x?x20xf64>
            %20 = affine.load %arg4[%arg7, %arg8, %arg9, 13] : memref<?x?x?x20xf64>
            %21 = affine.load %arg4[%arg7, %arg8, %arg9, 14] : memref<?x?x?x20xf64>
            %22 = affine.load %arg4[%arg7, %arg8, %arg9, 15] : memref<?x?x?x20xf64>
            %23 = affine.load %arg4[%arg7, %arg8, %arg9, 16] : memref<?x?x?x20xf64>
            %24 = affine.load %arg4[%arg7, %arg8, %arg9, 17] : memref<?x?x?x20xf64>
            %25 = affine.load %arg4[%arg7, %arg8, %arg9, 18] : memref<?x?x?x20xf64>
            %26 = affine.load %arg6[%arg7, %arg8, %arg9] : memref<?x?x?xi32>
            %27 = arith.andi %26, %c1_i32 : i32
            %28 = arith.cmpi ne, %27, %c0_i32 : i32
            scf.if %28 {
              affine.store %7, %arg5[%arg7, %arg8, %arg9, 0] : memref<?x?x?x20xf64>
              affine.store %8, %arg5[%arg7, %arg8 - 1, %arg9, 2] : memref<?x?x?x20xf64>
              affine.store %9, %arg5[%arg7, %arg8 + 1, %arg9, 1] : memref<?x?x?x20xf64>
              affine.store %10, %arg5[%arg7, %arg8, %arg9 - 1, 4] : memref<?x?x?x20xf64>
              affine.store %11, %arg5[%arg7, %arg8, %arg9 + 1, 3] : memref<?x?x?x20xf64>
              affine.store %12, %arg5[%arg7 - 1, %arg8, %arg9, 6] : memref<?x?x?x20xf64>
              affine.store %13, %arg5[%arg7 + 1, %arg8, %arg9, 5] : memref<?x?x?x20xf64>
              affine.store %14, %arg5[%arg7, %arg8 - 1, %arg9 - 1, 10] : memref<?x?x?x20xf64>
              affine.store %15, %arg5[%arg7, %arg8 - 1, %arg9 + 1, 9] : memref<?x?x?x20xf64>
              affine.store %16, %arg5[%arg7, %arg8 + 1, %arg9 - 1, 8] : memref<?x?x?x20xf64>
              affine.store %17, %arg5[%arg7, %arg8 + 1, %arg9 + 1, 7] : memref<?x?x?x20xf64>
              affine.store %18, %arg5[%arg7 - 1, %arg8 - 1, %arg9, 14] : memref<?x?x?x20xf64>
              affine.store %19, %arg5[%arg7 + 1, %arg8 - 1, %arg9, 13] : memref<?x?x?x20xf64>
              affine.store %20, %arg5[%arg7 - 1, %arg8 + 1, %arg9, 12] : memref<?x?x?x20xf64>
              affine.store %21, %arg5[%arg7 + 1, %arg8 + 1, %arg9, 11] : memref<?x?x?x20xf64>
              affine.store %22, %arg5[%arg7 - 1, %arg8, %arg9 - 1, 18] : memref<?x?x?x20xf64>
              affine.store %23, %arg5[%arg7 + 1, %arg8, %arg9 - 1, 17] : memref<?x?x?x20xf64>
              affine.store %24, %arg5[%arg7 - 1, %arg8, %arg9 + 1, 16] : memref<?x?x?x20xf64>
              affine.store %25, %arg5[%arg7 + 1, %arg8, %arg9 + 1, 15] : memref<?x?x?x20xf64>
            } else {
              %29 = arith.addf %7, %8 : f64
              %30 = arith.addf %29, %9 : f64
              %31 = arith.addf %30, %10 : f64
              %32 = arith.addf %31, %11 : f64
              %33 = arith.addf %32, %12 : f64
              %34 = arith.addf %33, %13 : f64
              %35 = arith.addf %34, %14 : f64
              %36 = arith.addf %35, %15 : f64
              %37 = arith.addf %36, %16 : f64
              %38 = arith.addf %37, %17 : f64
              %39 = arith.addf %38, %18 : f64
              %40 = arith.addf %39, %19 : f64
              %41 = arith.addf %40, %20 : f64
              %42 = arith.addf %41, %21 : f64
              %43 = arith.addf %42, %22 : f64
              %44 = arith.addf %43, %23 : f64
              %45 = arith.addf %44, %24 : f64
              %46 = arith.addf %45, %25 : f64
              %47 = arith.subf %10, %11 : f64
              %48 = arith.addf %47, %14 : f64
              %49 = arith.subf %48, %15 : f64
              %50 = arith.addf %49, %16 : f64
              %51 = arith.subf %50, %17 : f64
              %52 = arith.addf %51, %22 : f64
              %53 = arith.addf %52, %23 : f64
              %54 = arith.subf %53, %24 : f64
              %55 = arith.subf %54, %25 : f64
              %56 = arith.subf %8, %9 : f64
              %57 = arith.addf %56, %14 : f64
              %58 = arith.addf %57, %15 : f64
              %59 = arith.subf %58, %16 : f64
              %60 = arith.subf %59, %17 : f64
              %61 = arith.addf %60, %18 : f64
              %62 = arith.addf %61, %19 : f64
              %63 = arith.subf %62, %20 : f64
              %64 = arith.subf %63, %21 : f64
              %65 = arith.subf %12, %13 : f64
              %66 = arith.addf %65, %18 : f64
              %67 = arith.subf %66, %19 : f64
              %68 = arith.addf %67, %20 : f64
              %69 = arith.subf %68, %21 : f64
              %70 = arith.addf %69, %22 : f64
              %71 = arith.subf %70, %23 : f64
              %72 = arith.addf %71, %24 : f64
              %73 = arith.subf %72, %25 : f64
              %74 = arith.divf %55, %46 : f64
              %75 = arith.divf %64, %46 : f64
              %76 = arith.divf %73, %46 : f64
              %77 = arith.andi %26, %c2_i32 : i32
              %78 = arith.cmpi ne, %77, %c0_i32 : i32
              %79 = arith.select %78, %cst_1, %76 : f64
              %80 = arith.select %78, %cst_0, %75 : f64
              %81 = arith.select %78, %cst, %74 : f64
              %82 = arith.mulf %81, %81 : f64
              %83 = arith.mulf %80, %80 : f64
              %84 = arith.addf %82, %83 : f64
              %85 = arith.mulf %79, %79 : f64
              %86 = arith.addf %84, %85 : f64
              %87 = arith.mulf %86, %cst_2 : f64
              %88 = arith.mulf %7, %cst_3 : f64
              %89 = arith.mulf %46, %cst_9 : f64
              %90 = arith.subf %cst_4, %87 : f64
              %91 = arith.mulf %89, %90 : f64
              %92 = arith.addf %88, %91 : f64
              affine.store %92, %arg5[%arg7, %arg8, %arg9, 0] : memref<?x?x?x20xf64>
              %93 = arith.mulf %8, %cst_3 : f64
              %94 = arith.mulf %46, %cst_8 : f64
              %95 = arith.mulf %80, %cst_5 : f64
              %96 = arith.addf %95, %cst_6 : f64
              %97 = arith.mulf %80, %96 : f64
              %98 = arith.addf %97, %cst_4 : f64
              %99 = arith.subf %98, %87 : f64
              %100 = arith.mulf %94, %99 : f64
              %101 = arith.addf %93, %100 : f64
              affine.store %101, %arg5[%arg7, %arg8 + 1, %arg9, 1] : memref<?x?x?x20xf64>
              %102 = arith.mulf %9, %cst_3 : f64
              %103 = arith.subf %95, %cst_6 : f64
              %104 = arith.mulf %80, %103 : f64
              %105 = arith.addf %104, %cst_4 : f64
              %106 = arith.subf %105, %87 : f64
              %107 = arith.mulf %94, %106 : f64
              %108 = arith.addf %102, %107 : f64
              affine.store %108, %arg5[%arg7, %arg8 - 1, %arg9, 2] : memref<?x?x?x20xf64>
              %109 = arith.mulf %10, %cst_3 : f64
              %110 = arith.mulf %81, %cst_5 : f64
              %111 = arith.addf %110, %cst_6 : f64
              %112 = arith.mulf %81, %111 : f64
              %113 = arith.addf %112, %cst_4 : f64
              %114 = arith.subf %113, %87 : f64
              %115 = arith.mulf %94, %114 : f64
              %116 = arith.addf %109, %115 : f64
              affine.store %116, %arg5[%arg7, %arg8, %arg9 + 1, 3] : memref<?x?x?x20xf64>
              %117 = arith.mulf %11, %cst_3 : f64
              %118 = arith.subf %110, %cst_6 : f64
              %119 = arith.mulf %81, %118 : f64
              %120 = arith.addf %119, %cst_4 : f64
              %121 = arith.subf %120, %87 : f64
              %122 = arith.mulf %94, %121 : f64
              %123 = arith.addf %117, %122 : f64
              affine.store %123, %arg5[%arg7, %arg8, %arg9 - 1, 4] : memref<?x?x?x20xf64>
              %124 = arith.mulf %12, %cst_3 : f64
              %125 = arith.mulf %79, %cst_5 : f64
              %126 = arith.addf %125, %cst_6 : f64
              %127 = arith.mulf %79, %126 : f64
              %128 = arith.addf %127, %cst_4 : f64
              %129 = arith.subf %128, %87 : f64
              %130 = arith.mulf %94, %129 : f64
              %131 = arith.addf %124, %130 : f64
              affine.store %131, %arg5[%arg7 + 1, %arg8, %arg9, 5] : memref<?x?x?x20xf64>
              %132 = arith.mulf %13, %cst_3 : f64
              %133 = arith.subf %125, %cst_6 : f64
              %134 = arith.mulf %79, %133 : f64
              %135 = arith.addf %134, %cst_4 : f64
              %136 = arith.subf %135, %87 : f64
              %137 = arith.mulf %94, %136 : f64
              %138 = arith.addf %132, %137 : f64
              affine.store %138, %arg5[%arg7 - 1, %arg8, %arg9, 6] : memref<?x?x?x20xf64>
              %139 = arith.mulf %14, %cst_3 : f64
              %140 = arith.mulf %46, %cst_7 : f64
              %141 = arith.addf %81, %80 : f64
              %142 = arith.mulf %141, %cst_5 : f64
              %143 = arith.addf %142, %cst_6 : f64
              %144 = arith.mulf %141, %143 : f64
              %145 = arith.addf %144, %cst_4 : f64
              %146 = arith.subf %145, %87 : f64
              %147 = arith.mulf %140, %146 : f64
              %148 = arith.addf %139, %147 : f64
              affine.store %148, %arg5[%arg7, %arg8 + 1, %arg9 + 1, 7] : memref<?x?x?x20xf64>
              %149 = arith.mulf %15, %cst_3 : f64
              %150 = arith.negf %81 : f64
              %151 = arith.addf %150, %80 : f64
              %152 = arith.mulf %151, %cst_5 : f64
              %153 = arith.addf %152, %cst_6 : f64
              %154 = arith.mulf %151, %153 : f64
              %155 = arith.addf %154, %cst_4 : f64
              %156 = arith.subf %155, %87 : f64
              %157 = arith.mulf %140, %156 : f64
              %158 = arith.addf %149, %157 : f64
              affine.store %158, %arg5[%arg7, %arg8 + 1, %arg9 - 1, 8] : memref<?x?x?x20xf64>
              %159 = arith.mulf %16, %cst_3 : f64
              %160 = arith.subf %81, %80 : f64
              %161 = arith.mulf %160, %cst_5 : f64
              %162 = arith.addf %161, %cst_6 : f64
              %163 = arith.mulf %160, %162 : f64
              %164 = arith.addf %163, %cst_4 : f64
              %165 = arith.subf %164, %87 : f64
              %166 = arith.mulf %140, %165 : f64
              %167 = arith.addf %159, %166 : f64
              affine.store %167, %arg5[%arg7, %arg8 - 1, %arg9 + 1, 9] : memref<?x?x?x20xf64>
              %168 = arith.mulf %17, %cst_3 : f64
              %169 = arith.subf %150, %80 : f64
              %170 = arith.mulf %169, %cst_5 : f64
              %171 = arith.addf %170, %cst_6 : f64
              %172 = arith.mulf %169, %171 : f64
              %173 = arith.addf %172, %cst_4 : f64
              %174 = arith.subf %173, %87 : f64
              %175 = arith.mulf %140, %174 : f64
              %176 = arith.addf %168, %175 : f64
              affine.store %176, %arg5[%arg7, %arg8 - 1, %arg9 - 1, 10] : memref<?x?x?x20xf64>
              %177 = arith.mulf %18, %cst_3 : f64
              %178 = arith.addf %80, %79 : f64
              %179 = arith.mulf %178, %cst_5 : f64
              %180 = arith.addf %179, %cst_6 : f64
              %181 = arith.mulf %178, %180 : f64
              %182 = arith.addf %181, %cst_4 : f64
              %183 = arith.subf %182, %87 : f64
              %184 = arith.mulf %140, %183 : f64
              %185 = arith.addf %177, %184 : f64
              affine.store %185, %arg5[%arg7 + 1, %arg8 + 1, %arg9, 11] : memref<?x?x?x20xf64>
              %186 = arith.mulf %19, %cst_3 : f64
              %187 = arith.subf %80, %79 : f64
              %188 = arith.mulf %187, %cst_5 : f64
              %189 = arith.addf %188, %cst_6 : f64
              %190 = arith.mulf %187, %189 : f64
              %191 = arith.addf %190, %cst_4 : f64
              %192 = arith.subf %191, %87 : f64
              %193 = arith.mulf %140, %192 : f64
              %194 = arith.addf %186, %193 : f64
              affine.store %194, %arg5[%arg7 - 1, %arg8 + 1, %arg9, 12] : memref<?x?x?x20xf64>
              %195 = arith.mulf %20, %cst_3 : f64
              %196 = arith.negf %80 : f64
              %197 = arith.addf %196, %79 : f64
              %198 = arith.mulf %197, %cst_5 : f64
              %199 = arith.addf %198, %cst_6 : f64
              %200 = arith.mulf %197, %199 : f64
              %201 = arith.addf %200, %cst_4 : f64
              %202 = arith.subf %201, %87 : f64
              %203 = arith.mulf %140, %202 : f64
              %204 = arith.addf %195, %203 : f64
              affine.store %204, %arg5[%arg7 + 1, %arg8 - 1, %arg9, 13] : memref<?x?x?x20xf64>
              %205 = arith.mulf %21, %cst_3 : f64
              %206 = arith.subf %196, %79 : f64
              %207 = arith.mulf %206, %cst_5 : f64
              %208 = arith.addf %207, %cst_6 : f64
              %209 = arith.mulf %206, %208 : f64
              %210 = arith.addf %209, %cst_4 : f64
              %211 = arith.subf %210, %87 : f64
              %212 = arith.mulf %140, %211 : f64
              %213 = arith.addf %205, %212 : f64
              affine.store %213, %arg5[%arg7 - 1, %arg8 - 1, %arg9, 14] : memref<?x?x?x20xf64>
              %214 = arith.mulf %22, %cst_3 : f64
              %215 = arith.addf %81, %79 : f64
              %216 = arith.mulf %215, %cst_5 : f64
              %217 = arith.addf %216, %cst_6 : f64
              %218 = arith.mulf %215, %217 : f64
              %219 = arith.addf %218, %cst_4 : f64
              %220 = arith.subf %219, %87 : f64
              %221 = arith.mulf %140, %220 : f64
              %222 = arith.addf %214, %221 : f64
              affine.store %222, %arg5[%arg7 + 1, %arg8, %arg9 + 1, 15] : memref<?x?x?x20xf64>
              %223 = arith.mulf %23, %cst_3 : f64
              %224 = arith.subf %81, %79 : f64
              %225 = arith.mulf %224, %cst_5 : f64
              %226 = arith.addf %225, %cst_6 : f64
              %227 = arith.mulf %224, %226 : f64
              %228 = arith.addf %227, %cst_4 : f64
              %229 = arith.subf %228, %87 : f64
              %230 = arith.mulf %140, %229 : f64
              %231 = arith.addf %223, %230 : f64
              affine.store %231, %arg5[%arg7 - 1, %arg8, %arg9 + 1, 16] : memref<?x?x?x20xf64>
              %232 = arith.mulf %24, %cst_3 : f64
              %233 = arith.addf %150, %79 : f64
              %234 = arith.mulf %233, %cst_5 : f64
              %235 = arith.addf %234, %cst_6 : f64
              %236 = arith.mulf %233, %235 : f64
              %237 = arith.addf %236, %cst_4 : f64
              %238 = arith.subf %237, %87 : f64
              %239 = arith.mulf %140, %238 : f64
              %240 = arith.addf %232, %239 : f64
              affine.store %240, %arg5[%arg7 + 1, %arg8, %arg9 - 1, 17] : memref<?x?x?x20xf64>
              %241 = arith.mulf %25, %cst_3 : f64
              %242 = arith.subf %150, %79 : f64
              %243 = arith.mulf %242, %cst_5 : f64
              %244 = arith.addf %243, %cst_6 : f64
              %245 = arith.mulf %242, %244 : f64
              %246 = arith.addf %245, %cst_4 : f64
              %247 = arith.subf %246, %87 : f64
              %248 = arith.mulf %140, %247 : f64
              %249 = arith.addf %241, %248 : f64
              affine.store %249, %arg5[%arg7 - 1, %arg8, %arg9 - 1, 18] : memref<?x?x?x20xf64>
            }
          }
        }
      }
    }
    return
  }
}
