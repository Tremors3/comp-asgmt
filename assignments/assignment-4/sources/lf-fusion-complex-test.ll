; ModuleID = '/home/tremore/Programming/GitHub/comp-asgmt/assignments/assignment-4/sources/lf-fusion-test.bc'
source_filename = "/home/tremore/Programming/GitHub/comp-asgmt/assignments/assignment-4/sources/lf-fusion-test.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @NotGuarded_NoIf() #0 {
  %1 = zext i32 10 to i64
  %2 = call ptr @llvm.stacksave.p0()
  %3 = alloca i32, i64 %1, align 16
  %4 = mul nuw i64 %1, 4
  call void @llvm.memset.p0.i64(ptr align 16 %3, i8 0, i64 %4, i1 false)
  %5 = zext i32 10 to i64
  %6 = alloca i32, i64 %5, align 16
  %7 = mul nuw i64 %5, 4
  call void @llvm.memset.p0.i64(ptr align 16 %6, i8 0, i64 %7, i1 false)
  br label %8

8:                                                ; preds = %12, %0
  %.01 = phi i32 [ 0, %0 ], [ %11, %12 ]
  %9 = sext i32 %.01 to i64
  %10 = getelementptr inbounds i32, ptr %6, i64 %9
  store i32 %.01, ptr %10, align 4
  %11 = add nsw i32 %.01, 1
  br label %12

12:                                               ; preds = %8
  %13 = icmp slt i32 %11, 10
  br i1 %13, label %8, label %14, !llvm.loop !6

14:                                               ; preds = %12
  br label %15

15:                                               ; preds = %19, %14
  %.0 = phi i32 [ 0, %14 ], [ %18, %19 ]
  %16 = sext i32 %.0 to i64
  %17 = getelementptr inbounds i32, ptr %3, i64 %16
  store i32 %.0, ptr %17, align 4
  %18 = add nsw i32 %.0, 1
  br label %19

19:                                               ; preds = %15
  %20 = icmp slt i32 %18, 10
  br i1 %20, label %15, label %21, !llvm.loop !8

21:                                               ; preds = %19
  call void @llvm.stackrestore.p0(ptr %2)
  ret void
}

; Function Attrs: nocallback nofree nosync nounwind willreturn
declare ptr @llvm.stacksave.p0() #1

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i64(ptr nocapture writeonly, i8, i64, i1 immarg) #2

; Function Attrs: nocallback nofree nosync nounwind willreturn
declare void @llvm.stackrestore.p0(ptr) #1

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @NotGuarded_FirstHasIf() #0 {
  %1 = zext i32 10 to i64
  %2 = call ptr @llvm.stacksave.p0()
  %3 = alloca i32, i64 %1, align 16
  %4 = mul nuw i64 %1, 4
  call void @llvm.memset.p0.i64(ptr align 16 %3, i8 0, i64 %4, i1 false)
  %5 = zext i32 10 to i64
  %6 = alloca i32, i64 %5, align 16
  %7 = mul nuw i64 %5, 4
  call void @llvm.memset.p0.i64(ptr align 16 %6, i8 0, i64 %7, i1 false)
  br label %8

8:                                                ; preds = %17, %0
  %.01 = phi i32 [ 0, %0 ], [ %16, %17 ]
  %.0 = phi i32 [ 0, %0 ], [ %.1, %17 ]
  %9 = sext i32 %.01 to i64
  %10 = getelementptr inbounds i32, ptr %6, i64 %9
  store i32 %.01, ptr %10, align 4
  %11 = call i32 @rand() #4
  %12 = icmp ne i32 %11, 0
  br i1 %12, label %13, label %15

13:                                               ; preds = %8
  %14 = add nsw i32 %.0, 1
  br label %15

15:                                               ; preds = %13, %8
  %.1 = phi i32 [ %14, %13 ], [ %.0, %8 ]
  %16 = add nsw i32 %.01, 1
  br label %17

