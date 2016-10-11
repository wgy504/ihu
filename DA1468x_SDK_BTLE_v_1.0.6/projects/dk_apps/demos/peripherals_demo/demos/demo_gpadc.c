/**
 ****************************************************************************************
 *
 * @file demo_gpadc.c
 *
 * @brief General Purpose ADC demo (hw_gpadc driver)
 *
 * Copyright (C) 2015. Dialog Semiconductor Ltd, unpublished work. This computer
 * program includes Confidential, Proprietary Information and is a Trade Secret of
 * Dialog Semiconductor Ltd.  All use, disclosure, and/or reproduction is prohibited
 * unless authorized in writing. All Rights Reserved.
 *
 * <black.orca.support@diasemi.com> and contributors.
 *
 ****************************************************************************************
 */

#include <stdio.h>
#include <string.h>
#include <hw_gpadc.h>
#include <ad_gpadc.h>
#include "common.h"
#include "config.h"
#include "platform_devices.h"

#if CFG_DEMO_HW_GPADC

static void gpadc_intr_cb(void)
{
        uint16_t raw, val;

        /*
         * Conversion result is a 16-bit value with number of valid bits depending on oversampling
         * settings. Invalid bits should be discarded. Actual result (voltage measured) depends on
         * ADC settings
         *
         * For example for single-ended conversion without input attenuator ADC measurement range
         * is 0V to +1.2V which means raw=0x0000 represents 0V and raw=0xFFFF represents +1.2V.
         */
        raw = hw_gpadc_get_raw_value();

        /*
         * In addition to raw value it's possible to get actual conversion result with only valid
         * bits includes, i.e. adjusted according to oversampling settings. Number of bits in
         * returned value is number of valid bits in result.
         */
        val = hw_gpadc_get_value();

        /* change could slightly distort menu's displaying */
        printf(NEWLINE "ADC result: raw=0x%04x val=0x%04x", raw, val);

        /*
         * If interrupt callback is registered, it's required to clear interrupt by application.
         * Otherwise, this is done automatically by driver.
         */
        hw_gpadc_clear_interrupt();
}

void demo_gpadc_init(void)
{
        /*
         * At initialization, use internal high-speed clock and measure VBAT. Other settings are at
         * default values (0).
         */
        static gpadc_config cfg = {
                .clock = HW_GPADC_CLOCK_INTERNAL,
                .input_mode = HW_GPADC_INPUT_MODE_SINGLE_ENDED,
                .input = HW_GPADC_INPUT_SE_VBAT,
        };

        /*
         * ADC initialization will reset its state to default values and will disable interrupt.
         * When called with parameter, it will also configure ADC according to parameters set in
         * configuration structure (no need to call APIs separately).
         */
        hw_gpadc_init(&cfg);

        /*
         * ADC has to be enabled before any conversion can be started. Note that enabling ADC does
         * not make it start doing conversions - it only makes it ready for it. To start actual
         * conversion application should call hw_gpadc_start().
         */
        hw_gpadc_enable();
}

void menu_gpadc_input_se_func(const struct menu_item *m, bool checked)
{
        HW_GPADC_INPUT input = (HW_GPADC_INPUT) m->param;

        /*
         * When changing source input, make sure proper input mode is selected.
         * Some values for input configuration are the same for single-ended and differential modes
         * so with invalid mode/input combination different input will be measured than expected.
         */
        hw_gpadc_set_input_mode(HW_GPADC_INPUT_MODE_SINGLE_ENDED);
        hw_gpadc_set_input(input);
}

bool menu_gpadc_input_se_checked_cb_func(const struct menu_item *m)
{
        HW_GPADC_INPUT input = (HW_GPADC_INPUT) m->param;

        return hw_gpadc_get_input_mode() == HW_GPADC_INPUT_MODE_SINGLE_ENDED &&
                hw_gpadc_get_input() == input;
}

void menu_gpadc_input_diff_func(const struct menu_item *m, bool checked)
{
        HW_GPADC_INPUT input = (HW_GPADC_INPUT) m->param;

        hw_gpadc_set_input_mode(HW_GPADC_INPUT_MODE_DIFFERENTIAL);
        hw_gpadc_set_input(input);
}

bool menu_gpadc_input_diff_checked_cb_func(const struct menu_item *m)
{
        HW_GPADC_INPUT input = (HW_GPADC_INPUT) m->param;

        return hw_gpadc_get_input_mode() == HW_GPADC_INPUT_MODE_DIFFERENTIAL &&
                hw_gpadc_get_input() == input;
}

void menu_gpadc_cfg_digiclk_func(const struct menu_item *m, bool checked)
{
        HW_GPADC_CLOCK new_clock;

        new_clock = checked ? HW_GPADC_CLOCK_INTERNAL : HW_GPADC_CLOCK_DIGITAL;

        hw_gpadc_set_clock(new_clock);
}

