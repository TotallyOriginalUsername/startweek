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