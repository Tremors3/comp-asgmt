; ModuleID = '/home/tremore/Programming/GitHub/comp-asgmt/workspace/custom-ndd-analyser/sources/lf-ndd-test.bc'
source_filename = "/home/tremore/Programming/GitHub/comp-asgmt/workspace/custom-ndd-analyser/sources/lf-ndd-test.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @array_NotND_SAME_INDEXES_INCREMENT_OF_1(i32 noundef %0) #0 {
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

14:                                               ; preds = %19, %13
  %.0 = phi i32 [ 0, %13 ], [ %18, %19 ]
  %15 = sext i32 %.0 to i64
  %16 = getelementptr inbounds i32, ptr %4, i64 %15
  %17 = load i32, ptr %16, align 4
  %18 = add nsw i32 %.0, 1
  br label %19

19:                                               ; preds = %14
  %20 = icmp slt i32 %18, 10
  br i1 %20, label %14, label %21, !llvm.loop !8

21:                                               ; preds = %19
  call void @llvm.stackrestore.p0(ptr %3)
  ret void
}

; Function Attrs: nocallback nofree nosync nounwind willreturn
declare ptr @llvm.stacksave.p0() #1

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i64(ptr writeonly captures(none), i8, i64, i1 immarg) #2

; Function Attrs: nocallback nofree nosync nounwind willreturn
declare void @llvm.stackrestore.p0(ptr) #1

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @array_NotND_SAME_INDEXES_INCREMENT_OF_2(i32 noundef %0) #0 {
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
  %10 = add nsw i32 %.01, 2
  br label %11

11:                                               ; preds = %6
  %12 = icmp slt i32 %10, 10
  br i1 %12, label %6, label %13, !llvm.loop !9

13:                                               ; preds = %11
  br label %14

14:                                               ; preds = %19, %13
  %.0 = phi i32 [ 0, %13 ], [ %18, %19 ]
  %15 = sext i32 %.0 to i64
  %16 = getelementptr inbounds i32, ptr %4, i64 %15
  %17 = load i32, ptr %16, align 4
  %18 = add nsw i32 %.0, 2
  br label %19

19:                                               ; preds = %14
  %20 = mul nsw i32 10, 2
  %21 = icmp slt i32 %18, %20
  br i1 %21, label %14, label %22, !llvm.loop !10

22:                                               ; preds = %19
  call void @llvm.stackrestore.p0(ptr %3)
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @array_NotND_SAME_INDEXES_INCREMENT_OF_3_INLINE(i32 noundef %0) #0 {
  %2 = zext i32 10 to i64
  %3 = call ptr @llvm.stacksave.p0()
  %4 = alloca i32, i64 %2, align 16
  %5 = mul nuw i64 %2, 4
  call void @llvm.memset.p0.i64(ptr align 16 %4, i8 0, i64 %5, i1 false)
  br label %6

6:                                                ; preds = %12, %1
  %.01 = phi i32 [ 0, %1 ], [ %11, %12 ]
  %7 = add nsw i32 %.01, 1
  %8 = add nsw i32 %.01, 3
  %9 = sext i32 %8 to i64
  %10 = getelementptr inbounds i32, ptr %4, i64 %9
  store i32 %7, ptr %10, align 4
  %11 = add nsw i32 %.01, 1
  br label %12

12:                                               ; preds = %6
  %13 = icmp slt i32 %11, 10
  br i1 %13, label %6, label %14, !llvm.loop !11

14:                                               ; preds = %12
  br label %15

15:                                               ; preds = %21, %14
  %.0 = phi i32 [ 0, %14 ], [ %20, %21 ]
  %16 = add nsw i32 %.0, 3
  %17 = sext i32 %16 to i64
  %18 = getelementptr inbounds i32, ptr %4, i64 %17
  %19 = load i32, ptr %18, align 4
  %20 = add nsw i32 %.0, 1
  br label %21

21:                                               ; preds = %15
  %22 = icmp slt i32 %20, 10
  br i1 %22, label %15, label %23, !llvm.loop !12

23:                                               ; preds = %21
  call void @llvm.stackrestore.p0(ptr %3)
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @array_NotND_SAME_INDEXES_START_AT_4(i32 noundef %0) #0 {
  %2 = zext i32 10 to i64
  %3 = call ptr @llvm.stacksave.p0()
  %4 = alloca i32, i64 %2, align 16
  %5 = mul nuw i64 %2, 4
  call void @llvm.memset.p0.i64(ptr align 16 %4, i8 0, i64 %5, i1 false)
  br label %6

