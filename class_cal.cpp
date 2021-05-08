#include <iostream>
#include <sstream>
#include <limits>
#include <chrono>
#include <exception>
#include "class_cal.h"

//初始化static成員
c_type Calculator::answer = 0;
bool Calculator::first_calculate = true;
std::map<char, int> Calculator::ops = {
    {'^', 3}, {'e', 3}, {'c', 3}, {'p', 3}, {'!', 3},
    {'*', 2}, {'/', 2}, {'%', 2},
    {'+', 1}, {'-', 1},
    {'|', 0},
};
std::map<char, int> Calculator::operand = {
    {'^', 2}, {'e', 2}, {'c', 2}, {'p', 2}, {'*', 2}, {'/', 2}, {'%', 2}, {'+', 2}, {'-', 2},
    {'!', 1}, {'|', 1},
};
std::map<std::string, Special> Calculator::spes = {
    {R"(\|/)", Special::Shang_xiang}, {"tk888", Special::TK888}, {"miku", Special::MIKU},
    {"miku3939", Special::MIKU3939}, {"ouo", Special::OuO}, {"c8763", Special::C8763},
    {"nekopara", Special::NEKOPARA},
};

//以下為public成員函式
//一般建構式
Calculator::Calculator() {
    this->absolute_status = 0;
    this->lost_absolute = false;
    this->wrong_absolute_number = false;
    this->wrong_brackets_number = false;
    this->wrong_expression = false;
    this->wrong_input = false;
    this->wrong_operation = false;
    this->brackets_have_nothing = false;
    this->have_answer = false;
    this->exit = false;
    this->absolute_statuses.clear();
}

//呼叫運算式分解並計算
bool Calculator::string_calculate(std::string &user_input) {
    Special special;
    static std::vector<std::string> exprs;
    if (!this->absolute_statuses.empty()) {  //如果絕對值的狀態不為空的話，先清除
        this->absolute_statuses.clear();
    }
    this->to_lower(user_input); //避免麻煩，把所有的字母轉成小寫
    if (user_input == "clear") {  //清除螢幕
        for (int i = 0; i < 50; i++) {
            std::cout << std::endl;
        }
        return false;
    }
    else if (this->is_special(user_input, special)) {   //是否有輸入特別的句子
        this->print_special(special);   //特別的輸出
        return false;
    }
    else if (user_input != "exit") {    //不是退出就繼續
        this->combine(user_input);  //先整理難處理的情況
        /*//debugger 查看找到的絕對值
        for (std::map<unsigned, Absolute_status>::iterator it = this->abs_status.begin(); it != this->abs_status.end(); ++it) {
            std::cout << '[' << it->first << "]: " << it->second << std::endl;
        }
        */
        // std::cout << user_input << std::endl; //debugger 查看合併後的狀況
        if (this->is_wrong(user_input)) {   //判斷輸入有無錯誤
            this->warning_message();    //告知錯誤
            return false;
        }
        exprs.clear();
        try {
            this->infix_to_postfix(exprs, user_input);  //嘗試拆解算試
            Calculator::answer = this->calcul_postfix(exprs);   //嘗試計算
        }
        catch (std::logic_error &ex) {  //抓取邏輯錯誤
            std::cout << std::endl << ex.what() << std::endl;   //輸出收集的訊息
            return false;
        }
        catch (std::overflow_error &ex) {   //抓取超出範圍錯誤
            std::cout << std::endl << ex.what() << std::endl;
            return false;
        }
        catch (const char* &message) {  //抓取錯誤訊息
            std::cout << std::endl << message << std::endl; //輸出抓到的訊息
            return false;
        }
        if (Calculator::first_calculate) {    //如果第一次算完答案的話，就不會是第一次算了
            Calculator::first_calculate = false;
        }
    }
    else if (user_input == "exit"){ //要離開了
        this->exit = true;
        return false;
    }
    return true;
}

//呼叫運算子做計算
c_type Calculator::op_calculate(char calcul, const c_type &front, const c_type &back) {
    c_type res = 0;
    switch (calcul) {
        case '+':
            res = front + back;
            break;
        case '-':
            res = front - back;
            break;
        case '*':
            res = front * back;
            break;
        case '/':
            if (back == 0) {
                throw std::logic_error("有除以0，請更正");
            }
            res = front / back;
            break;
        case '%':
            if (back == 0) {
                throw std::logic_error("有除以0，請更正");
            }
            res = front % back;
            break;
        case '^':   //次方的運算
            res = this->power(front, back);
            break;
        case 'e':   //科學記號的算式
            res = front * this->power(10, back);
            break;
        case '!':   //階乘
            res = this->factorial(this->op_calculate('|',front));
            if (front < 0) {    //對於前面加上負號的階乘
                res *= -1;
            }
            break;
        case 'c':   //組合
            if ((front - back) < 0) return 0;
            res = this->factorial(front) / (this->factorial(back) * this->factorial(front - back));
            break;
        case 'p':   //排列
            if ((front - back) < 0) return 0;
            res = this->factorial(front) / this->factorial(front - back);
            break;
        case '|':   //絕對值
            if (front < 0) {
                res = -front;
            }
            else {
                res = front;
            }
            break;
    }
    return res;
}

