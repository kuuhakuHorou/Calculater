#include <string>
#include <vector>
#include <stack>
#include <map>

#ifndef _CLASS_CAL_H_
#define _CLASS_CAL_H_   //避免再次定義class Calculator

using c_type = long long;   //利用c_type指定運算值的類型(無法使用浮點數(%運算))

//絕對值的狀態(開始或結束)
enum Absolute_status {
    Begin, End
};

//特殊字句
enum Special {
    Shang_xiang, TK888, MIKU, MIKU3939, OuO, C8763, NEKOPARA,
};

class Calculator {
private:

    //利用 map
    static std::map<char, int> ops; //搜尋運算子和運算優先度
    static std::map<char, int> operand; //取得運算子的運算元個數
    static std::map<std::string, Special> spes; //找尋特殊字句
    std::map<size_t, Absolute_status> absolute_statuses; //用於儲存絕對值的狀態

    static c_type answer;   //計算出的答案
    static bool first_calculate;    //第一次的計算判斷
    int absolute_status;    //絕對值的狀態以整數儲存
    bool lost_absolute; //缺少的絕對值判斷
    bool wrong_absolute_number;    //錯誤的絕對值數判斷
    bool wrong_brackets_number;    //錯誤的括號判斷
    bool wrong_expression;  //有少的運算子判斷
    bool wrong_input;       //錯誤的運算式判斷(沒有數字)
    bool wrong_operation;   //沒有的運算子判斷
    bool brackets_have_nothing; //括號裡沒東西
    bool have_answer;       //算式有無 ans
    bool exit;  //離開判斷

    bool is_wrong(const std::string &); //檢查有無錯誤
    void warning_message();     //錯誤訊息
    bool is_pm(const char &);   //是否為正負號('+','-')
    bool is_operator(const char &);     //是否為擁有的運算子(char 型態)
    bool is_operator(const std::string &);  //是否為擁有的運算子(std::string 型態)
    int pripority(char);    //查看運算子的優先度
    bool is_number(char);   //檢查是否為數字
    bool is_integer(const std::string &, const size_t &);  //檢查是否為數字(有正負數的情況)
    bool is_space(char);    //檢查是否為空白(space與tab)
    void to_lower(std::string &);  //把運算式的所有字母轉為小寫
    void find_abs(const std::string &); //找出所有的絕對值並確認開始及結束
    Absolute_status find_abs_status(const size_t &);  //搜尋絕對值的狀態
    void combine(std::string &);    //合併一些麻煩的符號
    void combine_pm(std::string &, std::string &, std::string &, const size_t &, const size_t &, char);    //合併不需要的正負號
    size_t infix_to_postfix(std::vector<std::string> &, const std::string &, const size_t start = 0, const char input_op = '\0');   //把輸入的運算式拆解成後綴式
    c_type calcul_postfix(const std::vector<std::string> &);   //運算後綴式的結果
    c_type factorial(const c_type &);     //計算階乘
    c_type power(const c_type &, const c_type &);    //計算指數(只有正整數的次方數)
    bool is_factorial_overflow(const c_type &); //階乘是否超出上限
    bool is_special(const std::string &, Special &);    //是否為特殊字句
    void print_special(const Special &);    //印出特殊字句的特殊輸出

public:
    Calculator();   //一般建構式
    bool string_calculate(std::string &);    //呼叫運算式分解並計算
    c_type op_calculate(char, const c_type &front = 0, const c_type &back = 0);  //呼叫運算子做計算
    bool is_exit(); //檢查是否須離開
    c_type get_answer(); //得到運算出的答案
    static void m_delay(const unsigned);  //以毫秒為單位的延遲
    static void print_dash();  //印出分行(----)

};

#endif
