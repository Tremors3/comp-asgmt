; ModuleID = '/home/tremore/Programming/GitHub/comp-asgmt/assignments/assignment-3/sources/licm-test.bc'
source_filename = "/home/tremore/Programming/GitHub/comp-asgmt/assignments/assignment-3/sources/licm-test.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @EsempioSlide20PDF12() #0 {
  %1 = call i32 @rand() #2
  br label %2

2:                                                ; preds = %10, %0
  %3 = call i32 @rand() #2
  %4 = icmp ne i32 %3, 0
  br i1 %4, label %5, label %13

5:                                                ; preds = %2
  %6 = add nsw i32 2, %1
  %7 = icmp eq i32 10, %6
  br i1 %7, label %8, label %9

8:                                                ; preds = %5
  br label %10

9:                                                ; preds = %5
  br label %10

10:                                               ; preds = %9, %8
  %.0 = phi i32 [ 2, %8 ], [ 3, %9 ]
  %11 = add nsw i32 10, 1
  %12 = add nsw i32 %.0, 2
  br label %2, !llvm.loop !6

13:                                               ; preds = %2
  ret void
}

; Function Attrs: nounwind
declare i32 @rand() #1

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
