; ModuleID = '/home/tremore/Programming/GitHub/comp-asgmt/assignments/assignment-4/sources/lf-ndd-test.bc'
source_filename = "/home/tremore/Programming/GitHub/comp-asgmt/assignments/assignment-4/sources/lf-ndd-test.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @array_ndd_test_INDEX_INC(i32 noundef %0) #0 {
  %2 = zext i32 10 to i64
  %3 = call ptr @llvm.stacksave.p0()
  %4 = alloca i32, i64 %2, align 16
  %5 = mul nuw i64 %2, 4
  call void @llvm.memset.p0.i64(ptr align 16 %4, i8 0, i64 %5, i1 false)
  br label %6

6:                                                ; preds = %11, %1
  %.01 = phi i32 [ 0, %1 ], [ %10, %11 ]
  %7 = add nsw i32 %.01, 1
  %8 = sext i32 %.01 to i64
  %9 = getelementptr inbounds i32, ptr %4, i64 %8
  store i32 %7, ptr %9, align 4
  %10 = add nsw i32 %.01, 1
  br label %11

11:                                               ; preds = %6
  %12 = icmp slt i32 %10, 10
  br i1 %12, label %6, label %13, !llvm.loop !6

13:                                               ; preds = %11
  br label %14

14:                                               ; preds = %20, %13
  %.0 = phi i32 [ 0, %13 ], [ %19, %20 ]
  %15 = add nsw i32 %.0, 3
  %16 = sext i32 %15 to i64
  %17 = getelementptr inbounds i32, ptr %4, i64 %16
  %18 = load i32, ptr %17, align 4
  %19 = add nsw i32 %.0, 1
  br label %20

20:                                               ; preds = %14
  %21 = icmp slt i32 %19, 10
  br i1 %21, label %14, label %22, !llvm.loop !8

22:                                               ; preds = %20
  call void @llvm.stackrestore.p0(ptr %3)
  ret void
}

; Function Attrs: nocallback nofree nosync nounwind willreturn
declare ptr @llvm.stacksave.p0() #1

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i64(ptr nocapture writeonly, i8, i64, i1 immarg) #2

; Function Attrs: nocallback nofree nosync nounwind willreturn
declare void @llvm.stackrestore.p0(ptr) #1

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @array_ndd_test_CONSTANT(i32 noundef %0) #0 {
  %2 = zext i32 10 to i64
  %3 = call ptr @llvm.stacksave.p0()
  %4 = alloca i32, i64 %2, align 16
  %5 = mul nuw i64 %2, 4
  call void @llvm.memset.p0.i64(ptr align 16 %4, i8 0, i64 %5, i1 false)
  br label %6

6:                                                ; preds = %15, %1
  %.01 = phi i32 [ 0, %1 ], [ %14, %15 ]
  %.0 = phi i32 [ 0, %1 ], [ %7, %15 ]
  %7 = add nsw i32 %.0, 1
  %8 = add nsw i32 %.01, 1
  %9 = add nsw i32 %8, 1
  %10 = add nsw i32 %8, %7
  %11 = add nsw i32 %10, 9
  %12 = sext i32 %11 to i64
  %13 = getelementptr inbounds i32, ptr %4, i64 %12
  store i32 %9, ptr %13, align 4
  %14 = add nsw i32 %8, 1
  br label %15

15:                                               ; preds = %6
  %16 = icmp slt i32 %14, 10
  br i1 %16, label %6, label %17, !llvm.loop !9

17:                                               ; preds = %15
  br label %18

18:                                               ; preds = %24, %17
  %.1 = phi i32 [ %7, %17 ], [ %23, %24 ]
  %19 = add nsw i32 %.1, 3
  %20 = sext i32 %19 to i64
  %21 = getelementptr inbounds i32, ptr %4, i64 %20
  %22 = load i32, ptr %21, align 4
  %23 = add nsw i32 %.1, 1
  br label %24

24:                                               ; preds = %18
  %25 = icmp slt i32 %23, 10
  br i1 %25, label %18, label %26, !llvm.loop !10

26:                                               ; preds = %24
  call void @llvm.stackrestore.p0(ptr %3)
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @array_ndd_test_CONSTANT_2(i32 noundef %0) #0 {
  %2 = zext i32 10 to i64
  %3 = call ptr @llvm.stacksave.p0()
  %4 = alloca i32, i64 %2, align 16
  %5 = mul nuw i64 %2, 4
  call void @llvm.memset.p0.i64(ptr align 16 %4, i8 0, i64 %5, i1 false)
  br label %6

