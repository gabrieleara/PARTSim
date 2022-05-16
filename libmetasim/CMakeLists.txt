set(LIBRARY_NAME                    metasim)
set(LIBRARY_TYPE                    SHARED)
set(LIBRARY_VERSION                 3.0)
set(LIBRARY_SOVERSION               3)
set(LIBRARY_INCLUDEDIR              include)
set(LIBRARY_INCLUDEDIR_PRIVATE      )
set(LIBRARY_INCLUDEDIR_INTERFACE    )
set(LIBRARY_PROPERTIES              )
set(LIBRARY_DEPENDENCIES            )

set(LIBRARY_SOURCE_FILES
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
    
    # set(HEADER_FILES
    #     baseexc.hpp
    #     basestat.hpp
    #     basetype.hpp
    #     cloneable.hpp
    #     debugstream.hpp
    #     entity.hpp
    #     event.hpp
    #     factory.hpp
    #     genericvar.hpp
    #     gevent.hpp
    #     history.hpp
    #     metasim.hpp
    #     memory.hpp
    #     particle.hpp
    #     plist.hpp
    #     randomvar.hpp
    #     regvar.hpp
    #     simul.hpp
    #     strtoken.hpp
    #     tick.hpp
    #     trace.hpp
    # )

include(${PROJECT_SOURCE_DIR}/cmakeopts/library.cmake)
