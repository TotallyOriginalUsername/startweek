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