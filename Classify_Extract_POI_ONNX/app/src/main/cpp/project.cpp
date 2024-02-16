#include "project.h"

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_example_myapplication_MainActivity_Load_1Models_10(JNIEnv *env, jclass clazz,
                                                            jobject asset_manager,
                                                            jboolean use_gpu,
                                                            jboolean use_fp16,
                                                            jboolean use_nnapi,
                                                            jboolean use_xnnpack,
                                                            jboolean use_qnn,
                                                            jboolean use_dsp_npu) {
    OrtStatus *status;
    OrtAllocator *allocator;
    OrtEnv *ort_env_A;
    OrtSessionOptions *session_options_0;
    {
        std::vector<char> fileBuffer;
        size_t fileSize;
        if (asset_manager != nullptr) {
            AAssetManager* mgr = AAssetManager_fromJava(env,asset_manager);
            AAsset* asset = AAssetManager_open(mgr,file_name_A.c_str(),AASSET_MODE_BUFFER);
            fileSize = AAsset_getLength(asset);
            fileBuffer.resize(fileSize);
            AAsset_read(asset,fileBuffer.data(),fileSize);
        } else {
            std::ifstream model_file(storage_path + file_name_A,std::ios::binary | std::ios::ate);
            if (!model_file.is_open()) {
                return JNI_FALSE;
            }
            fileSize = model_file.tellg();
            model_file.seekg(0,std::ios::beg);
            fileBuffer.resize(fileSize);
            if (!model_file.read(fileBuffer.data(), fileSize)) {
                return JNI_FALSE;
            }
            model_file.close();
        }
        ort_runtime_A = OrtGetApiBase()->GetApi(ORT_API_VERSION);
        ort_runtime_A->CreateEnv(ORT_LOGGING_LEVEL_ERROR, "myapplication", &ort_env_A);
        ort_runtime_A->CreateSessionOptions(&session_options_0);
        ort_runtime_A->DisableProfiling(session_options_0);
        ort_runtime_A->EnableCpuMemArena(session_options_0);
        ort_runtime_A->EnableMemPattern(session_options_0);
        ort_runtime_A->SetSessionExecutionMode(session_options_0, ORT_SEQUENTIAL);
        ort_runtime_A->SetInterOpNumThreads(session_options_0, 2);
        ort_runtime_A->AddSessionConfigEntry(session_options_0, "session.dynamic_block_base", "2");  // One block can contain 1 or more cores, and sharing 1 job.
        ort_runtime_A->AddSessionConfigEntry(session_options_0,  // Binding the #cpu to run the model. 'A;B' means A & B work respectively. 'A,B' means A & B work cooperatively.
                                             "session.intra_op_thread_affinities",
                                             "1;2");  // // It recommends to set at least two small-core for RexUniNLU model with max_token_limit_REX=40. If you export a larger one by yourself, it is better using the big-cores.
        ort_runtime_A->SetIntraOpNumThreads(session_options_0, 3); // dynamic_block_base + 1
        ort_runtime_A->AddSessionConfigEntry(session_options_0, "session.inter_op.allow_spinning",
                                             "1");  // 0 for low power
        ort_runtime_A->AddSessionConfigEntry(session_options_0, "session.intra_op.allow_spinning",
                                             "1");  // 0 for low power
        ort_runtime_A->AddSessionConfigEntry(session_options_0, "session.force_spinning_stop",
                                             "0");  // 1 for low power
        ort_runtime_A->SetSessionGraphOptimizationLevel(session_options_0, ORT_ENABLE_ALL);
        ort_runtime_A->AddSessionConfigEntry(session_options_0,
                                             "optimization.minimal_build_optimizations",
                                             "");   // Keep empty for full optimization
        ort_runtime_A->AddSessionConfigEntry(session_options_0, "session.disable_prepacking",
                                             "0");  // 0 for enable
        ort_runtime_A->AddSessionConfigEntry(session_options_0,
                                             "optimization.enable_gelu_approximation",
                                             "0");  // Set 0 is better for this model
        ort_runtime_A->AddSessionConfigEntry(session_options_0,
                                             "mlas.enable_gemm_fastmath_arm64_bfloat16",
                                             "1");  //
        ort_runtime_A->AddSessionConfigEntry(session_options_0,
                                             "session.disable_aot_function_inlining",
                                             "0");  // 0 for speed
        ort_runtime_A->AddSessionConfigEntry(session_options_0, "session.qdqisint8allowed",
                                             "1");  // 1 for Arm
        ort_runtime_A->AddSessionConfigEntry(session_options_0, "session.enable_quant_qdq_cleanup",
                                             "1");  // 0 for precision, 1 for performance
        ort_runtime_A->AddSessionConfigEntry(session_options_0,
                                             "session.disable_double_qdq_remover",
                                             "0");  // 1 for precision, 0 for performance
        ort_runtime_A->AddSessionConfigEntry(session_options_0, "session.disable_quant_qdq",
                                             "0");  // 0 for use Int8
        ort_runtime_A->AddSessionConfigEntry(session_options_0,
                                             "session.use_ort_model_bytes_directly",
                                             "1");  // Use this option to lower the peak memory during loading.
        ort_runtime_A->AddSessionConfigEntry(session_options_0,
                                             "session.use_ort_model_bytes_for_initializers",
                                             "0");  // If set use_ort_model_bytes_directly=1, use_ort_model_bytes_for_initializers should be 0.
        ort_runtime_A->AddSessionConfigEntry(session_options_0, "session.set_denormal_as_zero",
                                             "0");  // // Use 0 instead of NaN or Inf.
        ort_runtime_A->AddSessionConfigEntry(session_options_0, "session.use_env_allocators",
                                             "1");  // Use it to lower memory usage.
        ort_runtime_A->AddSessionConfigEntry(session_options_0,
                                             "session.use_device_allocator_for_initializers",
                                             "1");  // Use it to lower memory usage.
        std::vector<const char*> option_keys = {};
        std::vector<const char*> option_values = {};
        if (use_qnn) {  // It needs the permission of HTP hardware, and then follow the onnx document to generate the specific format to run on HTP.
            if (use_dsp_npu) {
                option_keys.push_back("backend_path");
                option_values.push_back(qnn_htp_so);
                option_keys.push_back("htp_performance_mode");
                option_values.push_back("burst");
                option_keys.push_back("htp_graph_finalization_optimization_mode");
                option_values.push_back("3");
                option_keys.push_back("soc_model");
                option_values.push_back("0");  // 0 for unknown
                option_keys.push_back("htp_arch");
                option_values.push_back("73");  // 0 for unknown
                option_keys.push_back("device_id");
                option_values.push_back("0");  // 0 for single device
                option_keys.push_back("vtcm_mb");
                option_values.push_back("8");  // 0 for auto
                option_keys.push_back("qnn_context_priority");
                option_values.push_back("high");
                ort_runtime_A->AddSessionConfigEntry(session_options_0,
                                                     "ep.context_enable", "1");
                ort_runtime_A->AddSessionConfigEntry(session_options_0,
                                                     "ep.context_embed_mode", "1");
                ort_runtime_A->AddSessionConfigEntry(session_options_0,
                                                     "ep.context_file_path", storage_path.c_str());  // Default to original_file_name_ctx.onnx if not specified
                ort_runtime_A->AddSessionConfigEntry(session_options_0,
                                                     "session.use_ort_model_bytes_directly",
                                                     "0");  // Cancel this option.
            } else {
                option_keys.push_back("backend_path");
                option_values.push_back(qnn_cpu_so);
            }
            ort_runtime_A->SessionOptionsAppendExecutionProvider(session_options_0, "QNN", option_keys.data(), option_values.data(), option_keys.size());
        } else if (use_nnapi) {  // It needs to add the app into /vendor/etc/nnapi_extensions_app_allowlist
            uint32_t npflags = 0;
            if (use_gpu | use_dsp_npu) {
                npflags |= NNAPI_FLAG_CPU_DISABLED;
            } else {
                npflags |= NNAPI_FLAG_CPU_ONLY;
            }
            if (use_fp16) {
                npflags |= NNAPI_FLAG_USE_FP16;
            }
            OrtSessionOptionsAppendExecutionProvider_Nnapi(session_options_0, npflags);
        } else if (use_xnnpack) {
            option_keys.push_back("intra_op_num_threads");
            option_values.push_back("4");
            ort_runtime_A->SessionOptionsAppendExecutionProvider(session_options_0, "XNNPACK", option_keys.data(), option_values.data(), option_keys.size());
        }
        status = ort_runtime_A->CreateSessionFromArray(ort_env_A, fileBuffer.data(), fileSize,
                                                       session_options_0, &session_model_A);
    }
    if (status != nullptr) {
        return JNI_FALSE;
    }
    std::size_t amount_of_input;
    ort_runtime_A->GetAllocatorWithDefaultOptions(&allocator);
    ort_runtime_A->SessionGetInputCount(session_model_A, &amount_of_input);
    input_names_A.resize(amount_of_input);
    input_dims_A.resize(amount_of_input);
    input_types_A.resize(amount_of_input);
    input_tensors_A.resize(amount_of_input);
    for (size_t i = 0; i < amount_of_input; i++) {
        char* name;
        OrtTypeInfo* typeinfo;
        size_t dimensions;
        size_t tensor_size;
        const OrtTensorTypeAndShapeInfo* tensor_info;
        ONNXTensorElementDataType type;
        ort_runtime_A->SessionGetInputName(session_model_A, i, allocator, &name);
        input_names_A[i] = name;
        ort_runtime_A->SessionGetInputTypeInfo(session_model_A, i, &typeinfo);
        ort_runtime_A->CastTypeInfoToTensorInfo(typeinfo, &tensor_info);
        ort_runtime_A->GetTensorElementType(tensor_info, &type);
        input_types_A[i] = type;
        ort_runtime_A->GetDimensionsCount(tensor_info, &dimensions);
        input_dims_A[i].resize(dimensions);
        ort_runtime_A->GetDimensions(tensor_info, input_dims_A[i].data(), dimensions);
        ort_runtime_A->GetTensorShapeElementCount(tensor_info, &tensor_size);
        if (typeinfo) ort_runtime_A->ReleaseTypeInfo(typeinfo);
    }
    std::size_t amount_of_output;
    ort_runtime_A->SessionGetOutputCount(session_model_A, &amount_of_output);
    output_names_A.resize(amount_of_output);
    output_dims_A.resize(amount_of_output);
    output_types_A.resize(amount_of_output);
    output_tensors_A.resize(amount_of_output);
    for (size_t i = 0; i < amount_of_output; i++) {
        char* name;
        OrtTypeInfo* typeinfo;
        size_t dimensions;
        size_t tensor_size;
        const OrtTensorTypeAndShapeInfo* tensor_info;
        ONNXTensorElementDataType type;
        ort_runtime_A->SessionGetOutputName(session_model_A, i, allocator, &name);
        output_names_A[i] = name;
        ort_runtime_A->SessionGetOutputTypeInfo(session_model_A, i, &typeinfo);
        ort_runtime_A->CastTypeInfoToTensorInfo(typeinfo, &tensor_info);
        ort_runtime_A->GetTensorElementType(tensor_info, &type);
        output_types_A[i] = type;
        ort_runtime_A->GetDimensionsCount(tensor_info, &dimensions);
        output_dims_A[i].resize(dimensions);
        ort_runtime_A->GetDimensions(tensor_info, output_dims_A[i].data(), dimensions);
        ort_runtime_A->GetTensorShapeElementCount(tensor_info, &tensor_size);
        if (typeinfo) ort_runtime_A->ReleaseTypeInfo(typeinfo);
    }
    return JNI_TRUE;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_example_myapplication_MainActivity_Pre_1Process(JNIEnv *env, jclass clazz) {
    for (int i = 0; i < amount_of_total_task; i++) {
        switch (i) {
            case 0:
                save_tag_size[i] = sizeof(tag_0) / sizeof(tag_0[0]);
                std::move(tag_0,tag_0 + save_tag_size[i],all_tags_ids[i].begin());
                std::move(position_ids_0,position_ids_0 + save_tag_size[i],all_position_ids[i].begin());
                break;
            case 1:
                save_tag_size[i] = sizeof(tag_1) / sizeof(tag_1[0]);
                std::move(tag_1,tag_1 + save_tag_size[i],all_tags_ids[i].begin());
                std::move(position_ids_1,position_ids_1 + save_tag_size[i],all_position_ids[i].begin());
                break;
            case 2:
                save_tag_size[i] = sizeof(tag_2) / sizeof(tag_2[0]);
                std::move(tag_2,tag_2 + save_tag_size[i],all_tags_ids[i].begin());
                std::move(position_ids_2,position_ids_2 + save_tag_size[i],all_position_ids[i].begin());
                break;
            case 3:
                save_tag_size[i] = sizeof(tag_3) / sizeof(tag_3[0]);
                std::move(tag_3,tag_3 + save_tag_size[i],all_tags_ids[i].begin());
                std::move(position_ids_3,position_ids_3 + save_tag_size[i],all_position_ids[i].begin());
                break;
            case 4:
                save_tag_size[i] = sizeof(tag_4) / sizeof(tag_4[0]);
                std::move(tag_4,tag_4 + save_tag_size[i],all_tags_ids[i].begin());
                std::move(position_ids_4,position_ids_4 + save_tag_size[i],all_position_ids[i].begin());
                break;
            case 5:
                save_tag_size[i] = sizeof(tag_5) / sizeof(tag_5[0]);
                std::move(tag_5,tag_5 + save_tag_size[i],all_tags_ids[i].begin());
                std::move(position_ids_5,position_ids_5 + save_tag_size[i],all_position_ids[i].begin());
                break;
            default:
                break;
        }
    }
    for (int k = 0; k < amount_of_classify_task; k++) {
        std::vector<int> tag_words(amount_of_tags,0);
        std::vector<int> sum_tag_words(amount_of_tags,0);
        int count = 1;
        for (int i = tag_position_offset + 1; i < save_tag_size[k]; i++) {
            if ((all_tags_ids[k][i] == tag_position_offset) | (all_position_ids[k][i] == tag_position_offset)) {
                tag_words[count] = all_position_ids[k][i - 1] - tag_position_offset;
                int sum = 0;
                for (int j = 0; j < count + 1; j++) {
                    sum += tag_words[j];
                }
                sum_tag_words[count] = sum + count + tag_position_offset;
                count++;
            }
        }
        sum_tag_words[0] += tag_position_offset;
        for (int i = 0; i < tag_position_offset; i++) {
            std::fill(attention_masks_for_classify[k][i].begin(), attention_masks_for_classify[k][i].begin() + save_tag_size[k], 1);
        }
        for (int i = save_tag_size[k] - tag_position_offset; i < save_tag_size[k]; i++) {
            std::fill(attention_masks_for_classify[k][i].begin(), attention_masks_for_classify[k][i].begin() + save_tag_size[k], 1);
        }
        for (int i = 0; i < save_tag_size[k]; i++) {
            std::fill(attention_masks_for_classify[k][i].begin(), attention_masks_for_classify[k][i].begin() + tag_position_offset, 1);
            std::fill(attention_masks_for_classify[k][i].end() - tag_position_offset, attention_masks_for_classify[k][i].end(), 1);
        }
        for (int i = 0; i < count - 1; i++) {
            for (int j = sum_tag_words[i]; j < sum_tag_words[i + 1]; j++) {
                std::fill(attention_masks_for_classify[k][j].begin() + sum_tag_words[i], attention_masks_for_classify[k][j].begin() + sum_tag_words[i] + tag_words[i + 1] + 1, 1);
            }
        }
    }
    return JNI_TRUE;
}

extern "C"
JNIEXPORT jintArray JNICALL
Java_com_example_myapplication_MainActivity_Run_1Classify_1Extract(JNIEnv *env, jclass clazz,
                                                                   jintArray token_index,
                                                                   jint words_count, jint task_id) {
    std::vector<int> input_ids(max_token_limit_REX, 0);
    {
        jint* input_token = env->GetIntArrayElements(token_index, nullptr);
        std::copy(all_tags_ids[task_id].begin(),all_tags_ids[task_id].begin() + save_tag_size[task_id],input_ids.begin());
        std::move(input_token,input_token + words_count, input_ids.begin() + save_tag_size[task_id]);
        env->ReleaseIntArrayElements(token_index,input_token,0);
    }
    std::vector<int> attention_masks(REX_attention_masks_size,0);
    int index_i = 0;
    {
        int total_tokens = words_count + save_tag_size[task_id];
        if (task_id > 1) {
            std::vector<int> temp_masks(max_token_limit_REX, 0);
            std::fill(temp_masks.begin(), temp_masks.begin() + total_tokens, 1);
            for (int i = 0; i < total_tokens; i++) {
                std::copy(temp_masks.begin(), temp_masks.end(), attention_masks.begin() + index_i);
                index_i += max_token_limit_REX;
            }
        } else {
            for (int i = 0; i < save_tag_size[task_id]; i++) {
                std::copy(attention_masks_for_classify[task_id][i].begin(),
                          attention_masks_for_classify[task_id][i].end(),
                          attention_masks.begin() + index_i);
                std::fill(attention_masks.begin() + index_i + save_tag_size[task_id], attention_masks.begin() + index_i + save_tag_size[task_id] + words_count, 1);
                index_i += max_token_limit_REX;
            }
            for (int i = save_tag_size[task_id]; i < total_tokens; i++) {
                std::fill(attention_masks.begin() + index_i, attention_masks.begin() + index_i + total_tokens, 1);
                index_i += max_token_limit_REX;
            }
        }
    }
    std::vector<int> position_ids(max_token_limit_REX, 0);
    {
        std::copy(all_position_ids[task_id].begin(),all_position_ids[task_id].begin() + save_tag_size[task_id],position_ids.begin());
        index_i = save_tag_size[task_id];
        for (int i = all_position_ids[task_id][save_tag_size[task_id] - 1] + 1;
             i < words_count + all_position_ids[task_id][save_tag_size[task_id] - 1] + 1; i++) {
            position_ids[index_i] = i;
            index_i++;
        }
    }
    void* output_tensors_buffer_0;
    {
        OrtMemoryInfo *memory_info;
        ort_runtime_A->CreateCpuMemoryInfo(OrtArenaAllocator, OrtMemTypeDefault, &memory_info);
        ort_runtime_A->CreateTensorWithDataAsOrtValue(
                memory_info, reinterpret_cast<void*>(input_ids.data()), REX_buffer_size_0_2,
                input_dims_A[0].data(), input_dims_A[0].size(), input_types_A[0], &input_tensors_A[0]);
        ort_runtime_A->CreateTensorWithDataAsOrtValue(
                memory_info, reinterpret_cast<void*>(attention_masks.data()), REX_buffer_size_1,
                input_dims_A[1].data(), input_dims_A[1].size(), input_types_A[1], &input_tensors_A[1]);
        ort_runtime_A->CreateTensorWithDataAsOrtValue(
                memory_info, reinterpret_cast<void*>(position_ids.data()), REX_buffer_size_0_2,
                input_dims_A[2].data(), input_dims_A[2].size(), input_types_A[2], &input_tensors_A[2]);
        ort_runtime_A->ReleaseMemoryInfo(memory_info);
        ort_runtime_A->Run(session_model_A, nullptr, input_names_A.data(), (const OrtValue* const*)input_tensors_A.data(),
                           input_tensors_A.size(), output_names_A.data(), output_names_A.size(),
                           output_tensors_A.data());
        ort_runtime_A->GetTensorMutableData(output_tensors_A[0], &output_tensors_buffer_0);
    }
    auto* output_A = reinterpret_cast<float*>(output_tensors_buffer_0);
    index_i = 0;
    int count = 0;
    std::vector<int> logit_results(max_token_limit_REX - save_tag_size[task_id],-1);
    if (task_id > 1) {  // For extract tasks.
        for (int i = 0; i < max_token_limit_REX; i++) {
            for (int j = 0; j < max_token_limit_REX; j++) {
                if ((i == tag_position_offset) | (j == tag_position_offset)) {
                    continue;
                }
                if (output_A[index_i + j] > 0) {
                    logit_results[count] = i - save_tag_size[task_id];
                    logit_results[count + 1] = j - save_tag_size[task_id];
                    count += tag_position_offset;
                }
            }
            index_i += max_token_limit_REX;
        }
    } else {  // For classify tasks.
        for (int i = 0; i < max_token_limit_REX; i++) {
            for (int j = 0; j < max_token_limit_REX; j++) {
                if (i == j) {
                    continue;
                }
                if (output_A[index_i + j] > 0) {
                    logit_results[count] = i;
                    logit_results[count + 1] = j;
                    count += tag_position_offset;
                }
            }
            index_i += max_token_limit_REX;
        }
    }
    if (count < 1) {
        count = 1;
    }
    jintArray final_results = env->NewIntArray(count);
    env->SetIntArrayRegion(final_results,0,count,logit_results.data());
    return final_results;
}