17:                                               ; preds = %15
  %18 = icmp slt i32 %16, 10
  br i1 %18, label %8, label %19, !llvm.loop !9

19:                                               ; preds = %17
  br label %20

20:                                               ; preds = %24, %19
  %.02 = phi i32 [ 0, %19 ], [ %23, %24 ]
  %21 = sext i32 %.02 to i64
  %22 = getelementptr inbounds i32, ptr %3, i64 %21
  store i32 %.02, ptr %22, align 4
  %23 = add nsw i32 %.02, 1
  br label %24

24:                                               ; preds = %20
  %25 = icmp slt i32 %23, 10
  br i1 %25, label %20, label %26, !llvm.loop !10

26:                                               ; preds = %24
  call void @llvm.stackrestore.p0(ptr %2)
  ret void
}

; Function Attrs: nounwind
declare i32 @rand() #3

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @NotGuarded_SecondHasIf() #0 {
  %1 = zext i32 10 to i64
  %2 = call ptr @llvm.stacksave.p0()
  %3 = alloca i32, i64 %1, align 16
  %4 = mul nuw i64 %1, 4
  call void @llvm.memset.p0.i64(ptr align 16 %3, i8 0, i64 %4, i1 false)
  %5 = zext i32 10 to i64
  %6 = alloca i32, i64 %5, align 16
  %7 = mul nuw i64 %5, 4
  call void @llvm.memset.p0.i64(ptr align 16 %6, i8 0, i64 %7, i1 false)
  br label %8

8:                                                ; preds = %12, %0
  %.01 = phi i32 [ 0, %0 ], [ %11, %12 ]
  %9 = sext i32 %.01 to i64
  %10 = getelementptr inbounds i32, ptr %6, i64 %9
  store i32 %.01, ptr %10, align 4
  %11 = add nsw i32 %.01, 1
  br label %12

12:                                               ; preds = %8
  %13 = icmp slt i32 %11, 10
  br i1 %13, label %8, label %14, !llvm.loop !11

14:                                               ; preds = %12
  br label %15

15:                                               ; preds = %24, %14
  %.02 = phi i32 [ 0, %14 ], [ %23, %24 ]
  %.0 = phi i32 [ 0, %14 ], [ %.1, %24 ]
  %16 = sext i32 %.02 to i64
  %17 = getelementptr inbounds i32, ptr %3, i64 %16
  store i32 %.02, ptr %17, align 4
  %18 = call i32 @rand() #4
  %19 = icmp ne i32 %18, 0
  br i1 %19, label %20, label %22

20:                                               ; preds = %15
  %21 = add nsw i32 %.0, 1
  br label %22

22:                                               ; preds = %20, %15
  %.1 = phi i32 [ %21, %20 ], [ %.0, %15 ]
  %23 = add nsw i32 %.02, 1
  br label %24

24:                                               ; preds = %22
  %25 = icmp slt i32 %23, 10
  br i1 %25, label %15, label %26, !llvm.loop !12

26:                                               ; preds = %24
  call void @llvm.stackrestore.p0(ptr %2)
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @NotGuarded_BothHaveIf() #0 {
  %1 = call i32 @rand() #4
  %2 = zext i32 10 to i64
  %3 = call ptr @llvm.stacksave.p0()
  %4 = alloca i32, i64 %2, align 16
  %5 = mul nuw i64 %2, 4
  call void @llvm.memset.p0.i64(ptr align 16 %4, i8 0, i64 %5, i1 false)
  %6 = zext i32 10 to i64
  %7 = alloca i32, i64 %6, align 16
  %8 = mul nuw i64 %6, 4
  call void @llvm.memset.p0.i64(ptr align 16 %7, i8 0, i64 %8, i1 false)
  br label %9

9:                                                ; preds = %17, %0
  %.03 = phi i32 [ 0, %0 ], [ %.14, %17 ]
  %.01 = phi i32 [ 0, %0 ], [ %16, %17 ]
  %10 = sext i32 %.01 to i64
  %11 = getelementptr inbounds i32, ptr %7, i64 %10
  store i32 %.01, ptr %11, align 4
  %12 = icmp ne i32 %1, 0
  br i1 %12, label %13, label %15

