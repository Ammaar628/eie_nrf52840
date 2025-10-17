/*
 * main.c
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

int main(void){
    int ret;

    if(!gpio_is_ready_dt(&led)){
        return -1;
    }

    ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
        return ret;
    }

    while (1) {
    
    }

    return 0;
}