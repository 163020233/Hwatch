/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Create Date : 2025-08
 */

#ifndef DATA_LOADER_PPG_H
#define DATA_LOADER_PPG_H

// --- 公共数据结构定义 ---
// 其他模块需要知道 CsvData 是什么样子，才能使用它
typedef struct {
    float **data;
    int rows;
    int cols;
} CsvData;

// --- 公共函数声明 (函数原型) ---
// 这里只声明函数的名称、参数和返回值，不包含函数体
// 这是向外界承诺：“我提供了下面这两个函数”

/**
 * @brief 从CSV文件加载数据
 * @param filepath CSV文件的路径
 * @return CsvData 结构体。如果失败，data成员为NULL。
 */
CsvData load_csv(const char *filepath, int num_cols);

/**
 * @brief 释放由load_csv分配的内存
 * @param csv_data 指向CsvData结构体的指针
 */
void free_csv_data(CsvData *csv_data);

#endif  // DATA_LOADER_H