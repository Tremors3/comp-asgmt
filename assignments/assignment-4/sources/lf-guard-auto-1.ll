
define i32 @no_guard(i32 %n) {
entry:
  %sum = alloca i32
  store i32 0, i32* %sum
  br label %loop1.header

loop1.header:
  %i1 = phi i32 [0, %entry], [%i1.next, %loop1.body]
  %cmp1 = icmp slt i32 %i1, %n
  br i1 %cmp1, label %loop1.body, label %loop1.exit

loop1.body:
  %val1 = load i32, i32* %sum
  %add1 = add i32 %val1, %i1
  store i32 %add1, i32* %sum
  %i1.next = add i32 %i1, 1
  br label %loop1.header

loop1.exit:
  br label %loop2.header

loop2.header:
  %i2 = phi i32 [0, %loop1.exit], [%i2.next, %loop2.body]
  %cmp2 = icmp slt i32 %i2, %n
  br i1 %cmp2, label %loop2.body, label %exit

loop2.body:
  %val2 = load i32, i32* %sum
  %add2 = add i32 %val2, %i2
  store i32 %add2, i32* %sum
  %i2.next = add i32 %i2, 1
  br label %loop2.header

exit:
  %result = load i32, i32* %sum
  ret i32 %result
}


define i32 @first_guard(i32 %n) {
entry:
  %sum = alloca i32
  store i32 0, i32* %sum
  %cond1 = icmp sgt i32 %n, 0
  br i1 %cond1, label %loop1.header, label %loop1.exit

loop1.header:
  %i1 = phi i32 [0, %entry], [%i1.next, %loop1.body]
  %cmp1 = icmp slt i32 %i1, %n
  br i1 %cmp1, label %loop1.body, label %loop1.exit

loop1.body:
  %val1 = load i32, i32* %sum
  %add1 = add i32 %val1, %i1
  store i32 %add1, i32* %sum
  %i1.next = add i32 %i1, 1
  br label %loop1.header

loop1.exit:
  br label %loop2.header

loop2.header:
  %i2 = phi i32 [0, %loop1.exit], [%i2.next, %loop2.body]
  %cmp2 = icmp slt i32 %i2, %n
  br i1 %cmp2, label %loop2.body, label %exit

loop2.body:
  %val2 = load i32, i32* %sum
  %add2 = add i32 %val2, %i2
  store i32 %add2, i32* %sum
  %i2.next = add i32 %i2, 1
  br label %loop2.header

exit:
  %result = load i32, i32* %sum
  ret i32 %result
}
