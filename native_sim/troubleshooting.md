# Setup
- Virtual machine with ubuntu 20.04
- Zephyr 4.0 (for LVGL 8.4)
- Visual Studio Code
	- Task Buttons extension

Build:
```
source ~/zephyrproject/.venv/bin/activate && west build -b native_sim applications/StartWeekAvans25/native_sim
```
Run:
```
west build -t run
```

Above commands can be added as tasks to vs code for one-step building and running.

# GDB
gdb ./build/zephyr/zephyr.exe
https://ftp.gnu.org/old-gnu/Manuals/gdb/html_node/gdb_42.html#SEC43

# Troubleshooting
## Black display
- Clear build folder and build again
- Have "display_blanking_off(display_dev);"

## asm/errno.h: No such file or directory
```
/usr/include/linux/errno.h:1:10: fatal error: asm/errno.h: No such file or directory
    1 | #include <asm/errno.h>
      |          ^~~~~~~~~~~~~
compilation terminated.
ninja: build stopped: subcommand failed.
FATAL ERROR: command exited with status 1: /usr/bin/cmake --build /home/npc/zephyrproject/zephyr/build

```

sudo apt-get install gcc-multilib

## SDL2/_real_SDL_config.h: No such file or directory

```
/usr/include/SDL2/SDL_config.h:4:10: fatal error: SDL2/_real_SDL_config.h: No such file or directory
    4 | #include <SDL2/_real_SDL_config.h>
      |          ^~~~~~~~~~~~~~~~~~~~~~~~~
compilation terminated.
```
sudo apt install libsdl2-dev:i386

## GPIO emul
```
[00:00:00.000,000] <inf> gpio_emul_sdl: GPIO sdl_gpio:0 = 30
[00:00:00.000,000] <inf> gpio_emul_sdl: GPIO sdl_gpio:1 = 31
[00:00:00.000,000] <inf> gpio_emul_sdl: GPIO sdl_gpio:2 = 32
[00:00:00.000,000] <inf> sdl_input: Init 'input-sdl-touch' device
[00:00:00.000,000] <err> gpio_keys: interrupt configuration failed: -95
[00:00:00.000,000] <err> gpio_keys: Pin 0 interrupt configuration failed: -95
*** Booting Zephyr OS build v4.0.0-45-g7d2ac022543d ***
[00:00:00.000,000] <err> app: Failed to enable button callback: -95

gpio_emul0: gpio_emul_0 {
		status = "okay";
		compatible = "zephyr,gpio-emul";
		rising-edge;
		falling-edge;
		high-level;
		low-level;
		gpio-controller;
		ngpios = <3>; // Number of GPIO pins
		#gpio-cells = <2>;
	
	};
	Needs rising-edge; to low-level;
```
## Undefined reference
```
/home/npc/zephyrproject/applications/StartWeekAvans25/native_sim/src/main.c: In function âmainâ:
/home/npc/zephyrproject/applications/StartWeekAvans25/native_sim/src/main.c:46:12: warning: unused variable âhello_world_labelâ [-Wunused-variable]
   46 |  lv_obj_t *hello_world_label;
      |            ^~~~~~~~~~~~~~~~~
At top level:
/home/npc/zephyrproject/applications/StartWeekAvans25/native_sim/src/main.c:36:13: warning: âlv_btn_click_callbackâ defined but not used [-Wunused-function]
   36 | static void lv_btn_click_callback(lv_event_t *e)
      |             ^~~~~~~~~~~~~~~~~~~~~
[3/6] Linking C executable zephyr/zephyr.elf
Generating files from /home/npc/zephyrproject/build/zephyr/zephyr.elf for board: native_sim
[4/6] Building native simulator runner, and linking final executable
FAILED: zephyr/CMakeFiles/native_runner_executable zephyr/zephyr.exe /home/npc/zephyrproject/build/zephyr/CMakeFiles/native_runner_executable /home/npc/zephyrproject/build/zephyr/zephyr.exe 
cd /home/npc/zephyrproject/build/zephyr && /usr/bin/make -f /home/npc/zephyrproject/zephyr/scripts/native_simulator/Makefile all --warn-undefined-variables -r NSI_CONFIG_FILE=/home/npc/zephyrproject/build/zephyr/NSI/nsi_config
/usr/bin/ld: /home/npc/zephyrproject/build/zephyr/NSI/home/npc/zephyrproject/build/zephyr/zephyr.elf.loc_cpusw.o: in function `main':
/home/npc/zephyrproject/applications/StartWeekAvans25/native_sim/src/main.c:92: undefined reference to `buttons4x4Get'
collect2: error: ld returned 1 exit status
make: *** [/home/npc/zephyrproject/zephyr/scripts/native_simulator/Makefile:131: /home/npc/zephyrproject/build/zephyr/zephyr.exe] Error 1
ninja: build stopped: subcommand failed.

```
Do a pristine rebuild to make sure the project gets build with new header files.

