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
    AVRTask.cpp
    bwi.cpp
    capacitytimer.cpp
    cbserver.cpp
    cpu.cpp
    edfsched.cpp
    energyMRTKernel.cpp
    exeinstr.cpp
    fcfsresmanager.cpp
    feedback.cpp
    feedbacktest.cpp
    fifosched.cpp
    fpsched.cpp
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
    multisched.cpp
    piresman.cpp
    pollingserver.cpp
    powermodel.cpp
    powermodel_params.cpp
    ps_trace.cpp
    reginstr.cpp
    regsched.cpp
    regtask.cpp
    resmanager.cpp
    resource.cpp
    rmsched.cpp
    rrsched.cpp
    rttask.cpp
    schedinstr.cpp
    schedpoints.cpp
    schedrta.cpp
    scheduler.cpp
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
