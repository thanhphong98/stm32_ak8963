// MIT License

// Copyright (c) 2020 phonght32

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "stm_err.h"
#include "stm_log.h"
#include "driver/i2c.h"

#include "mpu9250.h"
#include "ak8963.h"

/* 
 * This example shows how to read 9 motions from MPU9250.
 *
 * I2C parameters:
 *      - I2C NUM 1, pinspack 1.
 *      - Clock speed: 400000 KHz.
 *      - SCL: PB6
 *      - SDA: PB7
 *
 * MPU9250 parameters:
 *      - Accelerometer full scale range: 8G.
 *      - Gyroscope full scale range: 2000d deg/s.
 *      - Clock source: Auto select best clock source.
 *      - Digital Low Pass Filter: 41 Hz accel, 42 Hz gyro.
 *      - Sleep mode: disable.
 *
 * AK8963 parameters:
 *      - Mode: continous measurement 2.
 *      - Resolution: 16bit magnetometer full scale range.
 */

#define TEST_RAW
// #define TEST_CALI
// #define TEST_SCALE
// #define AUTO_CALIB

#define I2C_NUM                         I2C_NUM_2
#define I2C_PINS_PACK                   I2C_PINS_PACK_1
#define I2C_CLK_SPEED                   400000

#define MPU9250_AFS_RANGE               MPU9250_AFS_SEL_8G
#define MPU9250_FS_RAGNE                MPU9250_FS_SEL_2000
#define MPU9250_CLKSEL                  MPU9250_CLKSEL_AUTO   
#define MPU9250_DLPF                    MPU9250_41ACEL_42GYRO_BW_HZ
#define MPU9250_SLEEP_MODE              MPU9250_DISABLE_SLEEP_MODE
#define MPU9250_COMM_MODE_PROTOCOL      MPU9250_COMM_MODE_I2C

#define AK8963_MODE                     AK8963_MODE_CONT_MEASUREMENT_2
#define AK8963_RESOLUTION               AK8963_MFS_16BIT
#define AK8963_COMM_MODE_PROTOCOL       AK8963_COMM_MODE_I2C

/* Handle structure */
mpu9250_handle_t mpu9250_handle;
ak8963_handle_t ak8963_handle;

#ifdef TEST_RAW
mpu9250_raw_data_t gyro_data, accel_data;
ak8963_raw_data_t mag_data;
#endif

#ifdef TEST_CALI
mpu9250_cali_data_t gyro_data, accel_data;
ak8963_cali_data_t mag_data;
#endif

#ifdef TEST_SCALE
mpu9250_scale_data_t gyro_data, accel_data;
ak8963_scale_data_t mag_data;
#endif

const char *TAG = "APP_MAIN";

static void example_task(void* arg)
{
    /* Configure MPU9250 */
    mpu9250_hw_info_t mpu9250_hw_info = {
        .i2c_num = I2C_NUM, 
        .i2c_pins_pack = I2C_PINS_PACK,
        .i2c_speed = 400000,
        .is_init = false,
    };

    mpu9250_cfg_t mpu9250_cfg;
    mpu9250_cfg.afs_sel = MPU9250_AFS_RANGE;
    mpu9250_cfg.clksel = MPU9250_CLKSEL;
    mpu9250_cfg.dlpf_cfg =  MPU9250_DLPF;
    mpu9250_cfg.fs_sel = MPU9250_FS_RAGNE;
    mpu9250_cfg.sleep_mode = MPU9250_SLEEP_MODE;
    mpu9250_cfg.comm_mode = MPU9250_COMM_MODE_PROTOCOL;
    mpu9250_cfg.hw_info = mpu9250_hw_info;
    mpu9250_handle = mpu9250_init(&mpu9250_cfg);

    /* Configure AK8963 */
    ak8963_hw_info_t ak8963_hw_info = {
        .i2c_num = I2C_NUM, 
        .i2c_pins_pack = I2C_PINS_PACK,
        .i2c_speed = 400000,
        .is_init = true,
    };

    ak8963_cfg_t ak8963_cfg;
    ak8963_cfg.opr_mode = AK8963_MODE;
    ak8963_cfg.mfs_sel = AK8963_RESOLUTION;
    ak8963_cfg.comm_mode = AK8963_COMM_MODE_PROTOCOL;
    ak8963_cfg.hw_info = ak8963_hw_info;
    ak8963_handle = ak8963_init(&ak8963_cfg);

#ifdef AUTO_CALIB
    mpu9250_auto_calib(mpu9250_handle);
    ak8963_auto_calib(ak8963_handle);
#endif

    while(1)
    {
#ifdef TEST_RAW 
        mpu9250_get_accel_raw(mpu9250_handle, &accel_data);
        mpu9250_get_gyro_raw(mpu9250_handle, &gyro_data);
        ak8963_get_mag_raw(ak8963_handle, &mag_data);
        STM_LOGI(TAG, "accel_x: %d\taccel_y: %d\taccel_z: %d", accel_data.x_axis, accel_data.y_axis, accel_data.z_axis);
        STM_LOGI(TAG, "gyro_x : %d\tgyro_y:  %d\tgyro_z:  %d", gyro_data.x_axis, gyro_data.y_axis, gyro_data.z_axis);
        STM_LOGI(TAG, "mag_x:   %d\tmag_y:   %d\tmag_z:   %d", mag_data.x_axis, mag_data.y_axis, mag_data.z_axis);
#endif

#ifdef TEST_CALI
        mpu9250_get_accel_cali(mpu9250_handle, &accel_data);
        mpu9250_get_gyro_cali(mpu9250_handle, &gyro_data);
        ak8963_get_mag_cali(ak8963_handle, &mag_data);
        STM_LOGI(TAG, "accel_x: %d\taccel_y: %d\taccel_z: %d", accel_data.x_axis, accel_data.y_axis, accel_data.z_axis);
        STM_LOGI(TAG, "gyro_x : %d\tgyro_y:  %d\tgyro_z:  %d", gyro_data.x_axis, gyro_data.y_axis, gyro_data.z_axis);
        STM_LOGI(TAG, "mag_x:   %d\tmag_y:   %d\tmag_z:   %d", mag_data.x_axis, mag_data.y_axis, mag_data.z_axis);
#endif

#ifdef TEST_SCALE
        mpu9250_get_accel_scale(mpu9250_handle, &accel_data);
        mpu9250_get_gyro_scale(mpu9250_handle, &gyro_data);
        ak8963_get_mag_scale(ak8963_handle, &mag_data);
        STM_LOGI(TAG, "accel_x: %f\taccel_y: %f\taccel_z: %f", accel_data.x_axis, accel_data.y_axis, accel_data.z_axis);
        STM_LOGI(TAG, "gyro_x : %f\tgyro_y:  %f\tgyro_z:  %f", gyro_data.x_axis, gyro_data.y_axis, gyro_data.z_axis);
        STM_LOGI(TAG, "mag_x:   %f\tmag_y:   %f\tmag_z:   %f", mag_data.x_axis, mag_data.y_axis, mag_data.z_axis);
#endif

        STM_LOGI(TAG, "*****************************");
        vTaskDelay(200/portTICK_PERIOD_MS);
    }
}

int main(void)
{
    /* Set output log level */
    stm_log_level_set("*", STM_LOG_NONE);
    stm_log_level_set("APP_MAIN", STM_LOG_INFO);

    /* Create task */
    xTaskCreate(example_task, "example_task", 512, NULL, 5, NULL);

    /* Start RTOS scheduler */
    vTaskStartScheduler();
}