# LLVM_TARGET_DEFINITIONS must contain the name of the .td file to process.
# Extra parameters for `tblgen' may come after `ofn' parameter.
# Adds the name of the generated file to TABLEGEN_OUTPUT.

function(tablegen project ofn)
  # Validate calling context.
  foreach(v
      ${project}_TABLEGEN_EXE
      LLVM_MAIN_SRC_DIR
      LLVM_MAIN_INCLUDE_DIR
      )
    if(NOT ${v})
      message(FATAL_ERROR "${v} not set")
    endif()
  endforeach()

  file(GLOB local_tds "*.td")
  file(GLOB_RECURSE global_tds "${LLVM_MAIN_INCLUDE_DIR}/llvm/*.td")

  if (IS_ABSOLUTE ${LLVM_TARGET_DEFINITIONS})
    set(LLVM_TARGET_DEFINITIONS_ABSOLUTE ${LLVM_TARGET_DEFINITIONS})
  else()
    set(LLVM_TARGET_DEFINITIONS_ABSOLUTE
      ${CMAKE_CURRENT_SOURCE_DIR}/${LLVM_TARGET_DEFINITIONS})
  endif()
  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${ofn}.tmp
    # Generate tablegen output in a temporary file.
    COMMAND ${${project}_TABLEGEN_EXE} ${ARGN} -I ${CMAKE_CURRENT_SOURCE_DIR}
    -I ${LLVM_MAIN_SRC_DIR}/lib/Target -I ${LLVM_MAIN_INCLUDE_DIR}
    ${LLVM_TARGET_DEFINITIONS_ABSOLUTE}
    -o ${CMAKE_CURRENT_BINARY_DIR}/${ofn}.tmp
    # The file in LLVM_TARGET_DEFINITIONS may be not in the current
    # directory and local_tds may not contain it, so we must
    # explicitly list it here:
    DEPENDS ${${project}_TABLEGEN_EXE} ${local_tds} ${global_tds}
    ${LLVM_TARGET_DEFINITIONS_ABSOLUTE}
    COMMENT "Building ${ofn}..."
    )
  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${ofn}
    # Only update the real output file if there are any differences.
    # This prevents recompilation of all the files depending on it if there
    # aren't any.
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_BINARY_DIR}/${ofn}.tmp
        ${CMAKE_CURRENT_BINARY_DIR}/${ofn}
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${ofn}.tmp
    COMMENT "Updating ${ofn}..."
    )

  # `make clean' must remove all those generated files:
  set_property(DIRECTORY APPEND
    PROPERTY ADDITIONAL_MAKE_CLEAN_FILES ${ofn}.tmp ${ofn})

  set(TABLEGEN_OUTPUT ${TABLEGEN_OUTPUT} ${CMAKE_CURRENT_BINARY_DIR}/${ofn} PARENT_SCOPE)
  set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/${ofn} PROPERTIES
    GENERATED 1)
endfunction()

# Creates a target for publicly exporting tablegen dependencies.
function(add_public_tablegen_target target)
  if(NOT TABLEGEN_OUTPUT)
    message(FATAL_ERROR "Requires tablegen() definitions as TABLEGEN_OUTPUT.")
  endif()
  add_custom_target(${target}
    DEPENDS ${TABLEGEN_OUTPUT})
  if(LLVM_COMMON_DEPENDS)
    add_dependencies(${target} ${LLVM_COMMON_DEPENDS})
  endif()
  set_target_properties(${target} PROPERTIES FOLDER "Tablegenning")
  set(LLVM_COMMON_DEPENDS ${LLVM_COMMON_DEPENDS} ${target} PARENT_SCOPE)
endfunction()

