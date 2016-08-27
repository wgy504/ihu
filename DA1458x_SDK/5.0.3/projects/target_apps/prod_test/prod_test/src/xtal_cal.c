// auto calibration of 16MHz crystal block
// last update: 20141106 {hm}

#include "global_io.h"
#include "gpio.h"
#include <core_cm0.h>

// *** defines

#define TEMP_OFFSET		40			// 5 ppm offset because of temperature effect (5 ppm = 80 => at 0.5 sec => TEMP_OFFSET = 40)
#define IDEAL_CNT       (8000000-4+TEMP_OFFSET) // 500 msec  x = 1148
//#define IDEAL_CNT     (4800000-4) // 300 msec  x = 1148

#define DEFAULT_TRIM    1148        // ideal TRIM value
#define ACCURACY_STEP   9           // using the SYSTICK: accuracy is 7 clocks

#define DELAY_1MSEC     1777        // delay x * 1 msec
#define MAX_LOOP        10          // loop_counter

#define TRIM_MIN        256         // minimum TRIM value
#define TRIM_MAX        2047        // maximum TRIM value

#define PPM_1           20          // 1.2 ppm
#define PPM_BOUNDARY    80          // 10 ppm

#define CCC_MAX         1000        // ccc max wrt IDEAL_CNT
#define CCC_MIN         -400        // ccc min 5 ppm

// variables

long x_calc, y_input, x_output;
long delta_x_input;
long ccc;

long m1, m2;

//( x - 4 ) -9 <= c <=  ( x - 4 ) +9
//( 8,000,000 - 4 ) -9 <= c <=  ( 8,000,000 - 4 ) +9
// ideal = 8,000,000 - 4 = 7,999,996

long actual_trimming_value;
long new_trimming_value;

// *** routines ***

long calculations(long xx_input, long yy_input);
long Clock_Read(char port_number);
void delay(unsigned long dd);
void Setting_Trim(unsigned int Trim_Value);
long data_simulation(long x1);

// *** end of variables / routines ***


// *** delay routine x * 1 msec
void delay(unsigned long dd)
{
    unsigned long j, jj;
    jj = dd * DELAY_1MSEC;
    for (j = 1; j<=jj; j++)
    {
        __nop();
        __nop();
    }
}

// *** boundary testing new TRIM-value and storing in CLK_FREQ_TRIM_REG
void Setting_Trim(unsigned int Trim_Value)  // program new Trim_Value
{
    if (Trim_Value < TRIM_MIN && Trim_Value != 0)   Trim_Value = TRIM_MIN;
    if (Trim_Value > TRIM_MAX)                      Trim_Value = TRIM_MAX;

    SetWord16(CLK_FREQ_TRIM_REG,Trim_Value);
    delay(10);  // delay = 10msec
}

/**
******************************************************************************************
* @brief Measures the high duration of an externally applied square pulse in system ticks.
*
* @param[in] port_number    GPIO where the square pulse is applied on.
* @return   Zero if no external pulse is detected on the given GPIO.
*           Otherwise the pulse high duration in system ticks is returned.
******************************************************************************************
*/
long Clock_Read(char port_number)   // testing block wave via input e.g. P0_6 ... port can be selected
{
    long ccc1 = 0;
    char port_number_10, port_number_1; // 10th and 1th e.g. 2 and 3 => port P2_3
    char shift_bit;

    uint32_t datareg = 0xFFFFFFFF; /// initialize to an invalid address
    volatile uint32_t tick_counter = 0;
    int measure_pulse(int datareg, int shift_bit); // forward declaration

    port_number_10     = port_number / 10;
    port_number_1     = port_number % 10;

    shift_bit = (1 << port_number_1);

    switch (port_number_10)
    {
        case 0: datareg = P0_DATA_REG; break;
        case 1: datareg = P1_DATA_REG; break;
        case 2: datareg = P2_DATA_REG; break;
        case 3: datareg = P3_DATA_REG; break;
    }

    // configure systick timer
    SysTick->LOAD = 0xFFFFFF;
    SysTick->VAL = 0;
    SetBits32(&SysTick->CTRL, SysTick_CTRL_CLKSOURCE_Msk, 1);

    // during counting, no interrupts should appear
    GLOBAL_INT_DISABLE(); // disable interrupts

    tick_counter = measure_pulse(datareg, shift_bit);
    SetBits32(&SysTick->CTRL, SysTick_CTRL_ENABLE_Msk, 0); // stop systick timer

    GLOBAL_INT_RESTORE(); // enable interrupts

    if(tick_counter == 0)
        ccc1 = 0;
    else
        ccc1 = 0xFFFFFF - tick_counter;

    return ccc1;
}

