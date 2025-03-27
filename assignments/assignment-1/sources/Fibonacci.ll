; ModuleID = '/home/tremore/Programming/GitHub/comp-asgmt/assignments/assignment-1/sources/Fibonacci.bc'
source_filename = "/home/tremore/Programming/GitHub/comp-asgmt/assignments/assignment-1/sources/Fibonacci.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

%struct.__va_list_tag = type { i32, i32, ptr, ptr }

@stdout = external global ptr, align 8
@.str = private unnamed_addr constant [9 x i8] c"f(0) = 0\00", align 1
@.str.1 = private unnamed_addr constant [9 x i8] c"f(1) = 1\00", align 1
@.str.2 = private unnamed_addr constant [22 x i8] c"f(%d) = f(%d) + f(%d)\00", align 1

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i32 @printf(ptr noundef %0, ...) #0 {
  %2 = alloca [1 x %struct.__va_list_tag], align 16
  %3 = getelementptr inbounds [1 x %struct.__va_list_tag], ptr %2, i64 0, i64 0
  call void @llvm.va_start.p0(ptr %3)
  %4 = load ptr, ptr @stdout, align 8
  %5 = getelementptr inbounds [1 x %struct.__va_list_tag], ptr %2, i64 0, i64 0
  %6 = call i32 @vfprintf(ptr noundef %4, ptr noundef %0, ptr noundef %5) #3
  %7 = getelementptr inbounds [1 x %struct.__va_list_tag], ptr %2, i64 0, i64 0
  call void @llvm.va_end.p0(ptr %7)
  ret i32 %6
}

; Function Attrs: nocallback nofree nosync nounwind willreturn
declare void @llvm.va_start.p0(ptr) #1

; Function Attrs: nounwind
declare i32 @vfprintf(ptr noundef, ptr noundef, ptr noundef) #2

; Function Attrs: nocallback nofree nosync nounwind willreturn
declare void @llvm.va_end.p0(ptr) #1

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i32 @Fibonacci(i32 noundef %0) #0 {
  %2 = icmp eq i32 %0, 0
  br i1 %2, label %3, label %5

3:                                                ; preds = %1
  %4 = call i32 (ptr, ...) @printf(ptr noundef @.str)
  br label %18

5:                                                ; preds = %1
  %6 = icmp eq i32 %0, 1
  br i1 %6, label %7, label %9

7:                                                ; preds = %5
  %8 = call i32 (ptr, ...) @printf(ptr noundef @.str.1)
  br label %18

9:                                                ; preds = %5
  %10 = sub nsw i32 %0, 1
  %11 = sub nsw i32 %0, 2
  %12 = call i32 (ptr, ...) @printf(ptr noundef @.str.2, i32 noundef %0, i32 noundef %10, i32 noundef %11)
  %13 = sub nsw i32 %0, 1
  %14 = call i32 @Fibonacci(i32 noundef %13)
  %15 = sub nsw i32 %0, 2
  %16 = call i32 @Fibonacci(i32 noundef %15)
  %17 = add nsw i32 %14, %16
  br label %18

18:                                               ; preds = %9, %7, %3
  %.0 = phi i32 [ 0, %3 ], [ 1, %7 ], [ %17, %9 ]
  ret i32 %.0
}

attributes #0 = { noinline nounwind sspstrong uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nocallback nofree nosync nounwind willreturn }
attributes #2 = { nounwind "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #3 = { nounwind }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"clang version 19.1.7"}
