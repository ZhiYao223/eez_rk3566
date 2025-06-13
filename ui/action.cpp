/* author : 刘智耀 
   email  : zhiyao3256@qq.com
*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <thread>
#include <atomic>

#include "actions.h"
#include "../ui/vars.h"
#include "../ui/screens.h"
#include "../../main/src/gpio.h"
#include "../../main/rapidjson/writer.h"
#include "../../main/rapidjson/prettywriter.h"
#include "../../main/rapidjson/document.h"
#include "../../main/rapidjson/stringbuffer.h"
#include "../../main/rapidjson/filereadstream.h"
#include "../../lvgl/src/misc/lv_event.h"

// 全局标志位，表示不退出线程（线程运行中）。
static bool bQuitAutoTakeThread = false;

// #define CONFIG_PATH     "/opt/sample/config.json"
#define CONFIG_PATH     "../eez_rk3566/ui/config.json" // 配置文件路径，存储在项目目录下的 ui 文件夹中。

static void sample_motor_stop_func(); // 关闭采样电机

// 该函数用于在主界面上显示当前电机状态的标签，并根据传入的状态字符串设置标签的颜色。
void show_motor_status_with_color(lv_obj_t * parent, const char * status, lv_color_t color)
{
    lv_obj_t * label = lv_label_create(parent);  // 创建一个标签对象
    lv_label_set_text(label, status);             // 设置标签文本为传入的状态字符串
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);  // 将标签居中对齐到父对象
    lv_obj_set_style_text_color(label, color, LV_PART_MAIN | LV_STATE_DEFAULT); // 设置标签文本颜色
}

// 从 JSON 配置文件 "../eez_rk3566/ui/config.json" 中读取参数并加载到程序中。
void load_configs(){
    // 打开文件
    FILE *fp = fopen(CONFIG_PATH, "rb");                // 以二进制只读模式 "rb" 打开配置文件。
    if (NULL == fp){                                    // 如果打开失败，打印错误信息并返回。
        printf("open file %s failed\n", CONFIG_PATH);
        return;
    }
    // 获取文件大小，并分配内存
    fseek(fp, 0, SEEK_END);                             // 移动文件指针到末尾
    int size = ftell(fp);                               // 获取当前文件大小（单位：字节）。
    fseek(fp, 0, SEEK_SET);                             // 回到文件开头准备读取。
    char *buffer = (char *)malloc(size + 1);            // 分配一个多出1字节的缓冲区（用于末尾加 \0）,以存放文件内容
    if (NULL == buffer){                                // 如果分配失败，打印错误信息并返回。
        printf("malloc buffer failed\n");
        fclose(fp);                                     // 关闭文件指针
        return;
    }
    // 读取文件内容到缓冲区
    fread(buffer, size, 1, fp);                         // 一次性读取整个文件到 buffer
    fclose(fp);                                         // 关闭文件指针 fp。

    buffer[size] = '\0';                                // 手动添加 C 字符串结束符，确保 buffer 是合法的字符串（JSON 格式字符串）。
    rapidjson::Document doc;                            //
    doc.Parse(buffer);                                  // 解析 JSON 字符串。
    if (doc.HasParseError())                            // 若解析失败，释放内存并输出错误信息。
    {
        free(buffer);
        printf("parse json failed\n");
        return;
    }
    /*检查是否存在 "shift_setting" 字段且是对象。如果存在且是一个对象，则读取其中的电机相关设置。读取 "sample_setting" 对象中的电机设置参数，并赋值给系统变量。*/
    if (doc.HasMember("shift_setting") && doc["shift_setting"].IsObject()){
        const rapidjson::Value& shift = doc["shift_setting"]; // 获取 "shift_setting" 对象。
        // 读取字段并赋值给系统变量, 检查是否存在 "start_time1" 字段且是整数。
        if (shift.HasMember("start_time_1") && shift["start_time_1"].IsInt()){
            int start_time_1 = shift["start_time_1"].GetInt();
            set_var_start_time_1(start_time_1);   // 设置电机采样间隔时间
        }
        if (shift.HasMember("stop_time_1") && shift["stop_time_1"].IsInt()){
            int stop_time_1 = shift["stop_time_1"].GetInt();
            set_var_stop_time_1(stop_time_1);   // 设置电机采样间隔时间
        }
        if (shift.HasMember("sample_times_1") && shift["sample_times_1"].IsInt()){
            int sample_times_1 = shift["sample_times_1"].GetInt();
        }
        if (shift.HasMember("start_time_2") && shift["start_time_2"].IsInt()){
            int start_time_2 = shift["start_time_2"].GetInt();
            set_var_start_time_2(start_time_2);   // 设置电机采样间隔时间
        }
        if (shift.HasMember("stop_time_2") && shift["stop_time_2"].IsInt()){
            int stop_time_2 = shift["stop_time_2"].GetInt();
            set_var_start_time_2(stop_time_2);   // 设置电机采样间隔时间
        }
        if (shift.HasMember("sample_times_2") && shift["sample_times_2"].IsInt()){
            int sample_times_2 = shift["sample_times_2"].GetInt();
        }
        if (shift.HasMember("start_time_3") && shift["start_time_3"].IsInt()){
            int start_time_3 = shift["start_time_3"].GetInt();
            set_var_start_time_3(start_time_3);   // 设置电机采样间隔时间
        }
        if (shift.HasMember("stop_time_3") && shift["stop_time_3"].IsInt()){
            int stop_time_3 = shift["stop_time_3"].GetInt();
            set_var_start_time_3(stop_time_3);   // 设置电机采样间隔时间
        } 
        if (shift.HasMember("sample_times_3") && shift["sample_times_3"].IsInt()){
            int sample_times_3 = shift["sample_times_3"].GetInt();
        }
    }
    // 检查是否存在 "sample_setting" 字段且是对象。
    if (doc.HasMember("sample_setting") && doc["sample_setting"].IsObject()){
        const rapidjson::Value& sample = doc["sample_setting"]; // 获取 "sample_setting" 对象。
        if (sample.HasMember("motor_cw_seconds") && sample["motor_cw_seconds"].IsInt()){
            int motor_cw_seconds = sample["motor_cw_seconds"].GetInt();
            set_var_sample_motor_cw_seconds(motor_cw_seconds); // 设置电机顺时针旋转时间。
        } else  set_var_sample_motor_cw_seconds(300); // 若不存在, 设置电机顺时针旋转时间，默认值为 300。

        if (sample.HasMember("motor_ccw_seconds") && sample["motor_ccw_seconds"].IsInt()){
            int motor_ccw_seconds = sample["motor_ccw_seconds"].GetInt();
            set_var_sample_motor_ccw_seconds(motor_ccw_seconds); // 设置电机逆时针旋转时间。
        } else set_var_sample_motor_ccw_seconds(240); // 设若不存在, 置电机逆时针旋转时间，默认值为 240。

        if (sample.HasMember("motor_stop_time") && sample["motor_stop_time"].IsInt()){
            int motor_stop_time = sample["motor_stop_time"].GetInt();
            set_var_sample_motor_stop_seconds(motor_stop_time); // 设置电机停止延迟时间。
        } else set_var_sample_motor_stop_seconds(240);   // 设置电机停止延迟时间，默认值为 240。
    }
    // 检查是否存在 "comm_setting" 字段且是对象。
    if (doc.HasMember("comm_setting") && doc["comm_setting"].IsObject()){
        const rapidjson::Value& comm = doc["comm_setting"]; // 获取 "comm_setting" 对象。
        if (comm.HasMember("Address") && comm["Address"].IsString()){
            set_var_address(comm["Address"].GetString());   // 设置设备地址
        }
        if (comm.HasMember("Gateway") && comm["Gateway"].IsString()){
            set_var_gateway(comm["Gateway"].GetString());   // 设置网关地址
        }
        if (comm.HasMember("DNS") && comm["DNS"].IsString()){
            set_var_dns(comm["DNS"].GetString());           // 设置 DNS 地址
        }        
    }
    // 检查是否存在 "test_setting" 字段且是整数。
    if (doc.HasMember("test_setting") && doc["test_setting"].IsObject()){
        const rapidjson::Value& test = doc["test_setting"]; // 获取 "Test_setting" 对象。
        if (test.HasMember("rotate_motor_speed") && test["rotate_motor_speed"].IsInt()){
            int rotate_motor_speed = test["rotate_motor_speed"].GetInt();
            set_var_rotational_speed(rotate_motor_speed);   // 设置电机采样间隔时间
        }
    }
    free(buffer); //最后释放之前分配的 JSON 缓冲区内存。
}

