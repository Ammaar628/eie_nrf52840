/*
 * main.c
 */

#include <zephyr/kernel.h>

#include "BTN.h"
#include "LED.h"

#define SLEEP_MS 1
#define BOOT_WINDOW_MS 3000
#define MAX_PASSWORD_LENGTH 10

typedef enum {
  STATE_BOOT,
  STATE_ENTRY,
  STATE_LOCKED,
  STATE_WAITING
} system_state;

typedef enum {
  PASS_BTN0 = 0,
  PASS_BTN1,
  PASS_BTN2
} password_button;

int main(void) {
  system_state state = STATE_BOOT;
  uint32_t boot_start_time = 0;
  
  // Stored password
  password_button stored_password[MAX_PASSWORD_LENGTH];
  uint8_t stored_password_length = 0;
  
  // Current entry tracking
  password_button current_entry[MAX_PASSWORD_LENGTH];
  uint8_t current_entry_length = 0;

  if (0 > BTN_init()) {
    return 0;
  }
  if (0 > LED_init()) {
    return 0;
  }

  // Boot state - turn on LED3
  LED_set(LED3, LED_ON);
  boot_start_time = k_uptime_get_32();
  printk("Boot mode: Press BTN3 within 3 seconds to set password.\n");

  while(1) {
    if (state == STATE_BOOT) {
      // Check if BTN3 is pressed to enter password entry mode
      if (BTN_check_clear_pressed(BTN3)) {
        state = STATE_ENTRY;
        LED_set(LED3, LED_OFF);
        stored_password_length = 0;
        printk("\nEntry mode: Enter your password using BTN0, BTN1, BTN2.\n");
        printk("Press BTN3 when done.\n");
      }
      
      // Check if 3 seconds have passed
      if (k_uptime_get_32() - boot_start_time >= BOOT_WINDOW_MS) {
        LED_set(LED3, LED_OFF);
        
        // If no password was set, use default
        if (stored_password_length == 0) {
          stored_password[0] = PASS_BTN0;
          stored_password[1] = PASS_BTN2;
          stored_password_length = 2;
          printk("\nNo password set. Using default: BTN0, BTN2\n");
        }
        
        state = STATE_LOCKED;
        LED_set(LED0, LED_ON);
        printk("System locked. Enter password and press BTN3.\n");
      }
    }
    else if (state == STATE_ENTRY) {
      // Record button presses for password entry
      if (BTN_check_clear_pressed(BTN0)) {
        if (stored_password_length < MAX_PASSWORD_LENGTH) {
          stored_password[stored_password_length] = PASS_BTN0;
          stored_password_length++;
          printk("BTN0 added to password (length: %d)\n", stored_password_length);
        }
      }
      if (BTN_check_clear_pressed(BTN1)) {
        if (stored_password_length < MAX_PASSWORD_LENGTH) {
          stored_password[stored_password_length] = PASS_BTN1;
          stored_password_length++;
          printk("BTN1 added to password (length: %d)\n", stored_password_length);
        }
      }
      if (BTN_check_clear_pressed(BTN2)) {
        if (stored_password_length < MAX_PASSWORD_LENGTH) {
          stored_password[stored_password_length] = PASS_BTN2;
          stored_password_length++;
          printk("BTN2 added to password (length: %d)\n", stored_password_length);
        }
      }
      
      // BTN3 saves the password and enters locked state
      if (BTN_check_clear_pressed(BTN3)) {
        if (stored_password_length > 0) {
          printk("\nPassword saved! Length: %d\n", stored_password_length);
          state = STATE_LOCKED;
          LED_set(LED0, LED_ON);
          current_entry_length = 0;
          printk("System locked. Enter password and press BTN3.\n");
        } else {
          printk("Password cannot be empty. Enter at least one button.\n");
        }
      }
    }
    else if (state == STATE_LOCKED) {
      // Record button presses for password attempt
      if (BTN_check_clear_pressed(BTN0)) {
        if (current_entry_length < MAX_PASSWORD_LENGTH) {
          current_entry[current_entry_length] = PASS_BTN0;
          current_entry_length++;
          printk("BTN0 entered\n");
        }
      }
      if (BTN_check_clear_pressed(BTN1)) {
        if (current_entry_length < MAX_PASSWORD_LENGTH) {
          current_entry[current_entry_length] = PASS_BTN1;
          current_entry_length++;
          printk("BTN1 entered\n");
        }
      }
      if (BTN_check_clear_pressed(BTN2)) {
        if (current_entry_length < MAX_PASSWORD_LENGTH) {
          current_entry[current_entry_length] = PASS_BTN2;
          current_entry_length++;
          printk("BTN2 entered\n");
        }
      }
      
      // Check if BTN3 (enter) is pressed
      if (BTN_check_clear_pressed(BTN3)) {
        // Verify password
        bool password_correct = true;
        
        if (current_entry_length != stored_password_length) {
          password_correct = false;
        } else {
          for (uint8_t i = 0; i < stored_password_length; i++) {
            if (current_entry[i] != stored_password[i]) {
              password_correct = false;
              break;
            }
          }
        }
        
        if (password_correct) {
          printk("Correct!\n");
        } else {
          printk("Incorrect!\n");
        }
        
        // Turn off LED0 and enter waiting state
        LED_set(LED0, LED_OFF);
        state = STATE_WAITING;
        current_entry_length = 0;
        printk("Press any button to reset...\n");
      }
    } 
    else if (state == STATE_WAITING) {
      // Any button press resets to locked state
      if (BTN_check_clear_pressed(BTN0) || 
          BTN_check_clear_pressed(BTN1) || 
          BTN_check_clear_pressed(BTN2) || 
          BTN_check_clear_pressed(BTN3)) {
        
        state = STATE_LOCKED;
        LED_set(LED0, LED_ON);
        current_entry_length = 0;
        printk("\nSystem reset. System locked.\n");
        printk("Enter password and press BTN3.\n");
      }
    }
    
    k_msleep(SLEEP_MS);
  }
	return 0;
}
