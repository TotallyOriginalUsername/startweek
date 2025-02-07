#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <stdio.h>
#include <zephyr/kernel.h>

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app);

static struct gpio_dt_spec button_gpio = GPIO_DT_SPEC_GET_OR(
		DT_ALIAS(sw0), gpios, {0});

static struct gpio_callback button_callback;

static void button_isr_callback(const struct device *port,
				struct gpio_callback *cb,
				uint32_t pins)
{
	printk("Button pressed at %" PRIu32 "\n", k_cycle_get_32());
}

int main(void)
{

	if (gpio_is_ready_dt(&button_gpio)) {
		int err;

		// Configure the button GPIO pin as input with pull-up resistor
		err = gpio_pin_configure_dt(&button_gpio, GPIO_INPUT | GPIO_PULL_UP);
		if (err) {
			LOG_ERR("Failed to configure button GPIO: %d", err);
			return 0;
		}

		// Initialize the GPIO callback
		gpio_init_callback(&button_callback, button_isr_callback,
				   BIT(button_gpio.pin));

		// Add the callback to the GPIO port
		err = gpio_add_callback(button_gpio.port, &button_callback);
		if (err) {
			LOG_ERR("Failed to add button callback: %d", err);
			return 0;
		}

		// Configure the interrupt for the button
		err = gpio_pin_interrupt_configure_dt(&button_gpio,
						      GPIO_INT_EDGE_TO_INACTIVE);
		if (err) {
			LOG_ERR("Failed to enable button callback: %d", err);
			return 0;
		}
		printk("Button configured\n");
	} else {
		printk("Button GPIO not ready");
		return 0;
	}

	while (1) {
		int button_state = gpio_pin_get(button_gpio.port, button_gpio.pin);
		if (button_state < 0) {
			printk("Failed to read button state: %d", button_state);
		} else {
			printk("Button state: %s\n", button_state ? "Released" : "Pressed");
		}
		k_cpu_idle();
	}
}