void action_save_setting_func(lv_event_t * e){
    lv_obj_t *btn = (lv_obj_t *)lv_event_get_target(e);
    rapidjson::Document doc;                                   // 表示 JSON 的根文档对象
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();// 获取分配器，用于后续添加成员。
    if (!doc.IsObject()) doc.SetObject();   // 先检查配置文件是否存在，如果不存在则创建一个新的空文档。
 
    // 1. 先读取旧的 config.json 文件
    FILE* fp = fopen(CONFIG_PATH, "rb");    // 打开配置文件，二进制读取模式
    if (fp != NULL) {
        char readBuffer[65536];             // 定义一个缓冲区用于读取文件内容   
        rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer)); // 创建一个文件读取流
        doc.ParseStream(is);                // 解析 JSON 文件内容到 Document 对象中
        fclose(fp);                         // 关闭文件
    }

    // 2. 根据按钮指针执行不同的操作，只更新当前部分内容，保留其余结构
    if (btn == objects.btn_save_shift_setting) { 
        // 班次设置
        printf("班次设置按钮被按下\n"); // AddMember第一个参数“字段名”与控件Name无直接关系，只是用于JSON 键名
        rapidjson::Value shift_obj(rapidjson::kObjectType); // 创建一个 JSON 对象，用于存储班次设置。
        shift_obj.AddMember("start_time_1", get_var_start_time_1(),  allocator);// 添加字段：第1班开始时间1
        shift_obj.AddMember("stop_time_1",  get_var_stop_time_1(),   allocator);// 添加字段：第1班停止时间1
        shift_obj.AddMember("sample_times_1",get_var_sample_times_1(),allocator);// 添加字段：第1班采样次数
        shift_obj.AddMember("start_time_2", get_var_start_time_2(),  allocator);// 添加字段：第2班开始时间2
        shift_obj.AddMember("stop_time_2",  get_var_stop_time_2(),   allocator);// 添加字段: 第2班停止时间2
        shift_obj.AddMember("sample_times_2",get_var_sample_times_2(),allocator);// 添加字段：第2班采样次数
        shift_obj.AddMember("start_time_3", get_var_start_time_3(),  allocator);// 添加字段：第3班开始时间3
        shift_obj.AddMember("stop_time_3",  get_var_stop_time_3(),   allocator);// 添加字段: 第3班停止时间3
        shift_obj.AddMember("sample_times_3",get_var_sample_times_3(),allocator);// 添加字段：第3班采样次数
        // 如果文档中已经存在 "shift_setting" 键，则更新其值。
        if (doc.HasMember("shift_setting")) doc["shift_setting"] = shift_obj; // 更新现有的 "shift_setting" 对象。
        else doc.AddMember("shift_setting", shift_obj, allocator); // 否则添加新的 "shift_setting" 对象。
        printf("班次设置已保存\n");
    }
    // 如果是保存设置按钮
    if (btn == objects.btn_save_sample_setting) { 
        // 采样设置
        printf("采样设置按钮被按下\n");
        rapidjson::Value sample_obj(rapidjson::kObjectType); // 创建一个 JSON 对象，用于存储采样设置。
        sample_obj.AddMember("motor_cw_seconds",   get_var_sample_motor_cw_seconds(),   allocator);// 添加字段：清灰正转
        sample_obj.AddMember("motor_ccw_seconds",  get_var_sample_motor_ccw_seconds(),  allocator);// 添加字段：排灰反转
        sample_obj.AddMember("motor_stop_seconds", get_var_sample_motor_stop_seconds(), allocator);// 添加字段：等待时间
        // 如果文档中已经存在 "sample_setting" 键，则更新其值。
        if (doc.HasMember("sample_setting")) doc["sample_setting"] = sample_obj;//更新现有的 "sample_setting"对象
        else  doc.AddMember("sample_setting", sample_obj, allocator); //否则添加新的 "sample_setting" 对象。
        printf("采样设置已保存\n");
    }
    // 如果是网络通迅按钮
    if (btn == objects.btn_save_communicate_setting){
        // 通讯设置
        printf("通迅设置按钮被按下\n");
        rapidjson::Value comm_obj(rapidjson::kObjectType);  // 创建一个 JSON 对象，用于存储通讯设置。
        const char* str_address = get_var_address();        // 获取 IP 地址
        const char* str_gateway = get_var_gateway();        // 获取网关
        const char* str_dns     = get_var_dns();            // 获取 DNS
        comm_obj.AddMember("Address", rapidjson::Value(str_address, allocator), allocator);// 添加字段：IP地址
        comm_obj.AddMember("Gateway", rapidjson::Value(str_gateway, allocator), allocator);// 添加字段：网关
        comm_obj.AddMember("DNS",     rapidjson::Value(str_dns, allocator),     allocator);// 添加字段：DNS
        // 如果文档中已经存在 "comm_setting" 键，则更新其值。
        if (doc.HasMember("comm_setting")) doc["comm_setting"] = comm_obj; // 更新现有的 "comm_setting"对象
        else  doc.AddMember("comm_setting", comm_obj, allocator);// 否则添加新的 "comm_setting" 对象。
        printf("通讯设置已保存\n");
    }
    // 如果是电机测试按钮
    if (btn == objects.btn_save_motor_test_setting){
        // 测试设置
        printf("测试设置按钮被按下");
        rapidjson::Value test_obj(rapidjson::kObjectType); // 创建一个 JSON 对象，用于存储测试设置。
        test_obj.AddMember("rotate_motor_speed", get_var_rotational_speed(), allocator);
        // 如果文档中已经存在 "test_setting" 键，则更新其值。
        if (doc.HasMember("test_setting")) doc["test_setting"] = test_obj; // 更新现有的 "test_setting" 对象。
        else  doc.AddMember("test_setting", test_obj, allocator);// 否则添加新的 "test_setting" 对象。
        printf("测试设置已保存");
    }
    // 3. 准备写入新的 JSON 数据
    rapidjson::StringBuffer buffer;                                    // 字符串缓冲区，用于临时存放 JSON 文本。
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);   // 将 doc 序列化为 JSON 字符串，写入 buffer。
    // 4. 将 JSON 写入字符串并输出到文件
    writer.SetIndent(' ', 4);                           // 设置缩进字符和缩进宽度
    doc.Accept(writer);                                 // 将JSON 文档序列化成字符串。
    const char* json = buffer.GetString();              // 获取最终的 JSON 字符串内容。
    fp = fopen(CONFIG_PATH, "wb+");                     // 尝试以可读写模式打开（或创建）配置文件。
    if (NULL == fp){
        printf("open file %s failed\n", CONFIG_PATH);   // 如果失败，输出错误信息并提前返回。
        return;
    }
    fwrite(json, strlen(json), 1, fp);                  // 将 JSON 字符串写入文件。
    fclose(fp);                                         // 关闭文件，完成保存操作
}


