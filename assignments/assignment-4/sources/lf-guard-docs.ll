define void @test(i32 %n) {
entry:
  %guard_cond = icmp slt i32 0, %n
  br i1 %guard_cond, label %loop.preheader, label %exit

loop.preheader:
  br label %body

body:
  %i2 = phi i32 [ 0, %loop.preheader ], [ %i.next, %latch ]
  br label %latch

latch:
  %i.next = add nsw i32 %i2, 1
  %cond = icmp slt i32 %i.next, %n
  br i1 %cond, label %body, label %loop.exit

loop.exit:
  br label %exit

exit:
  ret void
}
