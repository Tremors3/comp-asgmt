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
  %.0 = phi i32 [ 0, %1 ], [ %10, %9 ]
  %5 = icmp slt i32 %.0, 10
  br i1 %5, label %6, label %11

6:                                                ; preds = %4
  %7 = sext i32 %.0 to i64
  %8 = getelementptr inbounds [0 x i32], ptr %3, i64 0, i64 %7
  store i32 %.0, ptr %8, align 4
  br label %9

9:                                                ; preds = %6
  %10 = add nsw i32 %.0, 1
  br label %4, !llvm.loop !6

11:                                               ; preds = %4
  br label %12

12:                                               ; preds = %17, %11
  %.01 = phi i32 [ 0, %11 ], [ %18, %17 ]
  %13 = icmp slt i32 %.01, 10
  br i1 %13, label %14, label %19

14:                                               ; preds = %12
  %15 = sext i32 %.01 to i64
  %16 = getelementptr inbounds [0 x i32], ptr %2, i64 0, i64 %15
  store i32 %.01, ptr %16, align 4
  br label %17

17:                                               ; preds = %14
  %18 = add nsw i32 %.01, 1
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
  %.0 = phi i32 [ 0, %1 ], [ %7, %8 ]
  %5 = sext i32 %.0 to i64
  %6 = getelementptr inbounds [0 x i32], ptr %3, i64 0, i64 %5
  store i32 %.0, ptr %6, align 4
  %7 = add nsw i32 %.0, 1
  br label %8

8:                                                ; preds = %4
  %9 = icmp slt i32 %7, 10
  br i1 %9, label %4, label %10, !llvm.loop !9

10:                                               ; preds = %8
  br label %11

11:                                               ; preds = %15, %10
  %.01 = phi i32 [ 0, %10 ], [ %14, %15 ]
  %12 = sext i32 %.01 to i64
  %13 = getelementptr inbounds [0 x i32], ptr %2, i64 0, i64 %12
  store i32 %.01, ptr %13, align 4
  %14 = add nsw i32 %.01, 1
  br label %15

15:                                               ; preds = %11
  %16 = icmp slt i32 %14, 10
  br i1 %16, label %11, label %17, !llvm.loop !10

17:                                               ; preds = %15
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @adjacent_guarded(i32 noundef %0) #0 {
  %2 = alloca [0 x i32], align 4
  %3 = alloca [0 x i32], align 4
  %4 = call i32 @rand() #2
  %5 = icmp ne i32 %4, 0
  br i1 %5, label %6, label %15

6:                                                ; preds = %1
  br label %7

7:                                                ; preds = %12, %6
  %.0 = phi i32 [ 0, %6 ], [ %13, %12 ]
  %8 = icmp slt i32 %.0, 10
  br i1 %8, label %9, label %14

9:                                                ; preds = %7
  %10 = sext i32 %.0 to i64
  %11 = getelementptr inbounds [0 x i32], ptr %3, i64 0, i64 %10
  store i32 %.0, ptr %11, align 4
  br label %12

12:                                               ; preds = %9
  %13 = add nsw i32 %.0, 1
  br label %7, !llvm.loop !11

14:                                               ; preds = %7
  br label %15

15:                                               ; preds = %14, %1
  %16 = call i32 @rand() #2
  %17 = icmp ne i32 %16, 0
  br i1 %17, label %18, label %27

18:                                               ; preds = %15
  br label %19

19:                                               ; preds = %24, %18
  %.01 = phi i32 [ 0, %18 ], [ %25, %24 ]
  %20 = icmp slt i32 %.01, 10
  br i1 %20, label %21, label %26

21:                                               ; preds = %19
  %22 = sext i32 %.01 to i64
  %23 = getelementptr inbounds [0 x i32], ptr %2, i64 0, i64 %22
  store i32 %.01, ptr %23, align 4
  br label %24

24:                                               ; preds = %21
  %25 = add nsw i32 %.01, 1
  br label %19, !llvm.loop !12

26:                                               ; preds = %19
  br label %27

27:                                               ; preds = %26, %15
  ret void
}

; Function Attrs: nounwind
declare i32 @rand() #1

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @adjacent_guarded_simplify_form(i32 noundef %0) #0 {
  %2 = alloca [0 x i32], align 4
  %3 = alloca [0 x i32], align 4
  %4 = icmp sgt i32 10, 0
  br i1 %4, label %5, label %13

5:                                                ; preds = %1
  br label %6

6:                                                ; preds = %10, %5
  %.0 = phi i32 [ 0, %5 ], [ %9, %10 ]
  %7 = sext i32 %.0 to i64
  %8 = getelementptr inbounds [0 x i32], ptr %3, i64 0, i64 %7
  store i32 %.0, ptr %8, align 4
  %9 = add nsw i32 %.0, 1
  br label %10

10:                                               ; preds = %6
  %11 = icmp slt i32 %9, 10
  br i1 %11, label %6, label %12, !llvm.loop !13

12:                                               ; preds = %10
  br label %13