bool menu_gpadc_cfg_digiclk_checked_cb_func(const struct menu_item *m)
{
        return hw_gpadc_get_clock() == HW_GPADC_CLOCK_DIGITAL;
}

void menu_gpadc_cfg_attenuator_func(const struct menu_item *m, bool checked)
{
        bool new_att_state = !checked;

        /*
         * By default, input voltage range is 0V - +1.2V in single-ended mode or -1.2V - +1.2V in
         * differential mode. An internal input attenuator can be enabled to scale input voltage
         * by factor of 3 which effectively increases input voltage range to 0V - +3.6V in single-ended
         * mode or -3.6V - +3.6V in differential mode.
         */
        hw_gpadc_set_input_attenuator_state(new_att_state);
}

bool menu_gpadc_cfg_attenuator_checked_cb_func(const struct menu_item *m)
{
        return hw_gpadc_get_input_attenuator_state();
}

void menu_gpadc_cfg_chopping_func(const struct menu_item *m, bool checked)
{
        bool new_chopping = !checked;

        /*
         * Enabling chopping function will make ADC take two samples with opposite polarity on
         * each conversion. This is to cancel offset.
         */
        hw_gpadc_set_chopping(new_chopping);
}

bool menu_gpadc_cfg_chopping_checked_cb_func(const struct menu_item *m)
{
        return hw_gpadc_get_chopping();
}

void menu_gpadc_cfg_sign_func(const struct menu_item *m, bool checked)
{
        bool new_sign = !checked;

        hw_gpadc_set_sign_change(new_sign);
}

bool menu_gpadc_cfg_sign_checked_cb_func(const struct menu_item *m)
{
        return hw_gpadc_get_sign_change();
}

void menu_gpadc_cfg_sample_func(const struct menu_item *m, bool checked)
{
        uint8_t sample_time = (int) m->param;

        /*
         * Sample time of ADC can also be configured. Actual sample time is 1 clock cycle when
         * \p sample_time == 0 and \p sample_time * 32 clock cycles for other values in allowable
         * range 0-15.
         */
        hw_gpadc_set_sample_time(sample_time);
}

bool menu_gpadc_cfg_sample_checked_cb_func(const struct menu_item *m)
{
        uint8_t sample_time = (int) m->param;

        return sample_time == hw_gpadc_get_sample_time();
}

void menu_gpadc_cfg_oversampling_func(const struct menu_item *m, bool checked)
{
        uint8_t n_samples = (int) m->param;

        /*
         * Actual precision of conversion results can be configured by enabling oversampling. In
         * this mode multiple successive conversions are performed and are added together which
         * increases precision of result - see reading ADC value example.
         *
         * Actual number of samples taken is 2^(n_samples) where n_samples can be any value in range
         * 0-7 this number of samples can be between 0 and 128. Number of valid bits in conversion
         * result is 10 + n_samples, i.e. can be in range 10 to 17. Note that with n_samples=7
         * result will have only 16 bits due to result register size (least significant bit is
         * discarded).
         */
        hw_gpadc_set_oversampling(n_samples);
}

bool menu_gpadc_cfg_oversampling_checked_cb_func(const struct menu_item *m)
{
        uint8_t n_samples = (int) m->param;

        return n_samples == hw_gpadc_get_oversampling();
}

void menu_gpadc_cfg_interval_func(const struct menu_item *m, bool checked)
{
        int interval = (int) m->param;

        /*
         * In continuous mode it's possible to configure interval between consecutive conversions
         * in range 0 to approx 261ms. Configured interval is \p interval * 1.024ms.
         * With continuous mode disable, this setting has no effect.
         */
        hw_gpadc_set_interval(interval);
}

bool menu_gpadc_cfg_interval_checked_cb_func(const struct menu_item *m)
{
        int interval = (int) m->param;

        return interval == hw_gpadc_get_interval();
}

void menu_gpadc_calibrate_func(const struct menu_item *m, bool checked)
{
        /*
         * Since continuous mode is selected calibration process can't be executed because of some
         * conversions occur inside of it.
         */
        if (hw_gpadc_get_continuous()) {
                printf(NEWLINE "Can't calibrate. Turn off continuous mode.");
                return;
        }
        /*
         * Before using ADC it's recommended to perform calibration. This will set proper offsets
         * using hw_gpadc_set_offset_positive() and hw_gpadc_set_offset_negative() calls and will
         * automatically detect single-ended and differential mode used. Driver's API for this
         * routine is an implementation of calibration procedure described in datasheet.
         */
        hw_gpadc_calibrate();
}

