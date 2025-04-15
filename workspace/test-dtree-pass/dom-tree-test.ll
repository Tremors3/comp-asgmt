; ModuleID = '/home/tremore/Programming/GitHub/university-year-3/secondo-semestre/Compilatori/laboratori/laboratorio4/sources/FiatDucato.bc'
source_filename = "/home/tremore/Programming/GitHub/university-year-3/secondo-semestre/Compilatori/laboratori/laboratorio4/sources/FiatDucato.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i32 @main() #0 {
  %1 = call i32 @rand() #2
  %2 = icmp ne i32 %1, 0
  br i1 %2, label %3, label %5

3:                                                ; preds = %0
  %4 = add nsw i32 1, 1
  br label %14

5:                                                ; preds = %0
  %6 = add nsw i32 1, 1
  %7 = icmp ne i32 %1, 0
  br i1 %7, label %8, label %10

8:                                                ; preds = %5
  %9 = add nsw i32 %6, 1
  br label %12

10:                                               ; preds = %5
  %11 = add nsw i32 %6, 1
  br label %12

12:                                               ; preds = %10, %8
  %.0 = phi i32 [ %9, %8 ], [ %11, %10 ]
  %13 = add nsw i32 %.0, 1
  br label %14

14:                                               ; preds = %12, %3
  %.1 = phi i32 [ %4, %3 ], [ %13, %12 ]
  %15 = add nsw i32 %.1, 1
  ret i32 %15
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
