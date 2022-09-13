function(L4_get_kernel_dir VARIABLE)
    execute_process(COMMAND uname -r OUTPUT_VARIABLE UNAME OUTPUT_STRIP_TRAILING_WHITESPACE)
    set(${VARIABLE} "/lib/modules/${UNAME}/build")

    find_file(KERNEL_MAKEFILE NAMES Makefile PATHS ${${VARIABLE}} NO_DEFAULT_PATH)
    if(NOT KERNEL_MAKEFILE)
        message(FATAL_ERROR "No Makefile was found for the kernel ${UNAME} in ${${VARIABLE}}!")
    endif()

    L4_hoist_variable(${VARIABLE})
endfunction()

function(L4_get_driver_sign_command VARIABLE)
    execute_process(COMMAND uname -r OUTPUT_VARIABLE UNAME OUTPUT_STRIP_TRAILING_WHITESPACE)
    set(${VARIABLE} /usr/src/kernels/${UNAME}/scripts/sign-file sha256 $ENV{KBUILD_MOK_PATH}.priv $ENV{KBUILD_MOK_PATH}.der)

    L4_hoist_variable(${VARIABLE})
endfunction()