// 保存设置按钮的回调函数 
/*
void action_save_setting_func_back(lv_event_t * e)
{
    rapidjson::Document doc;                                   // 表示 JSON 的根文档对象
    rapidjson::StringBuffer buffer;                            // 字符串缓冲区，用于临时存放 JSON 文本。
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer); // 将 doc 序列化为 JSON 字符串，写入 buffer。
    doc.SetObject();                                           //将文档设置为 JSON 对象（即键值对形式的结构）。
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();  // 获取分配器，用于后续添加成员。

    rapidjson::Value rpStartTime1(rapidjson::kNumberType); // 创建一个 JSON 字符串类型的字段 rpStartTime1。
    rpStartTime1.SetInt(get_var_start_time_1());           // 将其值设为当前系统变量 get_var_start_time_1()
    doc.AddMember("start_time_1", rpStartTime1, allocator);// 添加到 JSON 文档中，字段名为 "start_time_1"

    rapidjson::Value rpStopTime1(rapidjson::kNumberType);  // 创建一个 JSON 字符串类型的字段 rpStopTime1。
    rpStopTime1.SetInt(get_var_stop_time_1());             // 将其值设为当前系统变量 get_var_stop_time_1()
    doc.AddMember("stop_time_1", rpStopTime1, allocator);  // 添加到 JSON 文档中，字段名为 "stop_time_1"

    // 添加字段：采样时间间隔（单位：分钟）, 以 JSON 数值类型添加字段 "sample_interval_minutes"，表示采样时间间隔。
    rapidjson::Value elInterval(rapidjson::kNumberType);  // 创建一个 JSON 数值类型的字段 elInterval。
    // 将其值设为当前系统变量 get_var_sample_interval_minutes()
    elInterval.SetInt(get_var_sample_interval_minutes());       
    // 添加到 JSON 文档中，字段名为 "sample_interval_minutes"
    doc.AddMember("sample_interval_minutes", elInterval, doc.GetAllocator());   

    // 添加字段：正向旋转时间（单位：秒）,  以同样方式添加字段 "motor_cw_seconds"，表示电机顺时针旋转时间。
    rapidjson::Value elCwRotate(rapidjson::kNumberType);
    elCwRotate.SetInt(get_var_motor_cw_seconds());
    doc.AddMember("motor_cw_seconds", elCwRotate, doc.GetAllocator());

    // 添加字段：反向旋转时间（单位：秒）,  以同样方式添加字段 "motor_ccw_seconds"，表示电机逆时针旋转时间。
    rapidjson::Value elCcwRotate(rapidjson::kNumberType);
    elCcwRotate.SetInt(get_var_motor_ccw_seconds());
    doc.AddMember("motor_ccw_seconds", elCcwRotate, doc.GetAllocator());

    // 添加字段：电机停止时间（单位：秒）,  以同样方式添加字段 "input_motor_stop"，表示电机停止时间。
    rapidjson::Value elMotorStop(rapidjson::kNumberType);
    elMotorStop.SetInt(get_var_motor_stop_seconds());
    doc.AddMember("motor_stop_seconds", elMotorStop, doc.GetAllocator());

    // 添加字段：手动正转取灰时间, 以同样方式添加字段 "input_cw_manual_time"，表示手动正转取灰时间。
    rapidjson::Value elManualTakeCW(rapidjson::kNumberType);
    elManualTakeCW.SetInt(get_var_motor_cw_seconds());
    doc.AddMember("input_cw_manual_time", elManualTakeCW, doc.GetAllocator());

    // 添加字段：手动反转取灰时间, 以同样方式添加字段 "input_ccw_manual_time"，表示手动反转取灰时间。
    rapidjson::Value elManualTakeCCW(rapidjson::kNumberType);
    elManualTakeCCW.SetInt(get_var_motor_ccw_seconds());
    doc.AddMember("input_ccw_manual_time", elManualTakeCCW, doc.GetAllocator());

    // 将 JSON 写入字符串并输出到文件
    doc.Accept(writer);                     // 将JSON 文档序列化成字符串。
    const char* json = buffer.GetString();  // 获取最终的 JSON 字符串内容。
    // 保存到文件
    FILE *fp = fopen(CONFIG_PATH, "wb+");   // 尝试以可读写模式打开（或创建）配置文件。
    // 如果失败，输出错误信息并提前返回。
    if (NULL == fp){
        printf("open file %s failed\n", CONFIG_PATH);
        return;
    }
    fwrite(json, strlen(json), 1, fp);      // 将 JSON 字符串写入文件。
    fclose(fp);                             // 关闭文件，完成保存操作
}
*/