13:                                               ; preds = %12, %1
  %14 = icmp sgt i32 10, 0
  br i1 %14, label %15, label %23

15:                                               ; preds = %13
  br label %16

16:                                               ; preds = %20, %15
  %.01 = phi i32 [ 0, %15 ], [ %19, %20 ]
  %17 = sext i32 %.01 to i64
  %18 = getelementptr inbounds [0 x i32], ptr %2, i64 0, i64 %17
  store i32 %.01, ptr %18, align 4
  %19 = add nsw i32 %.01, 1
  br label %20

20:                                               ; preds = %16
  %21 = icmp slt i32 %19, 10
  br i1 %21, label %16, label %22, !llvm.loop !14

22:                                               ; preds = %20
  br label %23

23:                                               ; preds = %22, %13
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @adjacent_one_guarded(i32 noundef %0) #0 {
  %2 = alloca [0 x i32], align 4
  %3 = alloca [0 x i32], align 4
  %4 = call i32 @rand() #2
  %5 = icmp ne i32 %4, 0
  br i1 %5, label %6, label %15

6:                                                ; preds = %1
  br label %7

7:                                                ; preds = %12, %6
  %.0 = phi i32 [ 0, %6 ], [ %13, %12 ]
  %8 = icmp slt i32 %.0, 10
  br i1 %8, label %9, label %14

9:                                                ; preds = %7
  %10 = sext i32 %.0 to i64
  %11 = getelementptr inbounds [0 x i32], ptr %3, i64 0, i64 %10
  store i32 %.0, ptr %11, align 4
  br label %12

12:                                               ; preds = %9
  %13 = add nsw i32 %.0, 1
  br label %7, !llvm.loop !15

14:                                               ; preds = %7
  br label %15

15:                                               ; preds = %14, %1
  br label %16

16:                                               ; preds = %21, %15
  %.01 = phi i32 [ 0, %15 ], [ %22, %21 ]
  %17 = icmp slt i32 %.01, 10
  br i1 %17, label %18, label %23

18:                                               ; preds = %16
  %19 = sext i32 %.01 to i64
  %20 = getelementptr inbounds [0 x i32], ptr %2, i64 0, i64 %19
  store i32 %.01, ptr %20, align 4
  br label %21

21:                                               ; preds = %18
  %22 = add nsw i32 %.01, 1
  br label %16, !llvm.loop !16

23:                                               ; preds = %16
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @adjacent_one_guarded_simplify_form(i32 noundef %0) #0 {
  %2 = alloca [0 x i32], align 4
  %3 = alloca [0 x i32], align 4
  %4 = icmp sgt i32 10, 0
  br i1 %4, label %5, label %13

5:                                                ; preds = %1
  br label %6

6:                                                ; preds = %10, %5
  %.0 = phi i32 [ 0, %5 ], [ %9, %10 ]
  %7 = sext i32 %.0 to i64
  %8 = getelementptr inbounds [0 x i32], ptr %3, i64 0, i64 %7
  store i32 %.0, ptr %8, align 4
  %9 = add nsw i32 %.0, 1
  br label %10

10:                                               ; preds = %6
  %11 = icmp slt i32 %9, 10
  br i1 %11, label %6, label %12, !llvm.loop !17

12:                                               ; preds = %10
  br label %13

13:                                               ; preds = %12, %1
  br label %14

14:                                               ; preds = %18, %13
  %.01 = phi i32 [ 0, %13 ], [ %17, %18 ]
  %15 = sext i32 %.01 to i64
  %16 = getelementptr inbounds [0 x i32], ptr %2, i64 0, i64 %15
  store i32 %.01, ptr %16, align 4
  %17 = add nsw i32 %.01, 1
  br label %18

18:                                               ; preds = %14
  %19 = icmp slt i32 %17, 10
  br i1 %19, label %14, label %20, !llvm.loop !18

20:                                               ; preds = %18
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @adjacent_one_guarded_reverse(i32 noundef %0) #0 {
  %2 = alloca [0 x i32], align 4
  %3 = alloca [0 x i32], align 4
  br label %4

4:                                                ; preds = %9, %1
  %.0 = phi i32 [ 0, %1 ], [ %10, %9 ]
  %5 = icmp slt i32 %.0, 10
  br i1 %5, label %6, label %11

6:                                                ; preds = %4
  %7 = sext i32 %.0 to i64
  %8 = getelementptr inbounds [0 x i32], ptr %3, i64 0, i64 %7
  store i32 %.0, ptr %8, align 4
  br label %9

9:                                                ; preds = %6
  %10 = add nsw i32 %.0, 1
  br label %4, !llvm.loop !19

11:                                               ; preds = %4
  %12 = call i32 @rand() #2
  %13 = icmp ne i32 %12, 0
  br i1 %13, label %14, label %23

14:                                               ; preds = %11
  br label %15

15:                                               ; preds = %20, %14
  %.01 = phi i32 [ 0, %14 ], [ %21, %20 ]
  %16 = icmp slt i32 %.01, 10
  br i1 %16, label %17, label %22

