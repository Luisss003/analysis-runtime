; ModuleID = 'instrumented.bc'
source_filename = "test.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca ptr, align 8
  %3 = alloca ptr, align 8
  %4 = alloca ptr, align 8
  store i32 0, ptr %1, align 4
  %5 = call noalias ptr @malloc(i64 noundef 8) #2
  call void @record_malloc(ptr %5, i64 8)
  store ptr %5, ptr %2, align 8
  %6 = call noalias ptr @malloc(i64 noundef 8) #2
  call void @record_malloc(ptr %6, i64 8)
  store ptr %6, ptr %3, align 8
  %7 = load ptr, ptr %2, align 8
  call void @free(ptr noundef %7) #2
  call void @record_free(ptr %7)
  %8 = call noalias ptr @malloc(i64 noundef 8) #2
  call void @record_malloc(ptr %8, i64 8)
  store ptr %8, ptr %4, align 8
  %9 = load ptr, ptr %2, align 8
  store i8 122, ptr %9, align 1
  ret i32 0
}

; Function Attrs: nounwind
declare noalias ptr @malloc(i64 noundef) #1

; Function Attrs: nounwind
declare void @free(ptr noundef) #1

declare void @record_malloc(ptr, i64)

declare void @record_free(ptr)

attributes #0 = { noinline nounwind uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-builtins" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nounwind "frame-pointer"="all" "no-builtins" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { nobuiltin nounwind "no-builtins" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"Debian clang version 19.1.7 (3+b1)"}