//檢查是否須離開
bool Calculator::is_exit() {
    return this->exit;
}

//得到運算出的答案
c_type Calculator::get_answer() {
    return Calculator::answer;
}

//以毫秒為單位的延遲
void Calculator::m_delay(const unsigned time) {
    using namespace std::chrono;    //利用chrono裡的steady_clock進行時間運算
    static steady_clock::time_point start, end;
    start = steady_clock::now();    //先取得一次現在時間
    do {
        end = steady_clock::now();  //再取一次
    } while (duration_cast<milliseconds>(end - start).count() < time);  //當end - start不到time時繼續
}

//印出分行(----)
void Calculator::print_dash() {
    for (int i = 0; i < 40; i++) {
        std::cout << '-';
    }
    std::cout << std::endl;
}

//以下為private成員函式
//檢查有無錯誤
bool Calculator::is_wrong(const std::string &expr) {
    static std::stringstream Num;   //利用stringstream取得數字
    bool not_expression = true; //不是運算式
    bool not_operation = false; //不是運算子
    bool loss_something = false;    //少了什麼
    bool first_calculate_want_ans = false;  //第一次就想要上次的"ans"
    bool empty_brackets = false;    //空的括號
    int brackets = 0;   //括弧數
    size_t left_bracket = 0;  //左邊括號所在處
    this->absolute_status = 0;  //把絕對值的狀態歸零
    this->have_answer = false;  //假設算式沒有答案
    for (size_t i = 0; i < expr.size(); i++) {
        if (this->is_space(expr[i])) {  //略過空白字元
            continue;
        }
        else if (expr[i] == '(') {
            brackets--; //有左括號，括號數減1
            left_bracket = i;   //記下左括號的位置
            int n = i - 1;
            for (; n > 0 && (this->is_space(expr[n]) || expr[n] == '(' || expr[n] == '|'); n--);    //嘗試尋找前面的運算子
            if (n < 0) {   //前面沒東西，跳過
                continue;
            }
            else if (!((this->is_operator(expr[n]) && expr[n] != '!') || expr[n] == '(' || this->is_space(expr[n]))) {
                //前面不是運算子且不含階乘，或不是'('和空白，可能少了什麼
                loss_something = true;
            }
        }
        else if (expr[i] == ')') {
            brackets++; //有右括號，括號數加1
            if (!empty_brackets) {  //如有空括號，不用重複檢查
                size_t n = left_bracket + 1;
                for (; n < i && this->is_space(expr[n]); n++);  //檢查裡面是否為空
                if (n == i) {   //裡面為空
                    empty_brackets = true;
                }
            }
            size_t n = i + 1;
            for (; n < expr.size() && (this->is_space(expr[n]) || expr[n] == ')' || expr[n] == '|'); n++);  //嘗試尋找後面的運算子
            if (n >= expr.size()) { //後面沒東西，跳過
                continue;
            }
            else if (!this->is_operator(expr[n])) { //後面不是運算子，應該少了些什麼
                loss_something = true;
            }
        }
        else if (expr[i] == '|') {  //如果此處為絕對值
            if (this->find_abs_status(i) == Absolute_status::Begin) {   //如果為絕對值的開始的話
                this->absolute_status--;  //狀態-1
                int n = i - 1;
                for (; n > 0 && (this->is_space(expr[n]) || expr[n] == '(' || expr[n] == '|'); n--);    //嘗試尋找前面的運算子
                if (n < 0) {    //前面為空，跳過
                    continue;
                }
                else if (!((this->is_operator(expr[n]) && expr[n] != '!') || expr[n] == '(' || this->is_space(expr[n]))) {
                    //前面不是運算子且不含階乘，或不是'('和空白，可能少了什麼
                    loss_something = true;
                }
            }
            else if (this->find_abs_status(i) == Absolute_status::End) {    //如果為絕對值的結束的話
                this->absolute_status++;  //狀態+1
                size_t n = i + 1;
                for (; n < expr.size() && (this->is_space(expr[n]) || expr[n] == ')' || expr[n] == '|'); n++);  //嘗試尋找後面的運算子
                if (n >= expr.size()) { //後面沒東西，跳過
                    continue;
                }
                else if (!this->is_operator(expr[n])) { //後面不是運算子，應該少了些什麼
                    loss_something = true;
                }
            }
        }
        else if (this->is_number(expr[i])) {    //有數字的話會是算式
            not_expression = false;
        }
        else if (expr[i] == 'a' && expr.substr(i, 3) == "ans") {    //算式裡有ans
            this->have_answer = true;
            not_expression = false; //有ans也會是算式
            i += 2; //跳過"ans"
        }
        else if (!not_operation && !this->is_operator(expr[i])) {  //沒有的運算子，不會有功能，且不需重新檢查
            not_operation = true;
        }
        if (this->is_operator(expr[i]) && expr[i] != '|') { //是運算子且不是絕對值
            int n = i - 1;
            size_t m = i + 1;
            for (; n > 0 && (this->is_space(expr[n]) || expr[n] == ')' || expr[n] == '|'); n--);    //嘗試找出前面的數字
            for (; m < expr.size() && (this->is_space(expr[m]) || expr[m] == '(' || expr[m] == '|'); m++);  //嘗試找出後面的數字
            if (n < 0) {    //當i為第一個運算子且前面沒東西時
                if (this->is_pm(expr[i])) { //如果i的位置為'+'或'-'
                    if (this->is_integer(expr, i)) {
                        //如果i此處為整數
                        continue;
                    }
                }
            }
            else if (m >= expr.size()) {    //如果i為最後的字元
                if (expr.size() > 1) {  //確保不是只有'!'
                    if (expr[i] == '!') {   //最後為階乘運算子也沒錯
                        continue;
                    }
                }
            }
            else {  //當i為運算子時
                //看看前或後是不是答案
                bool front_have_ans = (expr[n] == 's' && n >= 2 && expr.substr(n - 2, 3) == "ans")? true: false;
                bool back_have_ans = (expr[m] == 'a' && m + 2 < expr.size() && expr.substr(m, 3) == "ans")? true: false;
                //看後面是不是整數
                bool back_is_integer = this->is_integer(expr, m);
                if (this->is_pm(expr[i])) { //運算子為+,-
                    //看看是不是整數
                    bool now_is_integer = this->is_integer(expr, i);
                    if ((this->is_number(expr[n]) || front_have_ans) && (this->is_number(expr[m]) || back_is_integer || back_have_ans)) {
                        //前後都是數字or ans，沒問題
                        continue;
                    }
                    else if ((this->is_operator(expr[n]) || expr[n] == '(') && now_is_integer) {
                        //前面為運算子或左括號或ans且i為整數，也沒問題
                        continue;
                    }
                    else if (expr[n] == '!') {  //前面是階乘也沒問題的拉
                        continue;
                    }
                    else {
                        for (n = i - 1; n > 0 && this->is_space(expr[n]); n--);
                        //前面都是空格，沒問題的
                        if (n == 0) {
                            continue;
                        }
                    }
                }
                else if (expr[i] == '!') {
                    if (this->is_operator(expr[m])) {   //階乘後面有個運算子很正常
                        continue;
                    }
                }
                else if ((this->is_number(expr[n]) || expr[n] == '!' || front_have_ans)
                      && (this->is_number(expr[m]) || back_is_integer || back_have_ans)) {
                    //其他的運算子前如果是數字或階乘或絕對值或ans和後面是數字或整數或ans，沒問題
                    continue;
                }
                else if (this->is_operator(expr[n]) || this->is_operator(expr[m])) {    //前後如果是運算子
                    if (expr[i] == '!' || expr[n] == '!') { //自己或前面是階乘，沒問題
                        continue;
                    }
                }
            }
            loss_something = true;  //大概少了什麼吧
        }
    }
    //更新狀態
    this->wrong_absolute_number = (this->absolute_statuses.size() % 2 == 0)? false: true; //絕對值成雙，所以對2取餘數應為0
    this->lost_absolute = (this->absolute_status == 0)? false: true;
    this->wrong_brackets_number = (brackets == 0)? false: true;    //括號應成對，所以括號數應為0
    this->wrong_expression = loss_something;
    this->wrong_input = not_expression;
    this->wrong_operation = not_operation;
    this->brackets_have_nothing = empty_brackets;
    first_calculate_want_ans = Calculator::first_calculate && this->have_answer;
    //回傳是否有錯誤(只要一個有就不該計算)
    return this->wrong_brackets_number || this->wrong_absolute_number || this->lost_absolute || loss_something || not_expression || not_operation || empty_brackets || first_calculate_want_ans;
}