6:                                                ; preds = %11, %1
  %.01 = phi i32 [ 0, %1 ], [ %10, %11 ]
  %7 = add nsw i32 %.01, 1
  %8 = sext i32 %.01 to i64
  %9 = getelementptr inbounds i32, ptr %4, i64 %8
  store i32 %7, ptr %9, align 4
  %10 = add nsw i32 %.01, 1
  br label %11

11:                                               ; preds = %6
  %12 = icmp slt i32 %10, 10
  br i1 %12, label %6, label %13, !llvm.loop !11

13:                                               ; preds = %11
  br label %14

14:                                               ; preds = %20, %13
  %.0 = phi i32 [ 0, %13 ], [ %19, %20 ]
  %15 = add nsw i32 %.0, 3
  %16 = sext i32 %15 to i64
  %17 = getelementptr inbounds i32, ptr %4, i64 %16
  %18 = load i32, ptr %17, align 4
  %19 = add nsw i32 %.0, 1
  br label %20

20:                                               ; preds = %14
  %21 = icmp slt i32 %19, 10
  br i1 %21, label %14, label %22, !llvm.loop !12

22:                                               ; preds = %20
  call void @llvm.stackrestore.p0(ptr %3)
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @matrix_ndd_test() #0 {
  %1 = add nsw i32 10, 3
  %2 = zext i32 %1 to i64
  %3 = zext i32 %1 to i64
  %4 = zext i32 %1 to i64
  %5 = zext i32 %1 to i64
  %6 = call ptr @llvm.stacksave.p0()
  %7 = mul nuw i64 %2, %3
  %8 = mul nuw i64 %7, %4
  %9 = mul nuw i64 %8, %5
  %10 = alloca i32, i64 %9, align 16
  %11 = mul nuw i64 %2, %3
  %12 = mul nuw i64 %11, %4
  %13 = mul nuw i64 %12, %5
  %14 = mul nuw i64 %13, 4
  call void @llvm.memset.p0.i64(ptr align 16 %10, i8 0, i64 %14, i1 false)
  br label %15

15:                                               ; preds = %32, %0
  %.01 = phi i32 [ 0, %0 ], [ %31, %32 ]
  %16 = add nsw i32 %.01, 1
  %17 = sext i32 %.01 to i64
  %18 = mul nuw i64 %3, %4
  %19 = mul nuw i64 %18, %5
  %20 = mul nsw i64 %17, %19
  %21 = getelementptr inbounds i32, ptr %10, i64 %20
  %22 = sext i32 %.01 to i64
  %23 = mul nuw i64 %4, %5
  %24 = mul nsw i64 %22, %23
  %25 = getelementptr inbounds i32, ptr %21, i64 %24
  %26 = sext i32 %.01 to i64
  %27 = mul nsw i64 %26, %5
  %28 = getelementptr inbounds i32, ptr %25, i64 %27
  %29 = sext i32 %.01 to i64
  %30 = getelementptr inbounds i32, ptr %28, i64 %29
  store i32 %16, ptr %30, align 4
  %31 = add nsw i32 %.01, 1
  br label %32

32:                                               ; preds = %15
  %33 = icmp slt i32 %31, 10
  br i1 %33, label %15, label %34, !llvm.loop !13

34:                                               ; preds = %32
  br label %35

35:                                               ; preds = %52, %34
  %.0 = phi i32 [ 0, %34 ], [ %51, %52 ]
  %36 = sext i32 %.0 to i64
  %37 = mul nuw i64 %3, %4
  %38 = mul nuw i64 %37, %5
  %39 = mul nsw i64 %36, %38
  %40 = getelementptr inbounds i32, ptr %10, i64 %39
  %41 = sext i32 %.0 to i64
  %42 = mul nuw i64 %4, %5
  %43 = mul nsw i64 %41, %42
  %44 = getelementptr inbounds i32, ptr %40, i64 %43
  %45 = sext i32 %.0 to i64
  %46 = mul nsw i64 %45, %5
  %47 = getelementptr inbounds i32, ptr %44, i64 %46
  %48 = sext i32 %.0 to i64
  %49 = getelementptr inbounds i32, ptr %47, i64 %48
  %50 = load i32, ptr %49, align 4
  %51 = add nsw i32 %.0, 2
  br label %52

