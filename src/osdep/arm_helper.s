@ Some functions and tests to increase performance in drawing.cpp and custom.cpp

.arm

.global save_host_fp_regs
.global restore_host_fp_regs
.global copy_screen_8bit_to_16bit
.global copy_screen_8bit_to_32bit
.global copy_screen_16bit_swap
.global copy_screen_16bit_to_32bit
.global copy_screen_32bit_to_16bit
.global copy_screen_32bit_to_32bit

.text

.align 8

@----------------------------------------------------------------
@ save_host_fp_regs
@----------------------------------------------------------------
save_host_fp_regs:
	vstmia    r0!, {d7-d15}
  bx        lr

@----------------------------------------------------------------
@ restore_host_fp_regs
@----------------------------------------------------------------
restore_host_fp_regs:
  vldmia    r0!, {d7-d15}
  bx        lr


@----------------------------------------------------------------
@ copy_screen_8bit_to_16bit
@
@ r0: uae_u8   *dst
@ r1: uae_u8   *src
@ r2: int      bytes always a multiple of 64: even number of lines, number of pixel per line is multiple of 32 (320, 640, 800, 1024, 1152, 1280)
@ r3: uae_u32  *clut
@
@ void copy_screen_8bit_to_16bit(uae_u8 *dst, uae_u8 *src, int bytes, uae_u32 *clut);
@
@----------------------------------------------------------------
copy_screen_8bit_to_16bit:
  stmdb     sp!, {r4-r6, lr}
copy_screen_8bit_loop:
  pld       [r1, #192]
  mov       lr, #64
copy_screen_8bit_loop_2:
  ldr       r4, [r1], #4
  and       r5, r4, #255
  ldr       r6, [r3, r5, lsl #2]
  lsr r5, r4, #8
  and r5, r5, #255
  strh      r6, [r0], #2
  ldr       r6, [r3, r5, lsl #2]
  lsr r5, r4, #16
  and r5, r5, #255
  strh      r6, [r0], #2
  ldr       r6, [r3, r5, lsl #2]
  lsr r5, r4, #24
  strh      r6, [r0], #2
  ldr       r6, [r3, r5, lsl #2]
  subs      lr, lr, #4
  strh      r6, [r0], #2
  bgt       copy_screen_8bit_loop_2
  subs      r2, r2, #64
  bgt       copy_screen_8bit_loop
  ldmia     sp!, {r4-r6, pc}


@----------------------------------------------------------------
@ copy_screen_8bit_to_32bit
@
@ r0: uae_u8   *dst
@ r1: uae_u8   *src
@ r2: int      bytes always a multiple of 64: even number of lines, number of pixel per line is multiple of 32 (320, 640, 800, 1024, 1152, 1280)
@ r3: uae_u32  *clut
@
@ void copy_screen_8bit_to_32bit(uae_u8 *dst, uae_u8 *src, int bytes, uae_u32 *clut);
@
@----------------------------------------------------------------
copy_screen_8bit_to_32bit:
  stmdb     sp!, {r4-r5, lr}
copy_screen_8bit_to_32bit_loop:
  ldr       r4, [r1], #4
  subs      r2, r2, #4
  and r5, r4, #255
  ldr       lr, [r3, r5, lsl #2]
  lsr r5, r4, #8
  and r5, r5, #255
  str       lr, [r0], #4
  ldr       lr, [r3, r5, lsl #2]
  lsr r5, r4, #16
  and r5, r5, #255
  str       lr, [r0], #4
  ldr       lr, [r3, r5, lsl #2]
  lsr r5, r4, #24
  and r5, r5, #255
  str       lr, [r0], #4
  ldr       lr, [r3, r5, lsl #2]
  str       lr, [r0], #4
  bgt       copy_screen_8bit_to_32bit_loop
  ldmia     sp!, {r4-r5, pc}


@----------------------------------------------------------------
@ copy_screen_16bit_swap
@
@ r0: uae_u8   *dst
@ r1: uae_u8   *src
@ r2: int      bytes always a multiple of 128: even number of lines, 2 bytes per pixel, number of pixel per line is multiple of 32 (320, 640, 800, 1024, 1152, 1280)
@
@ void copy_screen_16bit_swap(uae_u8 *dst, uae_u8 *src, int bytes);
@
@----------------------------------------------------------------
copy_screen_16bit_swap:
ldr r3, [r1], #4
rev16 r3, r3
str r3, [r0], #4
subs r2, r2, #4
bne copy_screen_16bit_swap
bx lr


@----------------------------------------------------------------
@ copy_screen_16bit_to_32bit
@
@ r0: uae_u8   *dst
@ r1: uae_u8   *src
@ r2: int      bytes always a multiple of 128: even number of lines, 2 bytes per pixel, number of pixel per line is multiple of 32 (320, 640, 800, 1024, 1152, 1280)
@
@ void copy_screen_16bit_to_32bit(uae_u8 *dst, uae_u8 *src, int bytes);
@
@----------------------------------------------------------------
copy_screen_16bit_to_32bit:
  stmdb     sp!, {r4, lr}
copy_screen_16bit_to_32bit_loop:
  ldrh      r3, [r1], #2
  subs      r2, r2, #2
  rev16     r3, r3
  and       lr, r3, #31
  lsl       lr, lr, #3
  lsr       r3, r3, #5
  and       r4, r3, #63
  orr       lr, lr, r4, lsl #10
  lsr       r3, r3, #6
  and       r4, r3, #31
  orr       lr, lr, r4, lsl #19
  str       lr, [r0], #4
  bne       copy_screen_16bit_to_32bit_loop
  ldmia     sp!, {r4, pc}


@----------------------------------------------------------------
@ copy_screen_32bit_to_16bit
@
@ r0: uae_u8   *dst - Format (bits): rrrr rggg gggb bbbb
@ r1: uae_u8   *src - Format (bytes) in memory rgba
@ r2: int      bytes
@
@ void copy_screen_32bit_to_16bit(uae_u8 *dst, uae_u8 *src, int bytes);
@
@----------------------------------------------------------------
copy_screen_32bit_to_16bit:
stmdb sp!, {r4-r6, lr}
copy_screen_32bit_to_16bit_loop:
ldr r3, [r1], #4
rev r3, r3
lsr r4, r3, #19
lsr r5, r3, #10
and r5, r5, #63
lsr r6, r3, #3
and r6, r6, #31
orr r6, r6, r5, lsl #5
orr r6, r6, r4, lsl #11
strh r6, [r0], #2
subs r2, r2, #4
bne copy_screen_32bit_to_16bit_loop
ldmia sp!, {r4-r6, pc}


@----------------------------------------------------------------
@ copy_screen_32bit_to_32bit
@
@ r0: uae_u8   *dst - Format (bytes): in memory rgba
@ r1: uae_u8   *src - Format (bytes): in memory rgba
@ r2: int      bytes
@
@ void copy_screen_32bit_to_32bit(uae_u8 *dst, uae_u8 *src, int bytes);
@
@----------------------------------------------------------------
copy_screen_32bit_to_32bit:
  ldr       r3, [r1], #4
  subs      r2, r2, #4
  str       r3, [r0], #4
  bne       copy_screen_32bit_to_32bit
  bx        lr
