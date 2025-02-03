#pragma once
#include "../../base.hpp"


namespace tools::virtual_machine {
    // 设置虚拟机的地址位数为32位
    using bit_size = u32;
    enum class data_type : u16 {
        i8,     // 8bit 有符号整数
        u8,     // 8bit 无符号整数
        i16,    // 16bit 有符号整数
        u16,    // 16bit 无符号整数
        i32,    // 32bit 有符号整数
        u32,    // 32bit 无符号整数
        i64,    // 64bit 有符号整数
        u64,    // 64bit 无符号整数

        f32,    // 32bit 浮点数
        f64,    // 64bit 浮点数
    };

    enum class ASM : u16 {
        // 算术运算 o=输出地址 i=输入地址 t=data_type
        add, // 加        add t1 o1 i1 i2 
        sub, // 减        sub t1 o1 i1 i2
        mul, // 乘        mul t1 o1 i1 i2
        div, // 除        div t1 o1 i1 i2
        mod, // 取余      mod t1 o1 i1 i2
        inc, // 自增      inc t1 o1 i1
        dec, // 自减      dec t1 o1 i1

        // 逻辑运算 o=输出地址 i=输入地址 t=data_type
        and_, // 与       and t1 o1 i1 i2
        or_,  // 或       or  t1 o1 i1 i2
        xor_, // 异或     xor t1 o1 i1 i2
        not_, // 非       not t1 o1 i1

        // 比较运算 o=输出地址 i=输入地址 t=data_type
        eq,  // 等于      eq  t1 o1 i1 i2
        neq, // 不等于    neq t1 o1 i1 i2
        lt,  // 小于      lt  t1 o1 i1 i2
        gt,  // 大于      gt  t1 o1 i1 i2
        lte, // 小于等于  lte t1 o1 i1 i2
        gte, // 大于等于  gte t1 o1 i1 i2

        // 内存操作 o=输出地址 i=输入地址
        push,  // 压栈    push t1 i1
        pop,   // 弹栈    pop  t1 o1

        alloc, // 申请内存  alloc   t1 o1 i1
        free,  // 释放内存  free    t1 o1 i1

        set_value,// 设置值 set_value t1 o1:设置地址 i1:设置值 i2:设置数量

        // 控制流 o=跳转地址 i=条件地址 t=data_type
        jmp,  // 跳转     jmp  t1 o1   
        jz,   // 为零跳转 jz   t1 o1 i1
        jnz,  // 非零跳转 jnz  t1 o1 i1
        call, // 调用     call t1 o1
        ret,  // 返回     ret

        // 系统调用

        // 其他
        nop,  // 空操作   nop
        halt, // 停机     halt
    };

    // 内存
    class memory {
    public:
        struct info {
            bit_size memory_size;   // 内存总大小
            bit_size program_entry; // 程序入口地址
            bit_size text_down;     // 代码区起始地址
            bit_size text_now_up;   // 代码区当前地址
            bit_size text_up;       // 代码区结束地址
            bit_size stack_down;    // 栈区起始地址
            bit_size stack_now_up;  // 栈区当前地址
            bit_size stack_up;      // 栈区结束地址
            bit_size heap_down;     // 堆区起始地址
            bit_size heap_up;       // 堆区结束地址
            u8 log_level;           // 日志等级
            u8 security_level;      // 安全等级
            // 寄存器组
            u64 registers[24];      // 寄存器
            // 当前指令
            ASM now_asm;            // 当前指令
            data_type now_data_type;// 当前数据类型
            bit_size now_out;       // 当前输出地址
            bit_size now_in1;       // 当前输入地址1
            bit_size now_in2;       // 当前输入地址2
        };
        // 内存分区(0->∞) [信息区|代码区|数据区|栈区|堆区>>>>>>]
        // 原始程序数据，不可修改，不存储栈和堆。
        u8* memory_static;
        // 运行时内存，可修改，存储栈和堆。
        u8* memory_;
    public:
        bool next_command() {
            // 获取内存信息
            auto ram = memory_;
            auto& info = *reinterpret_cast<memory::info*>(ram);
            // 当前指令
            info.now_asm = *reinterpret_cast<ASM*>(ram + info.text_now_up);
            info.now_data_type = *reinterpret_cast<data_type*>(ram + info.text_now_up + sizeof(ASM));
            info.now_out = *reinterpret_cast<bit_size*>(ram + info.text_now_up + sizeof(ASM) + sizeof(data_type));
            info.now_in1 = *reinterpret_cast<bit_size*>(ram + info.text_now_up + sizeof(ASM) + sizeof(data_type) + sizeof(bit_size));
            info.now_in2 = *reinterpret_cast<bit_size*>(ram + info.text_now_up + sizeof(ASM) + sizeof(data_type) + sizeof(bit_size) + sizeof(bit_size));
            // 下一条指令
            info.text_now_up += sizeof(ASM) + sizeof(data_type) + sizeof(bit_size) * 3;
            return true;
        }
    };


    // 解释器
    // -9'223'372'036'854'775'808 代表为执行完毕，遇到最大运行次数限制暂停运行。
    //i64 Interpreter(memory& memory_, u64 run_command_size = 1ull << 60) {
    //    auto ram = memory_.memory_;
    //    auto& info = *reinterpret_cast<memory::info*>(ram);
    //    // 循环执行程序
    //    while (run_command_size--) {
    //        // 解码下一条命令
    //        memory_.next_command();
    //        // 执行命令
    //    }
    //    return -9'223'372'036'854'775'807 - 1;
    //}
}