//錯誤訊息
void Calculator::warning_message() {
    static std::vector<std::string> error_messages; //利用vector收集先找到的錯誤訊息
    error_messages.clear(); //清掉上次的訊息
    if (this->wrong_absolute_number) {
        error_messages.push_back("絕對值的部分好像有錯");
    }
    if (this->lost_absolute) {
        std::string message("好像少了");
        message.append(std::to_string(this->op_calculate('|', this->absolute_status))); //取絕對值
        if (this->absolute_status < 0) {    //狀態為負數，開始為多
            message.append("個右絕對值");
        }
        else {  //狀態為正數，結束為多
            message.append("個左絕對值");
        }
        error_messages.push_back(message);
    }
    if (this->wrong_brackets_number) {
        error_messages.push_back("括號數有錯，請更正!!");
    }
    if (this->wrong_expression) {
        error_messages.push_back("好像少了些什麼?");
    }
    if (this->wrong_input) {
        error_messages.push_back("這不是個算式!!");
    }
    if (this->wrong_operation) {
        error_messages.push_back("有奇怪的東西混進來了");
    }
    if (this->brackets_have_nothing) {
        error_messages.push_back("有空的括號喔!");
    }
    if (Calculator::first_calculate && this->have_answer) {
        error_messages.push_back("第一次算並沒有先前的答案!!");
    }
    std::cout << std::endl; //先換行
    for (std::string error_message: error_messages) {   //輸出所有的錯誤訊息
        std::cout << error_message << std::endl;
    }
}

