; ModuleID = '/home/tremore/Programming/GitHub/comp-asgmt/assignments/assignment-3/sources/licm-test.bc'
source_filename = "/home/tremore/Programming/GitHub/comp-asgmt/assignments/assignment-3/sources/licm-test.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @WHILE_controlla_se_la_phi_esprime_un_conflitto_di_interessi_fra_variabili() #0 {
  %1 = call i32 @rand() #2
  %2 = call i32 @rand() #2
  %3 = call i32 @rand() #2
  %4 = add nsw i32 %3, 1
  %5 = add nsw i32 %4, 1
  br label %6

6:                                                ; preds = %9, %0
  %.0 = phi i32 [ %4, %0 ], [ %11, %9 ]
  %7 = call i32 @rand() #2
  %8 = icmp ne i32 %7, 0
  br i1 %8, label %9, label %12

9:                                                ; preds = %6
  %10 = add nsw i32 %2, 1
  %11 = add nsw i32 %10, 1
  br label %6, !llvm.loop !6

12:                                               ; preds = %6
  %13 = add nsw i32 %.0, 1
  ret void
}

; Function Attrs: nounwind
declare i32 @rand() #1

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @WHILE_BREAK_controlla_se_la_phi_esprime_un_conflitto_di_interessi_fra_variabili() #0 {
  %1 = call i32 @rand() #2
  %2 = call i32 @rand() #2
  %3 = call i32 @rand() #2
  %4 = add nsw i32 %3, 1
  %5 = add nsw i32 %4, 1
  br label %6

6:                                                ; preds = %15, %0
  %.0 = phi i32 [ %4, %0 ], [ %11, %15 ]
  %7 = call i32 @rand() #2
  %8 = icmp ne i32 %7, 0
  br i1 %8, label %9, label %16

9:                                                ; preds = %6
  %10 = add nsw i32 %2, 1
  %11 = add nsw i32 %10, 1
  %12 = call i32 @rand() #2
  %13 = icmp ne i32 %12, 0
  br i1 %13, label %14, label %15

14:                                               ; preds = %9
  br label %16

15:                                               ; preds = %9
  br label %6, !llvm.loop !8

16:                                               ; preds = %14, %6
  %.1 = phi i32 [ %11, %14 ], [ %.0, %6 ]
  %17 = add nsw i32 %.1, 1
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @FOR_controlla_se_la_phi_esprime_un_conflitto_di_interessi_fra_variabili() #0 {
  %1 = call i32 @rand() #2
  %2 = call i32 @rand() #2
  %3 = call i32 @rand() #2
  %4 = add nsw i32 %3, 1
  %5 = add nsw i32 %4, 1
  br label %6

6:                                                ; preds = %11, %0
  %.01 = phi i32 [ 0, %0 ], [ %12, %11 ]
  %.0 = phi i32 [ %4, %0 ], [ %10, %11 ]
  %7 = icmp slt i32 %.01, 100
  br i1 %7, label %8, label %13

8:                                                ; preds = %6
  %9 = add nsw i32 %2, 1
  %10 = add nsw i32 %9, 1
  br label %11

11:                                               ; preds = %8
  %12 = add nsw i32 %.01, 1
  br label %6, !llvm.loop !9

13:                                               ; preds = %6
  %14 = add nsw i32 %.0, 1
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @MORTA_controlla_se_la_variabile_e_morta_fuori_dal_loop() #0 {
  %1 = call i32 @rand() #2
  %2 = call i32 @rand() #2
  br label %3

3:                                                ; preds = %6, %0
  %4 = call i32 @rand() #2
  %5 = icmp ne i32 %4, 0
  br i1 %5, label %6, label %8

6:                                                ; preds = %3
  %7 = add nsw i32 %2, 1
  br label %3, !llvm.loop !10

8:                                                ; preds = %3
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @VIVA_controlla_se_la_variabile_e_viva_fuori_dal_loop() #0 {
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
  %9 = add nsw i32 %.0, %.0
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @ciclo_infinito_WHILE() #0 {
  %1 = call i32 @rand() #2
  %2 = call i32 @rand() #2
  br label %3

3:                                                ; preds = %3, %0
  %4 = add nsw i32 %2, 1
  br label %3
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @ciclo_infinito_FOR() #0 {
  %1 = call i32 @rand() #2
  %2 = call i32 @rand() #2
  br label %3

3:                                                ; preds = %3, %0
  %4 = add nsw i32 %1, 1
  br label %3
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @variabile_definita_molteplici_volte_CON_COSTANTE() #0 {
  %1 = call i32 @rand() #2
  %2 = call i32 @rand() #2
  br label %3

3:                                                ; preds = %12, %0
  %.01 = phi i32 [ %2, %0 ], [ %13, %12 ]
  %.0 = phi i32 [ %1, %0 ], [ %.1, %12 ]
  %4 = call i32 @rand() #2
  %5 = icmp ne i32 %4, 0
  br i1 %5, label %6, label %14

