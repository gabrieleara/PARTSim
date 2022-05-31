set(LIBRARY_NAME                    rtsim)
set(LIBRARY_TYPE                    SHARED)
set(LIBRARY_VERSION                 3.0)
set(LIBRARY_SOVERSION               3)
set(LIBRARY_INCLUDEDIR              include)
set(LIBRARY_INCLUDEDIR_PRIVATE      )
set(LIBRARY_INCLUDEDIR_INTERFACE    )
set(LIBRARY_PROPERTIES              )
set(LIBRARY_DEPENDENCIES            metasim)

set(LIBRARY_SOURCE_FILES
    resource/fcfsresmanager.cpp
    resource/resmanager.cpp
    resource/resource.cpp
    # resource/bwi.cpp                # Not ready for use
    # resource/piresmanager.cpp       # Not ready for use

    # TODO: check schedulers organization
    scheduler/edfsched.cpp
    scheduler/fifosched.cpp
    scheduler/fpsched.cpp
    scheduler/rmsched.cpp
    scheduler/rrsched.cpp
    scheduler/scheduler.cpp
    # Check this one
    scheduler/regsched.cpp
    # Check this one
    scheduler/multisched.cpp

    AVRTask.cpp
    capacitytimer.cpp
    cbserver.cpp
    cpu.cpp
    energyMRTKernel.cpp
    exeinstr.cpp
    feedback.cpp
    feedbacktest.cpp
    grubserver.cpp
    instr.cpp
    interrupt.cpp
    json_trace.cpp
    jtrace.cpp
    kernel.cpp
    kernevt.cpp
    load.cpp
    migrationmanager.cpp
    mrtkernel.cpp
    pollingserver.cpp
    powermodel.cpp
    powermodel_params.cpp
    ps_trace.cpp
    reginstr.cpp
    regtask.cpp
    rttask.cpp
    schedinstr.cpp
    schedpoints.cpp
    schedrta.cpp
    server.cpp
    sparepot.cpp
    sporadicserver.cpp
    supercbs.cpp
    suspend_instr.cpp
    system.cpp
    system_descriptor.cpp
    task.cpp
    taskevt.cpp
    texttrace.cpp
    threinstr.cpp
    timer.cpp
    traceevent.cpp
    tracepower.cpp
    waitinstr.cpp
    yaml.cpp
)

option(RTLIB_THERMAL "Enables the thermal modeling subsystem" OFF)
if (RTLIB_THERMAL EQUAL ON)
    find_package (Eigen3 3.3 REQUIRED NO_MODULE)
    set(LIBRARY_DEPENDENCIES ${LIBRARY_DEPENDENCIES} Eigen3::Eigen)
endif()

include(${PROJECT_SOURCE_DIR}/cmakeopts/library.cmake)