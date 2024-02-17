package com.example.myapplication;

import android.annotation.SuppressLint;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.view.WindowManager;
import android.widget.AutoCompleteTextView;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.example.myapplication.databinding.ActivityMainBinding;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Objects;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class MainActivity extends AppCompatActivity {
    static final int font_size = 18;
    private static int count_words = 0;
    private static final int token_end_flag_NLU = 2;  // Replace with your 'end' token index.
    private static final int token_unknown_NLU = 3;  // Replace with your 'unknown' token index.
    private static final int max_token_limit_REX = 40;  // Replace with your max input size. The same variable in the project.h, please modify at the same time.
    private static final int pre_set_tag_tokens = 25;  // The longest tag size+1 in project.h
    private static final int[] input_token_REX = new int[max_token_limit_REX];
    private static String usrInputText = "";
    private static final String enter_question = "请输入问题 \nEnter Questions";
    private static final String question_overflow = "您输入的单词太多了。\nYou have entered too much words.";
    private static final String cleared = "已清除 Cleared";
    private static final String file_name_vocab_REX = "vocab_REX.txt";
    private static final String[] classify_tag = {"AI助理 Assistant", "自驾导航 Adventurer", "音乐媒体 Media", "车辆调节 Vehicle Control", "大百科 Encyclopedia", "知己 Confidant", "说书人 Raconteur", "闲聊 Free Talk"};
    private static final List<String> vocab_REX = new ArrayList<>();
    Button sendButton;
    @SuppressLint("StaticFieldLeak")
    static AutoCompleteTextView inputBox;
    static RecyclerView answerView;
    private static ChatAdapter chatAdapter;
    private static List<ChatMessage> messages;

    static {
        System.loadLibrary("myapplication");
    }
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        com.example.myapplication.databinding.ActivityMainBinding binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_NOT_TOUCHABLE, WindowManager.LayoutParams.FLAG_NOT_TOUCHABLE);
        ImageView set_photo = findViewById(R.id.role_image);
        Button clearButton = findViewById(R.id.clear);
        sendButton = findViewById(R.id.send);
        inputBox = findViewById(R.id.input_text);
        messages = new ArrayList<>();
        chatAdapter = new ChatAdapter(messages);
        answerView = findViewById(R.id.result_text);
        answerView.setLayoutManager(new LinearLayoutManager(this));
        answerView.setAdapter(chatAdapter);
        set_photo.setImageResource(R.drawable.psyduck);
        clearButton.setOnClickListener(v -> clearHistory());
        AssetManager mgr = getAssets();
        runOnUiThread(() -> {
            Pre_Process();
            if (!Load_Models_0(mgr,false,false,false,false,false,false)) {
                addHistory(ChatMessage.TYPE_SYSTEM, "模型加载失败。\nModel loading failed.");
            }
        });
        runOnUiThread(() -> Read_Assets(file_name_vocab_REX, mgr));
        Init_Chat();
        getWindow().clearFlags(WindowManager.LayoutParams.FLAG_NOT_TOUCHABLE);
    }
    private void Init_Chat() {
        sendButton.setOnClickListener(view -> {
            usrInputText = String.valueOf(inputBox.getText());
            inputBox.setText("");
            if (usrInputText.isEmpty()){
                showToast(enter_question,false);
                return;
            }
            addHistory(ChatMessage.TYPE_USER, usrInputText);
            long start_time = System.currentTimeMillis();
            count_words = Tokenizer(usrInputText);
            usrInputText = "";
            if (count_words < 0) {
                runOnUiThread(() -> addHistory(ChatMessage.TYPE_SYSTEM, question_overflow));
                return;
            }
            StringBuilder extract_places = new StringBuilder();
            StringBuilder extract_numbers = new StringBuilder();
            StringBuilder extract_songs = new StringBuilder();
            StringBuilder extract_names = new StringBuilder();
            int[] extract_result = Run_Classify_Extract(input_token_REX, count_words,0);  // Classify the input query.
            int class_answer;
            switch (extract_result[0]) {
                case -1, 7 -> {  // The '7' corresponds to the tag_idx position, which is listed in project.h and was generated by the model encoder.
                    extract_result = Run_Classify_Extract(input_token_REX, count_words,1);  // Classify the input query.
                    if (extract_result[0] != -1) {
                        switch (extract_result[0]) {
                            case 2 -> class_answer = 5;
                            case 7 -> class_answer = 6;
                            case 12 -> class_answer = 4;
                            case 17 -> class_answer = 3;
                            default -> class_answer = 7;
                        }
                    } else {
                        class_answer = 7;
                    }
                    if ((class_answer == 3) | (class_answer == 7)) {
                        extract_result = Run_Classify_Extract(input_token_REX, count_words,4);  // Extract numbers
                        if (extract_result[0] != -1) {
                            if (extract_result.length == 2) {  // To handle the model extract problem in some cases.
                                if (extract_result[0] == extract_result[1]) {
                                    extract_result[1] += 8; // Set as 8 means: It supports 8 Arabic numerals to extract.
                                }
                            }
                            int word_offset = 0;
                            ArrayList<Integer> arrayList = new ArrayList<>();
                            for (int j = 0; j < extract_result.length; j += 2) {
                                for (int k = 0; k < count_words; k++) {
                                    if (k >= extract_result[j] - word_offset && k <= extract_result[j + 1] - word_offset) {
                                        extract_numbers.append(vocab_REX.get(input_token_REX[k]));
                                    } else {
                                        arrayList.add(input_token_REX[k]);
                                    }
                                }
                                extract_numbers.append(',');
                                int[] temp_input_token_REX = arrayList.stream().mapToInt(Integer::intValue).toArray();
                                count_words = temp_input_token_REX.length;
                                System.arraycopy(temp_input_token_REX,0, input_token_REX,0, count_words);
                                word_offset += extract_result[j + 1] - extract_result[j] + 1;
                            }
                        }
                    }
                }
                case 2, 12 -> {  // The '2' & '12' are the tag_idx position, too.
                    class_answer = 1;
                    extract_result = Run_Classify_Extract(input_token_REX, count_words,2);  // Extract navigation place
                    while (extract_result[0] != -1) {
                        int word_offset = 0;
                        for (int j = 0; j < extract_result.length; j += 2) {
                            ArrayList<Integer> arrayList = new ArrayList<>();
                            for (int k = 0; k < count_words; k++) {
                                if (k >= extract_result[j] - word_offset && k <= extract_result[j + 1] - word_offset) {
                                    extract_places.append(vocab_REX.get(input_token_REX[k]));
                                } else {
                                    arrayList.add(input_token_REX[k]);
                                }
                            }
                            extract_places.append(',');
                            int[] temp_input_token_REX = arrayList.stream().mapToInt(Integer::intValue).toArray();
                            count_words = temp_input_token_REX.length;
                            System.arraycopy(temp_input_token_REX,0, input_token_REX,0, count_words);
                            word_offset += extract_result[j + 1] - extract_result[j] + 1;
                        }
                        if (count_words > 2) {
                            extract_result = Run_Classify_Extract(input_token_REX, count_words,2);  // Extract navigation place
                        } else {
                            break;
                        }
                    }
                }
                case 17 -> {
                    class_answer = 2;
                    extract_result = Run_Classify_Extract(input_token_REX, count_words,3);  // Extract song name
                    if (extract_result[0] != -1) {
                        int word_offset = 0;
                        ArrayList<Integer> arrayList = new ArrayList<>();
                        for (int j = 0; j < extract_result.length; j += 2) {
                            for (int k = 0; k < count_words; k++) {
                                if (k >= extract_result[j] - word_offset && k <= extract_result[j + 1] - word_offset) {
                                    extract_songs.append(vocab_REX.get(input_token_REX[k]));
                                } else {
                                    arrayList.add(input_token_REX[k]);
                                }
                            }
                            extract_songs.append(',');
                            int[] temp_input_token_REX = arrayList.stream().mapToInt(Integer::intValue).toArray();
                            count_words = temp_input_token_REX.length;
                            System.arraycopy(temp_input_token_REX, 0, input_token_REX,0, count_words);
                            word_offset += extract_result[j + 1] - extract_result[j] + 1;
                        }
                    }
                    if (count_words > 2) {
                        extract_result = Run_Classify_Extract(input_token_REX, count_words,5);  // Extract person name
                        if (extract_result[0] != -1) {
                            int word_offset = 0;
                            ArrayList<Integer> arrayList = new ArrayList<>();
                            for (int j = 0; j < extract_result.length; j += 2) {
                                for (int k = 0; k < count_words; k++) {
                                    if (k >= extract_result[j] - word_offset && k <= extract_result[j + 1] - word_offset) {
                                        extract_names.append(vocab_REX.get(input_token_REX[k]));
                                    } else {
                                        arrayList.add(input_token_REX[k]);
                                    }
                                }
                                extract_names.append(',');
                                int[] temp_input_token_REX = arrayList.stream().mapToInt(Integer::intValue).toArray();
                                count_words = temp_input_token_REX.length;
                                System.arraycopy(temp_input_token_REX,0, input_token_REX,0, count_words);
                                word_offset += extract_result[j + 1] - extract_result[j] + 1;
                            }
                        }
                    }
                }
                default -> class_answer = 7;
            }
            addHistory(ChatMessage.TYPE_SERVER,"Time Cost: " + (System.currentTimeMillis() - start_time) + "ms");
            addHistory(ChatMessage.TYPE_SERVER,"\n\n分类结果 The Classification: \n" + classify_tag[class_answer]);
            addHistory(ChatMessage.TYPE_SERVER,"\n\n提取结果 The Extraction: \n");
            switch (class_answer) {
                case 1 -> {
                    if (extract_places.length() > 0) {
                        extract_places.insert(0, "目的地 Destination: ");
                        addHistory(ChatMessage.TYPE_SERVER,extract_places + "\n");
                    }
                }
                case 2 -> {
                    if (extract_names.length() > 0) {
                        extract_names.insert(0, "人名 Name: ");
                        addHistory(ChatMessage.TYPE_SERVER,extract_names + "\n");
                    }
                    if (extract_songs.length() > 0) {
                        extract_songs.insert(0, "歌曲名 Song: ");
                        addHistory(ChatMessage.TYPE_SERVER,extract_songs + "\n");
                    }
                }
                case 3, 7 -> {
                    if (extract_numbers.length() > 0) {
                        String digit_values = NumberConverter(extract_numbers.toString());
                        extract_numbers.setLength(0);
                        extract_numbers.append("数值 Values: ");
                        extract_numbers.append(digit_values);
                        addHistory(ChatMessage.TYPE_SERVER,extract_numbers + "\n");
                    }
                }
            }
        });
    }
    @SuppressLint("NotifyDataSetChanged")
    private static void addHistory(int messageType, String result) {
        int lastMessageIndex = messages.size() - 1;
        if (messageType == ChatMessage.TYPE_SYSTEM) {
            messages.add(new ChatMessage(messageType, result));
        } else if (lastMessageIndex >= 0 && messages.get(lastMessageIndex).type() == messageType) {
            if (messageType != ChatMessage.TYPE_USER ) {
                messages.set(lastMessageIndex, new ChatMessage(messageType, messages.get(lastMessageIndex).content() + result));
            } else {
                messages.set(lastMessageIndex, new ChatMessage(messageType, result));
            }
        } else {
            messages.add(new ChatMessage(messageType, result));
        }
        chatAdapter.notifyDataSetChanged();
        answerView.smoothScrollToPosition(messages.size() - 1);
    }
    @SuppressLint("NotifyDataSetChanged")
    private void clearHistory(){
        inputBox.setText("");
        messages.clear();
        chatAdapter.notifyDataSetChanged();
        answerView.smoothScrollToPosition(0);
        showToast( cleared,false);
    }
    private static int Search_Token_Index(@NonNull String word) {
        int index = vocab_REX.indexOf(word);
        if (index != -1) {
            return index;
        }
        return token_unknown_NLU;
    }
    private static int Tokenizer(String question) {
        Matcher matcher = Pattern.compile("\\p{InCJK_UNIFIED_IDEOGRAPHS}|([a-zA-Z]+)|\\d|\\p{Punct}").matcher(question.toLowerCase());
        int count = 0;
        while (matcher.find()) {
            String match = matcher.group();
            if (!match.trim().isEmpty()) {
                int search_result = Search_Token_Index(match);
                if (search_result != token_unknown_NLU) {
                    input_token_REX[count] = search_result;
                    count++;
                    if (count == max_token_limit_REX - pre_set_tag_tokens) {
                        break;
                    }
                } else {
                    String[] match_split = match.split("");
                    if (match_split.length > 1) {
                        for (String alphabet : match_split) {
                            input_token_REX[count] = Search_Token_Index(alphabet);
                            count++;
                            if (count == max_token_limit_REX - pre_set_tag_tokens) {
                                break;
                            }
                        }
                    } else {
                        input_token_REX[count] = token_unknown_NLU;
                        count++;
                        if (count == max_token_limit_REX - pre_set_tag_tokens) {
                            break;
                        }
                    }
                }
            }
        }
        MainActivity.input_token_REX[count] = MainActivity.token_end_flag_NLU;
        count++;
        return count;
    }
    private static class DigitConverter {
        public static String convertDigitArrayToNumber(char[] digitArray) {
            int result = 0;
            int multiplier = 1;
            for (int i = digitArray.length - 1; i >= 0; i--) {
                int digit = Character.getNumericValue(digitArray[i]);
                result += digit * multiplier;
                multiplier *= 10;
            }
            return Integer.toString(result);
        }
    }
    private static class NumberConverter {
        private static final HashMap<String, Integer> chineseNumberMap = new HashMap<>();
        static {
            chineseNumberMap.put("零", 0);
            chineseNumberMap.put("一", 1);
            chineseNumberMap.put("二", 2);
            chineseNumberMap.put("三", 3);
            chineseNumberMap.put("四", 4);
            chineseNumberMap.put("五", 5);
            chineseNumberMap.put("六", 6);
            chineseNumberMap.put("七", 7);
            chineseNumberMap.put("八", 8);
            chineseNumberMap.put("九", 9);
            chineseNumberMap.put("十", 10);
            chineseNumberMap.put("百", 100);
        }
        private static final HashMap<String, Integer> englishNumberMap = new HashMap<>();
        static {
            englishNumberMap.put("zero", 0);
            englishNumberMap.put("one", 1);
            englishNumberMap.put("two", 2);
            englishNumberMap.put("three", 3);
            englishNumberMap.put("four", 4);
            englishNumberMap.put("five", 5);
            englishNumberMap.put("six", 6);
            englishNumberMap.put("seven", 7);
            englishNumberMap.put("eight", 8);
            englishNumberMap.put("nine", 9);
            englishNumberMap.put("ten", 10);
            englishNumberMap.put("hundred", 100);
        }
        private static String convertWordNumberToDigit(String wordNumber) {
            int result = 0;
            int temp = 0;
            boolean pre_is_hundred = false;
            boolean is_first_digit = true;
            boolean is_english_words = wordNumber.split("")[0].matches("^[A-Za-z]+$");
            StringBuilder convert_results = new StringBuilder();
            String[] words;
            if (is_english_words) {
                words = wordNumber.split(" ");
            } else {
                words = wordNumber.split("");
            }
            for (String w : words) {
                if (Objects.equals(w,",")) {
                    convert_results.append((result + temp));
                    convert_results.append(", ");
                } else {
                    int digit;
                    if (is_english_words && englishNumberMap.containsKey(w)) {
                        digit = englishNumberMap.get(w);
                    } else if (chineseNumberMap.containsKey(w)) {
                        digit = chineseNumberMap.get(w);
                    } else {
                        continue;
                    }
                    if (digit >= 10) {
                        if (is_first_digit) {
                            temp = 1;
                        }
                        is_first_digit = false;
                        result += temp * digit;
                        temp = 0;
                        if (digit == 100) {
                            pre_is_hundred = true;
                        }
                    } else {
                        if (pre_is_hundred) {
                            temp += digit * 10;
                            pre_is_hundred = false;
                        } else {
                            temp = temp * 10 + digit;
                        }
                        is_first_digit = false;
                    }
                }
            }
            return convert_results.toString();
        }
    }
    private static String NumberConverter(String input_string) { // It cannot handle Arabic numerals and numbers in words within the same sentence simultaneously.
        StringBuilder digitSegment = new StringBuilder();
        StringBuilder wordsSegment = new StringBuilder();
        for (char c : input_string.toCharArray()) {
            if (Character.isDigit(c)) {
                digitSegment.append(c);
            } else {
                wordsSegment.append(c);
            }
        }
        if (digitSegment.length() > 0) {
            return DigitConverter.convertDigitArrayToNumber(digitSegment.toString().toCharArray());
        }
        if (wordsSegment.length() > 0) {
            return NumberConverter.convertWordNumberToDigit(wordsSegment.toString());
        }
        return "NaN";
    }
    private void showToast(final String content, boolean display_long){
        if (display_long) {
            Toast.makeText(this, content, Toast.LENGTH_LONG).show();
        } else {
            Toast.makeText(this, content, Toast.LENGTH_SHORT).show();
        }
    }
    private void Read_Assets(String file_name, AssetManager mgr) {
        switch (file_name) {
            case file_name_vocab_REX -> {
                try {
                    BufferedReader reader = new BufferedReader(new InputStreamReader(mgr.open(file_name_vocab_REX)));
                    String line;
                    while ((line = reader.readLine()) != null) {
                        vocab_REX.add(line);
                    }
                    reader.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }
    private native boolean Load_Models_0(AssetManager assetManager, boolean USE_GPU, boolean FP16, boolean USE_NNAPI, boolean USE_XNNPACK, boolean USE_QNN, boolean USE_DSP_NPU);
    private native boolean Pre_Process();
    private static native int[] Run_Classify_Extract(int[] token_index, int words_count, int task_id);
}