6:                                                ; preds = %3
  %7 = call i32 @rand() #2
  %8 = icmp ne i32 %7, 0
  br i1 %8, label %9, label %10

9:                                                ; preds = %6
  br label %12

10:                                               ; preds = %6
  %11 = add nsw i32 %.01, 1
  br label %12

12:                                               ; preds = %10, %9
  %.1 = phi i32 [ 5, %9 ], [ %11, %10 ]
  %13 = add nsw i32 %.1, 1
  br label %3, !llvm.loop !12

14:                                               ; preds = %3
  %15 = add nsw i32 %.0, 1
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @variabile_definita_molteplici_volte_CON_VARIABILE() #0 {
  %1 = call i32 @rand() #2
  %2 = call i32 @rand() #2
  %3 = call i32 @rand() #2
  br label %4

4:                                                ; preds = %14, %0
  %.01 = phi i32 [ %2, %0 ], [ %15, %14 ]
  %.0 = phi i32 [ %1, %0 ], [ %.1, %14 ]
  %5 = call i32 @rand() #2
  %6 = icmp ne i32 %5, 0
  br i1 %6, label %7, label %16

7:                                                ; preds = %4
  %8 = call i32 @rand() #2
  %9 = icmp ne i32 %8, 0
  br i1 %9, label %10, label %12

10:                                               ; preds = %7
  %11 = sub nsw i32 %3, 1
  br label %14

12:                                               ; preds = %7
  %13 = add nsw i32 %.01, 1
  br label %14

14:                                               ; preds = %12, %10
  %.1 = phi i32 [ %11, %10 ], [ %13, %12 ]
  %15 = add nsw i32 %.1, 1
  br label %4, !llvm.loop !13

16:                                               ; preds = %4
  %17 = add nsw i32 %.0, 1
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @WHILE_annidato() #0 {
  %1 = call i32 @rand() #2
  %2 = call i32 @rand() #2
  %3 = call i32 @rand() #2
  %4 = add nsw i32 %3, 1
  %5 = add nsw i32 %4, 1
  br label %6

6:                                                ; preds = %16, %0
  %.0 = phi i32 [ %4, %0 ], [ %.1, %16 ]
  %7 = call i32 @rand() #2
  %8 = icmp ne i32 %7, 0
  br i1 %8, label %9, label %17

9:                                                ; preds = %6
  br label %10

10:                                               ; preds = %13, %9
  %.1 = phi i32 [ %.0, %9 ], [ %15, %13 ]
  %11 = call i32 @rand() #2
  %12 = icmp ne i32 %11, 0
  br i1 %12, label %13, label %16

13:                                               ; preds = %10
  %14 = add nsw i32 %2, 1
  %15 = add nsw i32 %14, 1
  br label %10, !llvm.loop !14

16:                                               ; preds = %10
  br label %6, !llvm.loop !15

17:                                               ; preds = %6
  %18 = add nsw i32 %.0, 1
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @WHILE_annidato_due_volte() #0 {
  %1 = call i32 @rand() #2
  %2 = call i32 @rand() #2
  %3 = call i32 @rand() #2
  %4 = call i32 @rand() #2
  %5 = call i32 @rand() #2
  %6 = add nsw i32 %4, 1
  %7 = add nsw i32 %6, 1
  %8 = add nsw i32 %5, 1
  %9 = add nsw i32 %8, 1
  br label %10

10:                                               ; preds = %27, %0
  %.01 = phi i32 [ %8, %0 ], [ %.12, %27 ]
  %.0 = phi i32 [ %6, %0 ], [ %.1, %27 ]
  %11 = call i32 @rand() #2
  %12 = icmp ne i32 %11, 0
  br i1 %12, label %13, label %28

13:                                               ; preds = %10
  br label %14

14:                                               ; preds = %17, %13
  %.1 = phi i32 [ %.0, %13 ], [ %19, %17 ]
  %15 = call i32 @rand() #2
  %16 = icmp ne i32 %15, 0
  br i1 %16, label %17, label %20

17:                                               ; preds = %14
  %18 = add nsw i32 %3, 1
  %19 = add nsw i32 %18, 1
  br label %14, !llvm.loop !16

20:                                               ; preds = %14
  br label %21

21:                                               ; preds = %24, %20
  %.12 = phi i32 [ %.01, %20 ], [ %26, %24 ]
  %22 = call i32 @rand() #2
  %23 = icmp ne i32 %22, 0
  br i1 %23, label %24, label %27

24:                                               ; preds = %21
  %25 = add nsw i32 %3, 1
  %26 = add nsw i32 %25, 1
  br label %21, !llvm.loop !17

27:                                               ; preds = %21
  br label %10, !llvm.loop !18

28:                                               ; preds = %10
  %29 = add nsw i32 %.0, 1
  %30 = add nsw i32 %.01, 1
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
