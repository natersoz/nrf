###
# File: nrf/post_elf_build.cmake
# @copyright (c) 2026, natersoz. Distributed under the Apache 2.0 license.
#
# ARM Cortex M/R bare metal tool chain.
###

# For embedded executables, use the '.elf' suffix.
set(CMAKE_EXECUTABLE_SUFFIX ".elf")

# Create a Intel .hex file for flashing targets, as well as other information.
function(add_firmware_postbuild target)

    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${target}>

        COMMAND ${CMAKE_OBJCOPY} -O ihex
                $<TARGET_FILE:${target}>
                $<TARGET_FILE_DIR:${target}>/$<TARGET_FILE_BASE_NAME:${target}>.hex

        COMMAND ${CMAKE_OBJDUMP} -S
            $<TARGET_FILE:${target}> >
            $<TARGET_FILE_DIR:${target}>/$<TARGET_FILE_BASE_NAME:${target}>.lst

        COMMAND ${CMAKE_OBJDUMP} -tC
            $<TARGET_FILE:${target}> >
            $<TARGET_FILE_DIR:${target}>/$<TARGET_FILE_BASE_NAME:${target}>.symbols.txt

        COMMAND ${CMAKE_SIZE}
            $<TARGET_FILE:${target}> >>
            $<TARGET_FILE_DIR:${target}>/$<TARGET_FILE_BASE_NAME:${target}>.symbols.txt

        COMMENT "Generating HEX, LST, and symbol files for ${target}"
    )

    set(BUILT_EXE_ARTIFACTS
        $<TARGET_FILE_DIR:${target}>/$<TARGET_FILE_BASE_NAME:${target}>.map
        $<TARGET_FILE_DIR:${target}>/$<TARGET_FILE_BASE_NAME:${target}>.hex
        $<TARGET_FILE_DIR:${target}>/$<TARGET_FILE_BASE_NAME:${target}>.lst
        $<TARGET_FILE_DIR:${target}>/$<TARGET_FILE_BASE_NAME:${target}>.symbols.txt
    )

    set_target_properties(${target} PROPERTIES ADDITIONAL_CLEAN_FILES "${BUILT_EXE_ARTIFACTS}")

    message(DEBUG "Creating rm_linked_target for: '${target}'")
    message(DEBUG ${BUILT_EXE_ARTIFACTS})

    add_custom_target(rm_linked_target
        COMMAND ${CMAKE_COMMAND} -E remove
            "$<TARGET_FILE:${target}>"
            ${BUILT_EXE_ARTIFACTS}
        COMMENT "Removing built ${target} executable files"
        VERBATIM
    )

    add_custom_target(relink
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target rm_linked_target
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target ${target}
    )

endfunction()