52:                                               ; preds = %35
  %53 = mul nsw i32 10, 2
  %54 = icmp slt i32 %51, %53
  br i1 %54, label %35, label %55, !llvm.loop !14

55:                                               ; preds = %52
  call void @llvm.stackrestore.p0(ptr %6)
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @matrix_ndd_test_non_so_come_chiamarlo() #0 {
  %1 = add nsw i32 10, 3
  %2 = add nsw i32 %1, 3
  %3 = zext i32 %2 to i64
  %4 = sub nsw i32 %1, 1
  %5 = zext i32 %4 to i64
  %6 = mul nsw i32 %1, 4
  %7 = zext i32 %6 to i64
  %8 = sdiv i32 %1, 10
  %9 = zext i32 %8 to i64
  %10 = call ptr @llvm.stacksave.p0()
  %11 = mul nuw i64 %3, %5
  %12 = mul nuw i64 %11, %7
  %13 = mul nuw i64 %12, %9
  %14 = alloca i32, i64 %13, align 16
  %15 = mul nuw i64 %3, %5
  %16 = mul nuw i64 %15, %7
  %17 = mul nuw i64 %16, %9
  %18 = mul nuw i64 %17, 4
  call void @llvm.memset.p0.i64(ptr align 16 %14, i8 0, i64 %18, i1 false)
  br label %19

19:                                               ; preds = %51, %0
  %.07 = phi i32 [ 0, %0 ], [ %50, %51 ]
  br label %20

20:                                               ; preds = %47, %19
  %.05 = phi i32 [ 0, %19 ], [ %46, %47 ]
  br label %21

21:                                               ; preds = %43, %20
  %.04 = phi i32 [ 0, %20 ], [ %42, %43 ]
  br label %22

22:                                               ; preds = %39, %21
  %.03 = phi i32 [ 0, %21 ], [ %38, %39 ]
  %23 = add nsw i32 %.07, 1
  %24 = sext i32 %.07 to i64
  %25 = mul nuw i64 %5, %7
  %26 = mul nuw i64 %25, %9
  %27 = mul nsw i64 %24, %26
  %28 = getelementptr inbounds i32, ptr %14, i64 %27
  %29 = sext i32 %.05 to i64
  %30 = mul nuw i64 %7, %9
  %31 = mul nsw i64 %29, %30
  %32 = getelementptr inbounds i32, ptr %28, i64 %31
  %33 = sext i32 %.04 to i64
  %34 = mul nsw i64 %33, %9
  %35 = getelementptr inbounds i32, ptr %32, i64 %34
  %36 = sext i32 %.03 to i64
  %37 = getelementptr inbounds i32, ptr %35, i64 %36
  store i32 %23, ptr %37, align 4
  %38 = add nsw i32 %.03, 1
  br label %39

39:                                               ; preds = %22
  %40 = icmp slt i32 %38, 10
  br i1 %40, label %22, label %41, !llvm.loop !15

41:                                               ; preds = %39
  %42 = add nsw i32 %.04, 1
  br label %43

43:                                               ; preds = %41
  %44 = icmp slt i32 %42, 10
  br i1 %44, label %21, label %45, !llvm.loop !16

45:                                               ; preds = %43
  %46 = add nsw i32 %.05, 1
  br label %47

47:                                               ; preds = %45
  %48 = icmp slt i32 %46, 10
  br i1 %48, label %20, label %49, !llvm.loop !17

49:                                               ; preds = %47
  %50 = add nsw i32 %.07, 1
  br label %51

51:                                               ; preds = %49
  %52 = icmp slt i32 %50, 10
  br i1 %52, label %19, label %53, !llvm.loop !18

53:                                               ; preds = %51
  br label %54

54:                                               ; preds = %86, %53
  %.06 = phi i32 [ 0, %53 ], [ %85, %86 ]
  br label %55

55:                                               ; preds = %82, %54
  %.02 = phi i32 [ 0, %54 ], [ %81, %82 ]
  br label %56

56:                                               ; preds = %78, %55
  %.01 = phi i32 [ 0, %55 ], [ %77, %78 ]
  br label %57

