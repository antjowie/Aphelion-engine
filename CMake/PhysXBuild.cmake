function(BuildPhysX PX_ROOT_DIR BUILD_PX_SAMPLES)


# set(PX_ROOT_DIR ${CMAKE_SOURCE_DIR}/lib)

# string(TOLOWER ${TARGET_BUILD_PLATFORM} TARGET_BUILD_PLATFORM)
# message(STATUS "Building on platform: ${TARGET_BUILD_PLATFORM}")

# Fetch physx (CMake phcerdan branch)
# include(FetchContent)
# FetchContent_Declare(
    # physx
    # GIT_REPOSITORY https://github.com/phcerdan/PhysX
    # GIT_TAG cmake_for_easier_integration
    # )
    # FetchContent_GetProperties(physx)
    # if(NOT physx_POPULATED)
    # message(STATUS "  Populating PhysX...")
    # FetchContent_Populate(physx)
    # message(STATUS "  Configuring PhysX...")
    string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_CONFIG) 

    set(NV_USE_DEBUG_WINCRT ON)
    if({CMAKE_BUILD_TYPE} STREQUAL Release)
        set(NV_USE_DEBUG_WINCRT OFF)
    endif()

    if(${CMAKE_BUILD_TYPE} STREQUAL RelWithDebInfo)
        set(CMAKE_BUILD_TYPE release)
        # message(FATAL_ERROR "PhysX doesn't support this configuration (or it is called checked). This behavior has yet to be implemented")
    endif()

    # message(WARNING "Value of debug wincrt:${NV_USE_DEBUG_WINCRT}")
    message(STATUS "Building PhysX... with CONFIG: ${CMAKE_BUILD_TYPE}")
    execute_process(
        COMMAND 
            ${CMAKE_COMMAND}
            -S ${PX_ROOT_DIR}/physx
            -B ${CMAKE_BINARY_DIR}/physx
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DCMAKE_GENERATOR=${CMAKE_GENERATOR}
            -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
            -DCMAKE_MSVC_RUNTIME_LIBRARY=${CMAKE_MSVC_RUNTIME_LIBRARY}
            -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            -DNV_USE_DEBUG_WINCRT=${NV_USE_DEBUG_WINCRT}
            -DNV_USE_STATIC_WINCRT=ON

        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/physx
        COMMAND_ECHO STDOUT
        # OUTPUT_FILE       ${CMAKE_BINARY_DIR}/physx/configure_output.log
        # ERROR_FILE        ${CMAKE_BINARY_DIR}/physx/configure_output.log
        RESULT_VARIABLE   result_config
    )
    if(result_config)
        message(FATAL_ERROR "Failed PhysX configuration")
        # see configuration log at:\n    ${physx_BINARY_DIR}/configure_output.log")
    endif()

    execute_process(
        COMMAND 
            ${CMAKE_COMMAND}
            --build ${CMAKE_BINARY_DIR}/physx
            --config ${CMAKE_BUILD_TYPE}

        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/physx
        COMMAND_ECHO STDOUT
        # OUTPUT_FILE       ${CMAKE_BINARY_DIR}/physx/build_output.log
        # ERROR_FILE        ${CMAKE_BINARY_DIR}/physx/build_output.log
        RESULT_VARIABLE   result_build
    )
    if(result_build)
        message(FATAL_ERROR "Failed PhysX build")
        # see build log at:\n    ${physx_BINARY_DIR}/build_output.log")
    endif()
    message(STATUS "PhysX build complete")

    # NOTE: This just calls the original build script that PhysX supplies
    # You may want to build it elsewhere
    if(BUILD_PX_SAMPLES)
        set(TARGET_BUILD_PLATFORM ${CMAKE_SYSTEM_NAME})
        message(STATUS "Building PhysX samples...")
        # message(STATUS "Building on platform: ${TARGET_BUILD_PLATFORM}")
        string(TOLOWER ${TARGET_BUILD_PLATFORM} TARGET_BUILD_PLATFORM)
        
        # Set the right preset for PhysX
        # TODO: Only windows has been tested
        # I rely on the build tool supplied by PhysX since it sets some 
        # variables for us based on the platform. I let the build script set those
        if(TARGET_BUILD_PLATFORM STREQUAL windows)
            set(PX_PRESET vc16win64)
        else()
            message(SEND_ERROR "Can't defer platform to build PhysX for. Current platform is ${TARGET_BUILD_PLATFORM}")
        endif()

        execute_process(
            COMMAND 
                ${PX_ROOT_DIR}/physx/generate_projects.bat ${PX_PRESET}

            # WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/physx
            COMMAND_ECHO STDOUT
            RESULT_VARIABLE   result_samples_script
        )
        if(result_samples_script)
            message(FATAL_ERROR "Failed PhysX generate project script")
        endif()
        
        # Generate projects takes care of it. Just switch to that project
        # execute_process(
        #     COMMAND
        #         ${CMAKE_COMMAND} 
        #         -S ${PX_ROOT_DIR}/physx/compiler/public
        #         -B ${PX_ROOT_DIR}/physx/compiler/${PX_PRESET}
        #         -UPX_BUILDSNIPPETS -DPX_BUILDSNIPPETS=ON
        #         -UPX_BUILDPUBLICSAMPLES -DPX_BUILDPUBLICSAMPLES=ON

        #     # WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/physx
        #     COMMAND_ECHO STDOUT
        #     RESULT_VARIABLE   result_samples_config
        # )
        # if(result_samples_config)
        #     message(FATAL_ERROR "Failed PhysX samples configuration")
        # endif()

        # execute_process(
        #     COMMAND 
        #         ${CMAKE_COMMAND} 
        #         --build ${PX_ROOT_DIR}/physx/compiler/${PX_PRESET}

        #     # WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/physx
        #     COMMAND_ECHO STDOUT
        #     RESULT_VARIABLE   result_samples_build
        # )
        # if(result_samples_build)
        #     message(FATAL_ERROR "Failed PhysX samples build")
        #     # see build log at:\n    ${physx_BINARY_DIR}/build_output.log")
        # endif()
    endif()
    message(STATUS "PhysX snippets build complete")
    # add_subdirectory(${PX_ROOT_DIR}/physx ${CMAKE_BINARY_DIR}/physx)
    # # create rule to install PhysX when installing this project
    # install (CODE "
    # execute_process(
    # COMMAND ${CMAKE_COMMAND}
    # --build ${physx_BINARY_DIR}
    # --config ${PHYSX_CONFIG_TYPE}
    # --target install
    # WORKING_DIRECTORY ${physx_BINARY_DIR}
    # COMMAND_ECHO STDOUT
    # )")

    set(PhysX_DIR ${CMAKE_BINARY_DIR}/physx/sdk_source_bin PARENT_SCOPE)
endfunction()