6:                                                ; preds = %11, %1
  %.01 = phi i32 [ 4, %1 ], [ %10, %11 ]
  %7 = add nsw i32 %.01, 1
  %8 = sext i32 %.01 to i64
  %9 = getelementptr inbounds i32, ptr %4, i64 %8
  store i32 %7, ptr %9, align 4
  %10 = add nsw i32 %.01, 1
  br label %11

11:                                               ; preds = %6
  %12 = add nsw i32 10, 4
  %13 = icmp slt i32 %10, %12
  br i1 %13, label %6, label %14, !llvm.loop !13

14:                                               ; preds = %11
  br label %15

15:                                               ; preds = %20, %14
  %.0 = phi i32 [ 4, %14 ], [ %19, %20 ]
  %16 = sext i32 %.0 to i64
  %17 = getelementptr inbounds i32, ptr %4, i64 %16
  %18 = load i32, ptr %17, align 4
  %19 = add nsw i32 %.0, 1
  br label %20

20:                                               ; preds = %15
  %21 = add nsw i32 10, 4
  %22 = icmp slt i32 %19, %21
  br i1 %22, label %15, label %23, !llvm.loop !14

23:                                               ; preds = %20
  call void @llvm.stackrestore.p0(ptr %3)
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @array_IsND_DIFFERENT_INDEXES_INCREMENT_OF_2(i32 noundef %0) #0 {
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
  br i1 %12, label %6, label %13, !llvm.loop !15

13:                                               ; preds = %11
  br label %14

14:                                               ; preds = %19, %13
  %.0 = phi i32 [ 0, %13 ], [ %18, %19 ]
  %15 = sext i32 %.0 to i64
  %16 = getelementptr inbounds i32, ptr %4, i64 %15
  %17 = load i32, ptr %16, align 4
  %18 = add nsw i32 %.0, 2
  br label %19

19:                                               ; preds = %14
  %20 = icmp slt i32 %18, 10
  br i1 %20, label %14, label %21, !llvm.loop !16

21:                                               ; preds = %19
  call void @llvm.stackrestore.p0(ptr %3)
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @array_IsND_DIFFERENT_INDEXES_INCREMENT_OF_3_INLINE(i32 noundef %0) #0 {
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
  br i1 %12, label %6, label %13, !llvm.loop !17

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
  br i1 %21, label %14, label %22, !llvm.loop !18

22:                                               ; preds = %20
  call void @llvm.stackrestore.p0(ptr %3)
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @array_IsND_DIFFERENT_INDEXES_START_AT_4(i32 noundef %0) #0 {
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
  br i1 %12, label %6, label %13, !llvm.loop !19

13:                                               ; preds = %11
  br label %14

14:                                               ; preds = %19, %13
  %.0 = phi i32 [ 4, %13 ], [ %18, %19 ]
  %15 = sext i32 %.0 to i64
  %16 = getelementptr inbounds i32, ptr %4, i64 %15
  %17 = load i32, ptr %16, align 4
  %18 = add nsw i32 %.0, 1
  br label %19

19:                                               ; preds = %14
  %20 = icmp slt i32 %18, 10
  br i1 %20, label %14, label %21, !llvm.loop !20

21:                                               ; preds = %19
  call void @llvm.stackrestore.p0(ptr %3)
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @matrix_NotND_SAME_INDEXES_INCREMENT_OF_1(i32 noundef %0) #0 {
  %2 = zext i32 10 to i64
  %3 = zext i32 10 to i64
  %4 = call ptr @llvm.stacksave.p0()
  %5 = mul nuw i64 %2, %3
  %6 = alloca i32, i64 %5, align 16
  %7 = mul nuw i64 %2, %3
  %8 = mul nuw i64 %7, 4
  call void @llvm.memset.p0.i64(ptr align 16 %6, i8 0, i64 %8, i1 false)
  br label %9

9:                                                ; preds = %18, %1
  %.03 = phi i32 [ 0, %1 ], [ %16, %18 ]
  %.02 = phi i32 [ 0, %1 ], [ %17, %18 ]
  %10 = add nsw i32 %.03, 1
  %11 = sext i32 %.03 to i64
  %12 = mul nsw i64 %11, %3
  %13 = getelementptr inbounds i32, ptr %6, i64 %12
  %14 = sext i32 %.02 to i64
  %15 = getelementptr inbounds i32, ptr %13, i64 %14
  store i32 %10, ptr %15, align 4
  %16 = add nsw i32 %.03, 1
  %17 = add nsw i32 %.02, 1
  br label %18

18:                                               ; preds = %9
  %19 = icmp slt i32 %16, 10
  br i1 %19, label %9, label %20, !llvm.loop !21

20:                                               ; preds = %18
  br label %21

