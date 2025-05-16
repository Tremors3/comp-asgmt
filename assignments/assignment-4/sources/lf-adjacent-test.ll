; ModuleID = '/home/tremore/Programming/GitHub/comp-asgmt/assignments/assignment-4/sources/lf-adjacent-test.bc'
source_filename = "/home/tremore/Programming/GitHub/comp-asgmt/assignments/assignment-4/sources/lf-adjacent-test.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @adjacent(i32 noundef %0) #0 {
  %2 = alloca [0 x i32], align 4
  %3 = alloca [0 x i32], align 4
  br label %4

4:                                                ; preds = %9, %1
  %.01 = phi i32 [ 0, %1 ], [ %10, %9 ]
  %5 = icmp slt i32 %.01, %0
  br i1 %5, label %6, label %11

6:                                                ; preds = %4
  %7 = sext i32 %.01 to i64
  %8 = getelementptr inbounds [0 x i32], ptr %3, i64 0, i64 %7
  store i32 %.01, ptr %8, align 4
  br label %9

9:                                                ; preds = %6
  %10 = add nsw i32 %.01, 1
  br label %4, !llvm.loop !6

11:                                               ; preds = %4
  br label %12

12:                                               ; preds = %17, %11
  %.0 = phi i32 [ 0, %11 ], [ %18, %17 ]
  %13 = icmp slt i32 %.0, %0
  br i1 %13, label %14, label %19

14:                                               ; preds = %12
  %15 = sext i32 %.0 to i64
  %16 = getelementptr inbounds [0 x i32], ptr %2, i64 0, i64 %15
  store i32 %.0, ptr %16, align 4
  br label %17

17:                                               ; preds = %14
  %18 = add nsw i32 %.0, 1
  br label %12, !llvm.loop !8

19:                                               ; preds = %12
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @adjacent_simplify_form(i32 noundef %0) #0 {
  %2 = alloca [0 x i32], align 4
  %3 = alloca [0 x i32], align 4
  br label %4

4:                                                ; preds = %8, %1
  %.01 = phi i32 [ 0, %1 ], [ %7, %8 ]
  %5 = sext i32 %.01 to i64
  %6 = getelementptr inbounds [0 x i32], ptr %3, i64 0, i64 %5
  store i32 %.01, ptr %6, align 4
  %7 = add nsw i32 %.01, 1
  br label %8

8:                                                ; preds = %4
  %9 = icmp slt i32 %7, %0
  br i1 %9, label %4, label %10, !llvm.loop !9

10:                                               ; preds = %8
  br label %11

11:                                               ; preds = %15, %10
  %.0 = phi i32 [ 0, %10 ], [ %14, %15 ]
  %12 = sext i32 %.0 to i64
  %13 = getelementptr inbounds [0 x i32], ptr %2, i64 0, i64 %12
  store i32 %.0, ptr %13, align 4
  %14 = add nsw i32 %.0, 1
  br label %15

15:                                               ; preds = %11
  %16 = icmp slt i32 %14, %0
  br i1 %16, label %11, label %17, !llvm.loop !10

17:                                               ; preds = %15
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @adjacent_guarded_simplify_form(i32 noundef %0) #0 {
  %2 = alloca [0 x i32], align 4
  %3 = alloca [0 x i32], align 4
  %4 = icmp sgt i32 %0, 0
  br i1 %4, label %5, label %13

5:                                                ; preds = %1
  br label %6

6:                                                ; preds = %10, %5
  %.01 = phi i32 [ 0, %5 ], [ %9, %10 ]
  %7 = sext i32 %.01 to i64
  %8 = getelementptr inbounds [0 x i32], ptr %3, i64 0, i64 %7
  store i32 %.01, ptr %8, align 4
  %9 = add nsw i32 %.01, 1
  br label %10

10:                                               ; preds = %6
  %11 = icmp slt i32 %9, %0
  br i1 %11, label %6, label %12, !llvm.loop !11

12:                                               ; preds = %10
  br label %13

13:                                               ; preds = %12, %1
  %14 = icmp sgt i32 %0, 0
  br i1 %14, label %15, label %23

15:                                               ; preds = %13
  br label %16

16:                                               ; preds = %20, %15
  %.0 = phi i32 [ 0, %15 ], [ %19, %20 ]
  %17 = sext i32 %.0 to i64
  %18 = getelementptr inbounds [0 x i32], ptr %2, i64 0, i64 %17
  store i32 %.0, ptr %18, align 4
  %19 = add nsw i32 %.0, 1
  br label %20

20:                                               ; preds = %16
  %21 = icmp slt i32 %19, %0
  br i1 %21, label %16, label %22, !llvm.loop !12

22:                                               ; preds = %20
  br label %23

