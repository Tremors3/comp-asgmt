
; MULTIPLICATION TESTS

define dso_local i32 @testM1(i32 noundef %0) #0 {
  %2 = mul nsw i32 %0, 0
  ret i32 %2
}

define dso_local i32 @testM2(i32 noundef %0) #0 {
  %2 = mul nsw i32 %0, 1
  ret i32 %2
}

define dso_local i32 @testM3(i32 noundef %0) #0 {
  %2 = mul nsw i32 %0, 1022
  ret i32 %2
}

define dso_local i32 @testM4(i32 noundef %0) #0 {
  %2 = mul nsw i32 %0, 1023
  ret i32 %2
}

define dso_local i32 @testM5(i32 noundef %0) #0 {
  %2 = mul nsw i32 %0, 1024
  ret i32 %2
}

define dso_local i32 @testM6(i32 noundef %0) #0 {
  %2 = mul nsw i32 %0, 1025
  ret i32 %2
}

define dso_local i32 @testM7(i32 noundef %0) #0 {
  %2 = mul nsw i32 %0, 1026
  ret i32 %2
}

; DIVISION TESTS

define dso_local i32 @testD1(i32 noundef %0) #0 {
  %2 = sdiv i32 %0, 1
  ret i32 %2
}

define dso_local i32 @testD2(i32 noundef %0) #0 {
  %2 = sdiv i32 %0, 1023
  ret i32 %2
}

define dso_local i32 @testD3(i32 noundef %0) #0 {
  %2 = sdiv i32 %0, 1024
  ret i32 %2
}

define dso_local i32 @testD4(i32 noundef %0) #0 {
  %2 = sdiv i32 %0, 1025
  ret i32 %2
}