void init_main_label(){
    // lv_obj_add_event_cb(objects.main_label, label_event_cb, LV_EVENT_MSG_RECEIVED, NULL);
}

/*
*/
char main_label_text[128] = "电机停止状态";
static void update_label_cb(void *param) {
  lv_label_set_text((lv_obj_t *)param, main_label_text);
}
// 
typedef  struct btn_bg_param {
    lv_obj_t *btn;      // 按钮对象指针
    lv_color_t color;   // 按钮背景颜色
}BG_COLOR_PARAM;

static BG_COLOR_PARAM btnBgParam;

static void update_btn_bg_async_cb(void *param) {
    BG_COLOR_PARAM  *btn_param = (BG_COLOR_PARAM *)param;
    lv_obj_set_style_bg_color(btn_param->btn, btn_param->color, LV_PART_MAIN | LV_STATE_DEFAULT);
    // lv_obj_set_style_bg_color((lv_obj_t *)param, lv_palette_main(LV_PALETTE_RED), LV_PART_MAIN | LV_STATE_DEFAULT);
}

// 使用异步调用来更新按钮的背景颜色的函数，避免在主线程中直接操作 UI。
static void SetBtnBgColor(lv_obj_t *btn, lv_color_t color){ 
    btnBgParam.btn = btn;       // 设置按钮对象指针
    btnBgParam.color = color;   // 设置按钮背景颜色
    lv_async_call(update_btn_bg_async_cb, &btnBgParam);
}