57:                                               ; preds = %74, %56
  %.0 = phi i32 [ 0, %56 ], [ %73, %74 ]
  %58 = sext i32 %.06 to i64
  %59 = mul nuw i64 %5, %7
  %60 = mul nuw i64 %59, %9
  %61 = mul nsw i64 %58, %60
  %62 = getelementptr inbounds i32, ptr %14, i64 %61
  %63 = sext i32 %.02 to i64
  %64 = mul nuw i64 %7, %9
  %65 = mul nsw i64 %63, %64
  %66 = getelementptr inbounds i32, ptr %62, i64 %65
  %67 = sext i32 %.01 to i64
  %68 = mul nsw i64 %67, %9
  %69 = getelementptr inbounds i32, ptr %66, i64 %68
  %70 = sext i32 %.0 to i64
  %71 = getelementptr inbounds i32, ptr %69, i64 %70
  %72 = load i32, ptr %71, align 4
  %73 = add nsw i32 %.0, 1
  br label %74

74:                                               ; preds = %57
  %75 = icmp slt i32 %73, 10
  br i1 %75, label %57, label %76, !llvm.loop !19

76:                                               ; preds = %74
  %77 = add nsw i32 %.01, 1
  br label %78

78:                                               ; preds = %76
  %79 = icmp slt i32 %77, 10
  br i1 %79, label %56, label %80, !llvm.loop !20

80:                                               ; preds = %78
  %81 = add nsw i32 %.02, 1
  br label %82

82:                                               ; preds = %80
  %83 = icmp slt i32 %81, 10
  br i1 %83, label %55, label %84, !llvm.loop !21

84:                                               ; preds = %82
  %85 = add nsw i32 %.06, 2
  br label %86

86:                                               ; preds = %84
  %87 = mul nsw i32 10, 2
  %88 = icmp slt i32 %85, %87
  br i1 %88, label %54, label %89, !llvm.loop !22

89:                                               ; preds = %86
  call void @llvm.stackrestore.p0(ptr %10)
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @matrix_ndd_test_multiple_matrices() #0 {
  %1 = add nsw i32 10, 3
  %2 = zext i32 %1 to i64
  %3 = zext i32 %1 to i64
  %4 = zext i32 %1 to i64
  %5 = zext i32 %1 to i64
  %6 = call ptr @llvm.stacksave.p0()
  %7 = mul nuw i64 %2, %3
  %8 = mul nuw i64 %7, %4
  %9 = mul nuw i64 %8, %5
  %10 = alloca i32, i64 %9, align 16
  %11 = mul nuw i64 %2, %3
  %12 = mul nuw i64 %11, %4
  %13 = mul nuw i64 %12, %5
  %14 = mul nuw i64 %13, 4
  call void @llvm.memset.p0.i64(ptr align 16 %10, i8 0, i64 %14, i1 false)
  %15 = zext i32 %1 to i64
  %16 = zext i32 %1 to i64
  %17 = mul nuw i64 %15, %16
  %18 = alloca i32, i64 %17, align 16
  %19 = zext i32 %1 to i64
  %20 = zext i32 %1 to i64
  %21 = mul nuw i64 %19, %20
  %22 = alloca i32, i64 %21, align 16
  br label %23

23:                                               ; preds = %62, %0
  %.03 = phi i32 [ 0, %0 ], [ %61, %62 ]
  %.0 = phi i32 [ 0, %0 ], [ %60, %62 ]
  %24 = add nsw i32 %.03, 1
  %25 = sext i32 %.03 to i64
  %26 = mul nuw i64 %3, %4
  %27 = mul nuw i64 %26, %5
  %28 = mul nsw i64 %25, %27
  %29 = getelementptr inbounds i32, ptr %10, i64 %28
  %30 = sext i32 %.03 to i64
  %31 = mul nuw i64 %4, %5
  %32 = mul nsw i64 %30, %31
  %33 = getelementptr inbounds i32, ptr %29, i64 %32
  %34 = sext i32 %.03 to i64
  %35 = mul nsw i64 %34, %5
  %36 = getelementptr inbounds i32, ptr %33, i64 %35
  %37 = sext i32 %.03 to i64
  %38 = getelementptr inbounds i32, ptr %36, i64 %37
  store i32 %24, ptr %38, align 4
  %39 = mul nsw i32 %.03, 2
  %40 = sext i32 %.03 to i64
  %41 = mul nsw i64 %40, %16
  %42 = getelementptr inbounds i32, ptr %18, i64 %41
  %43 = sext i32 %.03 to i64
  %44 = getelementptr inbounds i32, ptr %42, i64 %43
  store i32 %39, ptr %44, align 4
  %45 = sext i32 %.03 to i64
  %46 = mul nuw i64 %3, %4
  %47 = mul nuw i64 %46, %5
  %48 = mul nsw i64 %45, %47
  %49 = getelementptr inbounds i32, ptr %10, i64 %48
  %50 = sext i32 %.03 to i64
  %51 = mul nuw i64 %4, %5
  %52 = mul nsw i64 %50, %51
  %53 = getelementptr inbounds i32, ptr %49, i64 %52
  %54 = sext i32 %.03 to i64
  %55 = mul nsw i64 %54, %5
  %56 = getelementptr inbounds i32, ptr %53, i64 %55
  %57 = sext i32 %.03 to i64
  %58 = getelementptr inbounds i32, ptr %56, i64 %57
  %59 = load i32, ptr %58, align 4
  %60 = add nsw i32 %.0, %59
  %61 = add nsw i32 %.03, 1
  br label %62