17:                                               ; preds = %15
  %18 = sext i32 %.01 to i64
  %19 = getelementptr inbounds [0 x i32], ptr %2, i64 0, i64 %18
  store i32 %.01, ptr %19, align 4
  br label %20

20:                                               ; preds = %17
  %21 = add nsw i32 %.01, 1
  br label %15, !llvm.loop !20

22:                                               ; preds = %15
  br label %23

23:                                               ; preds = %22, %11
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @adjacent_one_guarded_simplify_form_reverse(i32 noundef %0) #0 {
  %2 = alloca [0 x i32], align 4
  %3 = alloca [0 x i32], align 4
  br label %4

4:                                                ; preds = %8, %1
  %.0 = phi i32 [ 0, %1 ], [ %7, %8 ]
  %5 = sext i32 %.0 to i64
  %6 = getelementptr inbounds [0 x i32], ptr %3, i64 0, i64 %5
  store i32 %.0, ptr %6, align 4
  %7 = add nsw i32 %.0, 1
  br label %8

8:                                                ; preds = %4
  %9 = icmp slt i32 %7, 10
  br i1 %9, label %4, label %10, !llvm.loop !21

10:                                               ; preds = %8
  %11 = icmp sgt i32 10, 0
  br i1 %11, label %12, label %20

12:                                               ; preds = %10
  br label %13

13:                                               ; preds = %17, %12
  %.01 = phi i32 [ 0, %12 ], [ %16, %17 ]
  %14 = sext i32 %.01 to i64
  %15 = getelementptr inbounds [0 x i32], ptr %2, i64 0, i64 %14
  store i32 %.01, ptr %15, align 4
  %16 = add nsw i32 %.01, 1
  br label %17

17:                                               ; preds = %13
  %18 = icmp slt i32 %16, 10
  br i1 %18, label %13, label %19, !llvm.loop !22

19:                                               ; preds = %17
  br label %20

20:                                               ; preds = %19, %10
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @non_adjacent(i32 noundef %0) #0 {
  %2 = alloca [0 x i32], align 4
  %3 = alloca [0 x i32], align 4
  br label %4

4:                                                ; preds = %9, %1
  %.0 = phi i32 [ 0, %1 ], [ %10, %9 ]
  %5 = icmp slt i32 %.0, 10
  br i1 %5, label %6, label %11

6:                                                ; preds = %4
  %7 = sext i32 %.0 to i64
  %8 = getelementptr inbounds [0 x i32], ptr %3, i64 0, i64 %7
  store i32 %.0, ptr %8, align 4
  br label %9

9:                                                ; preds = %6
  %10 = add nsw i32 %.0, 1
  br label %4, !llvm.loop !23

11:                                               ; preds = %4
  %12 = call i32 @rand() #2
  br label %13

13:                                               ; preds = %18, %11
  %.01 = phi i32 [ 0, %11 ], [ %19, %18 ]
  %14 = icmp slt i32 %.01, 10
  br i1 %14, label %15, label %20

15:                                               ; preds = %13
  %16 = sext i32 %.01 to i64
  %17 = getelementptr inbounds [0 x i32], ptr %2, i64 0, i64 %16
  store i32 %.01, ptr %17, align 4
  br label %18

18:                                               ; preds = %15
  %19 = add nsw i32 %.01, 1
  br label %13, !llvm.loop !24

20:                                               ; preds = %13
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @non_adjacent_simplify_form(i32 noundef %0) #0 {
  %2 = alloca [0 x i32], align 4
  %3 = alloca [0 x i32], align 4
  br label %4

4:                                                ; preds = %8, %1
  %.0 = phi i32 [ 0, %1 ], [ %7, %8 ]
  %5 = sext i32 %.0 to i64
  %6 = getelementptr inbounds [0 x i32], ptr %3, i64 0, i64 %5
  store i32 %.0, ptr %6, align 4
  %7 = add nsw i32 %.0, 1
  br label %8

8:                                                ; preds = %4
  %9 = icmp slt i32 %7, 10
  br i1 %9, label %4, label %10, !llvm.loop !25

10:                                               ; preds = %8
  %11 = call i32 @rand() #2
  br label %12

12:                                               ; preds = %16, %10
  %.01 = phi i32 [ 0, %10 ], [ %15, %16 ]
  %13 = sext i32 %.01 to i64
  %14 = getelementptr inbounds [0 x i32], ptr %2, i64 0, i64 %13
  store i32 %.01, ptr %14, align 4
  %15 = add nsw i32 %.01, 1
  br label %16

16:                                               ; preds = %12
  %17 = icmp slt i32 %15, 10
  br i1 %17, label %12, label %18, !llvm.loop !26

18:                                               ; preds = %16
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @non_adjacent_guarded(i32 noundef %0) #0 {
  %2 = alloca [0 x i32], align 4
  %3 = alloca [0 x i32], align 4
  %4 = call i32 @rand() #2
  %5 = icmp ne i32 %4, 0
  br i1 %5, label %6, label %15

6:                                                ; preds = %1
  br label %7

