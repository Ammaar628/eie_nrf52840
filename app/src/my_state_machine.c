/**
 * @file my_state_machine.c
 */

#include <zephyr/smf.h>
#include <zephyr/sys/printk.h>

#include "BTN.h"
#include "LED.h"
#include "my_state_machine.h"

/*----------------------------------------------------------------------------
 * Function Prototypes                                                        
 *----------------------------------------------------------------------------*/
static void s0_state_entry(void *o);
static enum smf_state_result s0_state_run(void *o);
static void s0_state_exit(void *o);

static void s1_state_entry(void *o);
static enum smf_state_result s1_state_run(void *o);
static void s1_state_exit(void *o);

static void s2_state_entry(void *o);
static enum smf_state_result s2_state_run(void *o);
static void s2_state_exit(void *o);

static void s3_state_entry(void *o);
static enum smf_state_result s3_state_run(void *o);
static void s3_state_exit(void *o);

static void s4_state_entry(void *o);
static enum smf_state_result s4_state_run(void *o);
static void s4_state_exit(void *o);

/*----------------------------------------------------------------------------
 * Typedefs                                                                    
 *----------------------------------------------------------------------------*/
enum state_machine_states {
    S0_STATE,
    S1_STATE,
    S2_STATE,
    S3_STATE,
    S4_STATE
};

typedef struct {
    //Context variable used by zephyr to track state machine state. Must be first
    struct smf_ctx ctx;

    uint32_t timer_ms;
    uint16_t blink_counter;
} state_machine_context;

/*----------------------------------------------------------------------------
 * Local Variables                                                             
 *----------------------------------------------------------------------------*/
static const struct smf_state states[] = {
    [S0_STATE] = SMF_CREATE_STATE(s0_state_entry, s0_state_run, s0_state_exit, NULL, NULL),
    [S1_STATE] = SMF_CREATE_STATE(s1_state_entry, s1_state_run, s1_state_exit, NULL, NULL),
    [S2_STATE] = SMF_CREATE_STATE(s2_state_entry, s2_state_run, s2_state_exit, NULL, NULL),
    [S3_STATE] = SMF_CREATE_STATE(s3_state_entry, s3_state_run, s3_state_exit, NULL, NULL),
    [S4_STATE] = SMF_CREATE_STATE(s4_state_entry, s4_state_run, s4_state_exit, NULL, NULL)
};

static state_machine_context state_object;

void state_machine_init() {
    state_object.timer_ms = 0;
    state_object.blink_counter = 0;
    smf_set_initial(SMF_CTX(&state_object), &states[S0_STATE]);
}

int state_machine_run() {
    return smf_run_state(SMF_CTX(&state_object));
}

/*----------------------------------------------------------------------------
 * S0 State - All LEDs off
 *----------------------------------------------------------------------------*/
static void s0_state_entry(void *o) {
    printk("Entering S0: All LEDs off\n");
    LED_set(LED0, LED_OFF);
    LED_set(LED1, LED_OFF);
    LED_set(LED2, LED_OFF);
    LED_set(LED3, LED_OFF);
}

static enum smf_state_result s0_state_run(void *o) {
    if (BTN_check_clear_pressed(BTN1)) {
        smf_set_state(SMF_CTX(&state_object), &states[S1_STATE]);
    } else if (BTN_check_clear_pressed(BTN3)) {
        smf_set_state(SMF_CTX(&state_object), &states[S3_STATE]);
    }
    
    return SMF_EVENT_HANDLED;
}

static void s0_state_exit(void *o) {
    printk("Exiting S0\n");
}

/*----------------------------------------------------------------------------
 * S1 State - LED 1 blink at 4 Hz
 *----------------------------------------------------------------------------*/
static void s1_state_entry(void *o) {
    printk("Entering S1: LED1 blink at 4 Hz\n");
    state_object.blink_counter = 0;
    LED_set(LED0, LED_OFF);
    LED_set(LED1, LED_ON);
    LED_set(LED2, LED_OFF);
    LED_set(LED3, LED_OFF);
}

