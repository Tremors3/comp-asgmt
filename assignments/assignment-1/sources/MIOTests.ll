define dso_local i32 @test1(i32 noundef %0, i32 noundef %1) #0 {
  %3 = add nsw i32 %1, 1 ;; a = b + 1
  %4 = sub nsw i32 %3, 1 ;; c = a - 1
  %5 = add nsw i32 %4, 2 ;; d = c + 2
  %6 = sub nsw i32 %5, 2 ;; e = d - 2
  %7 = add nsw i32 %6, 1 ;; f = e + 1
  ret i32 %7
}

define dso_local i32 @test2(i32 noundef %0, i32 noundef %1) #0 {
  %3 = sub nsw i32 %1, 1 ;; a = b - 1
  %4 = add nsw i32 %3, 1 ;; c = a + 1
  %5 = sub nsw i32 %4, 2 ;; d = c - 2
  %6 = add nsw i32 %5, 2 ;; e = d + 2
  %7 = sub nsw i32 %6, 1 ;; f = e - 1
  ret i32 %7
}