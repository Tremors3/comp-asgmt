; ModuleID = '/home/herry/GitHub/comp-asgmt/assignments/assignment-4/sources/lf-guard-tests.bc'
source_filename = "/home/herry/GitHub/comp-asgmt/assignments/assignment-4/sources/lf-guard-tests.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @first_NO_second_NO() #0 {
  %1 = call i32 @rand() #2
  %2 = call i32 @rand() #2
  br label %3

3:                                                ; preds = %6, %0
  %.0 = phi i32 [ %1, %0 ], [ %7, %6 ]
  %4 = call i32 @rand() #2
  %5 = icmp ne i32 %4, 0
  br i1 %5, label %6, label %8

6:                                                ; preds = %3
  %7 = add nsw i32 %2, 1
  br label %3, !llvm.loop !6

8:                                                ; preds = %3
  br label %9

9:                                                ; preds = %12, %8
  %.1 = phi i32 [ %.0, %8 ], [ %13, %12 ]
  %10 = call i32 @rand() #2
  %11 = icmp ne i32 %10, 0
  br i1 %11, label %12, label %14

12:                                               ; preds = %9
  %13 = add nsw i32 %2, 1
  br label %9, !llvm.loop !8

14:                                               ; preds = %9
  %15 = add nsw i32 %.1, 1
  ret void
}

; Function Attrs: nounwind
declare i32 @rand() #1

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @first_YES_second_NO() #0 {
  %1 = call i32 @rand() #2
  %2 = call i32 @rand() #2
  %3 = call i32 @rand() #2
  %4 = icmp ne i32 %3, 0
  br i1 %4, label %5, label %12

5:                                                ; preds = %0
  br label %6

6:                                                ; preds = %9, %5
  %.0 = phi i32 [ %1, %5 ], [ %10, %9 ]
  %7 = call i32 @rand() #2
  %8 = icmp ne i32 %7, 0
  br i1 %8, label %9, label %11

9:                                                ; preds = %6
  %10 = add nsw i32 %2, 1
  br label %6, !llvm.loop !9

11:                                               ; preds = %6
  br label %12

12:                                               ; preds = %11, %0
  %.1 = phi i32 [ %.0, %11 ], [ %1, %0 ]
  br label %13

13:                                               ; preds = %16, %12
  %.2 = phi i32 [ %.1, %12 ], [ %17, %16 ]
  %14 = call i32 @rand() #2
  %15 = icmp ne i32 %14, 0
  br i1 %15, label %16, label %18

16:                                               ; preds = %13
  %17 = add nsw i32 %2, 1
  br label %13, !llvm.loop !10

18:                                               ; preds = %13
  %19 = add nsw i32 %.2, 1
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @first_NO_second_YES() #0 {
  %1 = call i32 @rand() #2
  %2 = call i32 @rand() #2
  br label %3

3:                                                ; preds = %6, %0
  %.0 = phi i32 [ %1, %0 ], [ %7, %6 ]
  %4 = call i32 @rand() #2
  %5 = icmp ne i32 %4, 0
  br i1 %5, label %6, label %8

6:                                                ; preds = %3
  %7 = add nsw i32 %2, 1
  br label %3, !llvm.loop !11

8:                                                ; preds = %3
  %9 = call i32 @rand() #2
  %10 = icmp ne i32 %9, 0
  br i1 %10, label %11, label %18

11:                                               ; preds = %8
  br label %12

12:                                               ; preds = %15, %11
  %.1 = phi i32 [ %.0, %11 ], [ %16, %15 ]
  %13 = call i32 @rand() #2
  %14 = icmp ne i32 %13, 0
  br i1 %14, label %15, label %17

15:                                               ; preds = %12
  %16 = add nsw i32 %2, 1
  br label %12, !llvm.loop !12

17:                                               ; preds = %12
  br label %18

18:                                               ; preds = %17, %8
  %.2 = phi i32 [ %.1, %17 ], [ %.0, %8 ]
  %19 = add nsw i32 %.2, 1
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @first_YES_second_YES() #0 {
  %1 = call i32 @rand() #2
  %2 = call i32 @rand() #2
  %3 = call i32 @rand() #2
  %4 = icmp ne i32 %3, 0
  br i1 %4, label %5, label %12

5:                                                ; preds = %0
  br label %6

6:                                                ; preds = %9, %5
  %.0 = phi i32 [ %1, %5 ], [ %10, %9 ]
  %7 = call i32 @rand() #2
  %8 = icmp ne i32 %7, 0
  br i1 %8, label %9, label %11

9:                                                ; preds = %6
  %10 = add nsw i32 %2, 1
  br label %6, !llvm.loop !13

11:                                               ; preds = %6
  br label %12

12:                                               ; preds = %11, %0
  %.1 = phi i32 [ %.0, %11 ], [ %1, %0 ]
  %13 = call i32 @rand() #2
  %14 = icmp ne i32 %13, 0
  br i1 %14, label %15, label %22

15:                                               ; preds = %12
  br label %16

16:                                               ; preds = %19, %15
  %.2 = phi i32 [ %.1, %15 ], [ %20, %19 ]
  %17 = call i32 @rand() #2
  %18 = icmp ne i32 %17, 0
  br i1 %18, label %19, label %21

19:                                               ; preds = %16
  %20 = add nsw i32 %2, 1
  br label %16, !llvm.loop !14

21:                                               ; preds = %16
  br label %22

22:                                               ; preds = %21, %12
  %.3 = phi i32 [ %.2, %21 ], [ %.1, %12 ]
  %23 = add nsw i32 %.3, 1
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