//是否為正負號('+','-')
bool Calculator::is_pm(const char &c) {
    return (c == '+' || c == '-');
}

//是否為擁有的運算子(char 型態)
bool Calculator::is_operator(const char &c) {
    //從ops裡找，如果有就對了
    return (Calculator::ops.find(c) != Calculator::ops.end())? true: false;
}

//是否為擁有的運算子(std::string 型態)
bool Calculator::is_operator(const std::string &op) {
    //如op長度只有1且是運算子，對拉
    return (op.size() == 1 && this->is_operator(op[0]))? true: false;
}

//查看運算子的優先度
int Calculator::pripority(char c) {
    //優先度為ops裡的value值，若沒有運算子，則為最低階運算(不太會發生了)
    return (this->is_operator(c))? Calculator::ops[c] : -1;
}

//檢查是否為數字
bool Calculator::is_number(char c) {
    return (c >= '0' && c <= '9')? true: false;
}

//檢查是否為數字(有正負數的情況)
bool Calculator::is_integer(const std::string &expr, const size_t &pos) {
    char op = expr[pos];
    char front = (pos > 0)? expr[pos - 1]: '\0';
    char back = (pos + 1 < expr.size())? expr[pos + 1]: '\0';
    if (this->is_pm(op)) {   //看op是否為+,-
        if (((this->is_operator(front) && front != '|' && front != '!' && !this->is_pm(front))) || front == '(' || front == '\0' || this->is_space(front) || (front == '|' && this->find_abs_status(pos-1) == Absolute_status::Begin)) {
            //若front為運算子且不為'!'、'+'、'-'、'|',或左括號,空字元,空白或為'|'且為絕對值的開始
            if (this->is_number(back)) {    //若back為數字，那就對了
                return true;
            }
        }
    }
    return false;   //若到這裡的話，那當然不對阿
}

//檢查是否為空白(space與tab)
bool Calculator::is_space(char c) {
    return (c == '\t' || c == ' ')? true: false;
}

//把運算式的所有字母轉為小寫
void Calculator::to_lower(std::string &s) {
    static std::string lower;
    lower.clear();  //清除上次的結果
    for (char c: s) {
        if (c >= 'A' && c <= 'Z'){  //判斷是否為大寫字母
            c += 32;    //透過ascii碼，可得大寫與小寫間差了32
            lower.push_back(c); //將結果存進lower
        }
        else {
            lower.push_back(c); //不是轉換目標，直接存
        }
    }
    s = std::move(lower);   //把lower的結果轉回參數s
}

//找出所有的絕對值並確認開始及結束
void Calculator::find_abs(const std::string &expr) {
    static std::vector<size_t> abs_position;  //儲存所有絕對值的位置
    if (expr.find("|") == std::string::npos) return;    //沒有絕對值，不用搜尋
    abs_position.clear();   //清除上次的結果
    this->absolute_statuses.clear();
    for (size_t i = 0; i < expr.size(); i++) {    //當i小於expr的長度
        i = expr.find("|", i);  //用搜尋的方式找絕對值
        if (i == std::string::npos) break;  //沒絕對值了，跳出
        abs_position.push_back(i);  //把找到的位置存入abs_position
    }
    for (std::vector<size_t>::iterator it = abs_position.begin(); it != abs_position.end(); ++it) {
        //檢查所有的絕對值狀態
        int n = *it - 1;
        for (; n > 0 && (this->is_space(expr[n]) || expr[n] == '|'); n--);  //尋找絕對值前的字元
        if (n <= 0) {   //絕對值前沒有別的東西，為開始
            this->absolute_statuses[*it] = Absolute_status::Begin;
        }
        else {
            if (this->is_number(expr[n]) || expr[n] == ')' || expr[n] == '!') { //如果絕對值的前面是數字或右括號或階乘，為結束
                this->absolute_statuses[*it] = Absolute_status::End;
            }
            else {  //其他的話，為開始
                this->absolute_statuses[*it] = Absolute_status::Begin;
            }
        }
    }
}

