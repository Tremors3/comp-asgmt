; 15 × 𝑥 = 𝑥 × 15 ⇒ (𝑥 ≪ 4) – x

define dso_local i32 @test1(i32 noundef %0) #0 {
  %2 = mul nsw i32 %0, 0
  ret i32 %2
}

define dso_local i32 @test2(i32 noundef %0) #0 {
  %2 = mul nsw i32 %0, 1
  ret i32 %2
}

define dso_local i32 @test3(i32 noundef %0) #0 {
  %2 = mul nsw i32 %0, 1022
  ret i32 %2
}

define dso_local i32 @test4(i32 noundef %0) #0 {
  %2 = mul nsw i32 %0, 1023
  ret i32 %2
}

define dso_local i32 @test5(i32 noundef %0) #0 {
  %2 = mul nsw i32 %0, 1024
  ret i32 %2
}

define dso_local i32 @test6(i32 noundef %0) #0 {
  %2 = mul nsw i32 %0, 1025
  ret i32 %2
}

define dso_local i32 @test7(i32 noundef %0) #0 {
  %2 = mul nsw i32 %0, 1026
  ret i32 %2
}