## Math not found
Clear build folder and build again.

## Segmentation faults
```
*** Booting Zephyr OS build v4.0.0-45-g7d2ac022543d ***
Segmentation fault (core dumped)
FAILED: zephyr/CMakeFiles/run_native /home/npc/zephyrproject/build/zephyr/CMakeFiles/run_native 
cd /home/npc/zephyrproject/build && /home/npc/zephyrproject/build/zephyr/zephyr.exe
ninja: build stopped: subcommand failed.
FATAL ERROR: command exited with status 1: /usr/bin/cmake --build /home/npc/zephyrproject/build --target run
```
gdb debugging
```
[New Thread 0xea4bbb40 (LWP 2528)]
[Thread 0xea4bbb40 (LWP 2525) exited]
[00:00:00.000,000] <inf> sdl_input: Init 'input-sdl-touch' device
[New Thread 0xe7ee5b40 (LWP 2529)]
*** Booting Zephyr OS build v4.0.0-45-g7d2ac022543d ***
[New Thread 0xe76e4b40 (LWP 2530)]
[New Thread 0xe6ee3b40 (LWP 2531)]

Thread 12 "main" received signal SIGSEGV, Segmentation fault.
[Switching to Thread 0xe9cbab40 (LWP 2526)]
--Type <RET> for more, q to quit, c to continue without paging--RET
0x080590d2 in lv_obj_get_state (obj=0x0)
    at /home/npc/zephyrproject/modules/lib/gui/lvgl/src/core/lv_obj.c:336
336	    return obj->state;
(gdb) 
--Type <RET> for more, q to quit, c to continue without paging--bt
0x0804c3f3 in cbvprintf_package (packaged=0x0, len=<optimized out>, flags=0, 
    fmt=0x8095248 "s state: %s\n", ap=0xe9abc1d8 "7R\t\bl\272\004\b`\267\n\b@")
    at /home/npc/zephyrproject/zephyr/lib/os/cbprintf_packaged.c:676
676	
```
Fixed by printing the char properly

```
[New Thread 0xea2bbb40 (LWP 2913)]
[00:00:00.000,000] <inf> sdl_input: Init 'input-sdl-touch' device
[New Thread 0xe7ce5b40 (LWP 2914)]
*** Booting Zephyr OS build v4.0.0-45-g7d2ac022543d ***
[New Thread 0xe74e4b40 (LWP 2915)]
[New Thread 0xe6ce3b40 (LWP 2916)]

Thread 12 "main" received signal SIGSEGV, Segmentation fault.
[Switching to Thread 0xe9abab40 (LWP 2911)]
0x080590d2 in lv_obj_get_state (obj=0x0)
    at /home/npc/zephyrproject/modules/lib/gui/lvgl/src/core/lv_obj.c:336