if(CMAKE_CROSSCOMPILING)
  set(CX_NATIVE_TG_DIR "${CMAKE_BINARY_DIR}/native")

  add_custom_command(OUTPUT ${CX_NATIVE_TG_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CX_NATIVE_TG_DIR}
    COMMENT "Creating ${CX_NATIVE_TG_DIR}...")

  # Forward a subset of configure options to discover additional tablegen modules.
  get_cmake_property(_variableNames CACHE_VARIABLES)
  foreach (_variableName ${_variableNames})
    if (_variableName MATCHES "^(LLVM_EXTERNAL_.*_SOURCE_DIR)$")
      list(APPEND CX_CMAKE_ARGUMENTS "-D${_variableName}=\"${${_variableName}}\"")
    endif ()
  endforeach()

  add_custom_command(OUTPUT ${CX_NATIVE_TG_DIR}/CMakeCache.txt
    # TODO: Clear the old CMakeCache.txt somehow without breaking restat.
    COMMAND ${CMAKE_COMMAND} -UMAKE_TOOLCHAIN_FILE -DCMAKE_BUILD_TYPE=Release
                             -DLLVM_BUILD_POLLY=OFF ${CX_CMAKE_ARGUMENTS}
                             -G "${CMAKE_GENERATOR}" ${CMAKE_SOURCE_DIR}
    WORKING_DIRECTORY ${CX_NATIVE_TG_DIR}
    DEPENDS ${CX_NATIVE_TG_DIR}
    COMMENT "Configuring native TableGen...")

  add_custom_target(ConfigureNativeTableGen DEPENDS ${CX_NATIVE_TG_DIR}/CMakeCache.txt)

  set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES ${CX_NATIVE_TG_DIR})
endif()

macro(add_tablegen target project)
  set(${target}_OLD_LLVM_LINK_COMPONENTS ${LLVM_LINK_COMPONENTS})
  set(LLVM_LINK_COMPONENTS ${LLVM_LINK_COMPONENTS} TableGen)
  add_llvm_utility(${target} ${ARGN})
  set(LLVM_LINK_COMPONENTS ${${target}_OLD_LLVM_LINK_COMPONENTS})

  set(${project}_TABLEGEN "${target}" CACHE
      STRING "Native TableGen executable. Saves building one when cross-compiling.")

  # Upgrade existing LLVM_TABLEGEN setting.
  if(${project} STREQUAL LLVM)
    if(${LLVM_TABLEGEN} STREQUAL tblgen)
      set(LLVM_TABLEGEN "${target}" CACHE
          STRING "Native TableGen executable. Saves building one when cross-compiling."
          FORCE)
    endif()
  endif()

  # Effective tblgen executable to be used:
  set(${project}_TABLEGEN_EXE ${${project}_TABLEGEN} PARENT_SCOPE)

  if(CMAKE_CROSSCOMPILING)
    if( ${${project}_TABLEGEN} STREQUAL "${target}" )
      set(${project}_TABLEGEN_EXE "${CX_NATIVE_TG_DIR}/bin/${target}")
      set(${project}_TABLEGEN_EXE ${${project}_TABLEGEN_EXE} PARENT_SCOPE)

      add_custom_command(OUTPUT ${${project}_TABLEGEN_EXE}
        COMMAND ${CMAKE_BUILD_TOOL} ${target}
        DEPENDS ${CX_NATIVE_TG_DIR}/CMakeCache.txt
        WORKING_DIRECTORY ${CX_NATIVE_TG_DIR}
        COMMENT "Building native TableGen...")
      add_custom_target(${project}NativeTableGen DEPENDS ${${project}_TABLEGEN_EXE})
      add_dependencies(${project}NativeTableGen ConfigureNativeTableGen)

      add_dependencies(${target} ${project}NativeTableGen)
    endif()
  endif()

  if( MINGW )
    if(CMAKE_SIZEOF_VOID_P MATCHES "8")
      set_target_properties(${target} PROPERTIES LINK_FLAGS -Wl,--stack,16777216)
    endif(CMAKE_SIZEOF_VOID_P MATCHES "8")
  endif( MINGW )
  if (${project} STREQUAL LLVM AND NOT LLVM_INSTALL_TOOLCHAIN_ONLY)
    install(TARGETS ${target}
            EXPORT LLVMExports
            RUNTIME DESTINATION bin)
  endif()
  set_property(GLOBAL APPEND PROPERTY LLVM_EXPORTS ${target})
endmacro()