void menu_gpadc_measure_func(const struct menu_item *m, bool checked)
{
        /*
         * If continuous mode is enabled, conversion takes place automatically.
         */
        if (hw_gpadc_get_continuous()) {
                return;
        }

        /*
         * Actual conversion in non-continuous mode takes place after ADC is started by an explicit
         * call. Before starting conversion, it's required to wait until ADC is in idle state (i.e.
         * previous conversion, if any, has finished) before calling hw_gpadc_start().
         */
        while (hw_gpadc_in_progress()) {
        }

        /*
         * Application can either actively wait for conversion to be completed (see above) or
         * simply register an interrupt handler which will be called once it finished.
         */
        hw_gpadc_register_interrupt(gpadc_intr_cb);

        /* Start actual conversion */
        hw_gpadc_start();
}

void menu_gpadc_continuous_func(const struct menu_item *m, bool checked)
{
        bool continous = !checked;

        /*
         * In continuous mode ADC will perform conversions at configured intervals automatically.
         */
        hw_gpadc_set_continuous(continous);

        /*
         * After continuous mode is enabled, it's still required to start ADC for 1st conversion
         * to be performed. Subsequent conversions will be started automatically according to
         * configuration. Here we also disable interrupt as we don't want it to be called on
         * every conversion. Instead, latest result can be read in ADC state.
         */
        if (continous) {
                hw_gpadc_unregister_interrupt();
                hw_gpadc_start();
        }
}

bool menu_gpadc_continuous_checked_cb_func(const struct menu_item *m)
{
        return hw_gpadc_get_continuous();
}

static const char *get_input_name(void)
{
        HW_GPADC_INPUT_MODE mode;
        HW_GPADC_INPUT input;

        mode = hw_gpadc_get_input_mode();
        input = hw_gpadc_get_input();

        if (mode == HW_GPADC_INPUT_MODE_SINGLE_ENDED) {
                switch (input) {
                case HW_GPADC_INPUT_SE_P06:
                        return "P0.6";
                case HW_GPADC_INPUT_SE_P07:
                        return "P0.7";
                case HW_GPADC_INPUT_SE_P10:
                        return "P1.0";
                case HW_GPADC_INPUT_SE_P12:
                        return "P1.2";
                case HW_GPADC_INPUT_SE_P13:
                        return "P1.3";
                case HW_GPADC_INPUT_SE_P14:
                        return "P1.4";
                case HW_GPADC_INPUT_SE_P15:
                        return "P1.5";
                case HW_GPADC_INPUT_SE_P24:
                        return "P2.4";
                case HW_GPADC_INPUT_SE_AVS:
                        return "analog ground level";
                case HW_GPADC_INPUT_SE_VDD:
                        return "VDD";
                case HW_GPADC_INPUT_SE_V33:
                        return "V33";
                case HW_GPADC_INPUT_SE_VBAT:
                        return "VBAT";
                default:
                        break;
                }
        } else {
                switch (input) {
                case HW_GPADC_INPUT_DIFF_P12_P14:
                        return "P1.2 vs P1.4";
                case HW_GPADC_INPUT_DIFF_P13_P07:
                        return "P1.3 vs P0.7";
                default:
                        break;
                }
        }

        return "unknown";
}

static inline const char *bool_str(bool v)
{
        return v ? "yes" : "no";
}

void menu_gpadc_state_func(const struct menu_item *m, bool checked)
{
        uint8_t sample_time_mult;
        uint8_t interval;
        uint8_t n_samples;

        sample_time_mult = hw_gpadc_get_sample_time();
        interval = hw_gpadc_get_interval();
        n_samples = hw_gpadc_get_oversampling();

        printf(NEWLINE "==== hw_gpadc state ===");

        printf(NEWLINE "last conversion value (raw): 0x%04X", hw_gpadc_get_raw_value());
        printf(NEWLINE "clock: %s", hw_gpadc_get_clock() == HW_GPADC_CLOCK_DIGITAL ? "digital" : "internal");
        printf(NEWLINE "input: %s (%s)", get_input_name(),
                                        hw_gpadc_get_input_mode() == HW_GPADC_INPUT_MODE_DIFFERENTIAL ?
                                        "differential" : "single-ended");
        printf(NEWLINE "busy: %s", bool_str(hw_gpadc_in_progress()));
        printf(NEWLINE "continuous mode: %s", bool_str(hw_gpadc_get_continuous()));
        printf(NEWLINE "input attenuator: %s", bool_str(hw_gpadc_get_input_attenuator_state()));
        printf(NEWLINE "chopping: %s", bool_str(hw_gpadc_get_chopping()));
        printf(NEWLINE "sign change: %s", bool_str(hw_gpadc_get_sign_change()));
        /* all getters return values with the same interpretation as used in setters */
        printf(NEWLINE "sample time: %d clock cycles (raw=%d)", sample_time_mult * 32, sample_time_mult);
        printf(NEWLINE "sample interval: %.3fms (raw=%d)", interval * 1.024f, interval);
        printf(NEWLINE "oversampling: %d samples (raw=%d)", 1 << n_samples, n_samples);

        printf(NEWLINE "========= /// =========");
}

