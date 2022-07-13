# ostrich-os

## Build

```
$ make clean && make build
```

## Flash

```
$ make flash
```
## Debug

```
$ openocd -f stlink_bluepill.cfg &
$ arm-none-eabi-gdb build/ostrich-os.elf
(gdb) target remote :3333
```

Reset: ```monitor reset halt```

## References

- https://www.st.com/en/embedded-software/stsw-stm32054.html
- https://www.youtube.com/watch?v=O0Z1D6p7J5A
- https://blog.stratifylabs.dev/device/2013-10-09-Context-Switching-on-the-Cortex-M3/
- https://github.com/adamheinrich/os.h