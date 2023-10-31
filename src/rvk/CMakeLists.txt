file(GLOB_RECURSE SOURCES "src/*.h" "src/*.hpp" "src/*.c" "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h" "include/*.hpp" "include/*.c" "include/*.cpp" "include/*.inl")

# GROUPING
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source Files" FILES ${SOURCES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/include" PREFIX "Header Files" FILES ${HEADERS})

# EXECUTABLE
add_library(${LIB_RVK} STATIC ${SOURCES} ${HEADERS})
add_library(${LIB_RVK}::${LIB_RVK} ALIAS ${LIB_RVK})
set_target_properties(${LIB_RVK} PROPERTIES FOLDER ${FRAMEWORK_TARGET_FOLDER})

# DEPS
# glslang
#find_package(glslang CONFIG REQUIRED)
#target_link_libraries(${LIB_RVK} PRIVATE glslang::SPIRV glslang::glslang glslang::OGLCompiler glslang::OSDependent)
target_include_directories(${LIB_RVK} PRIVATE "${EXTERNAL_DIR}/glslang/include/")
if(WIN32)
   # debug
   target_link_libraries(${LIB_RVK} PRIVATE debug "${EXTERNAL_DIR}/glslang/lib/debug/win32/glslangd.lib")
   target_link_libraries(${LIB_RVK} PRIVATE debug "${EXTERNAL_DIR}/glslang/lib/debug/win32/GenericCodeGend.lib")
   target_link_libraries(${LIB_RVK} PRIVATE debug "${EXTERNAL_DIR}/glslang/lib/debug/win32/MachineIndependentd.lib")
   target_link_libraries(${LIB_RVK} PRIVATE debug "${EXTERNAL_DIR}/glslang/lib/debug/win32/OGLCompilerd.lib")
   target_link_libraries(${LIB_RVK} PRIVATE debug "${EXTERNAL_DIR}/glslang/lib/debug/win32/OSDependentd.lib")
   target_link_libraries(${LIB_RVK} PRIVATE debug "${EXTERNAL_DIR}/glslang/lib/debug/win32/SPIRVd.lib")
   target_link_libraries(${LIB_RVK} PRIVATE debug "${EXTERNAL_DIR}/glslang/lib/debug/win32/SPVRemapperd.lib")
   #target_link_libraries(${LIB_RVK} PRIVATE debug "${EXTERNAL_DIR}/glslang/lib/debug/win32/glslang-default-resource-limitsd.lib")
   # release
   target_link_libraries(${LIB_RVK} PRIVATE optimized "${EXTERNAL_DIR}/glslang/lib/release/win32/glslang.lib")
   target_link_libraries(${LIB_RVK} PRIVATE optimized "${EXTERNAL_DIR}/glslang/lib/release/win32/GenericCodeGen.lib")
   target_link_libraries(${LIB_RVK} PRIVATE optimized "${EXTERNAL_DIR}/glslang/lib/release/win32/MachineIndependent.lib")
   target_link_libraries(${LIB_RVK} PRIVATE optimized "${EXTERNAL_DIR}/glslang/lib/release/win32/OGLCompiler.lib")
   target_link_libraries(${LIB_RVK} PRIVATE optimized "${EXTERNAL_DIR}/glslang/lib/release/win32/OSDependent.lib")
   target_link_libraries(${LIB_RVK} PRIVATE optimized "${EXTERNAL_DIR}/glslang/lib/release/win32/SPIRV.lib")
   target_link_libraries(${LIB_RVK} PRIVATE optimized "${EXTERNAL_DIR}/glslang/lib/release/win32/SPVRemapper.lib")
   #target_link_libraries(${LIB_RVK} PRIVATE optimized "${EXTERNAL_DIR}/glslang/lib/debug/win32/glslang-default-resource-limits.lib")
elseif(APPLE)
   # debug
   target_link_libraries(${LIB_RVK} PRIVATE debug "${EXTERNAL_DIR}/glslang/lib/debug/macos/libglslang.a")
   target_link_libraries(${LIB_RVK} PRIVATE debug "${EXTERNAL_DIR}/glslang/lib/debug/macos/libMachineIndependent.a")
   target_link_libraries(${LIB_RVK} PRIVATE debug "${EXTERNAL_DIR}/glslang/lib/debug/macos/libGenericCodeGen.a")
   target_link_libraries(${LIB_RVK} PRIVATE debug "${EXTERNAL_DIR}/glslang/lib/debug/macos/libOGLCompiler.a")
   target_link_libraries(${LIB_RVK} PRIVATE debug "${EXTERNAL_DIR}/glslang/lib/debug/macos/libOSDependent.a")
   target_link_libraries(${LIB_RVK} PRIVATE debug "${EXTERNAL_DIR}/glslang/lib/debug/macos/libSPIRV.a")
   target_link_libraries(${LIB_RVK} PRIVATE debug "${EXTERNAL_DIR}/glslang/lib/debug/macos/libSPVRemapper.a")
   #target_link_libraries(${LIB_RVK} PRIVATE debug "${EXTERNAL_DIR}/glslang/lib/debug/macos/libglslang-default-resource-limits.a")
   # release
   target_link_libraries(${LIB_RVK} PRIVATE optimized "${EXTERNAL_DIR}/glslang/lib/release/macos/libglslang.a")
   target_link_libraries(${LIB_RVK} PRIVATE optimized "${EXTERNAL_DIR}/glslang/lib/release/macos/libMachineIndependent.a")
   target_link_libraries(${LIB_RVK} PRIVATE optimized "${EXTERNAL_DIR}/glslang/lib/release/macos/libGenericCodeGen.a")
   target_link_libraries(${LIB_RVK} PRIVATE optimized "${EXTERNAL_DIR}/glslang/lib/release/macos/libOGLCompiler.a")
   target_link_libraries(${LIB_RVK} PRIVATE optimized "${EXTERNAL_DIR}/glslang/lib/release/macos/libOSDependent.a")
   target_link_libraries(${LIB_RVK} PRIVATE optimized "${EXTERNAL_DIR}/glslang/lib/release/macos/libSPIRV.a")
   target_link_libraries(${LIB_RVK} PRIVATE optimized "${EXTERNAL_DIR}/glslang/lib/release/macos/libSPVRemapper.a")
   #target_link_libraries(${LIB_RVK} PRIVATE optimized "${EXTERNAL_DIR}/glslang/lib/release/macos/libglslang-default-resource-limits.a")