7:                                                ; preds = %12, %6
  %.0 = phi i32 [ 0, %6 ], [ %13, %12 ]
  %8 = icmp slt i32 %.0, 10
  br i1 %8, label %9, label %14

9:                                                ; preds = %7
  %10 = sext i32 %.0 to i64
  %11 = getelementptr inbounds [0 x i32], ptr %3, i64 0, i64 %10
  store i32 %.0, ptr %11, align 4
  br label %12

12:                                               ; preds = %9
  %13 = add nsw i32 %.0, 1
  br label %7, !llvm.loop !27

14:                                               ; preds = %7
  br label %15

15:                                               ; preds = %14, %1
  %16 = call i32 @rand() #2
  %17 = call i32 @rand() #2
  %18 = icmp ne i32 %17, 0
  br i1 %18, label %19, label %28

19:                                               ; preds = %15
  br label %20

20:                                               ; preds = %25, %19
  %.01 = phi i32 [ 0, %19 ], [ %26, %25 ]
  %21 = icmp slt i32 %.01, 10
  br i1 %21, label %22, label %27

22:                                               ; preds = %20
  %23 = sext i32 %.01 to i64
  %24 = getelementptr inbounds [0 x i32], ptr %2, i64 0, i64 %23
  store i32 %.01, ptr %24, align 4
  br label %25

25:                                               ; preds = %22
  %26 = add nsw i32 %.01, 1
  br label %20, !llvm.loop !28

27:                                               ; preds = %20
  br label %28

28:                                               ; preds = %27, %15
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @non_adjacent_guarded_simplify_form(i32 noundef %0) #0 {
  %2 = alloca [0 x i32], align 4
  %3 = alloca [0 x i32], align 4
  %4 = icmp sgt i32 10, 0
  br i1 %4, label %5, label %13

5:                                                ; preds = %1
  br label %6

6:                                                ; preds = %10, %5
  %.0 = phi i32 [ 0, %5 ], [ %9, %10 ]
  %7 = sext i32 %.0 to i64
  %8 = getelementptr inbounds [0 x i32], ptr %3, i64 0, i64 %7
  store i32 %.0, ptr %8, align 4
  %9 = add nsw i32 %.0, 1
  br label %10

10:                                               ; preds = %6
  %11 = icmp slt i32 %9, 10
  br i1 %11, label %6, label %12, !llvm.loop !29

12:                                               ; preds = %10
  br label %13

13:                                               ; preds = %12, %1
  %14 = call i32 @rand() #2
  %15 = icmp sgt i32 10, 0
  br i1 %15, label %16, label %24

16:                                               ; preds = %13
  br label %17

17:                                               ; preds = %21, %16
  %.01 = phi i32 [ 0, %16 ], [ %20, %21 ]
  %18 = sext i32 %.01 to i64
  %19 = getelementptr inbounds [0 x i32], ptr %2, i64 0, i64 %18
  store i32 %.01, ptr %19, align 4
  %20 = add nsw i32 %.01, 1
  br label %21

21:                                               ; preds = %17
  %22 = icmp slt i32 %20, 10
  br i1 %22, label %17, label %23, !llvm.loop !30

23:                                               ; preds = %21
  br label %24

24:                                               ; preds = %23, %13
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @conditional_block_between_exit_and_guard(i32 noundef %0) #0 {
  %2 = alloca [0 x i32], align 4
  %3 = alloca [0 x i32], align 4
  %4 = icmp sgt i32 10, 0
  br i1 %4, label %5, label %14

5:                                                ; preds = %1
  br label %6

6:                                                ; preds = %11, %5
  %.0 = phi i32 [ 0, %5 ], [ %12, %11 ]
  %7 = icmp slt i32 %.0, 10
  br i1 %7, label %8, label %13

8:                                                ; preds = %6
  %9 = sext i32 %.0 to i64
  %10 = getelementptr inbounds [0 x i32], ptr %3, i64 0, i64 %9
  store i32 %.0, ptr %10, align 4
  br label %11

11:                                               ; preds = %8
  %12 = add nsw i32 %.0, 1
  br label %6, !llvm.loop !31

13:                                               ; preds = %6
  br label %14

14:                                               ; preds = %13, %1
  %15 = icmp slt i32 10, 0
  br i1 %15, label %16, label %25

16:                                               ; preds = %14
  br label %17

17:                                               ; preds = %22, %16
  %.01 = phi i32 [ 0, %16 ], [ %23, %22 ]
  %18 = icmp slt i32 %.01, 10
  br i1 %18, label %19, label %24

19:                                               ; preds = %17
  %20 = sext i32 %.01 to i64
  %21 = getelementptr inbounds [0 x i32], ptr %2, i64 0, i64 %20
  store i32 %.01, ptr %21, align 4
  br label %22

22:                                               ; preds = %19
  %23 = add nsw i32 %.01, 1
  br label %17, !llvm.loop !32

24:                                               ; preds = %17
  br label %25

25:                                               ; preds = %24, %14
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @adjacent_differently_guarded_simplify_form(i32 noundef %0) #0 {
  %2 = alloca [0 x i32], align 4
  %3 = alloca [0 x i32], align 4
  %4 = icmp sgt i32 10, 0
  br i1 %4, label %5, label %13

