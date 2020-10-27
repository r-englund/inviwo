
set(BUILD_FRAMEWORK            OFF CACHE INTERNAL "Build package as Mac OS X Framework bundle.")
set(ASSIMP_DOUBLE_PRECISION    OFF CACHE INTERNAL "Set to ON to enable double precision processing")
set(ASSIMP_OPT_BUILD_PACKAGES  OFF CACHE INTERNAL "Set to ON to generate CPack configuration files and packaging targets")
set(ASSIMP_INSTALL_PDB         OFF CACHE INTERNAL "Install MSVC debug files.")
set(ASSIMP_ANDROID_JNIIOSYSTEM OFF CACHE INTERNAL "Android JNI IOSystem support is active")
set(ASSIMP_NO_EXPORT           OFF CACHE INTERNAL "Disable Assimp's export functionality.")
set(ASSIMP_BUILD_ZLIB          OFF CACHE INTERNAL "Build your own zlib")
set(ASSIMP_BUILD_ASSIMP_TOOLS  OFF CACHE INTERNAL "If the supplementary tools for Assimp are built in addition to the library.")
set(ASSIMP_BUILD_SAMPLES       OFF CACHE INTERNAL "If the official samples are built as well (needs Glut).")
set(ASSIMP_BUILD_TESTS         OFF CACHE INTERNAL "If the test suite for Assimp is built in addition to the library.")
set(ASSIMP_COVERALLS           OFF CACHE INTERNAL "Enable this to measure test coverage.")
set(ASSIMP_WERROR              OFF CACHE INTERNAL "Treat warnings as errors.")
set(ASSIMP_ASAN                OFF CACHE INTERNAL "Enable AddressSanitizer.")
set(ASSIMP_UBSAN               OFF CACHE INTERNAL "Enable Undefined Behavior sanitizer.")
set(SYSTEM_IRRXML              OFF CACHE INTERNAL "Use system installed Irrlicht/IrrXML library.")
set(BUILD_DOCS                 OFF CACHE INTERNAL "Build documentation using Doxygen.")
set(INJECT_DEBUG_POSTFIX       ON  CACHE INTERNAL "Inject debug postfix in .a/.so lib names")
set(IGNORE_GIT_HASH            ON  CACHE INTERNAL "Don't call git to get the hash.")

SET( ASSIMP_LIB_INSTALL_DIR "${IVW_LIBRARY_INSTALL_DIR}" CACHE INTERNAL  "Path the built library files are installed to.")
SET( ASSIMP_INCLUDE_INSTALL_DIR "${IVW_INCLUDE_INSTALL_DIR}" CACHE INTERNAL  "Path the header files are installed to.")
SET( ASSIMP_BIN_INSTALL_DIR "${IVW_RUNTIME_INSTALL_DIR}" CACHE INTERNAL "Path the tool executables are installed to.")

add_subdirectory(assimp)

ivw_move_targets_in_dir_to_folder(${CMAKE_CURRENT_SOURCE_DIR} ext/assimp)
ivw_get_targets_in_dir_recursive(targets ${CMAKE_CURRENT_SOURCE_DIR})
ivw_suppress_compiler_warnings(${targets})
# Required when INSTALL_NAME_DIR is set
# https://blog.kitware.com/upcoming-in-cmake-2-8-12-osx-rpath-support/
set_target_properties(${targets} PROPERTIES MACOSX_RPATH ON)

set(incdir ${CMAKE_CURRENT_SOURCE_DIR}/assimp/include/assimp)
set(codedir ${CMAKE_CURRENT_SOURCE_DIR}/assimp/code)