62:                                               ; preds = %23
  %63 = icmp slt i32 %61, 10
  br i1 %63, label %23, label %64, !llvm.loop !23

64:                                               ; preds = %62
  br label %65

65:                                               ; preds = %96, %64
  %.02 = phi i32 [ 0, %64 ], [ %94, %96 ]
  %.01 = phi i32 [ 0, %64 ], [ %95, %96 ]
  %66 = sext i32 %.02 to i64
  %67 = mul nuw i64 %3, %4
  %68 = mul nuw i64 %67, %5
  %69 = mul nsw i64 %66, %68
  %70 = getelementptr inbounds i32, ptr %10, i64 %69
  %71 = sext i32 %.02 to i64
  %72 = mul nuw i64 %4, %5
  %73 = mul nsw i64 %71, %72
  %74 = getelementptr inbounds i32, ptr %70, i64 %73
  %75 = add nsw i32 %.02, 1
  %76 = sext i32 %75 to i64
  %77 = mul nsw i64 %76, %5
  %78 = getelementptr inbounds i32, ptr %74, i64 %77
  %79 = sext i32 %.02 to i64
  %80 = getelementptr inbounds i32, ptr %78, i64 %79
  %81 = load i32, ptr %80, align 4
  %82 = sext i32 %.01 to i64
  %83 = mul nsw i64 %82, %16
  %84 = getelementptr inbounds i32, ptr %18, i64 %83
  %85 = sext i32 %.02 to i64
  %86 = getelementptr inbounds i32, ptr %84, i64 %85
  %87 = load i32, ptr %86, align 4
  %88 = mul nsw i32 %.02, 2
  %89 = sext i32 %61 to i64
  %90 = mul nsw i64 %89, %20
  %91 = getelementptr inbounds i32, ptr %22, i64 %90
  %92 = sext i32 %61 to i64
  %93 = getelementptr inbounds i32, ptr %91, i64 %92
  store i32 %88, ptr %93, align 4
  %94 = add nsw i32 %.02, 1
  %95 = add nsw i32 %.01, 2
  br label %96

96:                                               ; preds = %65
  %97 = mul nsw i32 10, 2
  %98 = icmp slt i32 %94, %97
  br i1 %98, label %65, label %99, !llvm.loop !24

99:                                               ; preds = %96
  call void @llvm.stackrestore.p0(ptr %6)
  ret void
}

attributes #0 = { noinline nounwind sspstrong uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nocallback nofree nosync nounwind willreturn }
attributes #2 = { nocallback nofree nounwind willreturn memory(argmem: write) }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"clang version 19.1.7"}
!6 = distinct !{!6, !7}
!7 = !{!"llvm.loop.mustprogress"}
!8 = distinct !{!8, !7}
!9 = distinct !{!9, !7}
!10 = distinct !{!10, !7}
!11 = distinct !{!11, !7}
!12 = distinct !{!12, !7}
!13 = distinct !{!13, !7}
!14 = distinct !{!14, !7}
!15 = distinct !{!15, !7}
!16 = distinct !{!16, !7}
!17 = distinct !{!17, !7}
!18 = distinct !{!18, !7}
!19 = distinct !{!19, !7}
!20 = distinct !{!20, !7}
!21 = distinct !{!21, !7}
!22 = distinct !{!22, !7}
!23 = distinct !{!23, !7}
!24 = distinct !{!24, !7}