--Type <RET> for more, q to quit, c to continue without paging--bt
336	    return obj->state;
(gdb) 
```

Might be time to rename this entire segmentation fault part. It's kinda the catch-all error name.
```
Thread 12 "main" received signal SIGSEGV, Segmentation fault.
[Switching to Thread 0xe9abcb40 (LWP 17172)]
--Type <RET> for more, q to quit, c to continue without paging--bt
0x0805a803 in _lv_obj_get_ext_draw_size (obj=0x0)
    at /home/npc/zephyrproject/modules/lib/gui/lvgl/src/core/lv_obj_draw.c:385
385	    if(obj->spec_attr) return obj->spec_attr->ext_draw_size;
(gdb) bt
#0  0x0805a803 in _lv_obj_get_ext_draw_size (obj=0x0)
    at /home/npc/zephyrproject/modules/lib/gui/lvgl/src/core/lv_obj_draw.c:385
#1  0x0805b723 in lv_obj_invalidate (obj=0x0)
    at /home/npc/zephyrproject/modules/lib/gui/lvgl/src/core/lv_obj_pos.c:854
#2  0x08087710 in lv_label_set_text (obj=0x0, 
    text=0xe9abc200 "4320\252\377\252\252\252\252\252\252")
    at /home/npc/zephyrproject/modules/lib/gui/lvgl/src/widgets/lv_label.c:90
#3  0x0804b231 in set_segmented_display (input=<optimized out>, 
    dpPosition=<optimized out>)
    at /home/npc/zephyrproject/applications/StartWeekAvans25/native_sim/src/lvgl_ui.c:41
#4  0x0804b0a5 in sevenSegmentSet (
    input=0xe9abc200 "4320\252\377\252\252\252\252\252\252", 
    dpPosition=0 '\000')
    at /home/npc/zephyrproject/applications/StartWeekAvans25/native_sim/src/hardware.c:263
#5  0x0804bb4e in main ()
    at /home/npc/zephyrproject/applications/StartWeekAvans25/native_sim/src/main.c:78
#6  0x0808d5fc in bg_thread_main (unused1=0x0, unused2=0x0, unused3=0x0)
    at /home/npc/zephyrproject/zephyr/kernel/init.c:564
#7  0x0804c8a1 in z_thread_entry (entry=0x808d535 <bg_thread_main>, p1=0x0, 
    p2=0x0, p3=0x0) at /home/npc/zephyrproject/zephyr/lib/os/thread_entry.c:48
--Type <RET> for more, q to quit, c to continue without paging--
```
Fixed by creating the label properly.

```
*** Booting Zephyr OS build v4.0.0-45-g7d2ac022543d ***
[New Thread 0xe74e4b40 (LWP 3075)]
[New Thread 0xe6ce3b40 (LWP 3076)]

Thread 12 "main" received signal SIGSEGV, Segmentation fault.
[Switching to Thread 0xe98bbb40 (LWP 3071)]
--Type <RET> for more, q to quit, c to continue without paging--bt
0x0805e942 in get_local_style (obj=0x0, selector=0)
    at /home/npc/zephyrproject/modules/lib/gui/lvgl/src/core/lv_obj_style.c:543