static enum smf_state_result s1_state_run(void *o) {
    // 4 Hz = 250ms period (125ms on, 125ms off)
    state_object.blink_counter++;
    if (state_object.blink_counter >= 125) {
        LED_toggle(LED1);
        state_object.blink_counter = 0;
    }
    
    if (BTN_check_clear_pressed(BTN2)) {
        smf_set_state(SMF_CTX(&state_object), &states[S2_STATE]);
    } else if (BTN_check_clear_pressed(BTN3)) {
        smf_set_state(SMF_CTX(&state_object), &states[S4_STATE]);
    }
    
    return SMF_EVENT_HANDLED;
}

static void s1_state_exit(void *o) {
    printk("Exiting S1\n");
}

/*----------------------------------------------------------------------------
 * S2 State - LED 1 and 3 on, LED 2 and 4 off
 *----------------------------------------------------------------------------*/
static void s2_state_entry(void *o) {
    printk("Entering S2: LED1 and 3 on, LED2 and 4 off\n");
    state_object.timer_ms = 0;
    LED_set(LED0, LED_ON);
    LED_set(LED1, LED_OFF);
    LED_set(LED2, LED_ON);
    LED_set(LED3, LED_OFF);
}

static enum smf_state_result s2_state_run(void *o) {
    state_object.timer_ms++;
    
    if (state_object.timer_ms >= 2000) {
        smf_set_state(SMF_CTX(&state_object), &states[S3_STATE]);
    } else if (BTN_check_clear_pressed(BTN3)) {
        smf_set_state(SMF_CTX(&state_object), &states[S0_STATE]);
    }
    
    return SMF_EVENT_HANDLED;
}

static void s2_state_exit(void *o) {
    printk("Exiting S2\n");
}

/*----------------------------------------------------------------------------
 * S3 State - LED 1 and 3 off, LED 2 and 4 on
 *----------------------------------------------------------------------------*/
static void s3_state_entry(void *o) {
    printk("Entering S3: LED1 and 3 off, LED2 and 4 on\n");
    state_object.timer_ms = 0;
    LED_set(LED0, LED_OFF);
    LED_set(LED1, LED_ON);
    LED_set(LED2, LED_OFF);
    LED_set(LED3, LED_ON);
}

static enum smf_state_result s3_state_run(void *o) {
    state_object.timer_ms++;
    
    if (state_object.timer_ms >= 1000) {
        smf_set_state(SMF_CTX(&state_object), &states[S2_STATE]);
    } else if (BTN_check_clear_pressed(BTN3)) {
        smf_set_state(SMF_CTX(&state_object), &states[S0_STATE]);
    }
    
    return SMF_EVENT_HANDLED;
}

static void s3_state_exit(void *o) {
    printk("Exiting S3\n");
}

/*----------------------------------------------------------------------------
 * S4 State - All LEDs blink at 16 Hz
 *----------------------------------------------------------------------------*/
static void s4_state_entry(void *o) {
    printk("Entering S4: All LEDs blink at 16 Hz\n");
    state_object.blink_counter = 0;
    LED_set(LED0, LED_ON);
    LED_set(LED1, LED_ON);
    LED_set(LED2, LED_ON);
    LED_set(LED3, LED_ON);
}

static enum smf_state_result s4_state_run(void *o) {
    // 16 Hz = 62.5ms period (31.25ms on, 31.25ms off)
    // Using 31ms for approximation
    state_object.blink_counter++;
    if (state_object.blink_counter >= 31) {
        LED_toggle(LED0);
        LED_toggle(LED1);
        LED_toggle(LED2);
        LED_toggle(LED3);
        state_object.blink_counter = 0;
    }
    
    if (BTN_check_clear_pressed(BTN2)) {
        smf_set_state(SMF_CTX(&state_object), &states[S1_STATE]);
    } else if (BTN_check_clear_pressed(BTN3)) {
        smf_set_state(SMF_CTX(&state_object), &states[S0_STATE]);
    }
    
    return SMF_EVENT_HANDLED;
}

static void s4_state_exit(void *o) {
    printk("Exiting S4\n");
}
