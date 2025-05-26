#include <stdio.h>
#include <string.h>

#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

#define RED_LED_NODE DT_ALIAS(red_led)
#define GREEN_LED_NODE DT_ALIAS(green_led)
#define BLUE_LED_NODE DT_ALIAS(blue_led)

LOG_MODULE_REGISTER(lab4, LOG_LEVEL_DBG);

static const struct gpio_dt_spec red_led = GPIO_DT_SPEC_GET(RED_LED_NODE, gpios);
static const struct gpio_dt_spec green_led = GPIO_DT_SPEC_GET(GREEN_LED_NODE, gpios);
static const struct gpio_dt_spec blue_led = GPIO_DT_SPEC_GET(BLUE_LED_NODE, gpios);

int main(void)
{
  int ret;
  bool leds_state = true;

  if (!gpio_is_ready_dt(&red_led)) {
    return 0;
  }

  if (!gpio_is_ready_dt(&green_led)) {
    return 0;
  }

  if (!gpio_is_ready_dt(&blue_led)) {
    return 0;
  }

  ret = gpio_pin_configure_dt(&red_led, GPIO_OUTPUT_ACTIVE);
  if (ret < 0) {
    return 0;
  }

  ret = gpio_pin_configure_dt(&green_led, GPIO_OUTPUT_ACTIVE);
  if (ret < 0) {
    return 0;
  }

  ret = gpio_pin_configure_dt(&blue_led, GPIO_OUTPUT_ACTIVE);
  if (ret < 0) {
    return 0;
  }

  while (1) {
    gpio_pin_toggle_dt(&red_led);
    gpio_pin_toggle_dt(&green_led);
    gpio_pin_toggle_dt(&blue_led);

    leds_state = !leds_state;
    LOG_DBG("LEDs state: %s", leds_state ? "ON" : "OFF");
    k_msleep(CONFIG_LED_BLINK_PERIOD);
  }
  return 0;
}
