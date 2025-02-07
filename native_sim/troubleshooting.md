```
/usr/include/linux/errno.h:1:10: fatal error: asm/errno.h: No such file or directory
    1 | #include <asm/errno.h>
      |          ^~~~~~~~~~~~~
compilation terminated.
ninja: build stopped: subcommand failed.
FATAL ERROR: command exited with status 1: /usr/bin/cmake --build /home/npc/zephyrproject/zephyr/build

```

sudo apt-get install gcc-multilib

```
/usr/include/SDL2/SDL_config.h:4:10: fatal error: SDL2/_real_SDL_config.h: No such file or directory
    4 | #include <SDL2/_real_SDL_config.h>
      |          ^~~~~~~~~~~~~~~~~~~~~~~~~
compilation terminated.
```
sudo apt install libsdl2-dev:i386


```
[00:00:00.000,000] <inf> gpio_emul_sdl: GPIO sdl_gpio:0 = 30
[00:00:00.000,000] <inf> gpio_emul_sdl: GPIO sdl_gpio:1 = 31
[00:00:00.000,000] <inf> gpio_emul_sdl: GPIO sdl_gpio:2 = 32
[00:00:00.000,000] <inf> sdl_input: Init 'input-sdl-touch' device
[00:00:00.000,000] <err> gpio_keys: interrupt configuration failed: -95
[00:00:00.000,000] <err> gpio_keys: Pin 0 interrupt configuration failed: -95
*** Booting Zephyr OS build v4.0.0-45-g7d2ac022543d ***
[00:00:00.000,000] <err> app: Failed to enable button callback: -95
```
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