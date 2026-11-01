; ModuleID = 'test.c'
source_filename = "test.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 !dbg !10 {
  %1 = alloca i32, align 4
  %2 = alloca ptr, align 8
  store i32 0, ptr %1, align 4
    #dbg_declare(ptr %2, !15, !DIExpression(), !18)
  %3 = call noalias ptr @malloc(i64 noundef 128) #3, !dbg !19
  store ptr %3, ptr %2, align 8, !dbg !18
  %4 = load ptr, ptr %2, align 8, !dbg !20
  %5 = icmp ne ptr %4, null, !dbg !20
  br i1 %5, label %7, label %6, !dbg !22

6:                                                ; preds = %0
  store i32 1, ptr %1, align 4, !dbg !23
  br label %11, !dbg !23

7:                                                ; preds = %0
  %8 = load ptr, ptr %2, align 8, !dbg !24
  %9 = call i64 @read(i32 noundef 0, ptr noundef %8, i64 noundef 8) #4, !dbg !25
  %10 = load ptr, ptr %2, align 8, !dbg !26
  call void @free(ptr noundef %10) #3, !dbg !27
  store i32 0, ptr %1, align 4, !dbg !28
  br label %11, !dbg !28

11:                                               ; preds = %7, %6
  %12 = load i32, ptr %1, align 4, !dbg !29
  ret i32 %12, !dbg !29
}

; Function Attrs: nounwind
declare noalias ptr @malloc(i64 noundef) #1

declare i64 @read(i32 noundef, ptr noundef, i64 noundef) #2

; Function Attrs: nounwind
declare void @free(ptr noundef) #1

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-builtins" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nounwind "frame-pointer"="all" "no-builtins" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { "frame-pointer"="all" "no-builtins" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #3 = { nobuiltin nounwind "no-builtins" }
attributes #4 = { nobuiltin "no-builtins" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3, !4, !5, !6, !7, !8}
!llvm.ident = !{!9}

!0 = distinct !DICompileUnit(language: DW_LANG_C11, file: !1, producer: "Debian clang version 19.1.7 (3+b1)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "test.c", directory: "/home/sputnik/thesis/instrumentation", checksumkind: CSK_MD5, checksum: "3cbc21eab1f88bb279c0314f4008feef")
!2 = !{i32 7, !"Dwarf Version", i32 5}
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !{i32 1, !"wchar_size", i32 4}
!5 = !{i32 8, !"PIC Level", i32 2}
!6 = !{i32 7, !"PIE Level", i32 2}
!7 = !{i32 7, !"uwtable", i32 2}
!8 = !{i32 7, !"frame-pointer", i32 2}
!9 = !{!"Debian clang version 19.1.7 (3+b1)"}
!10 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 4, type: !11, scopeLine: 4, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !14)
!11 = !DISubroutineType(types: !12)
!12 = !{!13}
!13 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!14 = !{}
!15 = !DILocalVariable(name: "p", scope: !10, file: !1, line: 5, type: !16)
!16 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !17, size: 64)
!17 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!18 = !DILocation(line: 5, column: 11, scope: !10)
!19 = !DILocation(line: 5, column: 15, scope: !10)
!20 = !DILocation(line: 7, column: 10, scope: !21)
!21 = distinct !DILexicalBlock(scope: !10, file: !1, line: 7, column: 9)
!22 = !DILocation(line: 7, column: 9, scope: !10)
!23 = !DILocation(line: 8, column: 9, scope: !21)
!24 = !DILocation(line: 10, column: 24, scope: !10)
!25 = !DILocation(line: 10, column: 5, scope: !10)
!26 = !DILocation(line: 12, column: 10, scope: !10)
!27 = !DILocation(line: 12, column: 5, scope: !10)
!28 = !DILocation(line: 14, column: 5, scope: !10)
!29 = !DILocation(line: 15, column: 1, scope: !10)