ivw_target_precompile_headers(assimp PRIVATE 
    $<$<COMPILE_LANGUAGE:CXX>:${incdir}/types.h>
    $<$<COMPILE_LANGUAGE:CXX>:${incdir}/BaseImporter.h>
    $<$<COMPILE_LANGUAGE:CXX>:${incdir}/Exceptional.h>
    $<$<COMPILE_LANGUAGE:CXX>:${incdir}/TinyFormatter.h>
    $<$<COMPILE_LANGUAGE:CXX>:${codedir}/Step/STEPFile.h>
    $<$<COMPILE_LANGUAGE:CXX>:${incdir}/DefaultLogger.hpp>

    $<$<COMPILE_LANGUAGE:CXX>:${incdir}/material.h>
    $<$<COMPILE_LANGUAGE:CXX>:${incdir}/anim.h>
    $<$<COMPILE_LANGUAGE:CXX>:${incdir}/light.h>
    $<$<COMPILE_LANGUAGE:CXX>:${incdir}/texture.h>
    $<$<COMPILE_LANGUAGE:CXX>:${incdir}/camera.h>
    $<$<COMPILE_LANGUAGE:CXX>:${incdir}/scene.h>
    $<$<COMPILE_LANGUAGE:CXX>:${incdir}/metadata.h>
    $<$<COMPILE_LANGUAGE:CXX>:${incdir}/IOSystem.hpp>
    $<$<COMPILE_LANGUAGE:CXX>:${incdir}/Importer.hpp>
    $<$<COMPILE_LANGUAGE:CXX>:${incdir}/Hash.h>
    $<$<COMPILE_LANGUAGE:CXX>:${incdir}/GenericProperty.h>
    $<$<COMPILE_LANGUAGE:CXX>:${codedir}/Common/BaseProcess.h>
    $<$<COMPILE_LANGUAGE:CXX>:${incdir}/postprocess.h>
    $<$<COMPILE_LANGUAGE:CXX>:${incdir}/SpatialSort.h>
    $<$<COMPILE_LANGUAGE:CXX>:${incdir}/irrXMLWrapper.h>
    $<$<COMPILE_LANGUAGE:CXX>:${codedir}/PostProcessing/ProcessHelper.h>
    $<$<COMPILE_LANGUAGE:CXX>:${incdir}/DefaultIOSystem.h>
    $<$<COMPILE_LANGUAGE:CXX>:${incdir}/SceneCombiner.h>
    $<$<COMPILE_LANGUAGE:CXX>:${incdir}/Exporter.hpp>
    $<$<COMPILE_LANGUAGE:CXX>:${incdir}/cexport.h>
    $<$<COMPILE_LANGUAGE:CXX>:${codedir}/X3D/FIReader.hpp>
    $<$<COMPILE_LANGUAGE:CXX>:${incdir}/qnan.h>
    $<$<COMPILE_LANGUAGE:CXX>:${incdir}/MemoryIOWrapper.h>
    $<$<COMPILE_LANGUAGE:CXX>:${codedir}/PostProcessing/ConvertToLHProcess.h>
    $<$<COMPILE_LANGUAGE:CXX>:${codedir}/X3D/X3DImporter.hpp>
    $<$<COMPILE_LANGUAGE:CXX>:${codedir}/X3D/X3DImporter_Node.hpp>
    $<$<COMPILE_LANGUAGE:CXX>:${codedir}/Importer/IFC/IFCReaderGen_2x3.h>
    $<$<COMPILE_LANGUAGE:CXX>:${codedir}/FBX/FBXImporter.h>
    $<$<COMPILE_LANGUAGE:CXX>:${codedir}/FBX/FBXImportSettings.h>
    $<$<COMPILE_LANGUAGE:CXX>:${codedir}/FBX/FBXUtil.h>
    $<$<COMPILE_LANGUAGE:CXX>:${incdir}/version.h>
    $<$<COMPILE_LANGUAGE:CXX>:${codedir}/X3D/X3DImporter_Macro.hpp>
    $<$<COMPILE_LANGUAGE:CXX>:${codedir}/Common/Importer.h>
)

# debug
#ivw_print_targets_in_dir_recursive(${CMAKE_CURRENT_SOURCE_DIR})
