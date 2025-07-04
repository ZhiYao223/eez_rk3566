#ifndef EEZ_LVGL_UI_VARS_H
#define EEZ_LVGL_UI_VARS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// enum declarations



// Flow global variables

enum FlowGlobalVariables {
    FLOW_GLOBAL_VARIABLE_NONE
};

// Native global variables

extern int32_t get_var_start_time_1();
extern void set_var_start_time_1(int32_t value);
extern int32_t get_var_stop_time_1();
extern void set_var_stop_time_1(int32_t value);
extern int32_t get_var_start_time_2();
extern void set_var_start_time_2(int32_t value);
extern int32_t get_var_stop_time_2();
extern void set_var_stop_time_2(int32_t value);
extern int32_t get_var_tem();
extern void set_var_tem(int32_t value);
extern int32_t get_var_start_time_3();
extern void set_var_start_time_3(int32_t value);
extern int32_t get_var_stop_time_3();
extern void set_var_stop_time_3(int32_t value);
extern const char *get_var_address();
extern void set_var_address(const char *value);
extern const char *get_var_gateway();
extern void set_var_gateway(const char *value);
extern const char *get_var_dns();
extern void set_var_dns(const char *value);
extern int32_t get_var_rotational_speed();
extern void set_var_rotational_speed(int32_t value);
extern int32_t get_var_date_year();
extern void set_var_date_year(int32_t value);
extern int32_t get_var_time_hour();
extern void set_var_time_hour(int32_t value);
extern int32_t get_var_time_min();
extern void set_var_time_min(int32_t value);
extern int32_t get_var_date_month();
extern void set_var_date_month(int32_t value);
extern int32_t get_var_date_day();
extern void set_var_date_day(int32_t value);
extern int32_t get_var_sample_motor_ccw_seconds();
extern void set_var_sample_motor_ccw_seconds(int32_t value);
extern int32_t get_var_sample_motor_cw_seconds();
extern void set_var_sample_motor_cw_seconds(int32_t value);
extern int32_t get_var_sample_motor_stop_seconds();
extern void set_var_sample_motor_stop_seconds(int32_t value);
extern int32_t get_var_sample_times_1();
extern void set_var_sample_times_1(int32_t value);
extern int32_t get_var_sample_times_2();
extern void set_var_sample_times_2(int32_t value);
extern int32_t get_var_sample_times_3();
extern void set_var_sample_times_3(int32_t value);


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_VARS_H*/