// *** algorithm for standard graph
long Simulation(volatile long x)    // simulation graph: input = x (256 ... 2047)
{
    volatile float y_calc;
    volatile float aa, bb, cc, dd;

    // y = 1.57235707E-07x3 - 3.81812218E-04x2 + 4.83639200E-01x + 4.79971690E+06 at 300 msec
    // to be defined

    // y = 1.64115303E-07x3 - 2.82201453E-04x2 + 3.65408640E-01x + 7.99970196E+06
    aa = x * x * 1.64115303 / 10000;
    aa = aa * x / 1000;
    bb = x * x * 2.82201453 / 10000;
    cc = x * 3.65408640 / 10;
    y_calc = aa - bb + cc + 7999701;    // ideal: y_calc = 8000000 at x = 1148
                                        // y = f(x) => y = f(1148) = 8000000
    return ((long)y_calc);
}

// *** calculation for new best fit of TRIM
long calculations(long xx_input, long yy_input)
{
    long hy, dy;
    bool plus = false, minus = false;
    volatile short loop_calc = 0;

    x_calc = xx_input;          // e.g. 1148 at 8000000 counts
    y_input = yy_input;         // e.g. ideal value 4800000 at 300msec pulse or 8000000 at 500msec pulse

    hy = Simulation(x_calc);    // create start value
    dy = y_input - hy;          // y = f(x) + dy

    // iteration loops: changing step by step x_calc and re-calculate the result.
    // when the zero-line (= e.g. 8000000-4 counts) is crossed, system has new x_calc value
    do
    {
        loop_calc++; 

        hy = Simulation(x_calc);
        hy = hy + dy;

        if (hy >= IDEAL_CNT)
        {
            x_calc--;
            minus = true;
        }

        if (hy <= IDEAL_CNT)
        {
            x_calc++;
            plus = true;
        }
    }
    while (!(plus == true && minus == true) && (loop_calc < 2000));    // crossing the 0-line

    if (loop_calc >= 2000)
        return -1;

    if (x_calc < 256) x_calc = 256;
    if (x_calc > 2047) x_calc = 2047;

    return (x_calc);    // new TRIM-value is replied
}

