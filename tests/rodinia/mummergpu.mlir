module {
  func.func @mummergpu_match(%arg0: i32, %arg1: memref<?xi32>, %arg2: memref<?xi32>, %arg3: memref<?xi8>, %arg4: memref<?xi8>, %arg5: i32, %arg6: memref<?xi32>, %arg7: memref<?xi32>, %arg8: memref<?xi32>, %arg9: memref<?x4xi32>, %arg10: memref<?xi32>, %arg11: memref<?xi32>, %arg12: memref<?xi32>) attributes {llvm.linkage = #llvm.linkage<external>} {
    %c0 = arith.constant 0 : index
    %c1 = arith.constant 1 : index
    %true = arith.constant true
    %false = arith.constant false
    %c3_i32 = arith.constant 3 : i32
    %c116_i32 = arith.constant 116 : i32
    %c84_i32 = arith.constant 84 : i32
    %c2_i32 = arith.constant 2 : i32
    %c103_i32 = arith.constant 103 : i32
    %c71_i32 = arith.constant 71 : i32
    %c99_i32 = arith.constant 99 : i32
    %c67_i32 = arith.constant 67 : i32
    %c97_i32 = arith.constant 97 : i32
    %c65_i32 = arith.constant 65 : i32
    %c-1_i32 = arith.constant -1 : i32
    %c1_i32 = arith.constant 1 : i32
    %c0_i32 = arith.constant 0 : i32
    %0 = llvm.mlir.undef : i32
    %1 = llvm.mlir.undef : i8
    %2 = arith.index_cast %arg0 : i32 to index
    %3:11 = scf.for %arg13 = %c0 to %2 step %c1 iter_args(%arg14 = %0, %arg15 = %0, %arg16 = %1, %arg17 = %0, %arg18 = %0, %arg19 = %0, %arg20 = %0, %arg21 = %0, %arg22 = %0, %arg23 = %0, %arg24 = %0) -> (i32, i32, i8, i32, i32, i32, i32, i32, i32, i32, i32) {
      %4 = memref.load %arg1[%arg13] : memref<?xi32>
      %5 = memref.load %arg2[%arg13] : memref<?xi32>
      %6 = arith.subi %5, %arg5 : i32
      %7 = arith.addi %6, %c1_i32 : i32
      %8 = arith.index_cast %7 : i32 to index
      %9:14 = scf.for %arg25 = %c0 to %8 step %c1 iter_args(%arg26 = %arg14, %arg27 = %arg15, %arg28 = %arg16, %arg29 = %arg17, %arg30 = %arg18, %arg31 = %arg19, %arg32 = %arg20, %arg33 = %arg21, %arg34 = %arg22, %arg35 = %arg23, %arg36 = %arg24, %arg37 = %c0_i32, %arg38 = %c0_i32, %arg39 = %c1_i32) -> (i32, i32, i8, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32) {
        %10 = arith.index_cast %arg25 : index to i32
        %11 = arith.cmpi eq, %arg39, %c0_i32 : i32
        %12 = scf.if %11 -> (i1) {
          scf.yield %true : i1
        } else {
          %23 = arith.cmpi slt, %arg37, %c1_i32 : i32
          scf.yield %23 : i1
        }
        %13 = arith.select %12, %c1_i32, %arg37 : i32
        %14 = arith.select %12, %c0_i32, %arg38 : i32
        %15 = arith.select %12, %c1_i32, %arg39 : i32
        %16 = arith.addi %4, %10 : i32
        %17 = arith.addi %16, %13 : i32
        %18 = arith.index_cast %17 : i32 to index
        %19 = memref.load %arg3[%18] : memref<?xi8>
        %20:17 = scf.while (%arg40 = %arg28, %arg41 = %arg29, %arg42 = %arg30, %arg43 = %arg31, %arg44 = %arg32, %arg45 = %arg33, %arg46 = %arg34, %arg47 = %arg35, %arg48 = %arg36, %arg49 = %c0_i32, %arg50 = %c0_i32, %arg51 = %c0_i32, %arg52 = %19, %arg53 = %15, %arg54 = %13, %arg55 = %14, %arg56 = %15, %arg57 = %true) : (i8, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i8, i32, i32, i32, i32, i1) -> (i8, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i8, i32) {
          %23 = arith.extsi %arg52 : i8 to i32
          %24 = arith.cmpi ne, %23, %c0_i32 : i32
          %25 = arith.andi %24, %arg57 : i1
          scf.condition(%25) %arg40, %arg41, %arg42, %arg43, %arg44, %arg45, %arg46, %arg47, %arg48, %arg49, %arg51, %arg53, %arg54, %arg56, %arg50, %arg52, %arg55 : i8, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i8, i32
        } do {
        ^bb0(%arg40: i8, %arg41: i32, %arg42: i32, %arg43: i32, %arg44: i32, %arg45: i32, %arg46: i32, %arg47: i32, %arg48: i32, %arg49: i32, %arg50: i32, %arg51: i32, %arg52: i32, %arg53: i32, %arg54: i32, %arg55: i8, %arg56: i32):
          %23 = arith.extsi %arg55 : i8 to i32
          %24 = arith.cmpi eq, %23, %c65_i32 : i32
          %25 = scf.if %24 -> (i1) {
            scf.yield %true : i1
          } else {
            %30 = arith.cmpi eq, %23, %c97_i32 : i32
            scf.yield %30 : i1
          }
          %26:2 = scf.if %25 -> (i32, i1) {
            scf.yield %c0_i32, %true : i32, i1
          } else {
            %30 = arith.cmpi eq, %23, %c67_i32 : i32
            %31 = scf.if %30 -> (i1) {
              scf.yield %true : i1
            } else {
              %33 = arith.cmpi eq, %23, %c99_i32 : i32
              scf.yield %33 : i1
            }
            %32:2 = scf.if %31 -> (i32, i1) {
              scf.yield %c1_i32, %true : i32, i1
            } else {
              %33 = arith.cmpi eq, %23, %c71_i32 : i32
              %34 = scf.if %33 -> (i1) {
                scf.yield %true : i1
              } else {
                %36 = arith.cmpi eq, %23, %c103_i32 : i32
                scf.yield %36 : i1
              }
              %35:2 = scf.if %34 -> (i32, i1) {
                scf.yield %c2_i32, %true : i32, i1
              } else {
                %36 = arith.cmpi eq, %23, %c84_i32 : i32
                %37 = scf.if %36 -> (i1) {
                  scf.yield %true : i1
                } else {
                  %39 = arith.cmpi eq, %23, %c116_i32 : i32
                  scf.yield %39 : i1
                }
                %38 = arith.select %37, %c3_i32, %c-1_i32 : i32
                scf.yield %38, %37 : i32, i1
              }
              scf.yield %35#0, %35#1 : i32, i1
            }
            scf.yield %32#0, %32#1 : i32, i1
          }
          %27:3 = scf.if %26#1 -> (i32, i1, i1) {
            %30 = arith.index_cast %arg53 : i32 to index
            %31 = arith.index_cast %26#0 : i32 to index
            %32 = memref.load %arg9[%30, %31] : memref<?x4xi32>
            %33 = arith.cmpi ne, %32, %c0_i32 : i32
            %34 = arith.cmpi eq, %32, %c0_i32 : i32
            scf.yield %32, %33, %34 : i32, i1, i1
          } else {
            scf.yield %c0_i32, %false, %true : i32, i1, i1
          }
          %28 = arith.select %27#2, %c1_i32, %arg49 : i32
          %29:13 = scf.if %27#1 -> (i32, i32, i32, i32, i32, i32, i32, i8, i32, i32, i8, i32, i1) {
            %30 = arith.index_cast %27#0 : i32 to index
            %31 = memref.load %arg6[%30] : memref<?xi32>
            %32 = memref.load %arg7[%30] : memref<?xi32>
            %33 = arith.subi %32, %31 : i32
            %34 = arith.addi %33, %c1_i32 : i32
            %35 = arith.cmpi sgt, %arg56, %c0_i32 : i32
            %36:6 = scf.if %35 -> (i32, i32, i32, i32, i32, i32) {
              %42 = arith.subi %arg56, %34 : i32
              %43 = arith.addi %31, %arg56 : i32
              %44 = arith.addi %32, %c1_i32 : i32
              %45 = arith.cmpi slt, %43, %44 : i32
              %46 = arith.select %45, %43, %44 : i32
              %47 = arith.cmpi slt, %34, %arg56 : i32
              %48 = arith.select %47, %34, %arg56 : i32
              %49 = arith.addi %arg52, %48 : i32
              %50 = arith.cmpi sgt, %42, %c0_i32 : i32
              %51 = arith.select %50, %42, %c0_i32 : i32
              scf.yield %44, %43, %42, %46, %49, %51 : i32, i32, i32, i32, i32, i32
            } else {
              %42 = arith.addi %arg52, %c1_i32 : i32
              %43 = arith.addi %31, %c1_i32 : i32
              scf.yield %arg41, %arg42, %arg43, %43, %42, %arg56 : i32, i32, i32, i32, i32, i32
            }
            %37 = arith.addi %16, %36#4 : i32
            %38 = arith.index_cast %37 : i32 to index
            %39 = memref.load %arg3[%38] : memref<?xi8>
            %40:5 = scf.while (%arg57 = %arg40, %arg58 = %arg54, %arg59 = %36#3, %arg60 = %39, %arg61 = %36#4, %arg62 = %true) : (i8, i32, i32, i8, i32, i1) -> (i8, i32, i32, i8, i32) {
              %42 = arith.cmpi sle, %arg59, %32 : i32
              %43 = scf.if %42 -> (i1) {
                %45 = arith.extsi %arg60 : i8 to i32
                %46 = arith.cmpi ne, %45, %c0_i32 : i32
                scf.yield %46 : i1
              } else {
                scf.yield %false : i1
              }
              %44 = arith.andi %43, %arg62 : i1
              scf.condition(%44) %arg57, %arg58, %arg59, %arg60, %arg61 : i8, i32, i32, i8, i32
            } do {
            ^bb0(%arg57: i8, %arg58: i32, %arg59: i32, %arg60: i8, %arg61: i32):
              %42 = arith.index_cast %arg59 : i32 to index
              %43 = memref.load %arg4[%42] : memref<?xi8>
              %44 = arith.cmpi ne, %43, %arg60 : i8
              %45 = arith.select %44, %c1_i32, %arg58 : i32
              %46 = arith.cmpi eq, %43, %arg60 : i8
              %47:3 = scf.if %46 -> (i32, i32, i8) {
                %48 = arith.addi %arg61, %c1_i32 : i32
                %49 = arith.addi %arg59, %c1_i32 : i32
                %50 = arith.addi %16, %48 : i32
                %51 = arith.index_cast %50 : i32 to index
                %52 = memref.load %arg3[%51] : memref<?xi8>
                scf.yield %48, %49, %52 : i32, i32, i8
              } else {
                scf.yield %arg61, %arg59, %arg60 : i32, i32, i8
              }
              scf.yield %43, %45, %47#1, %47#2, %47#0, %46 : i8, i32, i32, i8, i32, i1
            }
            %41 = arith.cmpi eq, %40#1, %c0_i32 : i32
            scf.yield %31, %32, %34, %36#0, %36#1, %36#2, %36#5, %40#0, %40#1, %40#2, %40#3, %40#4, %41 : i32, i32, i32, i32, i32, i32, i32, i8, i32, i32, i8, i32, i1
          } else {
            scf.yield %arg46, %arg45, %arg44, %arg41, %arg42, %arg43, %arg56, %arg40, %arg54, %arg50, %arg55, %arg52, %false : i32, i32, i32, i32, i32, i32, i32, i8, i32, i32, i8, i32, i1
          }
          scf.yield %29#7, %29#3, %29#4, %29#5, %29#2, %29#1, %29#0, %27#0, %26#0, %28, %29#8, %29#9, %29#10, %arg53, %29#11, %29#6, %27#0, %29#12 : i8, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i8, i32, i32, i32, i32, i1
        }
        %21 = arith.cmpi ne, %20#9, %c0_i32 : i32
        %22:5 = scf.if %21 -> (i32, i32, i32, i32, i32) {
          %23 = arith.cmpi sgt, %20#12, %arg5 : i32
          %24 = arith.select %23, %16, %arg27 : i32
          scf.if %23 {
            %28 = arith.index_cast %16 : i32 to index
            memref.store %20#11, %arg10[%28] : memref<?xi32>
            memref.store %c0_i32, %arg11[%28] : memref<?xi32>
            memref.store %20#12, %arg12[%28] : memref<?xi32>
          }
          %25 = arith.addi %20#12, %c-1_i32 : i32
          %26 = arith.index_cast %20#11 : i32 to index
          %27 = memref.load %arg8[%26] : memref<?xi32>
          scf.yield %arg26, %24, %25, %c0_i32, %27 : i32, i32, i32, i32, i32
        } else {
          %23 = arith.cmpi sgt, %20#12, %arg5 : i32
          %24 = arith.select %23, %16, %arg26 : i32
          scf.if %23 {
            %32 = arith.index_cast %16 : i32 to index
            memref.store %20#13, %arg10[%32] : memref<?xi32>
            %33 = arith.index_cast %20#13 : i32 to index
            %34 = memref.load %arg6[%33] : memref<?xi32>
            %35 = arith.subi %20#10, %34 : i32
            memref.store %35, %arg11[%32] : memref<?xi32>
            memref.store %20#12, %arg12[%32] : memref<?xi32>
          }
          %25 = arith.index_cast %20#13 : i32 to index
          %26 = memref.load %arg6[%25] : memref<?xi32>
          %27 = arith.subi %20#10, %26 : i32
          %28 = arith.addi %27, %c1_i32 : i32
          %29 = arith.subi %20#12, %28 : i32
          %30 = arith.index_cast %20#11 : i32 to index
          %31 = memref.load %arg8[%30] : memref<?xi32>
          scf.yield %24, %arg27, %29, %27, %31 : i32, i32, i32, i32, i32
        }
        scf.yield %22#0, %22#1, %20#0, %20#1, %20#2, %20#3, %20#4, %20#5, %20#6, %20#7, %20#8, %22#2, %22#3, %22#4 : i32, i32, i8, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32
      }
      scf.yield %9#0, %9#1, %9#2, %9#3, %9#4, %9#5, %9#6, %9#7, %9#8, %9#9, %9#10 : i32, i32, i8, i32, i32, i32, i32, i32, i32, i32, i32
    }
    return
  }
}
