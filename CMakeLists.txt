set( LLVM_LINK_COMPONENTS support)

add_clang_executable(clang-annos
  FuncDecl.cpp
  ClangAnnos.cpp
  )

target_link_libraries(clang-annos
  clangAST
  clangBasic
  clangDriver
  clangFrontend
  #clangRewriteFrontend
  #clangStaticAnalyzerFrontend
  clangTooling
  )

install(TARGETS clang-annos
  RUNTIME DESTINATION bin)