// *** main function
int auto_trim(uint8_t port_number)
{
    volatile char loop_counter;
    long m1;

    // system init

    // Set port_number as an input with pull up resistor.
    switch (port_number)
    {
        case 00: SetBits16(P00_MODE_REG, PUPD, 1); break;
        case 01: SetBits16(P01_MODE_REG, PUPD, 1); break;
        case 02: SetBits16(P02_MODE_REG, PUPD, 1); break;
        case 03: SetBits16(P03_MODE_REG, PUPD, 1); break;
        case 04: SetBits16(P04_MODE_REG, PUPD, 1); break;
        case 05: SetBits16(P05_MODE_REG, PUPD, 1); break;
        case 06: SetBits16(P06_MODE_REG, PUPD, 1); break;
        case 07: SetBits16(P07_MODE_REG, PUPD, 1); break;
        case 10: SetBits16(P10_MODE_REG, PUPD, 1); break;
        case 11: SetBits16(P11_MODE_REG, PUPD, 1); break;
        case 12: SetBits16(P12_MODE_REG, PUPD, 1); break;
        case 13: SetBits16(P13_MODE_REG, PUPD, 1); break;
        case 14: SetBits16(P14_MODE_REG, PUPD, 1); break;
        case 15: SetBits16(P15_MODE_REG, PUPD, 1); break;
        case 20: SetBits16(P20_MODE_REG, PUPD, 1); break;
        case 21: SetBits16(P21_MODE_REG, PUPD, 1); break;
        case 22: SetBits16(P22_MODE_REG, PUPD, 1); break;
        case 23: SetBits16(P23_MODE_REG, PUPD, 1); break;
        case 24: SetBits16(P24_MODE_REG, PUPD, 1); break;
        case 25: SetBits16(P25_MODE_REG, PUPD, 1); break;
        case 26: SetBits16(P26_MODE_REG, PUPD, 1); break;
        case 27: SetBits16(P27_MODE_REG, PUPD, 1); break;
        case 28: SetBits16(P28_MODE_REG, PUPD, 1); break;
        case 29: SetBits16(P29_MODE_REG, PUPD, 1); break;
        case 30: SetBits16(P30_MODE_REG, PUPD, 1); break;
        case 31: SetBits16(P31_MODE_REG, PUPD, 1); break;
        case 32: SetBits16(P32_MODE_REG, PUPD, 1); break;
        case 33: SetBits16(P33_MODE_REG, PUPD, 1); break;
        case 34: SetBits16(P34_MODE_REG, PUPD, 1); break;
        case 35: SetBits16(P35_MODE_REG, PUPD, 1); break;
        case 36: SetBits16(P36_MODE_REG, PUPD, 1); break;
        case 37: SetBits16(P37_MODE_REG, PUPD, 1); break;
    }

    // start TRIMming value
    Setting_Trim(DEFAULT_TRIM);

    // measure pulse high duration 2 times for noise checking
    m1 = Clock_Read(port_number);
    if (m1 <= 1000000)
    {
        Setting_Trim(0);    // if no square pulse was detected, then TRIM = 0
        return (-2);        // no square pulse detected
    }

    // boarders are -200 ... + 700
    if (m1 < (IDEAL_CNT + CCC_MIN) || m1 > (IDEAL_CNT + CCC_MAX))   // testing on noise (14 = 2 * minimum step) 
    {
        Setting_Trim(0);    // if out of limits, then TRIM = 0
        return (-1);        // out of limits
    }

    ccc = m1;

    loop_counter = 0;

    // iteration of calculating new TRIM, setting TRIM, Clock_Read and comparing ... {hm}
    do
    {
        loop_counter++;

        actual_trimming_value = GetWord16(CLK_FREQ_TRIM_REG);           // read TRIM register
        new_trimming_value = calculations(actual_trimming_value, ccc);  // calculate new calibration value
        if (new_trimming_value == -1) 
        {
            Setting_Trim(0);    // if out of limits, then TRIM = 0
            return (-1);        // out of limits
        }

        Setting_Trim(new_trimming_value);

        actual_trimming_value = GetWord16(CLK_FREQ_TRIM_REG);           // read TRIM register
        ccc = Clock_Read(port_number);                                  // reading speed of DA14580
        if (ccc == 0)
        {
            Setting_Trim(0);    // if no square pulse was detected, then TRIM = 0
            return (-2);        // no square pulse detected
        }
    }
    while (loop_counter <= MAX_LOOP && (ccc < IDEAL_CNT - PPM_1 || ccc > IDEAL_CNT + PPM_1 )); // +- 8 = 1 ppm

    if (ccc < IDEAL_CNT - PPM_BOUNDARY || ccc > IDEAL_CNT + PPM_BOUNDARY)
    {
        Setting_Trim(0);    // if out of limits, then TRIM = 0
        return (-1);        // out of limits (= new) ***!!!!    ... jump out at +- 10 ppm
    }

    actual_trimming_value = GetWord16(CLK_FREQ_TRIM_REG);   // read TRIM register

    return (actual_trimming_value);
}