13:                                               ; preds = %9
  %14 = add nsw i32 %.03, 1
  br label %15

15:                                               ; preds = %13, %9
  %.14 = phi i32 [ %14, %13 ], [ %.03, %9 ]
  %16 = add nsw i32 %.01, 1
  br label %17

17:                                               ; preds = %15
  %18 = icmp slt i32 %16, 10
  br i1 %18, label %9, label %19, !llvm.loop !13

19:                                               ; preds = %17
  br label %20

20:                                               ; preds = %28, %19
  %.02 = phi i32 [ 0, %19 ], [ %27, %28 ]
  %.0 = phi i32 [ 0, %19 ], [ %.1, %28 ]
  %21 = sext i32 %.02 to i64
  %22 = getelementptr inbounds i32, ptr %4, i64 %21
  store i32 %.02, ptr %22, align 4
  %23 = icmp ne i32 %1, 0
  br i1 %23, label %24, label %26

24:                                               ; preds = %20
  %25 = add nsw i32 %.0, 1
  br label %26

26:                                               ; preds = %24, %20
  %.1 = phi i32 [ %25, %24 ], [ %.0, %20 ]
  %27 = add nsw i32 %.02, 1
  br label %28

28:                                               ; preds = %26
  %29 = icmp slt i32 %27, 10
  br i1 %29, label %20, label %30, !llvm.loop !14

30:                                               ; preds = %28
  call void @llvm.stackrestore.p0(ptr %3)
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @BothGuarded_NoIf() #0 {
  %1 = call i32 @rand() #4
  %2 = zext i32 10 to i64
  %3 = call ptr @llvm.stacksave.p0()
  %4 = alloca i32, i64 %2, align 16
  %5 = mul nuw i64 %2, 4
  call void @llvm.memset.p0.i64(ptr align 16 %4, i8 0, i64 %5, i1 false)
  %6 = zext i32 10 to i64
  %7 = alloca i32, i64 %6, align 16
  %8 = mul nuw i64 %6, 4
  call void @llvm.memset.p0.i64(ptr align 16 %7, i8 0, i64 %8, i1 false)
  %9 = icmp ne i32 %1, 0
  br i1 %9, label %10, label %18

10:                                               ; preds = %0
  br label %11

11:                                               ; preds = %15, %10
  %.01 = phi i32 [ 0, %10 ], [ %14, %15 ]
  %12 = sext i32 %.01 to i64
  %13 = getelementptr inbounds i32, ptr %7, i64 %12
  store i32 %.01, ptr %13, align 4
  %14 = add nsw i32 %.01, 1
  br label %15

15:                                               ; preds = %11
  %16 = icmp slt i32 %14, 10
  br i1 %16, label %11, label %17, !llvm.loop !15

17:                                               ; preds = %15
  br label %18

18:                                               ; preds = %17, %0
  %19 = icmp ne i32 %1, 0
  br i1 %19, label %20, label %28

20:                                               ; preds = %18
  br label %21

21:                                               ; preds = %25, %20
  %.0 = phi i32 [ 0, %20 ], [ %24, %25 ]
  %22 = sext i32 %.0 to i64
  %23 = getelementptr inbounds i32, ptr %4, i64 %22
  store i32 %.0, ptr %23, align 4
  %24 = add nsw i32 %.0, 1
  br label %25

25:                                               ; preds = %21
  %26 = icmp slt i32 %24, 10
  br i1 %26, label %21, label %27, !llvm.loop !16

27:                                               ; preds = %25
  br label %28