543	    for(i = 0; i < obj->style_cnt; i++) {
(gdb) 
```
set_button(1,1); works.
Fixed by not allowing an usecase where a pointer points to null.

## Start button
Key 8:
```
[00:00:06.720,000] <err> gpio_emul: Pin not supported port_pin_mask=1fffffff mask=20000000
[00:00:06.720,000] <wrn> gpio_emul_sdl: Failed to emulate input (-22)
```
Key B:
```
[00:00:39.860,000] <err> gpio_emul: Pin not supported port_pin_mask=1fffffff mask=20000000
[00:00:39.860,000] <wrn> gpio_emul_sdl: Failed to emulate input (-22)
```
Fix: Make ngpio number match the used gpio pin amount.

## Missing semicolon
```
/home/npc/zephyrproject/zephyr/include/zephyr/sys/cbprintf.h:78:1: warning: empty declaration
   78 | union cbprintf_package_hdr {
      | ^~~~~
/home/npc/zephyrproject/zephyr/include/zephyr/sys/cbprintf.h:96:1: warning: empty declaration
   96 | struct cbprintf_package_hdr_ext {
      | ^~~~~~
In file included from /home/npc/zephyrproject/zephyr/include/zephyr/sys/cbprintf.h:124,
                 from /home/npc/zephyrproject/zephyr/include/zephyr/logging/log_msg.h:11,
                 from /home/npc/zephyrproject/zephyr/include/zephyr/logging/log_core.h:9,
                 from /home/npc/zephyrproject/zephyr/include/zephyr/logging/log.h:11,
                 from /home/npc/zephyrproject/applications/StartWeekAvans25/native_sim/src/main.c:18:
/home/npc/zephyrproject/zephyr/include/zephyr/sys/cbprintf_internal.h:56:1: error: expected â=â, â,â, â;â, âasmâ or â__attribute__â before â{â token
   56 | {
      | ^
/home/npc/zephyrproject/zephyr/include/zephyr/sys/cbprintf_enums.h:15:1: warning: empty declaration
   15 | enum cbprintf_package_arg_type {
      | ^~~~
/home/npc/zephyrproject/zephyr/include/zephyr/logging/log.h:443:24: error: storage class specified for parameter â__log_levelâ
  443 |  static const uint32_t __log_level __unused =         \
      |                        ^~~~~~~~~~~
/home/npc/zephyrproject/zephyr/include/zephyr/logging/log.h:396:2: note: in expansion of macro âLOG_MODULE_DECLAREâ
  396 |  LOG_MODULE_DECLARE(__VA_ARGS__)
      |  ^~~~~~~~~~~~~~~~~~
/home/npc/zephyrproject/applications/StartWeekAvans25/native_sim/src/main.c:19:1: note: in expansion of macro âLOG_MODULE_REGISTERâ
   19 | LOG_MODULE_REGISTER(app);
      | ^~~~~~~~~~~~~~~~~~~
/home/npc/zephyrproject/zephyr/include/zephyr/logging/log.h:436:16: error: parameter â__log_levelâ is initialized
  436 |  static struct log_source_dynamic_data *          \
      |                ^~~~~~~~~~~~~~~~~~~~~~~
/home/npc/zephyrproject/zephyr/include/zephyr/logging/log.h:396:2: note: in expansion of macro âLOG_MODULE_DECLAREâ
  396 |  LOG_MODULE_DECLARE(__VA_ARGS__)
      |  ^~~~~~~~~~~~~~~~~~
/home/npc/zephyrproject/applications/StartWeekAvans25/native_sim/src/main.c:19:1: note: in expansion of macro âLOG_MODULE_REGISTERâ
   19 | LOG_MODULE_REGISTER(app);
      | ^~~~~~~~~~~~~~~~~~~
/home/npc/zephyrproject/applications/StartWeekAvans25/native_sim/src/main.c:22:1: error: expected â=â, â,â, â;â, âasmâ or â__attribute__â before â{â token
   22 | {
      | ^
/home/npc/zephyrproject/applications/StartWeekAvans25/native_sim/src/main.c:29:1: error: expected â=â, â,â, â;â, âasmâ or â__attribute__â before â{â token
   29 | {
      | ^
In file included from /home/npc/zephyrproject/applications/StartWeekAvans25/native_sim/src/main.c:14:
/home/npc/zephyrproject/applications/StartWeekAvans25/native_sim/src/../inc/lvgl_ui.h:12:6: error: old-style parameter declarations in prototyped function definition
   12 | void set_lcd_display(char *msg)
      |      ^~~~~~~~~~~~~~~
/home/npc/zephyrproject/applications/StartWeekAvans25/native_sim/src/main.c:104: error: expected â{â at end of input
  104 | }
      | 
ninja: build stopped: subcommand failed.
FATAL ERROR: command exited with status 1: /usr/bin/cmake --build /home/npc/zephyrproject/build --target run

```
Semicolon was missing in lvgl_ui.h

## Include nested too deeply
```
                 from /home/npc/zephyrproject/applications/StartWeekAvans25/native_sim/src/main.c:14:
/home/npc/zephyrproject/applications/StartWeekAvans25/native_sim/inc/threads.h:4:27: error: #include nested too deeply
    4 | #include <zephyr/kernel.h>
      |                           ^
```

## lv_tick_inc() is not called
```
lv_timer_handler: It seems lv_tick_inc() is not called. 	(in lv_timer.c line #94)
```

# LVGL

## Getting parent width/height
```
#define MAIN_WIDTH 490
#define MAIN_HEIGHT 490

lv_obj_t * cont_main = lv_obj_create(parent);
lv_obj_set_size(cont_main, MAIN_WIDTH, MAIN_WIDTH);
int32_t width = lv_obj_get_width(cont_main);
int32_t height = lv_obj_get_height(cont_main);
printk("Width: %d, Height: %d\n", width, height);
```
Output: Width: 0, Height: 0

There's one [issue](https://github.com/lvgl/lvgl/issues/2480) on the github closed by a link to the documentation for an explanation, but [that link](https://docs.lvgl.io/master/overview/coords.html#postponed-coordinate-calculation) leads to 404 now.

Using LV_SIZE_CONTENT also doesnt scale to parent's size by default, and neither does LV_PCT(100).
lv_obj_update_layout(); also doesnt cause a proper scaling to parent's size.
Above solutions have worked in other cases, so need to figure out why the scaling is odd here.
Flex grow also doesnt cause it to grow to parent size.
Might need to adjust styling

## Button reporting pressed at start
Add default state doesnt work. Removing pressed state doesnt work.
```
lv_obj_add_state(buttons[index], LV_STATE_PRESSED);
            lv_obj_clear_state(buttons[index], LV_STATE_PRESSED);
```
gives:
```
*** Booting Zephyr OS build v4.0.0-45-g7d2ac022543d ***
[00:00:00.000,000] <err> lvgl: (0.000, +0)	 _lv_obj_style_create_transition: Asserted at expression: tr != NULL (Out of memory) 	(in lv_obj_style.c line #355)
```

## lv_obj_set_style_pad_all
lv_obj_set_style_pad_all isnt mentioned in the style part of html doc. It also isnt in the 937 page pdf, but lv_obj_set_style_pad_top etc is.

And dereferencing the style, which works for bg_colour, it causes this error when used for pad:
```
*** Booting Zephyr OS build v4.0.0-45-g7d2ac022543d ***
[00:00:00.000,000] <wrn> lvgl: (0.000, +0)	 lv_obj_get_disp: No screen found 	(in lv_obj_tree.c line #281)

[00:00:00.000,000] <wrn> lvgl: (0.000, +0)	 lv_obj_get_disp: No screen found 	(in lv_obj_tree.c line #281)

[00:00:00.000,000] <wrn> lvgl: (0.000, +0)	 lv_obj_get_disp: No screen found 	(in lv_obj_tree.c line #281)

Segmentation fault (core dumped)
FAILED: zephyr/CMakeFiles/run_native /home/npc/zephyrproject/build/zephyr/CMakeFiles/run_native 
```

# Add to research later
keys: defining additional keys
lvgl_button_input: mapping touch
sdl_gpio: actual keyboard scancodes being used for the keys, which can be found here:
[page 53](https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf)

# TODO
- Proper scaling
- Styling
- Emulate buttons as matrix
- Adjust namings to be same as archive
- Add switches to overlay
- Maybe global buttons