set(LLVM_LINK_COMPONENTS
  support
  AllTargetsInfos
  FrontendOpenMP
  )

get_filename_component(CLANGD_SOURCE_DIR
  ${CMAKE_CURRENT_SOURCE_DIR}/../../clangd REALPATH)
get_filename_component(CLANGD_BINARY_DIR
  ${CMAKE_CURRENT_BINARY_DIR}/../../clangd REALPATH)
include_directories(
  ${CLANGD_SOURCE_DIR}
  ${CLANGD_BINARY_DIR}
  )

if(CLANG_BUILT_STANDALONE)
  # LLVMTestingSupport library is needed for clangd tests.
  if (EXISTS ${LLVM_MAIN_SRC_DIR}/lib/Testing/Support
      AND NOT TARGET LLVMTestingSupport)
    add_subdirectory(${LLVM_MAIN_SRC_DIR}/lib/Testing/Support
      lib/Testing/Support)
  endif()
endif()

add_custom_target(ClangdUnitTests)
add_unittest(ClangdUnitTests ClangdTests
  Annotations.cpp
  ASTTests.cpp
  BackgroundIndexTests.cpp
  CanonicalIncludesTests.cpp
  ClangdTests.cpp
  ClangdLSPServerTests.cpp
  CodeCompleteTests.cpp
  CodeCompletionStringsTests.cpp
  CollectMacrosTests.cpp
  CompileCommandsTests.cpp
  DexTests.cpp
  DiagnosticsTests.cpp
  DraftStoreTests.cpp
  ExpectedTypeTest.cpp
  FileDistanceTests.cpp
  FileIndexTests.cpp
  FindSymbolsTests.cpp
  FindTargetTests.cpp
  FormatTests.cpp
  FSTests.cpp
  FuzzyMatchTests.cpp
  GlobalCompilationDatabaseTests.cpp
  HeadersTests.cpp
  HeaderSourceSwitchTests.cpp
  HoverTests.cpp
  IndexActionTests.cpp
  IndexTests.cpp
  JSONTransportTests.cpp
  LSPClient.cpp
  ParsedASTTests.cpp
  PathMappingTests.cpp
  PreambleTests.cpp
  PrintASTTests.cpp
  QualityTests.cpp
  RenameTests.cpp
  RIFFTests.cpp
  SelectionTests.cpp
  SemanticHighlightingTests.cpp
  SemanticSelectionTests.cpp
  SerializationTests.cpp
  SourceCodeTests.cpp
  SymbolCollectorTests.cpp
  SymbolInfoTests.cpp
  SyncAPI.cpp
  TUSchedulerTests.cpp
  TestFS.cpp
  TestIndex.cpp
  TestTU.cpp
  TypeHierarchyTests.cpp
  TweakTests.cpp
  TweakTesting.cpp
  URITests.cpp
  XRefsTests.cpp

  support/CancellationTests.cpp
  support/ContextTests.cpp
  support/FunctionTests.cpp
  support/MarkupTests.cpp
  support/ThreadingTests.cpp
  support/TraceTests.cpp

  $<TARGET_OBJECTS:obj.clangDaemonTweaks>
  )

clang_target_link_libraries(ClangdTests
  PRIVATE
  clangAST
  clangASTMatchers
  clangBasic
  clangFormat
  clangFrontend
  clangIndex
  clangLex
  clangSema
  clangSerialization
  clangTooling
  clangToolingCore
  clangToolingInclusions
  clangToolingRefactoring
  clangToolingSyntax
  )
target_link_libraries(ClangdTests
  PRIVATE
  clangDaemon
  clangdSupport
  clangTidy
  LLVMSupport
  LLVMTestingSupport
  )

if (CLANGD_BUILD_XPC)
  add_subdirectory(xpc)
endif ()

configure_lit_site_cfg(
  ${CMAKE_CURRENT_SOURCE_DIR}/lit.site.cfg.py.in
  ${CMAKE_CURRENT_BINARY_DIR}/lit.site.cfg.py)
