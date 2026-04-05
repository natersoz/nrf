###
# File: nrf/post_elf_build.cmake
# @copyright (c) 2026, natersoz. Distributed under the Apache 2.0 license.
#
# ARM Cortex M/R bare metal tool chain.
###

# Create a Intel .hex file for flashing targets, as well as other information.
function(add_firmware_postbuild target)

    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${target}>

        COMMAND ${CMAKE_OBJCOPY} -O ihex
            $<TARGET_FILE:${target}> $<TARGET_FILE:${target}>.hex

        COMMAND ${CMAKE_OBJDUMP} -S
            $<TARGET_FILE:${target}> > $<TARGET_FILE:${target}>.lst

        COMMAND ${CMAKE_OBJDUMP} -tC
            $<TARGET_FILE:${target}> > $<TARGET_FILE:${target}>.symbols.txt

        COMMAND ${CMAKE_SIZE}
            $<TARGET_FILE:${target}> >> $<TARGET_FILE:${target}>.symbols.txt

        COMMENT "Generating HEX, LST, and symbol files for ${target}"
    )

    add_custom_target(rm_linked_target
        COMMAND ${CMAKE_COMMAND} -E remove
            "$<TARGET_FILE:${target}>"
            "$<TARGET_FILE:${target}>.hex"
            "$<TARGET_FILE:${target}>.lst"
            "$<TARGET_FILE:${target}>.symbols.txt"
            "$<TARGET_FILE:${target}>.map"
        COMMENT "Removing built ${target} executable files"
        VERBATIM
    )

    add_custom_target(relink
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target rm_linked_target
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target ${target}
    )

endfunction()
