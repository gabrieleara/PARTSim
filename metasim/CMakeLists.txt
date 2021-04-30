cmake_minimum_required(VERSION 3.0)

if (CMAKE_BUILD_TYPE MATCHES "^[Dd]ebug")
    add_definitions(-D__DEBUG__)
    message("Debug added as a compilation flag")
elseif (CMAKE_BUILD_TYPE MATCHES "^[Rr]elease")
    add_definitions(-DNDEBUG)
endif()

set(LIBRARY_NAME metasim)

# TODO: SHARED or STATIC?
set(LIBRARY_TYPE SHARED)

set(LIBRARY_VERSION 3.0)
set(LIBRARY_SOVERSION 3)

set(LIBRARY_DEPENDENCIES )

set(LIBRARY_INCLUDEDIR include)

set(SOURCE_FILES
    basestat.cpp
    debugstream.cpp
    entity.cpp
    event.cpp
    genericvar.cpp
    randomvar.cpp
    regvar.cpp
    simul.cpp
    strtoken.cpp
    tick.cpp
    trace.cpp
)

set(HEADER_FILES
    baseexc.hpp
    basestat.hpp
    basetype.hpp
    cloneable.hpp
    debugstream.hpp
    entity.hpp
    event.hpp
    factory.hpp
    genericvar.hpp
    gevent.hpp
    history.hpp
    metasim.hpp
    memory.hpp
    particle.hpp
    plist.hpp
    randomvar.hpp
    regvar.hpp
    simul.hpp
    strtoken.hpp
    tick.hpp
    trace.hpp
)

# ---------- You should not need to change anything below this line ---------- #

include(${PROJECT_SOURCE_DIR}/CMakeLists_library.txt)
