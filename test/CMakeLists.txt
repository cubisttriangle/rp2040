if (TARGET tinyusb_device)
    set(THIS_TARGET "test")
    add_executable(${THIS_TARGET}
            test.c
            i2c.h
            i2c.c
	    zoe-m8q.h
	    zoe-m8q.c
            )

    # pull in common dependencies
    target_link_libraries(${THIS_TARGET} pico_stdlib hardware_i2c)

    # enable usb output, disable uart output
    pico_enable_stdio_usb(${THIS_TARGET} 1)
    pico_enable_stdio_uart(${THIS_TARGET} 0)

    # create map/bin/hex/uf2 file etc.
    pico_add_extra_outputs(${THIS_TARGET})
elseif(PICO_ON_DEVICE)
    message(WARNING "not building ${THIS_TARGET} because TinyUSB submodule is not initialized in the SDK")
endif()

