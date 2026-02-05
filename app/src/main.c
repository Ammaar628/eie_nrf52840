/**
 * @file main.c
 */

#include <inttypes.h>

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

#include "BTN.h"
#include "LED.h"

#define SLEEP_MS 1

#define ARDUINO_I2C_NODE DT_NODELABEL(arduino_i2c)

#define ADDR 0x38
#define TD_STATUS 0x02
#define P1_XH 0x03
#define P1_XL 0x04
#define P1_YH 0x05
#define P1_YL 0x06
#define TOUCH_POS_MSB_MASK 0x0F
static const struct device * dev = DEVICE_DT_GET(ARDUINO_I2C_NODE);

void touch_control_cmd_rsp(uint8_t cmd, uint8_t *rsp) {
    struct i2c_msg cmd_rsp_msg[2] = {
        [0] = {.buf = &cmd, .len = 1, .flags = I2C_MSG_WRITE},
        [1] = {.buf = rsp, .len = 1, .flags = I2C_MSG_RESTART | I2C_MSG_READ | I2C_MSG_STOP},
    };
    i2c_transfer(dev, cmd_rsp_msg, 2, ADDR);
}


int main(void) {
  if(!device_is_ready(dev)){
    return 0;
  }

  if(0 > i2c_configure(dev, I2C_SPEED_SET(I2C_SPEED_FAST) | I2C_MODE_CONTROLLER)){
    return 0;
  }

  if (0 > BTN_init()){
    return 0;
  }

 while (1) {

    uint8_t touch_status;
    touch_control_cmd_rsp(TD_STATUS, &touch_status);

    if (touch_status == 1) {
        uint8_t x_pos_h;
        uint8_t x_pos_l;
        uint8_t y_pos_h;
        uint8_t y_pos_l;

        touch_control_cmd_rsp(P1_XH, &x_pos_h);
        touch_control_cmd_rsp(P1_XL, &x_pos_l);
        touch_control_cmd_rsp(P1_YH, &y_pos_h);
        touch_control_cmd_rsp(P1_YL, &y_pos_l);

        uint16_t x_pos = ((x_pos_h & TOUCH_POS_MSB_MASK) << 8) + x_pos_l;
        uint16_t y_pos = ((y_pos_h & TOUCH_POS_MSB_MASK) << 8) + y_pos_l;

        printk("Touch at %u, %u\n", x_pos, y_pos);        
    }
    k_msleep(SLEEP_MS);
  }
  return 0;
}