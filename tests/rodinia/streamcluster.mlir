module {
  func.func @streamcluster(%arg0: i32, %arg1: i32, %arg2: i32, %arg3: i32, %arg4: memref<?x?xf32>, %arg5: memref<?xf32>, %arg6: memref<?xi32>, %arg7: memref<?xf32>, %arg8: memref<?xi32>, %arg9: memref<?xi32>, %arg10: memref<?xi32>, %arg11: memref<?xf32>, %arg12: memref<?xf32>, %arg13: memref<?xi32>) attributes {llvm.linkage = #llvm.linkage<external>} {
    %c1 = arith.constant 1 : index
    %c-1_i32 = arith.constant -1 : i32
    %c0 = arith.constant 0 : index
    %c2 = arith.constant 2 : index
    %c-2_i32 = arith.constant -2 : i32
    %c1_i64 = arith.constant 1 : i64
    %c0_i32 = arith.constant 0 : i32
    %c25214903917_i64 = arith.constant 25214903917 : i64
    %c11_i64 = arith.constant 11 : i64
    %c281474976710655_i64 = arith.constant 281474976710655 : i64
    %c1_i32 = arith.constant 1 : i32
    %cst = arith.constant 0.000000e+00 : f64
    %cst_0 = arith.constant 0.000000e+00 : f32
    %cst_1 = arith.constant 2.000000e+00 : f64
    %c17_i64 = arith.constant 17 : i64
    %cst_2 = arith.constant 2.14748365E+9 : f32
    %false = arith.constant false
    %c3_i32 = arith.constant 3 : i32
    %c2_i32 = arith.constant 2 : i32
    %cst_3 = arith.constant 1.100000e+00 : f64
    %cst_4 = arith.constant 9.000000e-01 : f64
    %cst_5 = arith.constant 1.000000e-01 : f64
    %cst_6 = arith.constant 1.000000e-03 : f64
    %cst_7 = arith.constant 0.99899999999999999 : f64
    %cst_8 = arith.constant 1.000000e+00 : f64
    %c78606_i64 = arith.constant 78606 : i64
    %true = arith.constant true
    %0 = arith.index_cast %arg0 : i32 to index
    %1 = llvm.mlir.undef : i32
    %2 = llvm.mlir.undef : f32
    %3 = llvm.mlir.undef : f64
    %4 = llvm.mlir.undef : i64
    %5 = arith.extsi %arg0 : i32 to i64
    %6 = arith.extsi %arg1 : i32 to i64
    %7 = arith.muli %5, %6 : i64
    %8 = arith.index_cast %7 : i64 to index
    %9 = scf.for %arg14 = %c0 to %8 step %c1 iter_args(%arg15 = %c78606_i64) -> (i64) {
      %70 = arith.muli %arg15, %c25214903917_i64 : i64
      %71 = arith.addi %70, %c11_i64 : i64
      %72 = arith.andi %71, %c281474976710655_i64 : i64
      scf.yield %72 : i64
    }
    %10 = arith.index_cast %arg1 : i32 to index
    %11 = scf.for %arg14 = %c0 to %0 step %c1 iter_args(%arg15 = %cst) -> (f64) {
      %70 = scf.for %arg16 = %c0 to %10 step %c1 iter_args(%arg17 = %cst_0) -> (f32) {
        %75 = memref.load %arg4[%arg14, %arg16] : memref<?x?xf32>
        %76 = memref.load %arg4[%c0, %arg16] : memref<?x?xf32>
        %77 = arith.subf %75, %76 : f32
        %78 = arith.mulf %77, %77 : f32
        %79 = arith.addf %arg17, %78 : f32
        scf.yield %79 : f32
      }
      %71 = memref.load %arg5[%arg14] : memref<?xf32>
      %72 = arith.mulf %70, %71 : f32
      %73 = arith.extf %72 : f32 to f64
      %74 = arith.addf %arg15, %73 : f64
      scf.yield %74 : f64
    }
    %12 = arith.addf %11, %cst : f64
    %13 = arith.divf %12, %cst_1 : f64
    %14 = arith.addi %arg0, %c-1_i32 : i32
    %15 = arith.index_cast %14 : i32 to index
    %16 = arith.index_cast %arg1 : i32 to index
    %17 = scf.for %arg14 = %c0 to %15 step %c1 iter_args(%arg15 = %9) -> (i64) {
      %70 = arith.index_cast %arg14 : index to i32
      %71 = arith.muli %arg15, %c25214903917_i64 : i64
      %72 = arith.addi %71, %c11_i64 : i64
      %73 = arith.andi %72, %c281474976710655_i64 : i64
      %74 = arith.shrui %73, %c17_i64 : i64
      %75 = arith.subi %arg0, %70 : i32
      %76 = arith.extsi %75 : i32 to i64
      %77 = arith.remsi %74, %76 : i64
      %78 = arith.trunci %77 : i64 to i32
      %79 = arith.addi %78, %70 : i32
      %80 = memref.load %arg5[%arg14] : memref<?xf32>
      %81 = arith.index_cast %79 : i32 to index
      %82 = memref.load %arg5[%81] : memref<?xf32>
      memref.store %82, %arg5[%arg14] : memref<?xf32>
      memref.store %80, %arg5[%81] : memref<?xf32>
      scf.for %arg16 = %c0 to %16 step %c1 {
        %83 = memref.load %arg4[%arg14, %arg16] : memref<?x?xf32>
        %84 = memref.load %arg4[%81, %arg16] : memref<?x?xf32>
        memref.store %84, %arg4[%arg14, %arg16] : memref<?x?xf32>
        memref.store %83, %arg4[%81, %arg16] : memref<?x?xf32>
      }
      scf.yield %73 : i64
    }
    %18 = arith.index_cast %arg1 : i32 to index
    scf.for %arg14 = %c0 to %0 step %c1 {
      %70 = scf.for %arg15 = %c0 to %18 step %c1 iter_args(%arg16 = %cst_0) -> (f32) {
        %73 = memref.load %arg4[%arg14, %arg15] : memref<?x?xf32>
        %74 = memref.load %arg4[%c0, %arg15] : memref<?x?xf32>
        %75 = arith.subf %73, %74 : f32
        %76 = arith.mulf %75, %75 : f32
        %77 = arith.addf %arg16, %76 : f32
        scf.yield %77 : f32
      }
      %71 = memref.load %arg5[%arg14] : memref<?xf32>
      %72 = arith.mulf %70, %71 : f32
      memref.store %72, %arg7[%arg14] : memref<?xf32>
      memref.store %c0_i32, %arg6[%arg14] : memref<?xi32>
    }
    %19 = arith.truncf %13 : f64 to f32
    %20 = arith.index_cast %arg1 : i32 to index
    %21:7 = scf.for %arg14 = %c1 to %0 step %c1 iter_args(%arg15 = %1, %arg16 = %2, %arg17 = %c1_i32, %arg18 = %c1_i64, %arg19 = %17, %arg20 = %c1_i64, %arg21 = %17) -> (i32, f32, i32, i64, i64, i64, i64) {
      %70 = arith.muli %arg19, %c25214903917_i64 : i64
      %71 = arith.addi %70, %c11_i64 : i64
      %72 = arith.andi %71, %c281474976710655_i64 : i64
      %73 = arith.shrui %72, %c17_i64 : i64
      %74 = arith.sitofp %73 : i64 to f32
      %75 = arith.divf %74, %cst_2 : f32
      %76 = arith.index_cast %arg17 : i32 to index
      %77 = memref.load %arg7[%76] : memref<?xf32>
      %78 = arith.divf %77, %19 : f32
      %79 = arith.cmpf olt, %75, %78 : f32
      %80:3 = scf.if %79 -> (i32, f32, i64) {
        %82 = arith.addi %arg18, %c1_i64 : i64
        %83:2 = scf.for %arg22 = %c0 to %0 step %c1 iter_args(%arg23 = %arg15, %arg24 = %arg16) -> (i32, f32) {
          %84 = scf.for %arg25 = %c0 to %20 step %c1 iter_args(%arg26 = %cst_0) -> (f32) {
            %89 = memref.load %arg4[%76, %arg25] : memref<?x?xf32>
            %90 = memref.load %arg4[%arg22, %arg25] : memref<?x?xf32>
            %91 = arith.subf %89, %90 : f32
            %92 = arith.mulf %91, %91 : f32
            %93 = arith.addf %arg26, %92 : f32
            scf.yield %93 : f32
          }
          %85 = memref.load %arg5[%arg22] : memref<?xf32>
          %86 = arith.mulf %84, %85 : f32
          %87 = memref.load %arg7[%arg22] : memref<?xf32>
          %88 = arith.cmpf olt, %86, %87 : f32
          scf.if %88 {
            %89 = memref.load %arg5[%arg22] : memref<?xf32>
            %90 = arith.mulf %84, %89 : f32
            memref.store %90, %arg7[%arg22] : memref<?xf32>
            memref.store %arg17, %arg6[%arg22] : memref<?xi32>
          }
          scf.yield %arg1, %84 : i32, f32
        }
        scf.yield %83#0, %83#1, %82 : i32, f32, i64
      } else {
        scf.yield %arg15, %arg16, %arg18 : i32, f32, i64
      }
      %81 = arith.addi %arg17, %c1_i32 : i32
      scf.yield %80#0, %80#1, %81, %80#2, %72, %80#2, %72 : i32, f32, i32, i64, i64, i64, i64
    }
    %22 = arith.extsi %arg2 : i32 to i64
    %23 = arith.index_cast %arg1 : i32 to index
    %24 = arith.truncf %13 : f64 to f32
    %25 = arith.index_cast %arg1 : i32 to index
    %26:6 = scf.while (%arg14 = %1, %arg15 = %2, %arg16 = %1, %arg17 = %c0_i32, %arg18 = %21#5, %arg19 = %21#6) : (i32, f32, i32, i32, i64, i64) -> (i32, i64, i64, i32, f32, i32) {
      %70 = arith.cmpi slt, %arg18, %22 : i64
      %71 = arith.cmpi slt, %arg17, %c1_i32 : i32
      %72 = arith.andi %70, %71 : i1
      scf.condition(%72) %arg17, %arg18, %arg19, %arg14, %arg15, %arg16 : i32, i64, i64, i32, f32, i32
    } do {
    ^bb0(%arg14: i32, %arg15: i64, %arg16: i64, %arg17: i32, %arg18: f32, %arg19: i32):
      scf.for %arg20 = %c0 to %0 step %c1 {
        %72 = scf.for %arg21 = %c0 to %23 step %c1 iter_args(%arg22 = %cst_0) -> (f32) {
          %75 = memref.load %arg4[%arg20, %arg21] : memref<?x?xf32>
          %76 = memref.load %arg4[%c0, %arg21] : memref<?x?xf32>
          %77 = arith.subf %75, %76 : f32
          %78 = arith.mulf %77, %77 : f32
          %79 = arith.addf %arg22, %78 : f32
          scf.yield %79 : f32
        }
        %73 = memref.load %arg5[%arg20] : memref<?xf32>
        %74 = arith.mulf %72, %73 : f32
        memref.store %74, %arg7[%arg20] : memref<?xf32>
        memref.store %c0_i32, %arg6[%arg20] : memref<?xi32>
      }
      %70:5 = scf.for %arg20 = %c1 to %0 step %c1 iter_args(%arg21 = %arg17, %arg22 = %arg18, %arg23 = %arg19, %arg24 = %c1_i64, %arg25 = %arg16) -> (i32, f32, i32, i64, i64) {
        %72 = arith.index_cast %arg20 : index to i32
        %73 = arith.muli %arg25, %c25214903917_i64 : i64
        %74 = arith.addi %73, %c11_i64 : i64
        %75 = arith.andi %74, %c281474976710655_i64 : i64
        %76 = arith.shrui %75, %c17_i64 : i64
        %77 = arith.sitofp %76 : i64 to f32
        %78 = arith.divf %77, %cst_2 : f32
        %79 = memref.load %arg7[%arg20] : memref<?xf32>
        %80 = arith.divf %79, %24 : f32
        %81 = arith.cmpf olt, %78, %80 : f32
        %82 = arith.select %81, %arg0, %arg23 : i32
        %83:3 = scf.if %81 -> (i32, f32, i64) {
          %84 = arith.addi %arg24, %c1_i64 : i64
          %85:2 = scf.for %arg26 = %c0 to %0 step %c1 iter_args(%arg27 = %arg21, %arg28 = %arg22) -> (i32, f32) {
            %86 = scf.for %arg29 = %c0 to %25 step %c1 iter_args(%arg30 = %cst_0) -> (f32) {
              %91 = memref.load %arg4[%arg20, %arg29] : memref<?x?xf32>
              %92 = memref.load %arg4[%arg26, %arg29] : memref<?x?xf32>
              %93 = arith.subf %91, %92 : f32
              %94 = arith.mulf %93, %93 : f32
              %95 = arith.addf %arg30, %94 : f32
              scf.yield %95 : f32
            }
            %87 = memref.load %arg5[%arg26] : memref<?xf32>
            %88 = arith.mulf %86, %87 : f32
            %89 = memref.load %arg7[%arg26] : memref<?xf32>
            %90 = arith.cmpf olt, %88, %89 : f32
            scf.if %90 {
              %91 = memref.load %arg5[%arg26] : memref<?xf32>
              %92 = arith.mulf %86, %91 : f32
              memref.store %92, %arg7[%arg26] : memref<?xf32>
              memref.store %72, %arg6[%arg26] : memref<?xi32>
            }
            scf.yield %arg1, %86 : i32, f32
          }
          scf.yield %85#0, %85#1, %84 : i32, f32, i64
        } else {
          scf.yield %arg21, %arg22, %arg24 : i32, f32, i64
        }
        scf.yield %83#0, %83#1, %82, %83#2, %75 : i32, f32, i32, i64, i64
      }
      %71 = arith.addi %arg14, %c1_i32 : i32
      scf.yield %70#0, %70#1, %70#2, %71, %70#3, %70#4 : i32, f32, i32, i32, i64, i64
    }
    %27 = arith.index_cast %arg1 : i32 to index
    %28 = arith.index_cast %arg1 : i32 to index
    %29 = arith.index_cast %arg1 : i32 to index
    %30:8 = scf.while (%arg14 = %1, %arg15 = %2, %arg16 = %1, %arg17 = %26#0, %arg18 = %26#1, %arg19 = %13, %arg20 = %11, %arg21 = %26#2) : (i32, f32, i32, i32, i64, f64, f64, i64) -> (i64, f64, f64, i64, i32, f32, i32, i32) {
      %70 = arith.cmpi slt, %arg18, %22 : i64
      scf.condition(%70) %arg18, %arg19, %arg20, %arg21, %arg14, %arg15, %arg16, %arg17 : i64, f64, f64, i64, i32, f32, i32, i32
    } do {
    ^bb0(%arg14: i64, %arg15: f64, %arg16: f64, %arg17: i64, %arg18: i32, %arg19: f32, %arg20: i32, %arg21: i32):
      %70 = arith.cmpi sge, %arg21, %c1_i32 : i32
      %71 = arith.select %70, %c0_i32, %arg21 : i32
      %72 = arith.select %70, %arg15, %arg16 : f64
      %73 = scf.if %70 -> (f64) {
        %78 = arith.addf %arg15, %cst : f64
        %79 = arith.divf %78, %cst_1 : f64
        scf.yield %79 : f64
      } else {
        scf.yield %arg15 : f64
      }
      %74 = scf.for %arg22 = %c0 to %15 step %c1 iter_args(%arg23 = %arg17) -> (i64) {
        %78 = arith.index_cast %arg22 : index to i32
        %79 = arith.muli %arg23, %c25214903917_i64 : i64
        %80 = arith.addi %79, %c11_i64 : i64
        %81 = arith.andi %80, %c281474976710655_i64 : i64
        %82 = arith.shrui %81, %c17_i64 : i64
        %83 = arith.subi %arg0, %78 : i32
        %84 = arith.extsi %83 : i32 to i64
        %85 = arith.remsi %82, %84 : i64
        %86 = arith.trunci %85 : i64 to i32
        %87 = arith.addi %86, %78 : i32
        %88 = memref.load %arg5[%arg22] : memref<?xf32>
        %89 = arith.index_cast %87 : i32 to index
        %90 = memref.load %arg5[%89] : memref<?xf32>
        memref.store %90, %arg5[%arg22] : memref<?xf32>
        memref.store %88, %arg5[%89] : memref<?xf32>
        scf.for %arg24 = %c0 to %27 step %c1 {
          %91 = memref.load %arg4[%arg22, %arg24] : memref<?x?xf32>
          %92 = memref.load %arg4[%89, %arg24] : memref<?x?xf32>
          memref.store %92, %arg4[%arg22, %arg24] : memref<?x?xf32>
          memref.store %91, %arg4[%89, %arg24] : memref<?x?xf32>
        }
        scf.yield %81 : i64
      }
      scf.for %arg22 = %c0 to %0 step %c1 {
        %78 = scf.for %arg23 = %c0 to %28 step %c1 iter_args(%arg24 = %cst_0) -> (f32) {
          %81 = memref.load %arg4[%arg22, %arg23] : memref<?x?xf32>
          %82 = memref.load %arg4[%c0, %arg23] : memref<?x?xf32>
          %83 = arith.subf %81, %82 : f32
          %84 = arith.mulf %83, %83 : f32
          %85 = arith.addf %arg24, %84 : f32
          scf.yield %85 : f32
        }
        %79 = memref.load %arg5[%arg22] : memref<?xf32>
        %80 = arith.mulf %78, %79 : f32
        memref.store %80, %arg7[%arg22] : memref<?xf32>
        memref.store %c0_i32, %arg6[%arg22] : memref<?xi32>
      }
      %75 = arith.truncf %73 : f64 to f32
      %76:5 = scf.for %arg22 = %c1 to %0 step %c1 iter_args(%arg23 = %arg18, %arg24 = %arg19, %arg25 = %arg20, %arg26 = %c1_i64, %arg27 = %74) -> (i32, f32, i32, i64, i64) {
        %78 = arith.index_cast %arg22 : index to i32
        %79 = arith.muli %arg27, %c25214903917_i64 : i64
        %80 = arith.addi %79, %c11_i64 : i64
        %81 = arith.andi %80, %c281474976710655_i64 : i64
        %82 = arith.shrui %81, %c17_i64 : i64
        %83 = arith.sitofp %82 : i64 to f32
        %84 = arith.divf %83, %cst_2 : f32
        %85 = memref.load %arg7[%arg22] : memref<?xf32>
        %86 = arith.divf %85, %75 : f32
        %87 = arith.cmpf olt, %84, %86 : f32
        %88 = arith.select %87, %arg0, %arg25 : i32
        %89:3 = scf.if %87 -> (i32, f32, i64) {
          %90 = arith.addi %arg26, %c1_i64 : i64
          %91:2 = scf.for %arg28 = %c0 to %0 step %c1 iter_args(%arg29 = %arg23, %arg30 = %arg24) -> (i32, f32) {
            %92 = scf.for %arg31 = %c0 to %29 step %c1 iter_args(%arg32 = %cst_0) -> (f32) {
              %97 = memref.load %arg4[%arg22, %arg31] : memref<?x?xf32>
              %98 = memref.load %arg4[%arg28, %arg31] : memref<?x?xf32>
              %99 = arith.subf %97, %98 : f32
              %100 = arith.mulf %99, %99 : f32
              %101 = arith.addf %arg32, %100 : f32
              scf.yield %101 : f32
            }
            %93 = memref.load %arg5[%arg28] : memref<?xf32>
            %94 = arith.mulf %92, %93 : f32
            %95 = memref.load %arg7[%arg28] : memref<?xf32>
            %96 = arith.cmpf olt, %94, %95 : f32
            scf.if %96 {
              %97 = memref.load %arg5[%arg28] : memref<?xf32>
              %98 = arith.mulf %92, %97 : f32
              memref.store %98, %arg7[%arg28] : memref<?xf32>
              memref.store %78, %arg6[%arg28] : memref<?xi32>
            }
            scf.yield %arg1, %92 : i32, f32
          }
          scf.yield %91#0, %91#1, %90 : i32, f32, i64
        } else {
          scf.yield %arg23, %arg24, %arg26 : i32, f32, i64
        }
        scf.yield %89#0, %89#1, %88, %89#2, %81 : i32, f32, i32, i64, i64
      }
      %77 = arith.addi %71, %c1_i32 : i32
      scf.yield %76#0, %76#1, %76#2, %77, %76#3, %73, %72, %76#4 : i32, f32, i32, i32, i64, f64, f64, i64
    }
    %31 = arith.muli %arg2, %c3_i32 : i32
    %32 = arith.sitofp %31 : i32 to f64
    %33 = arith.sitofp %arg2 : i32 to f64
    %34 = math.log %33 : f64
    %35 = arith.mulf %32, %34 : f64
    %36 = arith.fptosi %35 : f64 to i32
    %37 = arith.cmpi sgt, %arg0, %36 : i32
    %38 = arith.select %37, %36, %arg0 : i32
    %39 = arith.cmpi eq, %38, %arg0 : i32
    %40 = scf.if %39 -> (i64) {
      %70 = arith.index_cast %38 : i32 to index
      scf.for %arg14 = %c0 to %70 step %c1 {
        %71 = arith.index_cast %arg14 : index to i32
        memref.store %71, %arg13[%arg14] : memref<?xi32>
      }
      scf.yield %30#3 : i64
    } else {
      %70 = affine.load %arg5[0] : memref<?xf32>
      affine.store %70, %arg11[0] : memref<?xf32>
      scf.for %arg14 = %c1 to %0 step %c1 {
        %74 = arith.index_cast %arg14 : index to i32
        %75 = arith.addi %74, %c-1_i32 : i32
        %76 = arith.index_cast %75 : i32 to index
        %77 = memref.load %arg11[%76] : memref<?xf32>
        %78 = memref.load %arg5[%arg14] : memref<?xf32>
        %79 = arith.addf %77, %78 : f32
        memref.store %79, %arg11[%arg14] : memref<?xf32>
      }
      %71 = affine.load %arg11[symbol(%0) - 1] : memref<?xf32>
      %72 = arith.index_cast %38 : i32 to index
      %73 = scf.for %arg14 = %c0 to %72 step %c1 iter_args(%arg15 = %30#3) -> (i64) {
        %74 = arith.muli %arg15, %c25214903917_i64 : i64
        %75 = arith.addi %74, %c11_i64 : i64
        %76 = arith.andi %75, %c281474976710655_i64 : i64
        %77 = arith.shrui %76, %c17_i64 : i64
        %78 = arith.sitofp %77 : i64 to f32
        %79 = arith.divf %78, %cst_2 : f32
        %80 = arith.mulf %79, %71 : f32
        %81 = affine.load %arg11[0] : memref<?xf32>
        %82 = arith.cmpf ogt, %81, %80 : f32
        scf.if %82 {
          memref.store %c0_i32, %arg13[%arg14] : memref<?xi32>
        } else {
          %83:2 = scf.while (%arg16 = %14, %arg17 = %c1_i32) : (i32, i32) -> (i32, i32) {
            %84 = arith.cmpi slt, %arg17, %arg16 : i32
            scf.condition(%84) %arg16, %arg17 : i32, i32
          } do {
          ^bb0(%arg16: i32, %arg17: i32):
            %84 = arith.addi %arg17, %c-1_i32 : i32
            %85 = arith.addi %84, %arg16 : i32
            %86 = arith.divsi %85, %c2_i32 : i32
            %87 = arith.index_cast %86 : i32 to index
            %88 = memref.load %arg11[%87] : memref<?xf32>
            %89 = arith.cmpf ogt, %88, %80 : f32
            %90 = arith.select %89, %86, %arg16 : i32
            %91 = arith.select %89, %84, %86 : i32
            %92 = arith.addi %91, %c1_i32 : i32
            scf.yield %90, %92 : i32, i32
          }
          memref.store %83#0, %arg13[%arg14] : memref<?xi32>
        }
        scf.yield %76 : i64
      }
      scf.yield %73 : i64
    }
    scf.for %arg14 = %c0 to %0 step %c1 {
      memref.store %c0_i32, %arg8[%arg14] : memref<?xi32>
    }
    scf.for %arg14 = %c0 to %0 step %c1 {
      %70 = memref.load %arg6[%arg14] : memref<?xi32>
      %71 = arith.index_cast %70 : i32 to index
      memref.store %c1_i32, %arg8[%71] : memref<?xi32>
    }
    %41 = scf.for %arg14 = %c0 to %0 step %c1 iter_args(%arg15 = %cst) -> (f64) {
      %70 = memref.load %arg7[%arg14] : memref<?xf32>
      %71 = arith.extf %70 : f32 to f64
      %72 = arith.addf %arg15, %71 : f64
      scf.yield %72 : f64
    }
    %42 = arith.sitofp %30#0 : i64 to f64
    %43 = arith.mulf %30#1, %42 : f64
    %44 = arith.addf %41, %43 : f64
    %45 = arith.extsi %arg3 : i32 to i64
    %46 = arith.sitofp %arg3 : i32 to f64
    %47 = arith.mulf %46, %cst_3 : f64
    %48 = arith.fptosi %47 : f64 to i32
    %49 = arith.extsi %48 : i32 to i64
    %50 = arith.mulf %33, %cst_4 : f64
    %51 = arith.fptosi %50 : f64 to i32
    %52 = arith.extsi %51 : i32 to i64
    %53 = arith.addi %arg3, %c2_i32 : i32
    %54 = arith.extsi %53 : i32 to i64
    %55 = arith.addi %arg2, %c-2_i32 : i32
    %56 = arith.extsi %55 : i32 to i64
    %57 = arith.index_cast %38 : i32 to index
    %58 = arith.muli %arg3, %c3_i32 : i32
    %59 = arith.sitofp %58 : i32 to f64
    %60 = arith.sitofp %arg3 : i32 to f64
    %61 = math.log %60 : f64
    %62 = arith.mulf %59, %61 : f64
    %63 = arith.fptosi %62 : f64 to i64
    %64 = arith.index_cast %63 : i64 to index
    %65 = arith.extsi %38 : i32 to i64
    %66 = arith.index_cast %arg1 : i32 to index
    %67 = arith.index_cast %arg1 : i32 to index
    %68:30 = scf.while (%arg14 = %1, %arg15 = %1, %arg16 = %2, %arg17 = %1, %arg18 = %1, %arg19 = %1, %arg20 = %1, %arg21 = %1, %arg22 = %2, %arg23 = %2, %arg24 = %1, %arg25 = %2, %arg26 = %1, %arg27 = %3, %arg28 = %1, %arg29 = %1, %arg30 = %1, %arg31 = %1, %arg32 = %1, %arg33 = %4, %arg34 = %4, %arg35 = %1, %arg36 = %3, %arg37 = %3, %arg38 = %44, %arg39 = %30#0, %arg40 = %30#1, %arg41 = %cst, %arg42 = %30#2, %arg43 = %40, %arg44 = %true) : (i32, i32, f32, i32, i32, i32, i32, i32, f32, f32, i32, f32, i32, f64, i32, i32, i32, i32, i32, i64, i64, i32, f64, f64, f64, i64, f64, f64, f64, i64, i1) -> (i32, i32, f32, i32, i32, i32, i32, i32, f32, f32, i32, f32, i32, f64, i32, i32, i32, i32, i32, i64, i64, i32, f64, f64, f64, i64, f64, f64, f64, i64) {
      scf.condition(%arg44) %arg14, %arg15, %arg16, %arg17, %arg18, %arg19, %arg20, %arg21, %arg22, %arg23, %arg24, %arg25, %arg26, %arg27, %arg28, %arg29, %arg30, %arg31, %arg32, %arg33, %arg34, %arg35, %arg36, %arg37, %arg38, %arg39, %arg40, %arg41, %arg42, %arg43 : i32, i32, f32, i32, i32, i32, i32, i32, f32, f32, i32, f32, i32, f64, i32, i32, i32, i32, i32, i64, i64, i32, f64, f64, f64, i64, f64, f64, f64, i64
    } do {
    ^bb0(%arg14: i32, %arg15: i32, %arg16: f32, %arg17: i32, %arg18: i32, %arg19: i32, %arg20: i32, %arg21: i32, %arg22: f32, %arg23: f32, %arg24: i32, %arg25: f32, %arg26: i32, %arg27: f64, %arg28: i32, %arg29: i32, %arg30: i32, %arg31: i32, %arg32: i32, %arg33: i64, %arg34: i64, %arg35: i32, %arg36: f64, %arg37: f64, %arg38: f64, %arg39: i64, %arg40: f64, %arg41: f64, %arg42: f64, %arg43: i64):
      %70:56 = scf.for %arg44 = %c0 to %c2 step %c1 iter_args(%arg45 = %arg14, %arg46 = %arg15, %arg47 = %arg16, %arg48 = %arg17, %arg49 = %arg18, %arg50 = %arg19, %arg51 = %arg20, %arg52 = %arg21, %arg53 = %arg22, %arg54 = %arg23, %arg55 = %arg24, %arg56 = %arg25, %arg57 = %arg26, %arg58 = %arg27, %arg59 = %arg28, %arg60 = %arg29, %arg61 = %arg30, %arg62 = %arg31, %arg63 = %arg32, %arg64 = %arg33, %arg65 = %arg34, %arg66 = %arg35, %arg67 = %arg36, %arg68 = %arg37, %arg69 = %arg38, %arg70 = %arg39, %arg71 = %arg43, %arg72 = %arg14, %arg73 = %arg15, %arg74 = %arg16, %arg75 = %arg17, %arg76 = %arg18, %arg77 = %arg19, %arg78 = %arg20, %arg79 = %arg21, %arg80 = %arg22, %arg81 = %arg23, %arg82 = %arg24, %arg83 = %arg25, %arg84 = %arg26, %arg85 = %arg27, %arg86 = %arg28, %arg87 = %arg29, %arg88 = %arg30, %arg89 = %arg31, %arg90 = %arg32, %arg91 = %arg33, %arg92 = %arg34, %arg93 = %arg35, %arg94 = %arg36, %arg95 = %arg37, %arg96 = %arg38, %arg97 = %arg39, %arg98 = %arg43, %arg99 = %true, %arg100 = %true) -> (i32, i32, f32, i32, i32, i32, i32, i32, f32, f32, i32, f32, i32, f64, i32, i32, i32, i32, i32, i64, i64, i32, f64, f64, f64, i64, i64, i32, i32, f32, i32, i32, i32, i32, i32, f32, f32, i32, f32, i32, f64, i32, i32, i32, i32, i32, i64, i64, i32, f64, f64, f64, i64, i64, i1, i1) {
        %81:56 = scf.if %arg100 -> (i32, i32, f32, i32, i32, i32, i32, i32, f32, f32, i32, f32, i32, f64, i32, i32, i32, i32, i32, i64, i64, i32, f64, f64, f64, i64, i64, i32, i32, f32, i32, i32, i32, i32, i32, f32, f32, i32, f32, i32, f64, i32, i32, i32, i32, i32, i64, i64, i32, f64, f64, f64, i64, i64, i1, i1) {
          %82 = arith.index_cast %arg44 : index to i32
          %83 = arith.cmpi eq, %82, %c1_i32 : i32
          %84 = arith.andi %83, %arg99 : i1
          %85 = scf.if %84 -> (i1) {
            %87 = arith.cmpi sle, %arg97, %49 : i64
            %88 = scf.if %87 -> (i1) {
              %91 = arith.cmpi sge, %arg97, %52 : i64
              scf.yield %91 : i1
            } else {
              scf.yield %false : i1
            }
            %89 = scf.if %88 -> (i1) {
              scf.yield %true : i1
            } else {
              %91 = arith.cmpi sle, %arg97, %54 : i64
              %92 = scf.if %91 -> (i1) {
                %93 = arith.cmpi sge, %arg97, %56 : i64
                scf.yield %93 : i1
              } else {
                scf.yield %false : i1
              }
              scf.yield %92 : i1
            }
            %90 = arith.andi %89, %arg99 : i1
            scf.yield %90 : i1
          } else {
            scf.yield %arg99 : i1
          }
          %86:27 = scf.if %85 -> (f64, i32, i32, f32, i32, i32, i32, i32, i32, f32, f32, i32, f32, i32, f64, i32, i32, i32, i32, i32, i64, i64, i32, f64, f64, i64, i64) {
            %87 = arith.cmpi eq, %82, %c0_i32 : i32
            %88 = arith.select %87, %cst_5, %cst_6 : f64
            %89:26 = scf.while (%arg101 = %arg72, %arg102 = %arg73, %arg103 = %arg74, %arg104 = %arg75, %arg105 = %arg76, %arg106 = %arg77, %arg107 = %arg78, %arg108 = %arg79, %arg109 = %arg80, %arg110 = %arg81, %arg111 = %arg82, %arg112 = %arg83, %arg113 = %arg84, %arg114 = %arg85, %arg115 = %arg86, %arg116 = %arg87, %arg117 = %arg88, %arg118 = %arg89, %arg119 = %arg90, %arg120 = %arg91, %arg121 = %arg92, %arg122 = %arg93, %arg123 = %arg96, %arg124 = %arg96, %arg125 = %arg97, %arg126 = %arg98) : (i32, i32, f32, i32, i32, i32, i32, i32, f32, f32, i32, f32, i32, f64, i32, i32, i32, i32, i32, i64, i64, i32, f64, f64, i64, i64) -> (i32, i32, f32, i32, i32, i32, i32, i32, f32, f32, i32, f32, i32, f64, i32, i32, i32, i32, i32, i64, i64, i32, f64, f64, i64, i64) {
              %90 = arith.divf %arg123, %arg124 : f64
              %91 = arith.cmpf ogt, %90, %88 : f64
              scf.condition(%91) %arg101, %arg102, %arg103, %arg104, %arg105, %arg106, %arg107, %arg108, %arg109, %arg110, %arg111, %arg112, %arg113, %arg114, %arg115, %arg116, %arg117, %arg118, %arg119, %arg120, %arg121, %arg122, %arg123, %arg124, %arg125, %arg126 : i32, i32, f32, i32, i32, i32, i32, i32, f32, f32, i32, f32, i32, f64, i32, i32, i32, i32, i32, i64, i64, i32, f64, f64, i64, i64
            } do {
            ^bb0(%arg101: i32, %arg102: i32, %arg103: f32, %arg104: i32, %arg105: i32, %arg106: i32, %arg107: i32, %arg108: i32, %arg109: f32, %arg110: f32, %arg111: i32, %arg112: f32, %arg113: i32, %arg114: f64, %arg115: i32, %arg116: i32, %arg117: i32, %arg118: i32, %arg119: i32, %arg120: i64, %arg121: i64, %arg122: i32, %arg123: f64, %arg124: f64, %arg125: i64, %arg126: i64):
              %90 = scf.for %arg127 = %c0 to %57 step %c1 iter_args(%arg128 = %arg126) -> (i64) {
                %93 = arith.index_cast %arg127 : index to i32
                %94 = arith.muli %arg128, %c25214903917_i64 : i64
                %95 = arith.addi %94, %c11_i64 : i64
                %96 = arith.andi %95, %c281474976710655_i64 : i64
                %97 = arith.shrui %96, %c17_i64 : i64
                %98 = arith.subi %38, %93 : i32
                %99 = arith.extsi %98 : i32 to i64
                %100 = arith.remsi %97, %99 : i64
                %101 = arith.trunci %100 : i64 to i32
                %102 = arith.addi %101, %93 : i32
                %103 = memref.load %arg13[%arg127] : memref<?xi32>
                %104 = arith.index_cast %102 : i32 to index
                %105 = memref.load %arg13[%104] : memref<?xi32>
                memref.store %105, %arg13[%arg127] : memref<?xi32>
                memref.store %103, %arg13[%104] : memref<?xi32>
                scf.yield %96 : i64
              }
              %91:21 = scf.for %arg127 = %c0 to %64 step %c1 iter_args(%arg128 = %arg101, %arg129 = %arg102, %arg130 = %arg103, %arg131 = %arg104, %arg132 = %arg107, %arg133 = %arg108, %arg134 = %arg109, %arg135 = %arg110, %arg136 = %arg111, %arg137 = %arg112, %arg138 = %cst, %arg139 = %arg125, %arg140 = %arg105, %arg141 = %arg106, %arg142 = %arg113, %arg143 = %arg114, %arg144 = %arg115, %arg145 = %arg116, %arg146 = %arg117, %arg147 = %arg118, %arg148 = %arg119) -> (i32, i32, f32, i32, i32, i32, f32, f32, i32, f32, f64, i64, i32, i32, i32, f64, i32, i32, i32, i32, i32) {
                %93 = arith.index_cast %arg127 : index to i64
                %94 = arith.remsi %93, %65 : i64
                %95 = arith.index_cast %94 : i64 to index
                %96 = memref.load %arg13[%95] : memref<?xi32>
                %97 = scf.for %arg149 = %c0 to %0 step %c1 iter_args(%arg150 = %c0_i32) -> (i32) {
                  %107 = memref.load %arg8[%arg149] : memref<?xi32>
                  %108 = arith.cmpi ne, %107, %c0_i32 : i32
                  %109 = scf.if %108 -> (i32) {
                    %110 = arith.addi %arg150, %c1_i32 : i32
                    memref.store %arg150, %arg9[%arg149] : memref<?xi32>
                    scf.yield %110 : i32
                  } else {
                    scf.yield %arg150 : i32
                  }
                  scf.yield %109 : i32
                }
                %98 = arith.index_cast %97 : i32 to index
                scf.for %arg149 = %c0 to %98 step %c1 {
                  memref.store %cst_0, %arg11[%arg149] : memref<?xf32>
                }
                scf.for %arg149 = %c0 to %0 step %c1 {
                  memref.store %c0_i32, %arg10[%arg149] : memref<?xi32>
                }
                %99 = arith.index_cast %96 : i32 to index
                %100:7 = scf.for %arg149 = %c0 to %0 step %c1 iter_args(%arg150 = %arg132, %arg151 = %arg133, %arg152 = %cst, %arg153 = %arg134, %arg154 = %arg135, %arg155 = %arg136, %arg156 = %arg137) -> (i32, i32, f64, f32, f32, i32, f32) {
                  %107 = scf.for %arg157 = %c0 to %66 step %c1 iter_args(%arg158 = %cst_0) -> (f32) {
                    %113 = memref.load %arg4[%arg149, %arg157] : memref<?x?xf32>
                    %114 = memref.load %arg4[%99, %arg157] : memref<?x?xf32>
                    %115 = arith.subf %113, %114 : f32
                    %116 = arith.mulf %115, %115 : f32
                    %117 = arith.addf %arg158, %116 : f32
                    scf.yield %117 : f32
                  }
                  %108 = memref.load %arg5[%arg149] : memref<?xf32>
                  %109 = arith.mulf %107, %108 : f32
                  %110 = memref.load %arg7[%arg149] : memref<?xf32>
                  %111 = arith.cmpf olt, %109, %110 : f32
                  %112:3 = scf.if %111 -> (i32, i32, f64) {
                    memref.store %c1_i32, %arg10[%arg149] : memref<?xi32>
                    %113 = arith.subf %109, %110 : f32
                    %114 = arith.extf %113 : f32 to f64
                    %115 = arith.addf %arg152, %114 : f64
                    scf.yield %arg150, %arg151, %115 : i32, i32, f64
                  } else {
                    %113 = memref.load %arg6[%arg149] : memref<?xi32>
                    %114 = arith.index_cast %113 : i32 to index
                    %115 = memref.load %arg9[%114] : memref<?xi32>
                    %116 = arith.index_cast %115 : i32 to index
                    %117 = arith.subf %110, %109 : f32
                    %118 = memref.load %arg11[%116] : memref<?xf32>
                    %119 = arith.addf %118, %117 : f32
                    memref.store %119, %arg11[%116] : memref<?xf32>
                    scf.yield %115, %113, %arg152 : i32, i32, f64
                  }
                  scf.yield %112#0, %112#1, %112#2, %110, %109, %arg1, %107 : i32, i32, f64, f32, f32, i32, f32
                }
                %101:2 = scf.for %arg149 = %c0 to %0 step %c1 iter_args(%arg150 = %c0_i32, %arg151 = %100#2) -> (i32, f64) {
                  %107 = memref.load %arg8[%arg149] : memref<?xi32>
                  %108 = arith.cmpi ne, %107, %c0_i32 : i32
                  %109:2 = scf.if %108 -> (i32, f64) {
                    %110 = memref.load %arg9[%arg149] : memref<?xi32>
                    %111 = arith.index_cast %110 : i32 to index
                    %112 = memref.load %arg11[%111] : memref<?xf32>
                    %113 = arith.extf %112 : f32 to f64
                    %114 = arith.addf %arg40, %113 : f64
                    %115 = arith.truncf %114 : f64 to f32
                    memref.store %115, %arg12[%111] : memref<?xf32>
                    %116 = arith.cmpf ogt, %114, %cst : f64
                    %117:2 = scf.if %116 -> (i32, f64) {
                      %118 = arith.addi %arg150, %c1_i32 : i32
                      %119 = arith.subf %arg151, %114 : f64
                      scf.yield %118, %119 : i32, f64
                    } else {
                      scf.yield %arg150, %arg151 : i32, f64
                    }
                    scf.yield %117#0, %117#1 : i32, f64
                  } else {
                    scf.yield %arg150, %arg151 : i32, f64
                  }
                  scf.yield %109#0, %109#1 : i32, f64
                }
                %102 = arith.addf %101#1, %arg40 : f64
                %103 = arith.cmpf olt, %102, %cst : f64
                %104 = arith.select %103, %arg0, %arg128 : i32
                %105 = arith.select %103, %arg0, %arg131 : i32
                %106:4 = scf.if %103 -> (i32, f32, f64, i64) {
                  %107 = arith.index_cast %96 : i32 to index
                  %108:2 = scf.for %arg149 = %c0 to %0 step %c1 iter_args(%arg150 = %arg129, %arg151 = %arg130) -> (i32, f32) {
                    %115 = memref.load %arg6[%arg149] : memref<?xi32>
                    %116 = arith.index_cast %115 : i32 to index
                    %117 = memref.load %arg9[%116] : memref<?xi32>
                    %118 = arith.index_cast %117 : i32 to index
                    %119 = memref.load %arg12[%118] : memref<?xf32>
                    %120 = arith.cmpf ogt, %119, %cst_0 : f32
                    %121 = memref.load %arg10[%arg149] : memref<?xi32>
                    %122 = arith.cmpi ne, %121, %c0_i32 : i32
                    %123 = arith.cmpi eq, %121, %c0_i32 : i32
                    %124 = arith.andi %123, %120 : i1
                    %125 = arith.ori %122, %124 : i1
                    %126 = arith.select %125, %arg1, %arg150 : i32
                    %127 = scf.if %125 -> (f32) {
                      %128 = scf.for %arg152 = %c0 to %67 step %c1 iter_args(%arg153 = %cst_0) -> (f32) {
                        %131 = memref.load %arg4[%arg149, %arg152] : memref<?x?xf32>
                        %132 = memref.load %arg4[%107, %arg152] : memref<?x?xf32>
                        %133 = arith.subf %131, %132 : f32
                        %134 = arith.mulf %133, %133 : f32
                        %135 = arith.addf %arg153, %134 : f32
                        scf.yield %135 : f32
                      }
                      %129 = memref.load %arg5[%arg149] : memref<?xf32>
                      %130 = arith.mulf %128, %129 : f32
                      memref.store %130, %arg7[%arg149] : memref<?xf32>
                      memref.store %96, %arg6[%arg149] : memref<?xi32>
                      scf.yield %128 : f32
                    } else {
                      scf.yield %arg151 : f32
                    }
                    scf.yield %126, %127 : i32, f32
                  }
                  scf.for %arg149 = %c0 to %0 step %c1 {
                    %115 = memref.load %arg8[%arg149] : memref<?xi32>
                    %116 = arith.cmpi ne, %115, %c0_i32 : i32
                    scf.if %116 {
                      %117 = memref.load %arg9[%arg149] : memref<?xi32>
                      %118 = arith.index_cast %117 : i32 to index
                      %119 = memref.load %arg12[%118] : memref<?xf32>
                      %120 = arith.cmpf ogt, %119, %cst_0 : f32
                      scf.if %120 {
                        memref.store %c0_i32, %arg8[%arg149] : memref<?xi32>
                      }
                    }
                  }
                  %109 = arith.index_cast %96 : i32 to index
                  memref.store %c1_i32, %arg8[%109] : memref<?xi32>
                  %110 = arith.addi %arg139, %c1_i64 : i64
                  %111 = arith.extsi %101#0 : i32 to i64
                  %112 = arith.subi %110, %111 : i64
                  %113 = arith.negf %102 : f64
                  %114 = arith.addf %arg138, %113 : f64
                  scf.yield %108#0, %108#1, %114, %112 : i32, f32, f64, i64
                } else {
                  scf.yield %arg129, %arg130, %arg138, %arg139 : i32, f32, f64, i64
                }
                scf.yield %104, %106#0, %106#1, %105, %100#0, %100#1, %100#3, %100#4, %100#5, %100#6, %106#2, %106#3, %arg0, %101#0, %arg0, %102, %arg0, %97, %arg0, %97, %96 : i32, i32, f32, i32, i32, i32, f32, f32, i32, f32, f64, i64, i32, i32, i32, f64, i32, i32, i32, i32, i32
              }
              %92 = arith.subf %arg124, %91#10 : f64
              scf.yield %91#0, %91#1, %91#2, %91#3, %91#12, %91#13, %91#4, %91#5, %91#6, %91#7, %91#8, %91#9, %91#14, %91#15, %91#16, %91#17, %91#18, %91#19, %91#20, %63, %63, %38, %91#10, %92, %91#11, %90 : i32, i32, f32, i32, i32, i32, i32, i32, f32, f32, i32, f32, i32, f64, i32, i32, i32, i32, i32, i64, i64, i32, f64, f64, i64, i64
            }
            scf.yield %88, %89#0, %89#1, %89#2, %89#3, %89#4, %89#5, %89#6, %89#7, %89#8, %89#9, %89#10, %89#11, %89#12, %89#13, %89#14, %89#15, %89#16, %89#17, %89#18, %89#19, %89#20, %89#21, %89#22, %89#23, %89#24, %89#25 : f64, i32, i32, f32, i32, i32, i32, i32, i32, f32, f32, i32, f32, i32, f64, i32, i32, i32, i32, i32, i64, i64, i32, f64, f64, i64, i64
          } else {
            scf.yield %arg95, %arg72, %arg73, %arg74, %arg75, %arg76, %arg77, %arg78, %arg79, %arg80, %arg81, %arg82, %arg83, %arg84, %arg85, %arg86, %arg87, %arg88, %arg89, %arg90, %arg91, %arg92, %arg93, %arg94, %arg96, %arg97, %arg98 : f64, i32, i32, f32, i32, i32, i32, i32, i32, f32, f32, i32, f32, i32, f64, i32, i32, i32, i32, i32, i64, i64, i32, f64, f64, i64, i64
          }
          scf.yield %86#1, %86#2, %86#3, %86#4, %86#5, %86#6, %86#7, %86#8, %86#9, %86#10, %86#11, %86#12, %86#13, %86#14, %86#15, %86#16, %86#17, %86#18, %86#19, %86#20, %86#21, %86#22, %86#23, %86#0, %86#24, %86#25, %86#26, %86#1, %86#2, %86#3, %86#4, %86#5, %86#6, %86#7, %86#8, %86#9, %86#10, %86#11, %86#12, %86#13, %86#14, %86#15, %86#16, %86#17, %86#18, %86#19, %86#20, %86#21, %86#22, %86#23, %86#0, %86#24, %86#25, %86#26, %85, %85 : i32, i32, f32, i32, i32, i32, i32, i32, f32, f32, i32, f32, i32, f64, i32, i32, i32, i32, i32, i64, i64, i32, f64, f64, f64, i64, i64, i32, i32, f32, i32, i32, i32, i32, i32, f32, f32, i32, f32, i32, f64, i32, i32, i32, i32, i32, i64, i64, i32, f64, f64, f64, i64, i64, i1, i1
        } else {
          scf.yield %arg45, %arg46, %arg47, %arg48, %arg49, %arg50, %arg51, %arg52, %arg53, %arg54, %arg55, %arg56, %arg57, %arg58, %arg59, %arg60, %arg61, %arg62, %arg63, %arg64, %arg65, %arg66, %arg67, %arg68, %arg69, %arg70, %arg71, %arg72, %arg73, %arg74, %arg75, %arg76, %arg77, %arg78, %arg79, %arg80, %arg81, %arg82, %arg83, %arg84, %arg85, %arg86, %arg87, %arg88, %arg89, %arg90, %arg91, %arg92, %arg93, %arg94, %arg95, %arg96, %arg97, %arg98, %arg99, %false : i32, i32, f32, i32, i32, i32, i32, i32, f32, f32, i32, f32, i32, f64, i32, i32, i32, i32, i32, i64, i64, i32, f64, f64, f64, i64, i64, i32, i32, f32, i32, i32, i32, i32, i32, f32, f32, i32, f32, i32, f64, i32, i32, i32, i32, i32, i64, i64, i32, f64, f64, f64, i64, i64, i1, i1
        }
        scf.yield %81#0, %81#1, %81#2, %81#3, %81#4, %81#5, %81#6, %81#7, %81#8, %81#9, %81#10, %81#11, %81#12, %81#13, %81#14, %81#15, %81#16, %81#17, %81#18, %81#19, %81#20, %81#21, %81#22, %81#23, %81#24, %81#25, %81#26, %81#27, %81#28, %81#29, %81#30, %81#31, %81#32, %81#33, %81#34, %81#35, %81#36, %81#37, %81#38, %81#39, %81#40, %81#41, %81#42, %81#43, %81#44, %81#45, %81#46, %81#47, %81#48, %81#49, %81#50, %81#51, %81#52, %81#53, %81#54, %81#55 : i32, i32, f32, i32, i32, i32, i32, i32, f32, f32, i32, f32, i32, f64, i32, i32, i32, i32, i32, i64, i64, i32, f64, f64, f64, i64, i64, i32, i32, f32, i32, i32, i32, i32, i32, f32, f32, i32, f32, i32, f64, i32, i32, i32, i32, i32, i64, i64, i32, f64, f64, f64, i64, i64, i1, i1
      }
      %71 = arith.cmpi sgt, %70#25, %45 : i64
      %72 = arith.select %71, %arg40, %arg41 : f64
      %73:2 = scf.if %71 -> (f64, f64) {
        %81 = arith.addf %arg42, %arg40 : f64
        %82 = arith.divf %81, %cst_1 : f64
        %83 = arith.subf %82, %arg40 : f64
        %84 = arith.sitofp %70#25 : i64 to f64
        %85 = arith.mulf %83, %84 : f64
        %86 = arith.addf %70#24, %85 : f64
        scf.yield %86, %82 : f64, f64
      } else {
        scf.yield %70#24, %arg40 : f64, f64
      }
      %74 = arith.cmpi slt, %70#25, %22 : i64
      %75 = arith.select %74, %73#1, %arg42 : f64
      %76:2 = scf.if %74 -> (f64, f64) {
        %81 = arith.addf %73#1, %72 : f64
        %82 = arith.divf %81, %cst_1 : f64
        %83 = arith.subf %82, %73#1 : f64
        %84 = arith.sitofp %70#25 : i64 to f64
        %85 = arith.mulf %83, %84 : f64
        %86 = arith.addf %73#0, %85 : f64
        scf.yield %86, %82 : f64, f64
      } else {
        scf.yield %73#0, %73#1 : f64, f64
      }
      %77 = arith.cmpi sle, %70#25, %45 : i64
      %78 = scf.if %77 -> (i1) {
        %81 = arith.cmpi sge, %70#25, %22 : i64
        scf.yield %81 : i1
      } else {
        scf.yield %false : i1
      }
      %79 = scf.if %78 -> (i1) {
        scf.yield %true : i1
      } else {
        %81 = arith.mulf %75, %cst_7 : f64
        %82 = arith.cmpf oge, %72, %81 : f64
        scf.yield %82 : i1
      }
      %80 = arith.xori %79, %true : i1
      scf.yield %70#0, %70#1, %70#2, %70#3, %70#4, %70#5, %70#6, %70#7, %70#8, %70#9, %70#10, %70#11, %70#12, %70#13, %70#14, %70#15, %70#16, %70#17, %70#18, %70#19, %70#20, %70#21, %70#22, %70#23, %76#0, %70#25, %76#1, %72, %75, %70#26, %80 : i32, i32, f32, i32, i32, i32, i32, i32, f32, f32, i32, f32, i32, f64, i32, i32, i32, i32, i32, i64, i64, i32, f64, f64, f64, i64, f64, f64, f64, i64, i1
    }
    %69 = arith.index_cast %arg1 : i32 to index
    scf.for %arg14 = %c0 to %0 step %c1 {
      %70 = arith.index_cast %arg14 : index to i32
      %71 = memref.load %arg6[%arg14] : memref<?xi32>
      %72 = arith.cmpi ne, %71, %70 : i32
      scf.if %72 {
        %73 = arith.index_cast %71 : i32 to index
        %74 = memref.load %arg5[%73] : memref<?xf32>
        %75 = memref.load %arg5[%arg14] : memref<?xf32>
        %76 = arith.addf %74, %75 : f32
        %77 = arith.divf %75, %76 : f32
        %78 = arith.extf %77 : f32 to f64
        %79 = arith.subf %cst_8, %78 : f64
        %80 = arith.truncf %79 : f64 to f32
        scf.for %arg15 = %c0 to %69 step %c1 {
          %84 = memref.load %arg4[%73, %arg15] : memref<?x?xf32>
          %85 = arith.mulf %84, %80 : f32
          memref.store %85, %arg4[%73, %arg15] : memref<?x?xf32>
          %86 = memref.load %arg4[%arg14, %arg15] : memref<?x?xf32>
          %87 = arith.mulf %86, %77 : f32
          %88 = memref.load %arg4[%73, %arg15] : memref<?x?xf32>
          %89 = arith.addf %88, %87 : f32
          memref.store %89, %arg4[%73, %arg15] : memref<?x?xf32>
        }
        %81 = memref.load %arg5[%arg14] : memref<?xf32>
        %82 = memref.load %arg5[%73] : memref<?xf32>
        %83 = arith.addf %82, %81 : f32
        memref.store %83, %arg5[%73] : memref<?xf32>
      }
    }
    scf.for %arg14 = %c0 to %0 step %c1 {
      memref.store %c0_i32, %arg8[%arg14] : memref<?xi32>
    }
    scf.for %arg14 = %c0 to %0 step %c1 {
      %70 = memref.load %arg6[%arg14] : memref<?xi32>
      %71 = arith.index_cast %70 : i32 to index
      memref.store %c1_i32, %arg8[%71] : memref<?xi32>
    }
    return
  }
}