else(UNIX)
   # debug
   target_link_libraries(${LIB_RVK} PRIVATE debug "${EXTERNAL_DIR}/glslang/lib/debug/unix/libglslang.a")
   target_link_libraries(${LIB_RVK} PRIVATE debug "${EXTERNAL_DIR}/glslang/lib/debug/unix/libMachineIndependent.a")
   target_link_libraries(${LIB_RVK} PRIVATE debug "${EXTERNAL_DIR}/glslang/lib/debug/unix/libGenericCodeGen.a")
   target_link_libraries(${LIB_RVK} PRIVATE debug "${EXTERNAL_DIR}/glslang/lib/debug/unix/libOGLCompiler.a")
   target_link_libraries(${LIB_RVK} PRIVATE debug "${EXTERNAL_DIR}/glslang/lib/debug/unix/libOSDependent.a")
   target_link_libraries(${LIB_RVK} PRIVATE debug "${EXTERNAL_DIR}/glslang/lib/debug/unix/libSPIRV.a")
   target_link_libraries(${LIB_RVK} PRIVATE debug "${EXTERNAL_DIR}/glslang/lib/debug/unix/libSPVRemapper.a")
   #target_link_libraries(${LIB_RVK} PRIVATE debug "${EXTERNAL_DIR}/glslang/lib/debug/unix/libglslang-default-resource-limits.a")
   # release
   target_link_libraries(${LIB_RVK} PRIVATE optimized "${EXTERNAL_DIR}/glslang/lib/release/unix/libglslang.a")
   target_link_libraries(${LIB_RVK} PRIVATE optimized "${EXTERNAL_DIR}/glslang/lib/release/unix/libMachineIndependent.a")
   target_link_libraries(${LIB_RVK} PRIVATE optimized "${EXTERNAL_DIR}/glslang/lib/release/unix/libGenericCodeGen.a")
   target_link_libraries(${LIB_RVK} PRIVATE optimized "${EXTERNAL_DIR}/glslang/lib/release/unix/libOGLCompiler.a")
   target_link_libraries(${LIB_RVK} PRIVATE optimized "${EXTERNAL_DIR}/glslang/lib/release/unix/libOSDependent.a")
   target_link_libraries(${LIB_RVK} PRIVATE optimized "${EXTERNAL_DIR}/glslang/lib/release/unix/libSPIRV.a")
   target_link_libraries(${LIB_RVK} PRIVATE optimized "${EXTERNAL_DIR}/glslang/lib/release/unix/libSPVRemapper.a")
   #target_link_libraries(${LIB_RVK} PRIVATE optimized "${EXTERNAL_DIR}/glslang/lib/release/unix/libglslang-default-resource-limits.a")

   find_package(Threads REQUIRED)
   target_link_libraries(${LIB_RVK} PRIVATE Threads::Threads)
   target_link_libraries(${LIB_RVK} PRIVATE  ${CMAKE_DL_LIBS})
endif()

# dxc
target_include_directories(${LIB_RVK} PRIVATE "${EXTERNAL_DIR}/dxc/include/")
if(WIN32)
   set(DXC_BIN_DIR "${EXTERNAL_DIR}/dxc/bin/windows-x64/release/")
   add_library(DXC_LIB SHARED IMPORTED GLOBAL)
   set_property(TARGET DXC_LIB PROPERTY IMPORTED_IMPLIB "${DXC_BIN_DIR}/dxcompiler.lib")
   set_property(TARGET DXC_LIB PROPERTY IMPORTED_LOCATION "${DXC_BIN_DIR}/dxcompiler.dll")
   target_link_libraries(${LIB_RVK} PRIVATE debug DXC_LIB)
   target_link_libraries(${LIB_RVK} PRIVATE optimized DXC_LIB)
elseif(APPLE)
   set(DXC_BIN_DIR "${EXTERNAL_DIR}/dxc/bin/macos-x64/release/")
   target_link_libraries(${LIB_RVK} PRIVATE debug "${DXC_BIN_DIR}/libdxcompiler.3.7.dylib")
   target_link_libraries(${LIB_RVK} PRIVATE optimized "${DXC_BIN_DIR}/libdxcompiler.3.7.dylib")
else(UNIX)
   set(DXC_BIN_DIR "${EXTERNAL_DIR}/dxc/bin/linux-x64/release/")
   target_link_libraries(${LIB_RVK} PRIVATE debug "${DXC_BIN_DIR}/libdxcompiler.so")
   target_link_libraries(${LIB_RVK} PRIVATE optimized "${DXC_BIN_DIR}/libdxcompiler.so")
endif()

# INCLUDES
find_package(Vulkan REQUIRED)
target_include_directories(${LIB_RVK} PUBLIC "${Vulkan_INCLUDE_DIR}" "${SOURCE_DIR}/src/rvk/include")

# COMPILER
target_compile_definitions(${LIB_RVK} PRIVATE _R_VK_EXPORT_DLL)