28:                                               ; preds = %27, %18
  call void @llvm.stackrestore.p0(ptr %3)
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @BothGuarded_FirstHasIf() #0 {
  %1 = call i32 @rand() #4
  %2 = zext i32 10 to i64
  %3 = call ptr @llvm.stacksave.p0()
  %4 = alloca i32, i64 %2, align 16
  %5 = mul nuw i64 %2, 4
  call void @llvm.memset.p0.i64(ptr align 16 %4, i8 0, i64 %5, i1 false)
  %6 = zext i32 10 to i64
  %7 = alloca i32, i64 %6, align 16
  %8 = mul nuw i64 %6, 4
  call void @llvm.memset.p0.i64(ptr align 16 %7, i8 0, i64 %8, i1 false)
  %9 = icmp ne i32 %1, 0
  br i1 %9, label %10, label %22

10:                                               ; preds = %0
  br label %11

11:                                               ; preds = %19, %10
  %.01 = phi i32 [ 0, %10 ], [ %18, %19 ]
  %.0 = phi i32 [ 0, %10 ], [ %.1, %19 ]
  %12 = sext i32 %.01 to i64
  %13 = getelementptr inbounds i32, ptr %7, i64 %12
  store i32 %.01, ptr %13, align 4
  %14 = icmp ne i32 %1, 0
  br i1 %14, label %15, label %17

15:                                               ; preds = %11
  %16 = add nsw i32 %.0, 1
  br label %17

17:                                               ; preds = %15, %11
  %.1 = phi i32 [ %16, %15 ], [ %.0, %11 ]
  %18 = add nsw i32 %.01, 1
  br label %19

19:                                               ; preds = %17
  %20 = icmp slt i32 %18, 10
  br i1 %20, label %11, label %21, !llvm.loop !17

21:                                               ; preds = %19
  br label %22

22:                                               ; preds = %21, %0
  %23 = icmp ne i32 %1, 0
  br i1 %23, label %24, label %32

24:                                               ; preds = %22
  br label %25

25:                                               ; preds = %29, %24
  %.02 = phi i32 [ 0, %24 ], [ %28, %29 ]
  %26 = sext i32 %.02 to i64
  %27 = getelementptr inbounds i32, ptr %4, i64 %26
  store i32 %.02, ptr %27, align 4
  %28 = add nsw i32 %.02, 1
  br label %29

29:                                               ; preds = %25
  %30 = icmp slt i32 %28, 10
  br i1 %30, label %25, label %31, !llvm.loop !18

31:                                               ; preds = %29
  br label %32

32:                                               ; preds = %31, %22
  call void @llvm.stackrestore.p0(ptr %3)
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @BothGuarded_SecondHasIf() #0 {
  %1 = call i32 @rand() #4
  %2 = zext i32 10 to i64
  %3 = call ptr @llvm.stacksave.p0()
  %4 = alloca i32, i64 %2, align 16
  %5 = mul nuw i64 %2, 4
  call void @llvm.memset.p0.i64(ptr align 16 %4, i8 0, i64 %5, i1 false)
  %6 = zext i32 10 to i64
  %7 = alloca i32, i64 %6, align 16
  %8 = mul nuw i64 %6, 4
  call void @llvm.memset.p0.i64(ptr align 16 %7, i8 0, i64 %8, i1 false)
  %9 = icmp ne i32 %1, 0
  br i1 %9, label %10, label %18

10:                                               ; preds = %0
  br label %11

11:                                               ; preds = %15, %10
  %.01 = phi i32 [ 0, %10 ], [ %14, %15 ]
  %12 = sext i32 %.01 to i64
  %13 = getelementptr inbounds i32, ptr %7, i64 %12
  store i32 %.01, ptr %13, align 4
  %14 = add nsw i32 %.01, 1
  br label %15

15:                                               ; preds = %11
  %16 = icmp slt i32 %14, 10
  br i1 %16, label %11, label %17, !llvm.loop !19

17:                                               ; preds = %15
  br label %18

18:                                               ; preds = %17, %0
  %19 = icmp ne i32 %1, 0
  br i1 %19, label %20, label %32

20:                                               ; preds = %18
  br label %21

