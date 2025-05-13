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

attributes #0 = { noinline nounwind sspstrong uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

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
