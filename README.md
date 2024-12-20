---

# Classify-and-Extract for Android

## Overview

This project demonstrates tag classification and Point of Interest (POI) extraction on Android devices, using optimized models for high-speed performance.

## Getting Started

1. **Download the Models:**
   - The demo models are available for download [here](https://drive.google.com/drive/folders/1ZWwhSwnEM2DtlLKVMdEB3OxKKPjWUn4T?usp=drive_link).

2. **Setup:**
   - After downloading, place the models into the `assets` folder.
   - Decompress the `*.so` zip file stored in the `libs/arm64-v8a` folder.

3. **Model Information:**
   - The demo model, named **RexUniNLU**, is converted from ModelScope and has undergone code optimizations for extreme execution speed.
   - The inputs and outputs of this demo model differ slightly from the original version.

4. **ONNX Runtime Adaptation:**
   - To better adapt to ONNX Runtime on Android, dynamic axes were not used during export. As a result, the exported ONNX model may not be optimal for x86_64 systems.
   - We plan to make the export method public in the future.

5. **Performance:**
   - A single inference takes about 42ms, and typically 3 rounds are required to complete classification and extraction tasks.

6. **Limitations:**
   - The REX model currently supports Chinese text exclusively.

## Additional Resources

- Explore more projects: [https://dakeqq.github.io/overview/](https://dakeqq.github.io/overview/)

## 演示结果 Demo Results

![Demo Animation](https://github.com/DakeQQ/Classify-and-Extract-for-Android/blob/main/extract.gif?raw=true)

---

# 标签分类和POI提取-安卓

## 概述

该项目在Android设备上展示了标签分类和兴趣点（POI）提取，使用经过优化的模型以实现高速度性能。

## 快速开始

1. **下载模型：**
   - 演示模型已上传至云端硬盘：[点击这里下载](https://drive.google.com/drive/folders/1ZWwhSwnEM2DtlLKVMdEB3OxKKPjWUn4T?usp=drive_link)
   - 百度链接: [点击这里](https://pan.baidu.com/s/18sZ97WeYTpljLjcsO9YvFg?pwd=dake) 提取码: dake

2. **设置：**
   - 下载后，请将模型文件放入`assets`文件夹。
   - 解压存放在`libs/arm64-v8a`文件夹中的`*.so`压缩文件。

3. **模型信息：**
   - 演示模型名为**RexUniNLU**，它是从ModelScope转换来的，并经过代码优化，以实现极致执行速度。
   - 因此，演示模型的输入输出与原始模型略有不同。

4. **ONNX Runtime 适配：**
   - 为了更好地适配ONNX Runtime-Android，导出时未使用动态轴。因此，导出的ONNX模型对x86_64可能不是最佳选择。
   - 我们计划在未来公开转换导出的方法。

5. **性能：**
   - 一次推理大约需要42毫秒。通常需要3轮才能完成分类和提取任务。

6. **限制：**
   - REX模型现在仅支持中文文本。

## 其他资源

- 看更多项目: [https://dakeqq.github.io/overview/](https://dakeqq.github.io/overview/)

---
