; ModuleID = '/home/herry/GitHub/comp-asgmt/assignments/assignment-4/sources/lf-fusion-simple.bc'
source_filename = "/home/herry/GitHub/comp-asgmt/assignments/assignment-4/sources/lf-fusion-simple.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @NotGuarded() #0 {
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
define dso_local void @Guarded() #0 {
  %1 = zext i32 10 to i64
  %2 = call ptr @llvm.stacksave.p0()
  %3 = alloca i32, i64 %1, align 16
  %4 = mul nuw i64 %1, 4
  call void @llvm.memset.p0.i64(ptr align 16 %3, i8 0, i64 %4, i1 false)
  %5 = zext i32 10 to i64
  %6 = alloca i32, i64 %5, align 16
  %7 = mul nuw i64 %5, 4
  call void @llvm.memset.p0.i64(ptr align 16 %6, i8 0, i64 %7, i1 false)
  %8 = icmp sgt i32 10, 0
  br i1 %8, label %9, label %17

9:                                                ; preds = %0
  br label %10

10:                                               ; preds = %14, %9
  %.01 = phi i32 [ 0, %9 ], [ %13, %14 ]
  %11 = sext i32 %.01 to i64
  %12 = getelementptr inbounds i32, ptr %6, i64 %11
  store i32 %.01, ptr %12, align 4
  %13 = add nsw i32 %.01, 1
  br label %14

14:                                               ; preds = %10
  %15 = icmp slt i32 %13, 10
  br i1 %15, label %10, label %16, !llvm.loop !9

16:                                               ; preds = %14
  br label %17

17:                                               ; preds = %16, %0
  %18 = icmp sgt i32 10, 0
  br i1 %18, label %19, label %27

19:                                               ; preds = %17
  br label %20

20:                                               ; preds = %24, %19
  %.0 = phi i32 [ 0, %19 ], [ %23, %24 ]
  %21 = sext i32 %.0 to i64
  %22 = getelementptr inbounds i32, ptr %3, i64 %21
  store i32 %.0, ptr %22, align 4
  %23 = add nsw i32 %.0, 1
  br label %24

24:                                               ; preds = %20
  %25 = icmp slt i32 %23, 10
  br i1 %25, label %20, label %26, !llvm.loop !10

26:                                               ; preds = %24
  br label %27

27:                                               ; preds = %26, %17
  call void @llvm.stackrestore.p0(ptr %2)
  ret void
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @withIf() #0 {
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

15:                                               ; preds = %25, %14
  %.0 = phi i32 [ 0, %14 ], [ %24, %25 ]
  %16 = sext i32 %.0 to i64
  %17 = getelementptr inbounds i32, ptr %3, i64 %16
  store i32 %.0, ptr %17, align 4
  %18 = icmp sgt i32 10, 0
  br i1 %18, label %19, label %23

19:                                               ; preds = %15
  %20 = add nsw i32 %.0, %.0
  %21 = sext i32 %.0 to i64
  %22 = getelementptr inbounds i32, ptr %3, i64 %21
  store i32 %20, ptr %22, align 4
  br label %23

23:                                               ; preds = %19, %15
  %24 = add nsw i32 %.0, 1
  br label %25

25:                                               ; preds = %23
  %26 = icmp slt i32 %24, 10
  br i1 %26, label %15, label %27, !llvm.loop !12

27:                                               ; preds = %25
  call void @llvm.stackrestore.p0(ptr %2)
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
