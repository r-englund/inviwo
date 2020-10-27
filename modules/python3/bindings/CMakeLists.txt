# Create python module
set(HEADER_FILES
    include/inviwopy/inviwopy.h
    include/inviwopy/pybuffer.h
    include/inviwopy/pycamera.h
    include/inviwopy/pydataformat.h
    include/inviwopy/pydatamapper.h
    include/inviwopy/pyevent.h
    include/inviwopy/pyflags.h
    include/inviwopy/pyglmmattypes.h
    include/inviwopy/pyglmtypes.h
    include/inviwopy/pyimage.h
    include/inviwopy/pyinviwoapplication.h
    include/inviwopy/pyinviwomodule.h
    include/inviwopy/pylogging.h
    include/inviwopy/pymesh.h
    include/inviwopy/pynetwork.h
    include/inviwopy/pypickingmapper.h
    include/inviwopy/pyport.h
    include/inviwopy/pyprocessors.h
    include/inviwopy/pyproperties.h
    include/inviwopy/pypropertyowner.h
    include/inviwopy/pytfprimitiveset.h
    include/inviwopy/pyvolume.h
    include/inviwopy/vectoridentifierwrapper.h
)
ivw_group("Header Files" BASE ${CMAKE_CURRENT_SOURCE_DIR}/include/inviwopy ${HEADER_FILES})

set(SOURCE_FILES
    src/inviwopy.cpp
    src/pybuffer.cpp
    src/pycamera.cpp
    src/pydataformat.cpp
    src/pydatamapper.cpp
    src/pyevent.cpp
    src/pyflags.cpp
    src/pyglmmattypes.cpp
    src/pyglmtypes.cpp
    src/pyimage.cpp
    src/pyinviwoapplication.cpp
    src/pyinviwomodule.cpp
    src/pylogging.cpp
    src/pymesh.cpp
    src/pynetwork.cpp
    src/pypickingmapper.cpp
    src/pyport.cpp
    src/pyprocessors.cpp
    src/pyproperties.cpp
    src/pypropertyowner.cpp
    src/pytfprimitiveset.cpp
    src/pyvolume.cpp
    src/vectoridentifierwrapper.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})

ivw_add_py_wrapper(inviwopy ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(inviwopy PUBLIC inviwo::core inviwo::module::python3)
target_include_directories(inviwopy PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

ivw_target_precompile_headers(inviwopy PRIVATE 
    ${IVW_INCLUDE_DIR}/inviwo/core/common/inviwo.h
    ${IVW_INCLUDE_DIR}/inviwo/core/common/inviwoapplication.h
    ${IVW_INCLUDE_DIR}/inviwo/core/util/glm.h
    ${IVW_INCLUDE_DIR}/inviwo/core/processors/processor.h
    ${IVW_INCLUDE_DIR}/inviwo/core/properties/property.h
    ${IVW_INCLUDE_DIR}/inviwo/core/processors/processorobserver.h
    ${IVW_INCLUDE_DIR}/inviwo/core/util/glmvec.h
    ${IVW_INCLUDE_DIR}/inviwo/core/properties/valuewrapper.h
    ${IVW_INCLUDE_DIR}/inviwo/core/processors/canvasprocessor.h
    ${IVW_INCLUDE_DIR}/inviwo/core/io/serialization/serializer.h
    ${IVW_INCLUDE_DIR}/inviwo/core/util/formats.h
    ${IVW_INCLUDE_DIR}/inviwo/core/util/settings/settings.h

    <pybind11/pybind11.h>
    <pybind11/stl.h>
    <pybind11/functional.h>

)

ivw_default_install_targets(inviwopy)