/*  在多线程环境下，bool 类型访问本身不是原子操作，因此需要使用原子变量来确保线程安全，
    这样可以避免潜在的数据竞争或内存可见性问题。*/
static std::atomic<bool> bMotorInUse(false);    // 使用原子变量来表示电机使用状态，确保线程安全。
static std::atomic<bool> bInTakeCCW(false);     // 使用原子变量来表示手动排灰状态，确保线程安全。
static std::atomic<bool> bQuitCCWThread(false); // 使用原子变量来表示手动排灰线程退出标志，确保线程安全。
static std::atomic<bool> bInTakeCW(false);      // 使用原子变量来表示手动取灰状态，确保线程安全。
static std::atomic<bool> bQuitThreadCW(false);  // 使用原子变量来表示手动取灰线程退出标志，确保线程安全。

/***********************************************************/
/* 设置电机使用状态的宏定义, 并引入形参use(文本参数)，表示是否在使用电机。*/
#define   SET_MOTOR_IN_USE(use) \
          do{ \
                printf("[%s:%d] set motor %s use\n", __FUNCTION__, __LINE__, use ? "in" : "not in"); \
                bMotorInUse.store(use);\
            }while(0);
/***********************************************************/


// 手动取灰按钮的回调函数 
void action_btn_manual_take_cw(lv_event_t * e){
    lv_obj_add_state(objects.btn_manual_take_ccw, LV_STATE_DISABLED);// 禁用手动排灰按钮，防止在手动取灰过程中点击。
    if (!bInTakeCW.load()){    // 手动取灰状态为 false，当前没有进行手动取灰操作，可以进行手动取灰。
        if (bMotorInUse.load()){  
            printf("Motor in use!\n");
            return;
        }
        bInTakeCW.store(true);  // 设置手动取灰状态为 true，表示正在进行手动取灰操作。
        SET_MOTOR_IN_USE(true); // 设置电机使用状态为 true，表示电机正在被使用。
        // 设置按钮背景颜色为亮绿色，表示正在进行手动取灰操作。
        SetBtnBgColor(objects.btn_manual_take_cw, lv_color_hex(0x5ddc27)); //取灰按钮-->亮绿色
        bQuitThreadCW.store(false); // 设置退出标志为 false，表示线程可以继续运行。
        // 创建一个新的线程来执行手动取灰操作
        std::thread t1([](){
            printf("manual take start cw\n");
            snprintf(main_label_text, sizeof(main_label_text), "电机转动中(正转)");
            lv_async_call(update_label_cb, objects.main_label);
            ACES::GPIO::GetInstance().SetMotorDirection(ACES::MotorDirection::CW);// 设置电机方向为顺时针旋转。
            ACES::GPIO::GetInstance().SetMotorPower(ACES::MotoPowerState::ON);// 设置电机供电状态为 ON，开始电机工作。
            int s2 = get_var_sample_motor_cw_seconds(); // 获取手动取灰时间（正转旋转时间）。
            int secondCount = 0;
            while((secondCount / 10) < s2){ 
                if (bQuitThreadCW.load()){
                    action_btn_motor_stop(nullptr); // 停止取灰电机
                    printf("手动取灰程序已结束！\n");// 打印调试信息，表示手动取灰操作已退出。
                    // 清除手动排灰按钮的禁用状态，允许再次按下。
                    lv_obj_clear_state(objects.btn_manual_take_ccw, LV_STATE_PRESSED);
                    return;     // 退出线程
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(100));// 每 100 毫秒检查一次退出标志。
                secondCount++;
            }
            action_btn_motor_stop(nullptr); // 停止取灰电机
            printf("手动取灰程序已结束！\n"); // 打印调试信息，表示手动取灰操作已退出。
            // 清除手动排灰按钮的禁用状态，允许再次点击。
            lv_obj_clear_state(objects.btn_manual_take_ccw, LV_STATE_PRESSED);
        });
        t1.detach();
    } else // 如果手动取灰状态为 true，表示当前正在进行手动取灰操作。
    {   
        // 将手动取灰按钮设置为红色背景色，表示正在进行手动取灰操作。且不可点击。
        // SetBtnBgColor(objects.btn_manual_take_ccw, lv_color_hex(0xFF0000)); // 红色背景
        bQuitThreadCW.store(true);
    }
}