//搜尋絕對值的狀態
Absolute_status Calculator::find_abs_status(const size_t &pos) {
    return this->absolute_statuses[pos];
}

//合併一些麻煩的符號
void Calculator::combine(std::string &expr) {
    static std::string front, back;    //暫存用的字串
    bool is_change = false;
    this->find_abs(expr);   //先搜尋絕對值
    for (size_t i = 0; i < expr.size(); i++) {
        switch (expr[i]) {
            case '+':
            case '-':
                {
                    size_t n = i + 1;
                    for (; n < expr.size() && is_space(expr[n]); n++);  //跳過空白字元且小心超出儲存空間
                    size_t m = n + 1;
                    if (m >= expr.size() || n >= expr.size()) { //超過儲存空間，不管了
                        continue;
                    }
                    int k = i - 1;
                    for (; k >= 0 && is_space(expr[k]); k--);   //跳過前面的空格
                    //看k是數字或(不是運算子或是'!'或(是'|'且為結束))或')'且不是'('
                    bool front_no_problem = (k < 0)? false: expr[k] != '(' && (this->is_number(expr[k]) || (!this->is_operator(expr[k]) || expr[k] == '!' || (expr[k] == '|' && this->find_abs_status(k) == Absolute_status::End)) || expr[k] == ')');
                    //檢查略過空白後的第二個字元是數字或左括號或(是'|'且為開始)
                    bool back_no_problem = (this->is_number(expr[m]) || expr[m] == '(' || (expr[m] == '|' && this->find_abs_status(m) == Absolute_status::Begin));
                    if (front_no_problem && back_no_problem) {  //前後都沒問題
                        is_change = true;
                        front.clear();  //先清一下，避免問題
                        back.clear();
                        switch (expr[i]) {
                            case '+':   //當前為+
                                switch (expr[n]) {
                                    case '+':   //當第一個字元為+
                                        this->combine_pm(expr, front, back, i, n, '+');    //++得+
                                        i++;    //跳到第二個字元
                                        break;
                                    case '-':   //當第一個字元為-
                                        this->combine_pm(expr, front, back, i, n, '-');    //+-得-
                                        i++;    //跳到第二個字元
                                        break;
                                }
                                break;
                            case '-':   //當前為-
                                switch (expr[n]) {
                                    case '+':   //當第一個字元為+
                                        this->combine_pm(expr, front, back, i, n, '-');    //-+得-
                                        i++;    //跳到第二個字元
                                        break;
                                    case '-':   //當第一個字元為-
                                        this->combine_pm(expr, front, back, i, n, '+');    //--得+
                                        i++;    //跳到第二個字元
                                        break;
                                }
                                break;
                        }
                    }
                }
                break;
            case '(':
                front.clear();  //先清一下，避免問題
                back.clear();
                if (i > 0) {
                    size_t n = i;
                    int backets = -1;    //此處為左括號，先減一
                    while (n < expr.size() && !(backets == 0)) {    //找出配對到的右括號，當數字歸零就找到了
                        n = expr.find_first_of("()", n + 1);    //用搜尋的方式找左右括號
                        if (n == std::string::npos) break;  //沒有了就跳出
                        if (expr[n] == '(') { //左括號就減一
                            backets--;
                        }
                        else if (expr[n] == ')') {  //右括號就加一
                            backets++;
                        }
                    }
                    if (backets == 0) {
                        if (this->is_pm(expr[i-1])) { //如果左括號前面為正負號
                            is_change = true;
                            int m = i - 2;
                            for (; m > 0 && this->is_space(expr[m]); m--);  //跳過前一格前面的空格
                            if (m < 0 || (this->is_operator(expr[m]) && expr[m] != '!' && !(expr[m] == '|' && this->find_abs_status(m) == Absolute_status::End))) {
                                //如果m小於0或m為運算子且不為'!'&非(為'|'且為結束)
                                front = expr.substr(0, i - 1);   //從開頭切到括號的前兩格
                                back = expr.substr(i, n - i + 1);   //從左括號切到配對的右括號
                                if (expr[i-1] == '-') { //負的用括號刮起來，確保先算
                                    front.append("(").append(back).append("*-1)");
                                    i++;    //因為多了一個左括號，i往後一格就不需要檢查這個
                                }
                                else {  //正的沒有關係
                                    front.append(back);
                                    i--;    //因為往前移了，所以i-1才能繼續檢查原本位置後一格的地方
                                }
                                if (n < expr.size() - 1) {  //右括號後面還有東西的話
                                    back.clear();   //先清掉後面
                                    back = expr.substr(n + 1);  //從右括號後一格開始切到最後
                                    front.append(back); //再合起來
                                }
                                expr = std::move(front);    //把合併結果還回給expr
                            }
                        }
                        else if (this->is_number(expr[i-1])) {  //如果左括號前是數字的話
                            is_change = true;
                            front = expr.substr(0, i);  //從0分割expr到左括號前一格
                            back = expr.substr(i, n - i + 1);   //從左括號分割expr到配對的右括號
                            front.append("*").append(back); //先合併前面的
                            i++;    //因為多了一個乘號，i往後一格就不需要檢查這個
                            if (n < expr.size() - 1) {  //右括號後面還有東西的話
                                back.clear();   //先清掉後面
                                back = expr.substr(n + 1);  //從右括號後一格開始切到最後
                                front.append(back); //再合起來
                            }
                            expr = std::move(front);    //把結果還回expr
                        }
                    }
                }
                break;
            case ')':
                front.clear();  //先清一下，避免問題
                back.clear();
                if (i + 1 < expr.size() && this->is_number(expr[i+1])) {   //如果i+1小於字串長度和右括號後面數字
                    is_change = true;
                    front = expr.substr(0, i + 1);  //從0分割expr到右括號
                    back = expr.substr(i + 1);  //從右括號後面分割expr到最後
                    front.append("*").append(back); //front接上'*'和後面
                    i++;    //因為多了一個乘號，i往後一格就不需要檢查這個
                    expr = std::move(front);    //把結果還回expr
                }
                break;
            case '|':
                front.clear();  //先清一下，避免問題
                back.clear();
                if (i > 0) {
                    if (this->find_abs_status(i) == Absolute_status::Begin) {   //此處為絕對值的開頭的話
                        //從此處開始尋找
                        std::map<size_t, Absolute_status>::iterator it = this->absolute_statuses.lower_bound(i);
                        int status = -1;    //開始處先減一
                        while (it != this->absolute_statuses.end() && !(status == 0)) { //當不為尾端且狀態不為0
                            it++;   //找下一個
                            if (it->second == Absolute_status::Begin) { //此處為開始，減一
                                status--;
                            }
                            else if (it->second == Absolute_status::End) {  //此處為結束，加一
                                status++;
                            }
                        }
                        if (status == 0) {
                            size_t n = it->first; //找到配對的絕對值的位置
                            int m = i - 2;
                            for (; m > 0 && this->is_space(expr[m]); m--);  //跳過前一格前面的空格
                            if (m < 0 || (this->is_operator(expr[m]) && expr[m] != '!' && expr[m] != '|')) {
                                //如果m小於0或m為運算子且不為'!'&'|'
                                if (this->is_pm(expr[i-1])) {   //這之前是正負的話
                                    is_change = true;
                                    front = expr.substr(0, i - 1);  //從開頭切到此處的前兩格
                                    back = expr.substr(i, n - i + 1);   //從此處切到配對的絕對值
                                    if (expr[i-1] == '-') { //負的用括號確保先算
                                        front.append("(").append(back).append("*-1)");
                                        i++;    //因為多了一個左括號，i往後一格就不需要檢查這個
                                    }
                                    else {  //正的不管沒差
                                        front.append(back);
                                        i--;    //因為往前移了，所以i-1才能繼續檢查原本位置後一格的地方
                                    }
                                    if (n < expr.size() - 1) {  //配對的絕對值後面還有東西的話
                                        back.clear();   //先清掉後面
                                        back = expr.substr(n + 1);  //從絕對值後一格開始切到最後
                                        front.append(back); //再合起來
                                    }
                                    expr = std::move(front);    //把合併結果還回給expr
                                }
                                else if (this->is_number(expr[i-1])) {  //這之前是數字的話
                                    is_change = true;
                                    front = expr.substr(0, i);  //從0分割expr到此處前一格
                                    back = expr.substr(i, n - i + 1);   //從此處分割expr到配對的絕對值
                                    front.append("*").append(back); //先合併前面的
                                    i++;    //因為多了一個乘號，i往後一格就不需要檢查這個
                                    if (n < expr.size() - 1) {  //配對的絕對值後面還有東西的話
                                        back.clear();   //先清掉後面
                                        back = expr.substr(n + 1);  //從絕對值後一格開始切到最後
                                        front.append(back); //再合起來
                                    }
                                    expr = std::move(front);    //把結果還回expr
                                }
                            }
                        }
                    }
                    else if (this->find_abs_status(i) == Absolute_status::End) {    //此處為絕對值的結束的話
                        front.clear();  //先清一下，避免問題
                        back.clear();
                        if (i + 1 < expr.size() && this->is_number(expr[i+1])) {   //如果i+1小於字串長度和絕對值後面數字
                            is_change = true;
                            front = expr.substr(0, i + 1);  //從0分割expr到絕對值
                            back = expr.substr(i + 1);  //從絕對值後面分割expr到最後
                            front.append("*").append(back); //front接上'*'和後面
                            i++;    //因為多了一個乘號，i往後一格就不需要檢查這個
                            expr = std::move(front);    //把結果還回expr
                        }
                    }
                }
                break;
        }
        if (is_change) {
            is_change = false;
            this->find_abs(expr);   //更新絕對值狀態
        }
    }
}