5:                                                ; preds = %1
  br label %6

6:                                                ; preds = %10, %5
  %.0 = phi i32 [ 0, %5 ], [ %9, %10 ]
  %7 = sext i32 %.0 to i64
  %8 = getelementptr inbounds [0 x i32], ptr %3, i64 0, i64 %7
  store i32 %.0, ptr %8, align 4
  %9 = add nsw i32 %.0, 1
  br label %10

10:                                               ; preds = %6
  %11 = icmp slt i32 %9, 10
  br i1 %11, label %6, label %12, !llvm.loop !33

12:                                               ; preds = %10
  br label %13

13:                                               ; preds = %12, %1
  %14 = icmp slt i32 10, 0
  br i1 %14, label %15, label %23

15:                                               ; preds = %13
  br label %16

16:                                               ; preds = %20, %15
  %.01 = phi i32 [ 0, %15 ], [ %19, %20 ]
  %17 = sext i32 %.01 to i64
  %18 = getelementptr inbounds [0 x i32], ptr %2, i64 0, i64 %17
  store i32 %.01, ptr %18, align 4
  %19 = add nsw i32 %.01, 1
  br label %20

20:                                               ; preds = %16
  %21 = icmp slt i32 %19, 10
  br i1 %21, label %16, label %22, !llvm.loop !34

22:                                               ; preds = %20
  br label %23

23:                                               ; preds = %22, %13
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @adjacent_global_guard(i32 noundef %0) #0 {
  %2 = alloca [0 x i32], align 4
  %3 = alloca [0 x i32], align 4
  %4 = icmp sgt i32 10, 0
  br i1 %4, label %5, label %20

5:                                                ; preds = %1
  br label %6

6:                                                ; preds = %10, %5
  %.0 = phi i32 [ 0, %5 ], [ %9, %10 ]
  %7 = sext i32 %.0 to i64
  %8 = getelementptr inbounds [0 x i32], ptr %3, i64 0, i64 %7
  store i32 %.0, ptr %8, align 4
  %9 = add nsw i32 %.0, 1
  br label %10

10:                                               ; preds = %6
  %11 = icmp slt i32 %9, 10
  br i1 %11, label %6, label %12, !llvm.loop !35

12:                                               ; preds = %10
  br label %13

13:                                               ; preds = %17, %12
  %.01 = phi i32 [ 0, %12 ], [ %16, %17 ]
  %14 = sext i32 %.01 to i64
  %15 = getelementptr inbounds [0 x i32], ptr %2, i64 0, i64 %14
  store i32 %.01, ptr %15, align 4
  %16 = add nsw i32 %.01, 1
  br label %17

17:                                               ; preds = %13
  %18 = icmp slt i32 %16, 10
  br i1 %18, label %13, label %19, !llvm.loop !36

19:                                               ; preds = %17
  br label %20

20:                                               ; preds = %19, %1
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @adjacent_nested_guard(i32 noundef %0) #0 {
  %2 = alloca [0 x i32], align 4
  %3 = alloca [0 x i32], align 4
  %4 = icmp sgt i32 10, 0
  br i1 %4, label %5, label %23

5:                                                ; preds = %1
  br label %6

6:                                                ; preds = %10, %5
  %.0 = phi i32 [ 0, %5 ], [ %9, %10 ]
  %7 = sext i32 %.0 to i64
  %8 = getelementptr inbounds [0 x i32], ptr %3, i64 0, i64 %7
  store i32 %.0, ptr %8, align 4
  %9 = add nsw i32 %.0, 1
  br label %10

10:                                               ; preds = %6
  %11 = icmp slt i32 %9, 10
  br i1 %11, label %6, label %12, !llvm.loop !37

12:                                               ; preds = %10
  %13 = icmp sgt i32 10, 0
  br i1 %13, label %14, label %22

14:                                               ; preds = %12
  br label %15

15:                                               ; preds = %19, %14
  %.01 = phi i32 [ 0, %14 ], [ %18, %19 ]
  %16 = sext i32 %.01 to i64
  %17 = getelementptr inbounds [0 x i32], ptr %2, i64 0, i64 %16
  store i32 %.01, ptr %17, align 4
  %18 = add nsw i32 %.01, 1
  br label %19

19:                                               ; preds = %15
  %20 = icmp slt i32 %18, 10
  br i1 %20, label %15, label %21, !llvm.loop !38

21:                                               ; preds = %19
  br label %22

22:                                               ; preds = %21, %12
  br label %23

23:                                               ; preds = %22, %1
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @adjacent_one_nested_guard(i32 noundef %0) #0 {
  %2 = alloca [0 x i32], align 4
  %3 = alloca [0 x i32], align 4
  %4 = icmp eq i32 10, 0
  br i1 %4, label %5, label %13

5:                                                ; preds = %1
  br label %6