21:                                               ; preds = %29, %20
  %.02 = phi i32 [ 0, %20 ], [ %28, %29 ]
  %.0 = phi i32 [ 0, %20 ], [ %.1, %29 ]
  %22 = sext i32 %.02 to i64
  %23 = getelementptr inbounds i32, ptr %4, i64 %22
  store i32 %.02, ptr %23, align 4
  %24 = icmp ne i32 %1, 0
  br i1 %24, label %25, label %27

25:                                               ; preds = %21
  %26 = add nsw i32 %.0, 1
  br label %27

27:                                               ; preds = %25, %21
  %.1 = phi i32 [ %26, %25 ], [ %.0, %21 ]
  %28 = add nsw i32 %.02, 1
  br label %29

29:                                               ; preds = %27
  %30 = icmp slt i32 %28, 10
  br i1 %30, label %21, label %31, !llvm.loop !20

31:                                               ; preds = %29
  br label %32

32:                                               ; preds = %31, %18
  call void @llvm.stackrestore.p0(ptr %3)
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @BothGuarded_BothHaveIf() #0 {
  %1 = call i32 @rand() #4
  %2 = zext i32 10 to i64
  %3 = call ptr @llvm.stacksave.p0()
  %4 = alloca i32, i64 %2, align 16
  %5 = mul nuw i64 %2, 4
  call void @llvm.memset.p0.i64(ptr align 16 %4, i8 0, i64 %5, i1 false)
  %6 = zext i32 10 to i64
  %7 = alloca i32, i64 %6, align 16
  %8 = mul nuw i64 %6, 4
  call void @llvm.memset.p0.i64(ptr align 16 %7, i8 0, i64 %8, i1 false)
  %9 = icmp ne i32 %1, 0
  br i1 %9, label %10, label %22

10:                                               ; preds = %0
  br label %11

11:                                               ; preds = %19, %10
  %.03 = phi i32 [ 0, %10 ], [ %.14, %19 ]
  %.01 = phi i32 [ 0, %10 ], [ %18, %19 ]
  %12 = sext i32 %.01 to i64
  %13 = getelementptr inbounds i32, ptr %7, i64 %12
  store i32 %.01, ptr %13, align 4
  %14 = icmp ne i32 %1, 0
  br i1 %14, label %15, label %17

15:                                               ; preds = %11
  %16 = add nsw i32 %.03, 1
  br label %17

17:                                               ; preds = %15, %11
  %.14 = phi i32 [ %16, %15 ], [ %.03, %11 ]
  %18 = add nsw i32 %.01, 1
  br label %19

19:                                               ; preds = %17
  %20 = icmp slt i32 %18, 10
  br i1 %20, label %11, label %21, !llvm.loop !21

21:                                               ; preds = %19
  br label %22

22:                                               ; preds = %21, %0
  %23 = icmp ne i32 %1, 0
  br i1 %23, label %24, label %36

24:                                               ; preds = %22
  br label %25

25:                                               ; preds = %33, %24
  %.02 = phi i32 [ 0, %24 ], [ %32, %33 ]
  %.0 = phi i32 [ 0, %24 ], [ %.1, %33 ]
  %26 = sext i32 %.02 to i64
  %27 = getelementptr inbounds i32, ptr %4, i64 %26
  store i32 %.02, ptr %27, align 4
  %28 = icmp ne i32 %1, 0
  br i1 %28, label %29, label %31

29:                                               ; preds = %25
  %30 = add nsw i32 %.0, 1
  br label %31

31:                                               ; preds = %29, %25
  %.1 = phi i32 [ %30, %29 ], [ %.0, %25 ]
  %32 = add nsw i32 %.02, 1
  br label %33

33:                                               ; preds = %31
  %34 = icmp slt i32 %32, 10
  br i1 %34, label %25, label %35, !llvm.loop !22

35:                                               ; preds = %33
  br label %36

36:                                               ; preds = %35, %22
  call void @llvm.stackrestore.p0(ptr %3)
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