//合併不需要的正負號
void Calculator::combine_pm(std::string &expr, std::string &front, std::string &back, const size_t &before, const size_t &after, char op) {
    front = expr.substr(0, before); //從0分割到before
    back = expr.substr(after + 1);  //從after+1分割到最後
    front.push_back(op);    //接上op
    front.append(back); //接上尾端
    expr.clear();   //清空expr
    expr = std::move(front);    //結果給回expr
}

//把輸入的運算式拆解成後綴式
size_t Calculator::infix_to_postfix(std::vector<std::string> &result, const std::string &expr, const size_t start, const char input_op) {
    std::stack<char> op;    //利用stack儲存運算子
    size_t i;
    for (i = start; i < expr.size(); i++) {
        //如果是數字或整數
        if (this->is_number(expr[i]) || this->is_integer(expr, i)) {
            std::string num("");
            do {
                num.push_back(expr[i++]);   //把數字集合在一個string裡
            } while (this->is_number(expr[i]));
            i--;    //因迴圈會加一，所以要先減一
            result.push_back(num);  //把num加到result尾端
        }
        else if (expr[i] == 'a' && expr.substr(i, 3) == "ans") {    //如果這一段為"ans"
            result.push_back(std::to_string(this->get_answer()));  //把上一個的運算結果加到result尾端
            i += 2; //略過"ans"
        }
        else if (this->is_space(expr[i])) {   //是空白的話跳過
            continue;
        }
        else if (expr[i] == '(') {  //是左括號的話呼叫自身且start = i + 1, input_op = '('
            i = this->infix_to_postfix(result, expr, i + 1, '(');
        }
        else if (expr[i] == ')') {  //是右括號的話
            if (input_op == '(') {  //input_op為左括號的話
                break;  //跳出for迴圈，以回到上一層
            }
            else {  //括號與預期的輸入不同，ex:")(","|(|)"
                throw std::logic_error("要不要檢查一下，括號好像怪怪的");
            }
        }
        else if (expr[i] == '|') {  //是絕對值的話
            if (this->find_abs_status(i) == Absolute_status::End) {  //input_op為絕對值的話
                break;  //跳出for迴圈，以回到上一層
            }
            else {  //呼叫自身且start = i + 1, input_op = '|'
                i = this->infix_to_postfix(result, expr, i + 1, '|');
            }
        }
        else {  //理論上應為運算子
            while (!op.empty() && (this->pripority(expr[i]) <= this->pripority(op.top()))) {
                //op不為空且(expr[i]的運算優先度小於等於op最上面的運算優先度)
                result.push_back(std::string(1, op.top())); //抽空op放入result
                op.pop();   //把已加入result的取出
            }
            if (this->is_operator(expr[i])) {   //是運算子的話
                op.push(expr[i]);   //塞入op
            }
        }
    }
    while (!op.empty()) {   //如果op不為空
        result.push_back(std::string(1, op.top())); //抽空op放入result
        op.pop();   //把已加入result的取出
    }
    if (input_op == '|') {
        result.push_back("|");  //再加入絕對值以運算
    }
    return i;   //回傳i值給上一層
}