6:                                                ; preds = %10, %5
  %.0 = phi i32 [ 0, %5 ], [ %9, %10 ]
  %7 = sext i32 %.0 to i64
  %8 = getelementptr inbounds [0 x i32], ptr %3, i64 0, i64 %7
  store i32 %.0, ptr %8, align 4
  %9 = add nsw i32 %.0, 1
  br label %10

10:                                               ; preds = %6
  %11 = icmp slt i32 %9, 10
  br i1 %11, label %6, label %12, !llvm.loop !39

12:                                               ; preds = %10
  br label %13

13:                                               ; preds = %12, %1
  %14 = icmp eq i32 10, 0
  br i1 %14, label %15, label %26

15:                                               ; preds = %13
  %16 = icmp eq i32 10, 0
  br i1 %16, label %17, label %25

17:                                               ; preds = %15
  br label %18

18:                                               ; preds = %22, %17
  %.01 = phi i32 [ 0, %17 ], [ %21, %22 ]
  %19 = sext i32 %.01 to i64
  %20 = getelementptr inbounds [0 x i32], ptr %2, i64 0, i64 %19
  store i32 %.01, ptr %20, align 4
  %21 = add nsw i32 %.01, 1
  br label %22

22:                                               ; preds = %18
  %23 = icmp slt i32 %21, 10
  br i1 %23, label %18, label %24, !llvm.loop !40

24:                                               ; preds = %22
  br label %25

25:                                               ; preds = %24, %15
  br label %26

26:                                               ; preds = %25, %13
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @different_trip_count(i32 noundef %0) #0 {
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
  %9 = icmp slt i32 %7, 10
  br i1 %9, label %4, label %10, !llvm.loop !41

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
  %16 = icmp slt i32 %14, 11
  br i1 %16, label %11, label %17, !llvm.loop !42

17:                                               ; preds = %15
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @dependency_simplify_form(i32 noundef %0) #0 {
  %2 = alloca [0 x i32], align 4
  %3 = alloca [0 x i32], align 4
  br label %4

4:                                                ; preds = %9, %1
  %.0 = phi i32 [ 0, %1 ], [ %8, %9 ]
  %5 = add nsw i32 %.0, %.0
  %6 = sext i32 %.0 to i64
  %7 = getelementptr inbounds [0 x i32], ptr %3, i64 0, i64 %6
  store i32 %5, ptr %7, align 4
  %8 = add nsw i32 %.0, 1
  br label %9

9:                                                ; preds = %4
  %10 = icmp slt i32 %8, 10
  br i1 %10, label %4, label %11, !llvm.loop !43

11:                                               ; preds = %9
  br label %12

12:                                               ; preds = %21, %11
  %.01 = phi i32 [ 0, %11 ], [ %20, %21 ]
  %13 = add nsw i32 %.01, 3
  %14 = sext i32 %13 to i64
  %15 = getelementptr inbounds [0 x i32], ptr %3, i64 0, i64 %14
  %16 = load i32, ptr %15, align 4
  %17 = add nsw i32 %16, %.01
  %18 = sext i32 %.01 to i64
  %19 = getelementptr inbounds [0 x i32], ptr %2, i64 0, i64 %18
  store i32 %17, ptr %19, align 4
  %20 = add nsw i32 %.01, 1
  br label %21

21:                                               ; preds = %12
  %22 = icmp slt i32 %20, 10
  br i1 %22, label %12, label %23, !llvm.loop !44

23:                                               ; preds = %21
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @dependency(i32 noundef %0) #0 {
  %2 = alloca [0 x i32], align 4
  %3 = alloca [0 x i32], align 4
  br label %4

4:                                                ; preds = %10, %1
  %.0 = phi i32 [ 0, %1 ], [ %11, %10 ]
  %5 = icmp slt i32 %.0, 10
  br i1 %5, label %6, label %12

6:                                                ; preds = %4
  %7 = add nsw i32 %.0, %.0
  %8 = sext i32 %.0 to i64
  %9 = getelementptr inbounds [0 x i32], ptr %3, i64 0, i64 %8
  store i32 %7, ptr %9, align 4
  br label %10

10:                                               ; preds = %6
  %11 = add nsw i32 %.0, 1
  br label %4, !llvm.loop !45

12:                                               ; preds = %4
  br label %13

13:                                               ; preds = %23, %12
  %.1 = phi i32 [ 0, %12 ], [ %24, %23 ]
  %14 = icmp slt i32 %.1, 10
  br i1 %14, label %15, label %25

15:                                               ; preds = %13
  %16 = add nsw i32 %.1, 3
  %17 = sext i32 %16 to i64
  %18 = getelementptr inbounds [0 x i32], ptr %3, i64 0, i64 %17
  %19 = load i32, ptr %18, align 4
  %20 = add nsw i32 %19, %.1
  %21 = sext i32 %.1 to i64
  %22 = getelementptr inbounds [0 x i32], ptr %2, i64 0, i64 %21
  store i32 %20, ptr %22, align 4
  br label %23

23:                                               ; preds = %15
  %24 = add nsw i32 %.1, 1
  br label %13, !llvm.loop !46

