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