21:                                               ; preds = %30, %20
  %.01 = phi i32 [ 0, %20 ], [ %28, %30 ]
  %.0 = phi i32 [ 0, %20 ], [ %29, %30 ]
  %22 = sext i32 %.01 to i64
  %23 = mul nsw i64 %22, %3
  %24 = getelementptr inbounds i32, ptr %6, i64 %23
  %25 = sext i32 %.0 to i64
  %26 = getelementptr inbounds i32, ptr %24, i64 %25
  %27 = load i32, ptr %26, align 4
  %28 = add nsw i32 %.01, 1
  %29 = add nsw i32 %.0, 1
  br label %30

30:                                               ; preds = %21
  %31 = icmp slt i32 %17, 10
  br i1 %31, label %21, label %32, !llvm.loop !22

32:                                               ; preds = %30
  call void @llvm.stackrestore.p0(ptr %4)
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @matrix_IsND_DIFFERENT_INDEXES_INCREMENT_OF_1(i32 noundef %0) #0 {
  %2 = zext i32 10 to i64
  %3 = zext i32 10 to i64
  %4 = call ptr @llvm.stacksave.p0()
  %5 = mul nuw i64 %2, %3
  %6 = alloca i32, i64 %5, align 16
  %7 = mul nuw i64 %2, %3
  %8 = mul nuw i64 %7, 4
  call void @llvm.memset.p0.i64(ptr align 16 %6, i8 0, i64 %8, i1 false)
  br label %9

9:                                                ; preds = %18, %1
  %.03 = phi i32 [ 0, %1 ], [ %16, %18 ]
  %.02 = phi i32 [ 0, %1 ], [ %17, %18 ]
  %10 = add nsw i32 %.03, 1
  %11 = sext i32 %.03 to i64
  %12 = mul nsw i64 %11, %3
  %13 = getelementptr inbounds i32, ptr %6, i64 %12
  %14 = sext i32 %.02 to i64
  %15 = getelementptr inbounds i32, ptr %13, i64 %14
  store i32 %10, ptr %15, align 4
  %16 = add nsw i32 %.03, 1
  %17 = add nsw i32 %.02, 1
  br label %18

18:                                               ; preds = %9
  %19 = mul nsw i32 10, 2
  %20 = icmp slt i32 %16, %19
  br i1 %20, label %9, label %21, !llvm.loop !23

21:                                               ; preds = %18
  br label %22

22:                                               ; preds = %31, %21
  %.01 = phi i32 [ 0, %21 ], [ %29, %31 ]
  %.0 = phi i32 [ 0, %21 ], [ %30, %31 ]
  %23 = sext i32 %.01 to i64
  %24 = mul nsw i64 %23, %3
  %25 = getelementptr inbounds i32, ptr %6, i64 %24
  %26 = sext i32 %.0 to i64
  %27 = getelementptr inbounds i32, ptr %25, i64 %26
  %28 = load i32, ptr %27, align 4
  %29 = add nsw i32 %.01, 2
  %30 = add nsw i32 %.0, 1
  br label %31

31:                                               ; preds = %22
  %32 = mul nsw i32 10, 2
  %33 = icmp slt i32 %17, %32
  br i1 %33, label %22, label %34, !llvm.loop !24

34:                                               ; preds = %31
  call void @llvm.stackrestore.p0(ptr %4)
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @matrix_x4_IsND_DIFFERENT_INDEXES_INCREMENT_OF_2() #0 {
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
  br i1 %33, label %15, label %34, !llvm.loop !25

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
  br i1 %54, label %35, label %55, !llvm.loop !26

55:                                               ; preds = %52
  call void @llvm.stackrestore.p0(ptr %6)
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @matrix_IsND_SPECIAL_CASE_RANDOM_INDEX(i32 noundef %0) #0 {
  %2 = zext i32 10 to i64
  %3 = zext i32 10 to i64
  %4 = call ptr @llvm.stacksave.p0()
  %5 = mul nuw i64 %2, %3
  %6 = alloca i32, i64 %5, align 16
  %7 = mul nuw i64 %2, %3
  %8 = mul nuw i64 %7, 4
  call void @llvm.memset.p0.i64(ptr align 16 %6, i8 0, i64 %8, i1 false)
  %9 = call i32 @rand() #4
  br label %10