23:                                               ; preds = %22, %13
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @adjacent_one_guarded_simplify_form(i32 noundef %0) #0 {
  %2 = alloca [0 x i32], align 4
  %3 = alloca [0 x i32], align 4
  %4 = icmp sgt i32 %0, 0
  br i1 %4, label %5, label %13

5:                                                ; preds = %1
  br label %6

6:                                                ; preds = %10, %5
  %.01 = phi i32 [ 0, %5 ], [ %9, %10 ]
  %7 = sext i32 %.01 to i64
  %8 = getelementptr inbounds [0 x i32], ptr %3, i64 0, i64 %7
  store i32 %.01, ptr %8, align 4
  %9 = add nsw i32 %.01, 1
  br label %10

10:                                               ; preds = %6
  %11 = icmp slt i32 %9, %0
  br i1 %11, label %6, label %12, !llvm.loop !13

12:                                               ; preds = %10
  br label %13

13:                                               ; preds = %12, %1
  br label %14

14:                                               ; preds = %18, %13
  %.0 = phi i32 [ 0, %13 ], [ %17, %18 ]
  %15 = sext i32 %.0 to i64
  %16 = getelementptr inbounds [0 x i32], ptr %2, i64 0, i64 %15
  store i32 %.0, ptr %16, align 4
  %17 = add nsw i32 %.0, 1
  br label %18

18:                                               ; preds = %14
  %19 = icmp slt i32 %17, %0
  br i1 %19, label %14, label %20, !llvm.loop !14

20:                                               ; preds = %18
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @non_adjacent_simplify_form(i32 noundef %0) #0 {
  %2 = alloca [0 x i32], align 4
  %3 = alloca [0 x i32], align 4
  br label %4

4:                                                ; preds = %8, %1
  %.01 = phi i32 [ 0, %1 ], [ %7, %8 ]
  %5 = sext i32 %.01 to i64
  %6 = getelementptr inbounds [0 x i32], ptr %3, i64 0, i64 %5
  store i32 %.01, ptr %6, align 4
  %7 = add nsw i32 %.01, 1
  br label %8

8:                                                ; preds = %4
  %9 = icmp slt i32 %7, %0
  br i1 %9, label %4, label %10, !llvm.loop !15

10:                                               ; preds = %8
  %11 = call i32 @rand() #2
  br label %12

12:                                               ; preds = %16, %10
  %.0 = phi i32 [ 0, %10 ], [ %15, %16 ]
  %13 = sext i32 %.0 to i64
  %14 = getelementptr inbounds [0 x i32], ptr %2, i64 0, i64 %13
  store i32 %.0, ptr %14, align 4
  %15 = add nsw i32 %.0, 1
  br label %16

16:                                               ; preds = %12
  %17 = icmp slt i32 %15, %0
  br i1 %17, label %12, label %18, !llvm.loop !16

18:                                               ; preds = %16
  ret void
}

; Function Attrs: nounwind
declare i32 @rand() #1

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @non_adjacent_guarded_simplify_form(i32 noundef %0) #0 {
  %2 = alloca [0 x i32], align 4
  %3 = alloca [0 x i32], align 4
  %4 = icmp sgt i32 %0, 0
  br i1 %4, label %5, label %13

5:                                                ; preds = %1
  br label %6

6:                                                ; preds = %10, %5
  %.01 = phi i32 [ 0, %5 ], [ %9, %10 ]
  %7 = sext i32 %.01 to i64
  %8 = getelementptr inbounds [0 x i32], ptr %3, i64 0, i64 %7
  store i32 %.01, ptr %8, align 4
  %9 = add nsw i32 %.01, 1
  br label %10

10:                                               ; preds = %6
  %11 = icmp slt i32 %9, %0
  br i1 %11, label %6, label %12, !llvm.loop !17

12:                                               ; preds = %10
  br label %13

13:                                               ; preds = %12, %1
  %14 = call i32 @rand() #2
  %15 = icmp sgt i32 %0, 0
  br i1 %15, label %16, label %24

16:                                               ; preds = %13
  br label %17

17:                                               ; preds = %21, %16
  %.0 = phi i32 [ 0, %16 ], [ %20, %21 ]
  %18 = sext i32 %.0 to i64
  %19 = getelementptr inbounds [0 x i32], ptr %2, i64 0, i64 %18
  store i32 %.0, ptr %19, align 4
  %20 = add nsw i32 %.0, 1
  br label %21

21:                                               ; preds = %17
  %22 = icmp slt i32 %20, %0
  br i1 %22, label %17, label %23, !llvm.loop !18

23:                                               ; preds = %21
  br label %24

24:                                               ; preds = %23, %13
  ret void
}

attributes #0 = { noinline nounwind sspstrong uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nounwind "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { nounwind }

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
