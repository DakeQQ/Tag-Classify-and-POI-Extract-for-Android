# Classify-and-Extract-for-Android
1. Demonstration of tag classification and POI extraction on Android device.
2. The demo models were uploaded to the drive: https://drive.google.com/drive/folders/1ZWwhSwnEM2DtlLKVMdEB3OxKKPjWUn4T?usp=drive_link
3. After downloading, place the model into the assets folder.
4. Remember to decompress the *.so zip file stored in the libs/arm64-v8a folder.
5. The demo models, named 'RexUniNLU', were converted from ModelScope and underwent code optimizations to achieve extreme execution speed.
6. Therefore, the inputs & outputs of the demo models are slightly different from the original one.
7. A single inference takes about 42ms. It usually requires 3 rounds to complete classification and extraction tasks.
8. The REX model now supports Chinese text exclusively.
9. We will make the exported method public later.
10. See more projects: https://dakeqq.github.io/overview/
# 标签分类和POI提取-安卓
1. 在Android设备上进行标签分类和POI提取的演示。
2. 演示模型已上传至云端硬盘：https://drive.google.com/drive/folders/1ZWwhSwnEM2DtlLKVMdEB3OxKKPjWUn4T?usp=drive_link
3. 百度: https://pan.baidu.com/s/18sZ97WeYTpljLjcsO9YvFg?pwd=dake 提取码: dake
4. 下载后，请将模型文件放入assets文件夹。
5. 记得解压存放在libs/arm64-v8a文件夹中的*.so压缩文件。
6. 演示模型名为'RexUniNLU'，它们是从ModelScope转换来的，并经过代码优化，以实现极致执行速度。
7. 因此，演示模型的输入输出与原始模型略有不同。
8. 一次推理大约需要42ms。通常需要3轮才能完成分类和提取任务。
9. REX模型现在仅支持中文文本。
10. 我们未来会提供转换导出的方法。
11. 看更多項目: https://dakeqq.github.io/overview/
# 演示结果 Demo Results

![Demo Animation](https://github.com/DakeQQ/Classify-and-Extract-for-Android/blob/main/extract.gif?raw=true)