25:                                               ; preds = %13
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @nested_dependency(i32 noundef %0) #0 {
  %2 = alloca [0 x i32], align 4
  %3 = alloca [0 x i32], align 4
  %4 = alloca [0 x i32], align 4
  br label %5

5:                                                ; preds = %16, %1
  %.01 = phi i32 [ 0, %1 ], [ %15, %16 ]
  %.0 = phi i32 [ 0, %1 ], [ %11, %16 ]
  %6 = sext i32 %.01 to i64
  %7 = getelementptr inbounds [0 x i32], ptr %3, i64 0, i64 %6
  store i32 %.01, ptr %7, align 4
  br label %8

8:                                                ; preds = %12, %5
  %.1 = phi i32 [ %.0, %5 ], [ %11, %12 ]
  %9 = sext i32 %.1 to i64
  %10 = getelementptr inbounds [0 x i32], ptr %4, i64 0, i64 %9
  store i32 %.1, ptr %10, align 4
  %11 = add nsw i32 %.1, 1
  br label %12

12:                                               ; preds = %8
  %13 = icmp slt i32 %11, 10
  br i1 %13, label %8, label %14, !llvm.loop !47

14:                                               ; preds = %12
  %15 = add nsw i32 %.01, 1
  br label %16

16:                                               ; preds = %14
  %17 = icmp slt i32 %15, 10
  br i1 %17, label %5, label %18, !llvm.loop !48

18:                                               ; preds = %16
  br label %19

19:                                               ; preds = %23, %18
  %.02 = phi i32 [ 0, %18 ], [ %22, %23 ]
  %20 = sext i32 %.02 to i64
  %21 = getelementptr inbounds [0 x i32], ptr %2, i64 0, i64 %20
  store i32 %.02, ptr %21, align 4
  %22 = add nsw i32 %.02, 1
  br label %23

23:                                               ; preds = %19
  %24 = icmp slt i32 %22, 10
  br i1 %24, label %19, label %25, !llvm.loop !49

25:                                               ; preds = %23
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @if_body_test(i32 noundef %0) #0 {
  %2 = alloca [0 x i32], align 4
  br label %3

3:                                                ; preds = %17, %1
  %.0 = phi i32 [ 0, %1 ], [ %16, %17 ]
  %4 = sext i32 %.0 to i64
  %5 = getelementptr inbounds [0 x i32], ptr %2, i64 0, i64 %4
  store i32 0, ptr %5, align 4
  %6 = call i32 @rand() #2
  %7 = icmp ne i32 %6, 0
  br i1 %7, label %8, label %11

8:                                                ; preds = %3
  %9 = sext i32 %.0 to i64
  %10 = getelementptr inbounds [0 x i32], ptr %2, i64 0, i64 %9
  store i32 %.0, ptr %10, align 4
  br label %15

11:                                               ; preds = %3
  %12 = sub nsw i32 0, %.0
  %13 = sext i32 %.0 to i64
  %14 = getelementptr inbounds [0 x i32], ptr %2, i64 0, i64 %13
  store i32 %12, ptr %14, align 4
  br label %15

15:                                               ; preds = %11, %8
  %16 = add nsw i32 %.0, 1
  br label %17

17:                                               ; preds = %15
  %18 = icmp slt i32 %16, 10
  br i1 %18, label %3, label %19, !llvm.loop !50

19:                                               ; preds = %17
  br label %20

20:                                               ; preds = %34, %19
  %.1 = phi i32 [ %16, %19 ], [ %33, %34 ]
  %21 = sext i32 %.1 to i64
  %22 = getelementptr inbounds [0 x i32], ptr %2, i64 0, i64 %21
  store i32 0, ptr %22, align 4
  %23 = call i32 @rand() #2
  %24 = icmp ne i32 %23, 0
  br i1 %24, label %25, label %28

25:                                               ; preds = %20
  %26 = sext i32 %.1 to i64
  %27 = getelementptr inbounds [0 x i32], ptr %2, i64 0, i64 %26
  store i32 %.1, ptr %27, align 4
  br label %32

28:                                               ; preds = %20
  %29 = sub nsw i32 0, %.1
  %30 = sext i32 %.1 to i64
  %31 = getelementptr inbounds [0 x i32], ptr %2, i64 0, i64 %30
  store i32 %29, ptr %31, align 4
  br label %32

32:                                               ; preds = %28, %25
  %33 = add nsw i32 %.1, 1
  br label %34

34:                                               ; preds = %32
  %35 = icmp slt i32 %33, 10
  br i1 %35, label %20, label %36, !llvm.loop !51

36:                                               ; preds = %34
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @fusion_candidate_both_different_body_and_latch(i32 noundef %0) #0 {
  %2 = alloca [0 x i32], align 4
  %3 = alloca [0 x i32], align 4
  br label %4

4:                                                ; preds = %11, %1
  %.0 = phi i32 [ 0, %1 ], [ %10, %11 ]
  %5 = sext i32 %.0 to i64
  %6 = getelementptr inbounds [0 x i32], ptr %3, i64 0, i64 %5
  store i32 %.0, ptr %6, align 4
  %7 = icmp ne i32 10, 0
  br i1 %7, label %8, label %9

