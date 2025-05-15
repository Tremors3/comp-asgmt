; ModuleID = '/home/tremore/Programming/GitHub/comp-asgmt/assignments/assignment-4/sources/lf-guard-auto-2.bc'
source_filename = "/home/tremore/Programming/GitHub/comp-asgmt/assignments/assignment-4/sources/lf-guard-auto-2.ll"

define i32 @second_guard(i32 %n) {
entry:
  br label %loop1.header

loop1.header:                                     ; preds = %loop1.body, %entry
  %sum.0 = phi i32 [ 0, %entry ], [ %add1, %loop1.body ]
  %i1 = phi i32 [ 0, %entry ], [ %i1.next, %loop1.body ]
  %cmp1 = icmp slt i32 %i1, %n
  br i1 %cmp1, label %loop1.body, label %loop1.exit

loop1.body:                                       ; preds = %loop1.header
  %add1 = add i32 %sum.0, %i1
  %i1.next = add i32 %i1, 1
  br label %loop1.header

loop1.exit:                                       ; preds = %loop1.header
  br label %check2

check2:                                           ; preds = %loop1.exit
  %cond2 = icmp sgt i32 %n, 0
  br i1 %cond2, label %loop2.header, label %exit

loop2.header:                                     ; preds = %loop2.body, %check2
  %sum.1 = phi i32 [ %sum.0, %check2 ], [ %add2, %loop2.body ]
  %i2 = phi i32 [ 0, %check2 ], [ %i2.next, %loop2.body ]
  %cmp2 = icmp slt i32 %i2, %n
  br i1 %cmp2, label %loop2.body, label %exit

loop2.body:                                       ; preds = %loop2.header
  %add2 = add i32 %sum.1, %i2
  %i2.next = add i32 %i2, 1
  br label %loop2.header

exit:                                             ; preds = %loop2.header, %check2
  %sum.2 = phi i32 [ %sum.1, %loop2.header ], [ %sum.0, %check2 ]
  ret i32 %sum.2
}