#endif // CFG_DEMO_HW_GPADC

#if CFG_DEMO_AD_GPADC

#include <hw_tempsens.h>

/*
 * This defines analog supply voltage of the chip which was measured manually by using voltage gauge.
 * By connecting another supply source this can be different. It is only approximate
 * value. The perfect value should be 3.300 V.
 */
#define SUPPLY_3V3      3.235
/*
 * This will be used for keeping digital value of 3.3 V analog voltage.
 */
static uint16 ref_point_3V3;

static bool cyclic_read_enabled;

typedef struct {
        OS_EVENT event;
        uint16_t result_val;
} user_data_t;

void demo_ad_gpadc_init(void)
{
        gpadc_source src;

        /*
         * Take the reference value of 3.3V
         */
        src = ad_gpadc_open(REF_POINT_3V3);
        ad_gpadc_read(src, &ref_point_3V3);
        ad_gpadc_close(src);

        /*
         * Temperature sensor will be used in demo so there is a necessity to enable it. It will be
         * enabled since demo is running.
         */
        hw_tempsens_enable();
}

static int convert_digital_to_analog(uint16_t *value)
{
        /*
         * Conversion from digital to analog value. We would like to get value in mV that's why
         * result is multiply by 1000.
         */
        return (SUPPLY_3V3 * 1000 * (*value)) / ref_point_3V3;
}

static int convert_vbat_to_analog(uint16_t *value)
{
        /* The equation is applicable for BATTERY_LEVEL measurements having the ADC configured
         * as in platform_devices.h.
         */
        return (12 * (*value)  + 85) / 10;
}


void task_ad_gpadc_worker_func(const struct task_item *task)
{
        gpadc_source src;
        uint16_t value;

        /*
         * Open source connected to GPADC.
         * This will not start any measurement yet.
         */
        src = ad_gpadc_open(LIGHT_SENSOR);

        for (;;) {
                if (cyclic_read_enabled) {
                        OS_DELAY(OS_MS_2_TICKS(1000));

                        ad_gpadc_read(src, &value);

                        printf(NEWLINE "Light: %d[mV]", convert_digital_to_analog(&value));
                }
        }

        /*
         * This task will not use GPADC for time being, close it.
         */
        ad_gpadc_close(src);
}

static void read_gpadc_cb(void *user_data, int value)
{
        user_data_t *ud = (user_data_t *) user_data;

        ud->result_val = value;

        OS_EVENT_SIGNAL_FROM_ISR(ud->event);
}

void menu_gpadc_read_func(const struct menu_item *m, bool checked)
{
        user_data_t user_data;
        gpadc_source src = NULL;
        int device_id = (int) m->param;

        OS_EVENT_CREATE(user_data.event);

        switch (device_id) {
        case GPADC_BATTERY_LEVEL_ID:
                src = ad_gpadc_open(BATTERY_LEVEL);
                break;

        case GPADC_TEMP_SENSOR_ID:
                src = ad_gpadc_open(TEMP_SENSOR);
                break;

        case GPADC_LIGHT_SENSOR_ID:
                src = ad_gpadc_open(LIGHT_SENSOR);
                break;

        case GPADC_ENCODER_SENSOR_ID:
                src = ad_gpadc_open(ENCODER_SENSOR);
                break;

        default:
                /* Only the above device_id are supported by this program */
                OS_ASSERT(0);
        }

        ad_gpadc_read_async(src, read_gpadc_cb, &user_data);

        printf(NEWLINE "Waiting for the result ...");
        OS_EVENT_WAIT(user_data.event, OS_EVENT_FOREVER);

        ad_gpadc_close(src);

        switch (device_id) {
        case GPADC_BATTERY_LEVEL_ID:
                printf(NEWLINE "Voltage: %d[mV]", convert_vbat_to_analog(&user_data.result_val));
                break;
        case GPADC_TEMP_SENSOR_ID:
                printf(NEWLINE "Temperature: %d[C]", hw_tempsens_convert_to_temperature(user_data.result_val));
                break;
        default:
                printf(NEWLINE "Voltage: %d[mV]", convert_digital_to_analog(&user_data.result_val));
        }
}

void menu_gpadc_read_cyclic_func(const struct menu_item *m, bool checked)
{
        cyclic_read_enabled ^= true;
}

bool menu_gpadc_read_cyclic_checked_cb_func(const struct menu_item *m)
{
        return cyclic_read_enabled;
}

#endif // CFG_DEMO_AD_GPADC