//運算後綴式的結果
c_type Calculator::calcul_postfix(const std::vector<std::string> &exprs) {
/*    //debugger 查看分解的情況時用
    for (std::string expr: exprs) {
        expr.push_back(' ');
        std::cout << expr;
    }
    std::cout << std::endl;
*/
    static std::stack<c_type> numbers;  //利用stack儲存運算元
    static std::stringstream Num;   //利用stringstream轉換數字
    while (!numbers.empty()) {  //清空上次的運算結果
        numbers.pop();
    }
    for (std::string expr: exprs) {    //輪過每個exprs的元素
        if (this->is_operator(expr) && (Calculator::operand[expr[0]] == 2)) {
            //expr為運算子且該需要的運算元為2
            c_type back = 0, front = 0;
            if (!numbers.empty()) { //確保不會超抽
                back = numbers.top();   //取出最上面的數儲存
                numbers.pop();  //抽出
            }
            if (!numbers.empty()) { //確保不會超抽
                front = numbers.top();  //再取一次
                numbers.pop();  //抽出
            }
            numbers.push(this->op_calculate(expr[0], front, back)); //運算(後抽出的在前面(stack特性))並存回去
        }
        else if (this->is_operator(expr) && (Calculator::operand[expr[0]] == 1)) {
            //expr為運算子且該需要的運算元為1
            c_type num = 0;
            if (!numbers.empty()) { //確保不會超抽
                num = numbers.top();    //取出最上面的數儲存
                numbers.pop();  //抽出
            }
            numbers.push(this->op_calculate(expr[0], num)); //運算
        }
        else {  //應為數字
            Num.clear();    //清空Num
            Num.str("");
            Num << expr;    //把expr輸入Num
            c_type number;
            Num >> number;  //把Num輸入number
            numbers.push(number);   //加進numbers
        }
    }
    return numbers.top();   //最後最上面為運算結果
}

