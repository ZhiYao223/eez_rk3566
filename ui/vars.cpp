#include "vars.h"
#include <cstdlib>   // 提供 free()
#include <cstring>   // 提供 strdup()
#include <cstdint>   // 提供 int32_t

int32_t start_time_1;
extern "C" int32_t get_var_start_time_1(){
    return start_time_1;
}
extern "C" void set_var_start_time_1(int32_t value){
    start_time_1 = value;
}
int32_t stop_time_1;
extern "C" int32_t get_var_stop_time_1(){
    return stop_time_1;
}
extern "C" void set_var_stop_time_1(int32_t value){
    stop_time_1 = value;
}
int32_t start_time_2;
extern "C" int32_t get_var_start_time_2(){
    return start_time_2;
}
extern "C" void set_var_start_time_2(int32_t value){
    start_time_2 = value;
}

int32_t stop_time_2;
extern "C" int32_t get_var_stop_time_2(){
    return stop_time_2;
}
extern "C" void set_var_stop_time_2(int32_t value){
    stop_time_2 = value;
}

int32_t tem;
extern "C" int32_t get_var_tem(){
    return tem;
}
extern "C" void set_var_tem(int32_t value){
    tem = value;
}

int32_t start_time_3;
extern "C" int32_t get_var_start_time_3(){
    return start_time_3;
}
extern "C" void set_var_start_time_3(int32_t value){
    start_time_3 = value;
}

int32_t stop_time_3;
extern "C" int32_t get_var_stop_time_3(){
    return stop_time_3;
}
extern "C" void set_var_stop_time_3(int32_t value){
    stop_time_3 = value;
}

int32_t ccw_time;
extern "C" int32_t get_var_ccw_time(){
    return ccw_time;
}
extern "C" void set_var_ccw_time(int32_t value){
    ccw_time = value;
}

int32_t cw_time;
extern "C" int32_t get_var_cw_time(){
    return cw_time;
}
extern "C" void set_var_cw_time(int32_t value){
    cw_time = value;
}

int32_t stop_time;
extern "C" int32_t get_var_stop_time(){
    return stop_time;
}
extern "C" void set_var_stop_time(int32_t value){
    stop_time = value;
}

const char *address = nullptr;
extern "C" const char *get_var_address(){
    return address;
}
extern "C" void set_var_address(const char *value){
    if (address) free((void *)address); // 释放之前的字符串内存
        address = strdup(value); // 复制新的字符串（malloc 的）
}

const char* gateway = nullptr;
extern "C" const char *get_var_gateway(){
    return gateway;
}
extern "C" void set_var_gateway(const char *value){
    if (gateway) free((void *)gateway); // Free the previous gateway if it exists
        gateway = strdup(value);        // Duplicate the new gateway string
}

const char* dns = nullptr;
extern "C" const char *get_var_dns(){
    return dns;
}
extern "C" void set_var_dns(const char *value){
    if (dns) free((void *)dns); // Free the previous DNS if it exists
        dns = strdup(value);    // Duplicate the new DNS string
}



int32_t rotational_speed;
extern "C" int32_t get_var_rotational_speed(){
    return rotational_speed;
}
extern "C" void set_var_rotational_speed(int32_t value){
    rotational_speed = value;
}

int32_t date_year;
extern "C" int32_t get_var_date_year(){
    return date_year;
}
extern "C" void set_var_date_year(int32_t value){
    date_year = value;
}

int32_t time_hour;
extern "C" int32_t get_var_time_hour(){
    return time_hour;
}
extern "C" void set_var_time_hour(int32_t value){
    time_hour = value;
}

int32_t time_min;
extern "C" int32_t get_var_time_min(){
    return time_min;
}
extern "C" void set_var_time_min(int32_t value){
    time_min = value;
}

int32_t date_month;
extern "C" int32_t get_var_date_month(){
    return date_month;
}
extern "C" void set_var_date_month(int32_t value){
    date_month = value;
}

int32_t date_day;
extern "C" int32_t get_var_date_day(){
    return date_day;
}
extern "C" void set_var_date_day(int32_t value){
    date_day = value;
}


int32_t sample_interval_minutes;

extern "C" int32_t get_var_sample_interval_minutes() {
    return sample_interval_minutes;
}

extern "C" void set_var_sample_interval_minutes(int32_t value) {
    sample_interval_minutes = value;
}

// int32_t motor_ccw_seconds;
// extern "C" int32_t get_var_motor_ccw_seconds(){
//     return motor_ccw_seconds;
// }
// extern "C" void set_var_motor_ccw_seconds(int32_t value){
//     motor_ccw_seconds = value;
// }

// int32_t motor_cw_seconds;

// extern "C" int32_t get_var_motor_cw_seconds() {
//     return motor_cw_seconds;
// }

// extern "C" void set_var_motor_cw_seconds(int32_t value) {
//     motor_cw_seconds = value;
// }

// int32_t motor_stop_seconds;

// extern "C" int32_t get_var_motor_stop_seconds() {
//     return motor_stop_seconds;
// }

// extern "C" void set_var_motor_stop_seconds(int32_t value) {
//     motor_stop_seconds = value;
// }


int32_t manual_take_time_cw;

extern "C" int32_t get_var_manual_take_time_cw() {
    return manual_take_time_cw;
}

extern "C" void set_var_manual_take_time_cw(int32_t value) {
    manual_take_time_cw = value;
}


int32_t manual_take_time_ccw;

extern "C" int32_t get_var_manual_take_time_ccw() {
    return manual_take_time_ccw;
}

extern "C" void set_var_manual_take_time_ccw(int32_t value) {
    manual_take_time_ccw = value;
}

int32_t take_hour;
extern "C" int32_t get_var_take_hour(){
    return take_hour;
}
extern "C" void set_var_take_hour(int32_t value){
    take_hour = value;
}

int32_t take_minute;
extern "C" int32_t get_var_take_miniute(){
    return take_minute;
}
extern "C" void set_var_take_miniute(int32_t value){
    take_minute = value;
}

int32_t sample_motor_ccw_seconds;
extern "C" int32_t get_var_sample_motor_ccw_seconds(){
    return sample_motor_ccw_seconds;
}
extern "C" void set_var_sample_motor_ccw_seconds(int32_t value){
    sample_motor_ccw_seconds = value;
}
#include "vars.h"

int32_t sample_motor_cw_seconds;
extern "C" int32_t get_var_sample_motor_cw_seconds(){
    return sample_motor_cw_seconds;
}
extern "C" void set_var_sample_motor_cw_seconds(int32_t value){
    sample_motor_cw_seconds = value;
}

int32_t sample_motor_stop_seconds;
extern "C" int32_t get_var_sample_motor_stop_seconds(){
    return sample_motor_stop_seconds;
}
extern "C" void set_var_sample_motor_stop_seconds(int32_t value){
    sample_motor_stop_seconds = value;
}

int32_t sample_times_1;
extern "C" int32_t get_var_sample_times_1(){
    return sample_times_1;
}
extern "C" void set_var_sample_times_1(int32_t value){
    sample_times_1 = value;
}

int32_t sample_times_2;
extern "C" int32_t get_var_sample_times_2(){
    return sample_times_2;
}
extern "C" void set_var_sample_times_2(int32_t value){
    sample_times_2 = value;
}

int32_t sample_times_3;
extern "C" int32_t get_var_sample_times_3(){
    return sample_times_3;
}
extern "C" void set_var_sample_times_3(int32_t value){
    sample_times_3 = value;
}