8:                                                ; preds = %4
  br label %9

9:                                                ; preds = %8, %4
  %10 = add nsw i32 %.0, 1
  br label %11

11:                                               ; preds = %9
  %12 = icmp slt i32 %10, 10
  br i1 %12, label %4, label %13, !llvm.loop !52

13:                                               ; preds = %11
  br label %14

14:                                               ; preds = %21, %13
  %.01 = phi i32 [ 0, %13 ], [ %20, %21 ]
  %15 = sext i32 %.01 to i64
  %16 = getelementptr inbounds [0 x i32], ptr %2, i64 0, i64 %15
  store i32 %.01, ptr %16, align 4
  %17 = icmp ne i32 10, 0
  br i1 %17, label %18, label %19

18:                                               ; preds = %14
  br label %19

19:                                               ; preds = %18, %14
  %20 = add nsw i32 %.01, 1
  br label %21

21:                                               ; preds = %19
  %22 = icmp slt i32 %20, 10
  br i1 %22, label %14, label %23, !llvm.loop !53

23:                                               ; preds = %21
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @fusion_candidate_single_same_body_and_latch(i32 noundef %0) #0 {
  %2 = alloca [0 x i32], align 4
  %3 = alloca [0 x i32], align 4
  br label %4

4:                                                ; preds = %8, %1
  %.0 = phi i32 [ 0, %1 ], [ %7, %8 ]
  %5 = sext i32 %.0 to i64
  %6 = getelementptr inbounds [0 x i32], ptr %3, i64 0, i64 %5
  store i32 %.0, ptr %6, align 4
  %7 = add nsw i32 %.0, 1
  br label %8

8:                                                ; preds = %4
  %9 = icmp slt i32 %7, 10
  br i1 %9, label %4, label %10, !llvm.loop !54

10:                                               ; preds = %8
  br label %11

11:                                               ; preds = %18, %10
  %.01 = phi i32 [ 0, %10 ], [ %17, %18 ]
  %12 = sext i32 %.01 to i64
  %13 = getelementptr inbounds [0 x i32], ptr %2, i64 0, i64 %12
  store i32 %.01, ptr %13, align 4
  %14 = icmp ne i32 10, 0
  br i1 %14, label %15, label %16

15:                                               ; preds = %11
  br label %16

16:                                               ; preds = %15, %11
  %17 = add nsw i32 %.01, 1
  br label %18

18:                                               ; preds = %16
  %19 = icmp slt i32 %17, 10
  br i1 %19, label %11, label %20, !llvm.loop !55

20:                                               ; preds = %18
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @fusion_candidate_both_same_body_and_latch(i32 noundef %0) #0 {
  %2 = alloca [0 x i32], align 4
  %3 = alloca [0 x i32], align 4
  br label %4

4:                                                ; preds = %8, %1
  %.0 = phi i32 [ 0, %1 ], [ %7, %8 ]
  %5 = sext i32 %.0 to i64
  %6 = getelementptr inbounds [0 x i32], ptr %3, i64 0, i64 %5
  store i32 %.0, ptr %6, align 4
  %7 = add nsw i32 %.0, 1
  br label %8

8:                                                ; preds = %4
  %9 = icmp slt i32 %7, 10
  br i1 %9, label %4, label %10, !llvm.loop !56

10:                                               ; preds = %8
  br label %11

11:                                               ; preds = %15, %10
  %.01 = phi i32 [ 0, %10 ], [ %14, %15 ]
  %12 = sext i32 %.01 to i64
  %13 = getelementptr inbounds [0 x i32], ptr %2, i64 0, i64 %12
  store i32 %.01, ptr %13, align 4
  %14 = add nsw i32 %.01, 1
  br label %15

15:                                               ; preds = %11
  %16 = icmp slt i32 %14, 10
  br i1 %16, label %11, label %17, !llvm.loop !57

17:                                               ; preds = %15
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
!31 = distinct !{!31, !7}
!32 = distinct !{!32, !7}
!33 = distinct !{!33, !7}
!34 = distinct !{!34, !7}
!35 = distinct !{!35, !7}
!36 = distinct !{!36, !7}
!37 = distinct !{!37, !7}
!38 = distinct !{!38, !7}
!39 = distinct !{!39, !7}
!40 = distinct !{!40, !7}
!41 = distinct !{!41, !7}
!42 = distinct !{!42, !7}
!43 = distinct !{!43, !7}
!44 = distinct !{!44, !7}
!45 = distinct !{!45, !7}
!46 = distinct !{!46, !7}
!47 = distinct !{!47, !7}
!48 = distinct !{!48, !7}
!49 = distinct !{!49, !7}
!50 = distinct !{!50, !7}
!51 = distinct !{!51, !7}
!52 = distinct !{!52, !7}
!53 = distinct !{!53, !7}
!54 = distinct !{!54, !7}
!55 = distinct !{!55, !7}
!56 = distinct !{!56, !7}
!57 = distinct !{!57, !7}