// 手动排灰按钮的回调函数
void action_btn_manual_take_ccw(lv_event_t * e)
{
    if (!bInTakeCCW.load()) // 如果手动排灰状态为 false，表示当前没有进行手动排灰操作。
    {   
        // 如果电机正在使用中，打印提示信息并返回。
        if (bMotorInUse.load()){printf("Motor in use!\n");  return; }
        bInTakeCCW.store(true);  // 设置手动排灰状态为 true，表示正在进行手动排灰操作。
        SET_MOTOR_IN_USE(true);  // 设置电机使用状态为 true，表示电机正在被使用。
        // 设置按钮背景颜色为红色，表示正在进行手动排灰操作。
        SetBtnBgColor(objects.btn_manual_take_ccw, lv_color_hex(0X90ee90));//手动排灰按钮-->亮绿色，
        SetBtnBgColor(objects.btn_manual_take_cw, lv_color_hex(0x009ea2)); // 设置手动取灰按钮背景颜色为墨绿色。
        lv_obj_add_state(objects.btn_manual_take_cw, LV_STATE_DISABLED); // 禁用手动取灰按钮，防止在手动排灰过程中点击。
        bQuitCCWThread.store(false); // 设置退出标志为 false，表示线程可以继续运行。
        // 创建一个新的线程来执行手动排灰操作
        std::thread t1([](){
            printf("manual take start ccw\n");                      // 打印调试信息，表示手动排灰操作开始。
            int s2 = get_var_sample_motor_ccw_seconds();            // 获取手动排灰时间（逆时针旋转时间）。
            // 设置主标签文本为 "电机转动中(反转)"，表示电机正在逆时针旋转。
            snprintf(main_label_text, sizeof(main_label_text), "电机转动中(反转)");
            //update_label_cb 定义的回调函数，接收 main_label_text 内容并设置给 objects.main_label
            lv_async_call(update_label_cb, objects.main_label);     // 异步更新主标签文本显示。
            ACES::GPIO::GetInstance().SetMotorDirection(ACES::MotorDirection::CCW);// 设置电机方向为逆时针旋转。
            ACES::GPIO::GetInstance().SetMotorPower(ACES::MotoPowerState::ON);     // 设置电机供电状态为 ON，开始工作。
            int secondCount = 0;    // 初始化计时器变量 secondCount，用于记录电机运行时间。
            while((secondCount / 10) < s2){
                if (bQuitCCWThread.load()){
                    printf("smanual take  exited\n");
                    action_btn_motor_stop(nullptr); // 停止取灰电机
                    return;     // Quit Thread
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                secondCount++;
            }
            action_btn_motor_stop(nullptr); // 停止取灰电机
        });
        t1.detach();
    } else
    {
        // lv_obj_set_style_bg_color(objects.btn_manual_take_ccw, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN | LV_STATE_DEFAULT);
        bQuitCCWThread = true;
    }
}


extern "C" int32_t get_var_sample_interval_minutes();
extern "C" void set_var_sample_interval_minutes(int32_t value);

// 自动取灰
void action_btn_auto_take_cw(){
    printf("sample thread started\n");
    int secondCount = 0;
    while(!bQuitAutoTakeThread){
        if (bMotorInUse.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        int cwSeconds = get_var_sample_motor_cw_seconds(); // 获取正转时间
        bMotorInUse.store(true); // 设置电机使用状态为 true，表示电机正在被使用。
        memset(main_label_text, 0, sizeof(main_label_text));
        strcpy(main_label_text, "电机转动中(正转)");
        lv_async_call(update_label_cb, objects.main_label);
        // lv_label_set_text(objects.main_label, "电机转动中(正转)");
        ACES::GPIO::GetInstance().SetMotorDirection(ACES::MotorDirection::CW);
        ACES::GPIO::GetInstance().SetMotorPower(ACES::MotoPowerState::ON);
        secondCount = 0;
        while((secondCount / 10) < cwSeconds){
            if (bQuitAutoTakeThread){
                ACES::GPIO::GetInstance().SetMotorPower(ACES::MotoPowerState::OFF);
                bMotorInUse =false;
                memset(main_label_text, 0, sizeof(main_label_text));
                strcpy(main_label_text, "电机停止状态");
                lv_async_call(update_label_cb, objects.main_label);
                printf("sample thread exited\n");
                return;     // Quit Thread
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            secondCount++;
        }
        // Stop Motor
        ACES::GPIO::GetInstance().SetMotorPower(ACES::MotoPowerState::OFF);
        snprintf(main_label_text, sizeof(main_label_text), "电机冷却状态,禁止操作");
        lv_async_call(update_label_cb, objects.main_label);
        // lv_label_set_text(objects.main_label, "电机冷却状态,禁止操作");
        secondCount = 0;
        int stopTime = get_var_sample_motor_stop_seconds();
        while((secondCount / 10) < stopTime){
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            secondCount++;
        }
        // Push
        int ccwSeconds = get_var_sample_motor_ccw_seconds();
        snprintf(main_label_text, sizeof(main_label_text), "电机转动中(反转)");
        lv_async_call(update_label_cb, objects.main_label);
        // lv_label_set_text(objects.main_label, "电机转动中(反转)");
        ACES::GPIO::GetInstance().SetMotorDirection(ACES::MotorDirection::CCW);
        ACES::GPIO::GetInstance().SetMotorPower(ACES::MotoPowerState::ON);
        secondCount = 0;
        while((secondCount / 10) < ccwSeconds){
            if (bQuitAutoTakeThread){
                ACES::GPIO::GetInstance().SetMotorPower(ACES::MotoPowerState::OFF);
                printf("sample thread exited\n");
                snprintf(main_label_text, sizeof(main_label_text), "电机停止状态");
                lv_async_call(update_label_cb, objects.main_label);
                bMotorInUse = false;
                return;     // Quit Thread
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            secondCount++;
        }
        // Stop Motor
        ACES::GPIO::GetInstance().SetMotorPower(ACES::MotoPowerState::OFF);
        snprintf(main_label_text, sizeof(main_label_text), "电机停止状态");
        lv_async_call(update_label_cb, objects.main_label);
        bMotorInUse = false;
        int sampleIntervalMinutes = get_var_sample_interval_minutes();
        secondCount = 0;
        while((secondCount / 600) < sampleIntervalMinutes){
            if (bQuitAutoTakeThread){
                printf("sample thread exited\n");
                return;     // Quit Thread
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            secondCount++;
        }
    }
    printf("sample thread exited\n");
}

//用作自动取灰控制按钮
static bool bInAutoTake = false; //静态变量，记录是否处于自动取灰状态，初始为 false（未启动）。
/*
void action_btn_auto_take_cw(lv_event_t * e)
{
    // 如果当前不在自动取灰状态，说明这是第一次按下，要开启功能。
    if (!bInAutoTake){
        //  将按钮背景色改为红色（表示“工作中”状态）。
        lv_obj_set_style_bg_color(objects.btn_auto_take_cw, lv_palette_main(LV_PALETTE_RED), LV_PART_MAIN | LV_STATE_DEFAULT);
        bQuitAutoTakeThread = false;    //全局标志位，表示不退出线程（线程运行中）。
        std::thread t1(thread_auto_take); // 创建并启动新线程 thread_auto_take，并使用 detach() 让其后台运行，不阻塞主线程。
        t1.detach();

    } else  //如果已经在自动取灰状态：
    {
        // 将按钮背景色改为蓝色（表示“停止中”状态）。
        lv_obj_set_style_bg_color(objects.btn_auto_take_cw, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN | LV_STATE_DEFAULT);
        bQuitAutoTakeThread = true; //切换状态变量，以便下次按钮点击时执行相反操作。
    }
    bInAutoTake = !bInAutoTake;

}
*/
// 保存采样设置按钮的回调函数
extern void action_save_sample_setting(lv_event_t * e){
    // 调用保存设置函数，将当前采样设置保存到配置文件中。
    action_save_setting_func(e);
    // 显示保存成功的提示信息
    show_motor_status_with_color(objects.main_label, "采样设置已保存", lv_palette_main(LV_PALETTE_GREEN));
    // lv_label_set_text(objects.main_label, "采样设置已保存");
    // 延时 2 秒后清除提示信息
    std::this_thread::sleep_for(std::chrono::seconds(2));
    // 清除主界面标签文本
    memset(main_label_text, 0, sizeof(main_label_text));
    strcpy(main_label_text, "电机停止状态");
    lv_async_call(update_label_cb, objects.main_label);
    // lv_label_set_text(objects.main_label, "电机停止状态");
    // 重新加载配置文件
    load_configs(); // 重新加载配置文件，以确保界面显示最新的设置。
    // 更新界面上的采样设置显示
    //lv_label_set_text(objects.label_sample_interval, std::to_string(get_var_sample_interval_minutes()).c_str());
    //lv_label_set_text(objects.label_motor_cw_seconds, std::to_string(get_var_motor_cw_seconds()).c_str());  
}

static void sample_motor_stop_func()
{
    // 更新电机供电状态为 OFF，停止电机工作。
    ACES::GPIO::GetInstance().SetMotorPower(ACES::MotoPowerState::OFF); 
    // 更新主标签文本为 "电机停止状态"
    snprintf(main_label_text, sizeof(main_label_text), "电机停止状态");  
    // 异步更新主标签文本显示。
    lv_async_call(update_label_cb, objects.main_label); 
    // 设置电机使用状态为 false，表示电机不再被使用。
    SET_MOTOR_IN_USE(false);    
    SetBtnBgColor(objects.btn_manual_take_cw, lv_color_hex(0x009ea2)); //设置手动取灰按钮背景颜色为墨绿色。
    SetBtnBgColor(objects.btn_manual_take_ccw, lv_color_hex(0x009ea2));//设置手动排灰按钮背景颜色为墨绿色。
    // bInTakeCCW.store(false); // 设置手动排灰状态为 false，表示不再进行手动排灰操作。
    bInTakeCW.store(false);  // 设置手动取灰状态为 false, 表示不再进行手动取灰操作。
    return ;    
}
// 采样电机停止按钮的回调函数
extern void action_btn_motor_stop(lv_event_t * e){
    // 更新电机供电状态为 OFF，停止电机工作。
    ACES::GPIO::GetInstance().SetMotorPower(ACES::MotoPowerState::OFF); 
    // 更新主标签文本为 "电机停止状态"
    snprintf(main_label_text, sizeof(main_label_text), "电机停止状态");  
    // 异步更新主标签文本显示。
    lv_async_call(update_label_cb, objects.main_label); 
    // 设置电机使用状态为 false，表示电机不再被使用。
    SET_MOTOR_IN_USE(false);    
    SetBtnBgColor(objects.btn_manual_take_cw, lv_color_hex(0x009ea2)); //设置手动取灰按钮背景颜色为墨绿色。
    SetBtnBgColor(objects.btn_manual_take_ccw, lv_color_hex(0x009ea2));//设置手动排灰按钮背景颜色为墨绿色。
    
    // bInTakeCCW.store(false); // 设置手动排灰状态为 false，表示不再进行手动排灰操作。
    bInTakeCW.store(false);  // 设置手动取灰状态为 false, 表示不再进行手动取灰操作。
    return ;
}

// 采样电机正转取灰的回调函数
extern void action_btn_sample_motor_cw(lv_event_t * e){
    return ;
}
// 旋转电机正转的回调函数
extern void action_btn_rotate_motor_cw(lv_event_t * e){
    return ;
}
// 旋转电机反转的回调函数
extern void action_btn_rotate_motor_ccw(lv_event_t * e){
    return ;
}
// 采样电机反转排灰的回调函数
extern void action_btn_sample_motor_ccw(lv_event_t * e){
    return ;
}
// 自动取灰按钮的回调函数
extern void action_btn_auto_take_cw(lv_event_t * e){
    return ;
}
