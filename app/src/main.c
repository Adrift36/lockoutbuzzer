#include <stdbool.h>
#include <stdint.h>

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define LED_NODE DT_ALIAS(lockout_led)
#define BUTTON_NODE DT_ALIAS(lockout_button)
#define HOST_SEL_NODE DT_ALIAS(host_sel)

#if !DT_NODE_HAS_STATUS(LED_NODE, okay)
#error "Missing alias: lockout-led"
#endif

#if !DT_NODE_HAS_STATUS(BUTTON_NODE, okay)
#error "Missing alias: lockout-button"
#endif

#if !DT_NODE_HAS_STATUS(HOST_SEL_NODE, okay)
#error "Missing alias: host-sel"
#endif

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(BUTTON_NODE, gpios);
static const struct gpio_dt_spec host_sel = GPIO_DT_SPEC_GET(HOST_SEL_NODE, gpios);

static struct gpio_callback button_cb;
static volatile bool button_pressed;

static void button_handler(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
ARG_UNUSED(dev);
ARG_UNUSED(cb);
ARG_UNUSED(pins);
button_pressed = true;
}

static int setup_gpio(void)
{
int rc;

if (!device_is_ready(led.port) || !device_is_ready(button.port) || !device_is_ready(host_sel.port)) {
printk("GPIO device not ready\n");
return -ENODEV;
}

rc = gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
if (rc != 0) {
printk("LED config failed: %d\n", rc);
return rc;
}

rc = gpio_pin_configure_dt(&button, GPIO_INPUT);
if (rc != 0) {
printk("Button config failed: %d\n", rc);
return rc;
}

rc = gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);
if (rc != 0) {
printk("Button IRQ config failed: %d\n", rc);
return rc;
}

gpio_init_callback(&button_cb, button_handler, BIT(button.pin));
rc = gpio_add_callback(button.port, &button_cb);
if (rc != 0) {
printk("Button callback failed: %d\n", rc);
return rc;
}

rc = gpio_pin_configure_dt(&host_sel, GPIO_INPUT);
if (rc != 0) {
printk("Host select config failed: %d\n", rc);
return rc;
}

return 0;
}

static bool read_host_role(void)
{
int value = gpio_pin_get_dt(&host_sel);

if (value < 0) {
printk("Host select read failed: %d\n", value);
return false;
}

return value == 0;
}

static void blink(int on_ms, int off_ms)
{
gpio_pin_set_dt(&led, 1);
k_msleep(on_ms);
gpio_pin_set_dt(&led, 0);
k_msleep(off_ms);
}

static void announce_role(bool host_role)
{
printk("Role: %s\n", host_role ? "HOST" : "PLAYER");
}

void main(void)
{
bool host_role;
int rc = setup_gpio();

if (rc != 0) {
return;
}

host_role = read_host_role();
announce_role(host_role);
printk("Host select jumper: low = HOST, high = PLAYER\n");

while (true) {
if (button_pressed) {
button_pressed = false;
printk("Button press detected (%s)\n", host_role ? "HOST" : "PLAYER");

for (int i = 0; i < 3; i++) {
blink(60, 60);
}
}

if (host_role) {
blink(180, 820);
} else {
blink(80, 120);
blink(80, 720);
}
}
}
