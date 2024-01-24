# This is the default entrypoint. It does any required initialization, then
# jumps into the main function to begin execution.
# Once the main function returns, an EBREAK instruction is executed.
# This can be re-used by different examples. Each example must define a main function.

.text
.global _start
.type   _start, @function
.type   main, @function

_start:

# Initialize the global pointer register.
# Resources:
# https://five-embeddev.com/quickref/global_pointer.html
# https://www.sifive.com/blog/all-aboard-part-3-linker-relaxation-in-riscv-toolchain
# https://github.com/sifive/freedom-metal/blob/fa026d2ee08e5ba49e8ae703fb4cbcbb710a6a69/src/entry.S
# https://github.com/johnwinans/rvddt/blob/master/examples/freestanding/stand03/crt0.S
# https://philmulholland.medium.com/modern-c-for-bare-metal-risc-v-zero-to-blink-part-3-startup-bbdb288be681
# https://twilco.github.io/riscv-from-scratch/2019/04/27/riscv-from-scratch-2.html

.option push
.option norelax
    la gp, __global_pointer$
.option pop

# Call main

la x1, main
jalr x1, x1, 0

# Trigger EBREAK after program finishes

ebreak