//計算階乘
c_type Calculator::factorial(const c_type &num) {
    if (this->is_factorial_overflow(num)) {
        throw std::overflow_error("階乘超過可以接受的上限了");
    }
    if (num == 0) return 1; //0! == 1
    c_type res = 1;
    //從2開始乘，乘到num
    for (c_type i = 2; i <= num; i++) {
        res *= i;
    }
    return res;
}

//計算指數(只有正整數的次方數)
c_type Calculator::power(const c_type &base, const c_type &times) {
    c_type res = 1;
    if (times < 0) return 0; //負數為分數，以整數來說無法計算，故回傳0
    for (c_type i = 0; i < times; i++) {    //乘上times次
        res *= base;
    }
    return res;
}

//階乘是否超出上限
bool Calculator::is_factorial_overflow(const c_type &num) {
    c_type number = this->op_calculate('|', num);
    switch(sizeof(c_type)) {
        case sizeof(long long): //實測如果為8bytes，21!以上會超過上限
            if (number > 20) {
                return true;
            }
            break;
        case sizeof(int):   //實測如果為4bytes，11!以上會超過上限
            if (number > 10) {
                return true;
            }
            break;
    }
    return false;
}

//是否為特殊字句
bool Calculator::is_special(const std::string &input, Special &s) {
    bool special = false;
    if (Calculator::spes.find(input) != Calculator::spes.end()) {   //試找spes裡有無特殊句
        s = Calculator::spes[input];    //有的話s為特數句的引索
        special = true; //有特殊句
    }
    return special;
}

//印出特殊字句的特殊輸出
void Calculator::print_special(const Special &s) {
    switch(s) {
        case Special::Shang_xiang:
            std::cout << R"(\|/)" << std::flush;
            this->m_delay(1000);
            for (int i = 0; i < 3; i++) {
                std::cout << '.' << std::flush;
                this->m_delay(1000);
            }
            std::cout << std::string(6, '\b') << R"(\|_   )" << "\b\b\b" << std::flush;
            this->m_delay(500);
            std::cout << "\t阿" << std::flush;
            for (int i = 0; i < 3; i++) {
                this->m_delay(500);
                std::cout << '.' << std::flush;
            }
            this->m_delay(1000);
            std::cout << "被風吹倒了" << std::flush;
            for (int i = 0; i < 3; i++) {
                this->m_delay(500);
                std::cout << '.' << std::flush;
            }
            this->m_delay(1000);
            break;
        case Special::TK888:
            std::cout << "這裡沒有魔關羽喔!";
            break;
        case Special::MIKU:
            std::cout << "miku最高!!!";
            break;
        case Special::MIKU3939:
            std::cout << "把你咪哭咪哭掉喔";
            break;
        case Special::OuO:
            std::cout << "OuO";
            break;
        case Special::C8763:
            std::cout << "在這裡也想星爆是不是搞錯了什麼" << std::flush;
            for (int i = 0; i < 10; i++) {
                std::cout << '.' << std::flush;
                this->m_delay(1000);
            }
            std::cout << std::endl << std::endl << "星" << std::flush;
            this->m_delay(900);
            std::cout << "爆" << std::flush;
            this->m_delay(900);
            std::cout << "氣" << std::flush;
            this->m_delay(400);
            std::cout << "流" << std::flush;
            this->m_delay(400);
            std::cout << "斬!" << std::flush;
            this->m_delay(600);
            std::cout << "\t\t騙人的吧";
            break;
        case Special::NEKOPARA:
            std::cout << R"(在這裡也想要%%%是否搞錯了什麼...)" << std::flush;
            this->m_delay(1000);
            std::cout << std::endl << std::endl << R"(我就%!)";
            break;
    }
    std::cout << std::endl << std::endl;
}