10:                                               ; preds = %19, %1
  %.03 = phi i32 [ %9, %1 ], [ %17, %19 ]
  %.02 = phi i32 [ 0, %1 ], [ %18, %19 ]
  %11 = add nsw i32 %.03, 1
  %12 = sext i32 %.03 to i64
  %13 = mul nsw i64 %12, %3
  %14 = getelementptr inbounds i32, ptr %6, i64 %13
  %15 = sext i32 %.02 to i64
  %16 = getelementptr inbounds i32, ptr %14, i64 %15
  store i32 %11, ptr %16, align 4
  %17 = add nsw i32 %.03, 1
  %18 = add nsw i32 %.02, 1
  br label %19

19:                                               ; preds = %10
  %20 = mul nsw i32 10, 2
  %21 = icmp slt i32 %17, %20
  br i1 %21, label %10, label %22, !llvm.loop !27

22:                                               ; preds = %19
  br label %23

23:                                               ; preds = %32, %22
  %.01 = phi i32 [ 0, %22 ], [ %30, %32 ]
  %.0 = phi i32 [ 0, %22 ], [ %31, %32 ]
  %24 = sext i32 %.01 to i64
  %25 = mul nsw i64 %24, %3
  %26 = getelementptr inbounds i32, ptr %6, i64 %25
  %27 = sext i32 %.0 to i64
  %28 = getelementptr inbounds i32, ptr %26, i64 %27
  %29 = load i32, ptr %28, align 4
  %30 = add nsw i32 %.01, 1
  %31 = add nsw i32 %.0, 1
  br label %32

32:                                               ; preds = %23
  %33 = mul nsw i32 10, 2
  %34 = icmp slt i32 %18, %33
  br i1 %34, label %23, label %35, !llvm.loop !28

35:                                               ; preds = %32
  call void @llvm.stackrestore.p0(ptr %4)
  ret void
}

; Function Attrs: nounwind
declare i32 @rand() #3

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @matrix_IsND_SPECIAL_CASE_PARAMETER_INDEX(i32 noundef %0, i32 noundef %1) #0 {
  %3 = zext i32 10 to i64
  %4 = zext i32 10 to i64
  %5 = call ptr @llvm.stacksave.p0()
  %6 = mul nuw i64 %3, %4
  %7 = alloca i32, i64 %6, align 16
  %8 = mul nuw i64 %3, %4
  %9 = mul nuw i64 %8, 4
  call void @llvm.memset.p0.i64(ptr align 16 %7, i8 0, i64 %9, i1 false)
  br label %10

10:                                               ; preds = %19, %2
  %.03 = phi i32 [ 0, %2 ], [ %18, %19 ]
  %.0 = phi i32 [ %1, %2 ], [ %17, %19 ]
  %11 = add nsw i32 %.0, 1
  %12 = sext i32 %.0 to i64
  %13 = mul nsw i64 %12, %4
  %14 = getelementptr inbounds i32, ptr %7, i64 %13
  %15 = sext i32 %.03 to i64
  %16 = getelementptr inbounds i32, ptr %14, i64 %15
  store i32 %11, ptr %16, align 4
  %17 = add nsw i32 %.0, 1
  %18 = add nsw i32 %.03, 1
  br label %19

19:                                               ; preds = %10
  %20 = mul nsw i32 10, 2
  %21 = icmp slt i32 %17, %20
  br i1 %21, label %10, label %22, !llvm.loop !29

22:                                               ; preds = %19
  br label %23

23:                                               ; preds = %32, %22
  %.02 = phi i32 [ 0, %22 ], [ %30, %32 ]
  %.01 = phi i32 [ 0, %22 ], [ %31, %32 ]
  %24 = sext i32 %.02 to i64
  %25 = mul nsw i64 %24, %4
  %26 = getelementptr inbounds i32, ptr %7, i64 %25
  %27 = sext i32 %.01 to i64
  %28 = getelementptr inbounds i32, ptr %26, i64 %27
  %29 = load i32, ptr %28, align 4
  %30 = add nsw i32 %.02, 1
  %31 = add nsw i32 %.01, 1
  br label %32

32:                                               ; preds = %23
  %33 = mul nsw i32 10, 2
  %34 = icmp slt i32 %18, %33
  br i1 %34, label %23, label %35, !llvm.loop !30

35:                                               ; preds = %32
  call void @llvm.stackrestore.p0(ptr %5)
  ret void
}

attributes #0 = { noinline nounwind sspstrong uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nocallback nofree nosync nounwind willreturn }
attributes #2 = { nocallback nofree nounwind willreturn memory(argmem: write) }
attributes #3 = { nounwind "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #4 = { nounwind }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"clang version 21.1.6"}
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
!25 = distinct !{!25, !7}
!26 = distinct !{!26, !7}
!27 = distinct !{!27, !7}
!28 = distinct !{!28, !7}
!29 = distinct !{!29, !7}
!30 = distinct !{!30, !7}
