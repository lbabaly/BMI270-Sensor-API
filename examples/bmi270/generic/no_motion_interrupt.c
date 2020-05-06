/**\
 * Copyright (c) 2020 Bosch Sensortec GmbH. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

/******************************************************************************/
/*!                 Header Files                                              */
#include <stdio.h>
#include "bmi2.h"
#include "bmi270.h"
#include "bmi2_common.h"

/******************************************************************************/
/*!         Static Function Declaration                                       */

/*!
 *  @brief This internal API is used to set configurations for no-motion.
 *
 *  @param[in] bmi2_dev       : Structure instance of bmi2_dev.
 *
 *  @return Status of execution.
 */
static int8_t set_feature_config(struct bmi2_dev *bmi2_dev);

/******************************************************************************/
/*!            Functions                                        */

/* This function starts the execution of program. */
int main(void)
{

    /* Status of api are returned to this variable. */
    int8_t rslt;

    /* Accel sensor and no-motion feature are listed in array. */
    uint8_t sens_list[2] = { BMI2_ACCEL, BMI2_NO_MOTION };

    /* Variable to get no-motion interrupt status. */
    uint16_t int_status = 0;

    /*! Sensor initialization configuration. */
    struct bmi2_dev bmi2_dev;

    /* Select features and their pins to be mapped to. */
    struct bmi2_sens_int_config sens_int = { .type = BMI2_NO_MOTION, .hw_int_pin = BMI2_INT1 };

    /* Initialize the dev structure */
    rslt = bmi2_interface_selection(&bmi2_dev);
    bmi2_error_codes_print_result(rslt);

    /* Initialize bmi270. */
    rslt = bmi270_init(&bmi2_dev);
    bmi2_error_codes_print_result(rslt);

    if (rslt == BMI2_OK)
    {

        /* Enable the selected sensors. */
        rslt = bmi2_sensor_enable(sens_list, 2, &bmi2_dev);
        bmi2_error_codes_print_result(rslt);

        if (rslt == BMI2_OK)
        {

            /* Set feature configurations for no-motion. */
            rslt = set_feature_config(&bmi2_dev);
            bmi2_error_codes_print_result(rslt);

            if (rslt == BMI2_OK)
            {

                /* Map the feature interrupt for no-motion. */
                rslt = bmi2_map_feat_int(&sens_int, 1, &bmi2_dev);
                bmi2_error_codes_print_result(rslt);
                printf("Do not move the board\n");

                /* Loop to get no-motion interrupt. */
                do
                {

                    /* Clear buffer. */
                    int_status = 0;

                    /* To get the interrupt status of no-motion. */
                    rslt = bmi2_get_int_status(&int_status, &bmi2_dev);
                    bmi2_error_codes_print_result(rslt);

                    /* To check the interrupt status of no-motion. */
                    if (int_status & BMI270_NO_MOT_STATUS_MASK)
                    {
                        printf("No-motion interrupt is generated\n");
                        break;
                    }
                } while (rslt == BMI2_OK);
            }
        }
    }

    return rslt;
}

/*!
 * @brief This internal API is used to set configurations for no-motion.
 */
static int8_t set_feature_config(struct bmi2_dev *bmi2_dev)
{

    /* Status of api are returned to this variable. */
    int8_t rslt;

    /* Structure to define the type of sensor and its configurations. */
    struct bmi2_sens_config config;

    /* Configure the type of feature. */
    config.type = BMI2_NO_MOTION;

    /* Get default configurations for the type of feature selected. */
    rslt = bmi2_get_sensor_config(&config, 1, bmi2_dev);
    bmi2_error_codes_print_result(rslt);
    if (rslt == BMI2_OK)
    {

        /* NOTE: The user can change the following configuration parameters according to their requirement. */
        /* 1LSB equals 20ms. Default is 100ms, setting to 80ms. */
        config.cfg.no_motion.duration = 0x04;

        /* 1LSB equals to 0.48mg. Default is 83mg, setting to 50mg. */
        config.cfg.no_motion.threshold = 0x68;

        /* Set new configurations. */
        rslt = bmi2_set_sensor_config(&config, 1, bmi2_dev);
        bmi2_error_codes_print_result(rslt);
    }

    return rslt;
}
