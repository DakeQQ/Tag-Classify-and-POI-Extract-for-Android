
#include <jni.h>
#include "onnxruntime_cxx_api.h"
#include "nnapi_provider_factory.h"
#include <android/asset_manager_jni.h>
#include <iostream>
#include <fstream>

const OrtApi* ort_runtime_A;
OrtSession* session_model_A;
std::vector<const char*> input_names_A;
std::vector<const char*> output_names_A;
std::vector<std::vector<std::int64_t>> input_dims_A;
std::vector<std::vector<std::int64_t>> output_dims_A;
std::vector<ONNXTensorElementDataType> input_types_A;
std::vector<ONNXTensorElementDataType> output_types_A;
std::vector<OrtValue*> input_tensors_A;
std::vector<OrtValue*> output_tensors_A;
const std::string file_name_A = "Model_REX.ort";
const std::string storage_path = "/storage/emulated/0/Android/data/com.example.myapplication/files/";
const char* qnn_htp_so = "/data/user/0/com.example.myapplication/cache/libQnnHtp.so";  //  If use (std::string + "libQnnHtp.so").c_str() instead, it will open failed.
const char* qnn_cpu_so = "/data/user/0/com.example.myapplication/cache/libQnnCpu.so";  //  If use (std::string + "libQnnCpu.so").c_str() instead, it will open failed.
const int amount_of_tags = 10;  // Just pre-allocate, amount_of_tags >= the actual amount of tags.
const int tag_position_offset = 2;
const int amount_of_classify_task = 2;
const int amount_of_total_task = 6;
const int max_token_limit_REX = 40;
const int REX_attention_masks_size = max_token_limit_REX * max_token_limit_REX;
const int REX_buffer_size_0_2 = max_token_limit_REX * sizeof(int32_t);
const int REX_buffer_size_1 = REX_attention_masks_size * sizeof(int32_t);
// You can follow the model website to add tag_idx to perform personal tasks. https://modelscope.cn/models/iic/nlp_deberta_rex-uninlu_chinese-base/summary
const int tag_0[] = {1, 12800, 12801, 1902, 5766, 6469, 747, 12801, 6615, 2911, 805, 5650, 12801, 7043, 6818, 2426, 697, 12801, 7697, 287, 3487, 3081, 2, 12802};
// Tags_0 for classify: Vehicle_Assistant; Travel & Navi; Media;
const int tag_1[] = {1, 12800, 12801, 658, 2328, 2426, 5374, 12801, 2889, 315, 5099, 6578, 12801, 4795, 6557, 4657, 4992, 12801, 6893, 6921, 805, 5650, 2, 12802};
// Tags_1 for classify: Vehicle_Control; Encyclopedia; Confidant; Raconteur;
const int tag_2[] = {1, 12800, 12801, 4708, 4660, 1410, 2};
// Tags_2 for extract navigation place.
const int tag_3[] = {1, 12800, 12801,7697,287,1005,5006,2};
// Tags_3 for extract song name.
const int tag_4[] = {1, 12800, 12801, 2909,2};
// Tags_4 for extract numbers.
const int tag_5[] = {1, 12800, 12801, 6482,5794, 2};
// Tags_5 for extract singer name.
const int position_ids_0[] = {0, 1, 2, 3, 4, 5, 6, 2, 3, 4, 5, 6, 2, 3, 4, 5, 6, 2, 3, 4, 5, 6, 7, 8};
const int position_ids_1[] = {0, 1, 2, 3, 4, 5, 6, 2, 3, 4, 5, 6, 2, 3, 4, 5, 6, 2, 3, 4, 5, 6, 7, 8};
const int position_ids_2[] = {0, 1, 2, 3, 4, 5, 6};
const int position_ids_3[] = {0, 1, 2, 3, 4, 5,6,7};
const int position_ids_4[] = {0, 1, 2, 3, 4};
const int position_ids_5[] = {0, 1, 2, 3, 4, 5};
std::vector<int> save_tag_size(amount_of_total_task, 0);
std::vector<std::vector<std::vector<int>>> attention_masks_for_classify(amount_of_classify_task,std::vector<std::vector<int>> (sizeof(tag_0) / sizeof(tag_0[0]),std::vector<int> (sizeof(tag_0) / sizeof(tag_0[0]),0)));
std::vector<std::vector<int>> all_tags_ids(amount_of_total_task,std::vector<int> (max_token_limit_REX,0));
std::vector<std::vector<int>> all_position_ids(amount_of_total_task,std::vector<int> (max_token_limit_REX,0));







