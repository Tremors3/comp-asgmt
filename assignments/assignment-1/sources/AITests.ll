define dso_local i32 @test1(i32 noundef %0, i32 noundef %1) #0 {
  %3 = add nsw i32 %1, 0
  %4 = mul nsw i32 %1, 1
  %5 = add nsw i32 %1, 2
  %6 = mul nsw i32 %1, 2
  
  %7 = add nsw i32  %3, 2
  %8 = mul nsw i32  %4, 2
  %9 = add nsw i32  %5, 2
  %10 = mul nsw i32 %6, 2
  
  ret i32 %10
}

define dso_local i32 @test2(i32 noundef %0, i32 noundef %1) #0 {
  %3 = add nsw i32 5, 0
  %4 = mul nsw i32 5, 1
  %5 = add nsw i32 5, 2
  %6 = mul nsw i32 5, 2
  
  %7 = add nsw i32  %3, 2
  %8 = mul nsw i32  %4, 2
  %9 = add nsw i32  %5, 2
  %10 = mul nsw i32 %6, 2
  
  ret i32 %10
}