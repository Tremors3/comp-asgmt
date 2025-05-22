; ModuleID = '/home/tremore/Programming/GitHub/comp-asgmt/assignments/assignment-4/sources/lf-ndd-test.bc'
source_filename = "/home/tremore/Programming/GitHub/comp-asgmt/assignments/assignment-4/sources/lf-ndd-test.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local void @matrix_ndd_test() #0 {
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
  br i1 %33, label %15, label %34, !llvm.loop !6

34:                                               ; preds = %32
  br label %35

35:                                               ; preds = %56, %34
  %.0 = phi i32 [ 0, %34 ], [ %55, %56 ]
  %36 = add nsw i32 %.0, 3
  %37 = sext i32 %36 to i64
  %38 = mul nuw i64 %3, %4
  %39 = mul nuw i64 %38, %5
  %40 = mul nsw i64 %37, %39
  %41 = getelementptr inbounds i32, ptr %10, i64 %40
  %42 = add nsw i32 %.0, 3
  %43 = sext i32 %42 to i64
  %44 = mul nuw i64 %4, %5
  %45 = mul nsw i64 %43, %44
  %46 = getelementptr inbounds i32, ptr %41, i64 %45
  %47 = add nsw i32 %.0, 3
  %48 = sext i32 %47 to i64
  %49 = mul nsw i64 %48, %5
  %50 = getelementptr inbounds i32, ptr %46, i64 %49
  %51 = add nsw i32 %.0, 3
  %52 = sext i32 %51 to i64
  %53 = getelementptr inbounds i32, ptr %50, i64 %52
  %54 = load i32, ptr %53, align 4
  %55 = add nsw i32 %.0, 1
  br label %56

56:                                               ; preds = %35
  %57 = icmp slt i32 %55, 10
  br i1 %57, label %35, label %58, !llvm.loop !8

58:                                               ; preds = %56
  call void @llvm.stackrestore.p0(ptr %6)
  ret void
}

; Function Attrs: nocallback nofree nosync nounwind willreturn
declare ptr @llvm.stacksave.p0() #1

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i64(ptr nocapture writeonly, i8, i64, i1 immarg) #2

; Function Attrs: nocallback nofree nosync nounwind willreturn
declare void @llvm.stackrestore.p0(ptr) #1

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
