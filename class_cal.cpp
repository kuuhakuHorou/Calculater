#include <iostream>
#include <sstream>
#include <limits>
#include <chrono>
#include "class_cal.h"

//以下為public成員函式
//一般建構式
Calculator::Calculator() {
    this->divide_zero = false;
    this->wrong_absolute = false;
    this->wrong_brackets = false;
    this->wrong_expression = false;
    this->wrong_input = false;
    this->wrong_operation = false;
    this->brackets_have_nothing = false;
    this->first_calculate = true;
    this->have_answer = false;
    this->exit = false;
}

//呼叫一般建構式並設定first_calculate
Calculator::Calculator(bool first_calcul) : Calculator() {
    this->first_calculate = first_calcul;
}

//呼叫運算式分解並計算
bool Calculator::string_calculate(std::string &user_input, c_type &result) {
    Special special;
    static std::vector<std::string> exprs;
    this->to_lower(user_input); //避免麻煩，把所有的字母轉成小寫
    if (user_input == "clear") {  //清除螢幕
        for (int i = 0; i < 50; i++) {
            std::cout << std::endl;
        }
        return false;
    }
    else if (this->is_special(user_input, special)) {   //是否有輸入特別的句子
        this->print_special(special);   //特別的輸出
        this->print_dash(); //顯示分隔線
        return false;
    }
    else if (user_input != "exit") {    //不是退出就繼續
        this->combine(user_input);  //先過濾無用的+,-和括號
        // std::cout << user_input << std::endl; //debugger 查看合併後的狀況
        if (this->is_wrong(user_input)) {   //判斷輸入有無錯誤
            this->warning_message();    //告知錯誤
            this->print_dash(); //顯示分隔線
            return false;
        }
        exprs.clear();
        try {
            this->infix_to_postfix(exprs, user_input, result);  //嘗試拆解算試
            result = this->calcul_postfix(exprs);   //嘗試計算
        }
        catch (const char* &message) {  //抓取錯誤訊息
            std::cout << message << std::endl;  //輸出抓到的訊息
            this->print_dash();
            return false;
        }
        if (this->first_calculate) {    //如果第一次算完答案的話，就不會是第一次算了
            this->first_calculate = !this->first_calculate;
        }
    }
    else if (user_input == "exit"){ //要離開了
        this->exit = true;
        return false;
    }
    return true;
}

//呼叫運算元做計算
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
            if (back != 0) {
                res = front / back;
            }
            break;
        case '%':
            if (back != 0) {
                res = front % back;
            }
            break;
        case '^':   //次方的運算
            res = this->power(front, back);
            break;
        case 'e':   //科學記號的算式
            res = front * this->power(10, back);
            break;
        case '!':   //階層
            res = this->factorial(front);
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

//以下為private成員函式
//檢查有無錯誤
bool Calculator::is_wrong(const std::string &expr) {
    static std::stringstream Num;   //利用stringstream取得數字
    bool divide_zero = false;   //除以0
    bool not_expression = true; //不是運算式
    bool not_operation = false; //不是運算子
    bool loss_something = false;    //少了什麼
    bool first_calculate_want_ans = false;  //第一次就想要上次的 "ans"
    bool empty_brackets = false;
    int brackets = 0;   //括弧數
    std::vector<unsigned> abs;   //絕對值的位置
    unsigned left_bracket = 0;  //左邊括號所在處
    for (unsigned i = 0; i < expr.size(); i++) {
        if (this->is_space(expr[i])) {  //略過空白字元
            continue;
        }
        else if (!divide_zero && (expr[i] == '/' || expr[i] == '%')) {    //是否為除法(取餘數)，如有除0的情況不用重新檢查
            unsigned n = i + 1;
            while (!this->is_number(expr[n]) && n < expr.size()) {  //找出除法(取餘)的除數
                n++;
            }
            Num.str("");
            Num.clear();
            while (this->is_number(expr[n]) && i < expr.size()) {   //找出所有的數字
                Num << expr[n++]; //存入Num裡
            }
            if (Num.str().size() > 0) { //Num裡有數字
                c_type num;
                Num >> num; //從Num裡輸出到數字
                if (num == 0) {
                    divide_zero = true; //有除以0的情況
                }
            }
        }
        else if (expr[i] == '(') {  //有左括號，括號數減1
            left_bracket = i;
            brackets--;
        }
        else if (expr[i] == ')') {  //有右括號，括號數加1
            if (!empty_brackets) {  //如有空括號，不用重複檢查
                unsigned n = left_bracket + 1;
                while (n < i && this->is_space(expr[i])) {  //檢查裡面是否為空
                    n++;
                }
                if (n == i) {
                    empty_brackets = true;
                }
            }
            brackets++;
        }
        else if (expr[i] == '|') {  //有絕對值的符號，絕對值位置儲存
            abs.push_back(i);
        }
        else if (this->is_number(expr[i])) {    //有數字的話會是算式
            not_expression = false;
        }
        else if (expr[i] == 'a' && expr.substr(i, 3) == "ans") {    //算式裡有ans
            if (this->first_calculate) {    //第一次計算不會有答案
                first_calculate_want_ans = true;
            }
            else {
                not_expression = false;
            }
            i += 2; //跳過"ans"
        }
        else if (!not_operation && this->ops.find(expr[i]) == this->ops.end()) {  //沒有的運算元，不會有功能，且不需重新檢查
            not_operation = true;
        }
        if (this->is_operator(expr[i]) && expr[i] != '|') { //是運算元且不是絕對值
            int n = i - 1;
            unsigned m = i + 1;
            while (n > 0 && (this->is_space(expr[n]) || expr[n] == ')' || expr[n] == '|')) {    //嘗試找出前面的數字
                n--;
            }
            while (m < expr.size() && (this->is_space(expr[m]) || expr[m] == '(' || expr[m] == '|')) {  //嘗試找出後面的數字
                m++;
            };
            if (n < 0) {    //當i為第一個運算子且前面沒東西時，有可能是整數或後面為'('或'|'
                if (expr[i] == '+' || expr[i] == '-') {
                    for (m = i + 1; m < expr.size() && this->is_space(expr[m]); m++);
                    if (this->is_integer(expr[i], '\0', (i + 1 > expr.size())? '\0': expr[i+1]) || expr[m] == '|' || expr[m] == '(') {
                        continue;
                    }
                }
            }
            else if (m >= expr.size()) {
                if (expr.size() > 1) {
                    if (expr[i] == '!') {   //最後為階層運算子也沒錯
                        continue;
                    }
                }
            }
            else if (this->is_operator(expr[i])) {  //當i為運算子時
                //看看前或後是不是答案
                bool front_have_ans = (expr[n] == 's' && expr.substr(n - 2, 3) == "ans")? true: false;
                bool back_have_ans = (expr[m] == 'a' && expr.substr(m, 3) == "ans")? true: false;
                //看後面是不是整數
                bool back_is_integer = this->is_integer(expr[m], expr[m-1], ((m + 1) > expr.size()? '\0': expr[m+1]));
                if (expr[i] == '+' || expr[i] == '-') { //運算子為+,-
                    //看看是不是整數
                    bool now_is_integer = this->is_integer(expr[i], ((i - 1) < 0)? '\0': expr[i-1], ((i + 1) > expr.size())? '\0': expr[i+1]);
                    if ((this->is_number(expr[n]) || front_have_ans) && (this->is_number(expr[m]) || back_is_integer || back_have_ans)) { //前後都是數字or ans，沒問題
                        continue;
                    }
                    else if ((this->is_operator(expr[n]) || expr[n] == '(') && now_is_integer) {
                        //前面為運算子或左括號或ans且i為整數，也沒問題
                        continue;
                    }
                    else if (expr[n] == '!') {  //前面是階層也沒問題的拉
                        continue;
                    }
                    else {
                        for (n = i - 1; n > 0 && this->is_space(expr[n]); n--);
                        if (n == 0) {
                            continue;
                        }
                    }
                }
                else if (expr[i] == '!') {
                    if (this->is_operator(expr[m])) {   //階層後面有個運算元很正常
                        continue;
                    }
                }
                else if ((this->is_number(expr[n]) || expr[n] == '!' || front_have_ans)
                      && (this->is_number(expr[m]) || back_is_integer || back_have_ans)) {
                    //其他的運算子前如果是數字或階層或絕對值或ans和後面是數字或整數或ans，沒問題
                    continue;
                }
                else if (this->is_operator(expr[n]) || this->is_operator(expr[m])) {    //前後如果是運算子
                    if (expr[i] == '!' || expr[n] == '!') { //自己或前面是階層，沒問題
                        continue;
                    }
                }
            }
            loss_something = true;  //大概少了什麼吧
        }
    }
    //更新狀態
    this->divide_zero = divide_zero;
    this->wrong_absolute = (abs.size() % 2 == 0)? false: true; //絕對值成雙，所以對2取餘數應為0
    this->wrong_brackets = (brackets == 0)? false: true;    //括號應成對，所以括號數應為0
    this->wrong_expression = loss_something;
    this->wrong_input = not_expression;
    this->wrong_operation = not_operation;
    this->brackets_have_nothing = empty_brackets;
    this->have_answer = first_calculate_want_ans;
    //回傳是否有錯誤(只要一個有就不該計算)
    return divide_zero || this->wrong_brackets || this->wrong_absolute || loss_something || not_expression || not_operation || empty_brackets || first_calculate_want_ans;
}

//錯誤訊息
void Calculator::warning_message() {
    if (this->divide_zero) {
        std::cout << "有除以0，請更正!!" << std::endl;
    }
    if (this->wrong_absolute) {
        std::cout << "絕對值的部分好像有錯" << std::endl;
    }
    if (this->wrong_brackets) {
        std::cout << "括號數有錯，請更正!!" << std::endl;
    }
    if (this->wrong_expression) {
        std::cout << "好像少了些什麼?" << std::endl;
    }
    if (this->wrong_input) {
        std::cout << "這不是個算式!!" << std::endl;
    }
    if (this->wrong_operation) {
        std::cout << "有奇怪的東西混進來了" << std::endl;
    }
    if (this->brackets_have_nothing) {
        std::cout << "有空的括號喔!" << std::endl;
    }
    if (this->first_calculate && this->have_answer) {
        std::cout << "第一次算並沒有先前的答案!!" << std::endl;
    }
}

//是否為擁有的運算子(char 型態)
bool Calculator::is_operator(char c) {
    //從ops裡找，如果有就對了
    return (this->ops.find(c) != this->ops.end())? true: false;
}

//是否為擁有的運算子(std::string 型態)
bool Calculator::is_operator(const std::string &op) {
    //如op長度只有1且是運算子，對拉
    return (op.size() == 1 && this->is_operator(op[0]))? true: false;
}

//查看運算子的優先度
int Calculator::pripority(char c) {
    //優先度為ops裡的value值，若沒有運算子，則為最低階運算(不太會發生了)
    return (this->is_operator(c))? this->ops[c] : -1;
}

//檢查是否為數字
bool Calculator::is_number(char c) {
    return (c >= '0' && c <= '9')? true: false;
}

//檢查是否為數字(有正負數的情況)
bool Calculator::is_integer(char op, char front, char back) {
    if (op == '-' || op == '+') {   //看op是否為+,-
        if ((this->is_operator(front) || front == '(' || front == '\0' || is_space(front))
            && front != '!') {  //若front為運算子,左括號,空字元,空白且不為'!'
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
    lower.clear();
    for (auto c : s) {
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

//合併不需要的括號與正負號
void Calculator::combine(std::string &expr) {
    static std::string front, back;    //暫存用的字串
    for (unsigned i = 0; i < expr.size(); i++) {
        switch (expr[i]) {
            case '+':
            case '-':
            {
                front.clear();  //先清一下，避免問題
                back.clear();
                unsigned n = i + 1;
                while (n < expr.size() && is_space(expr[n])) {    //跳過空白字元且小心超出儲存空間
                    n++;
                }
                unsigned m = n + 1;
                if (m >= expr.size() || n >= expr.size()) { //超過儲存空間，不管了
                    continue;
                }
                if (!is_space(expr[m])) {   //略過空白後的第二個字元不為空白
                    switch (expr[i]) {
                        case '+':   //當前為+
                            switch (expr[n]) {
                                case '+':   //當第一個字元為+
                                    this->combine_add_sub(expr, front, back, i, n, '+');    //++得+
                                    i++;    //跳到第二個字元
                                    break;
                                case '-':   //當第一個字元為-
                                    this->combine_add_sub(expr, front, back, i, n, '-');    //+-得-
                                    i++;    //跳到第二個字元
                                    break;
                            }
                            break;
                        case '-':   //當前為-
                            switch (expr[n]) {
                                case '+':   //當第一個字元為+
                                    this->combine_add_sub(expr, front, back, i, n, '-');    //-+得-
                                    i++;    //跳到第二個字元
                                    break;
                                case '-':   //當第一個字元為-
                                    this->combine_add_sub(expr, front, back, i, n, '+');    //--得+
                                    i++;    //跳到第二個字元
                                    break;
                            }
                            break;
                    }
                }
            }
                break;
            case '(':
            {
                front.clear();  //先清一下，避免問題
                back.clear();
                unsigned n = i + 1;
                //跳過整數和空白且不超過字串的長度-1
                while (n < expr.size() - 1 && (this->is_number(expr[n]) || this->is_integer(expr[n], expr[n-1], expr[n+1]) || this->is_space(expr[n]))) {
                    n++;
                }
                if (expr[n] == ')') {   //若此處為右括號
                    unsigned m = i + 1;
                    while (m < n && this->is_space(expr[m])) {  //查看中間是否只有空白
                        m++;
                    }
                    if (m == n) {   //是的話跳過後面繼續迴圈
                        continue;
                    }
                    int k = i - 1;
                    while (k > 0 && this->is_space(expr[k])) {
                        k--;
                    }
                    front = expr.substr(0, i);  //從0到i分割expr
                    back = expr.substr(i + 1, n - i - 1);   //從i+1分割到n-i-1
                    if (expr[k] != '-' && expr[k] != '+') {
                        front.append(" ");
                    }
                    front.append(back).append(" "); //接在一起
                    back.clear();   //清一下
                    if (n < expr.size() - 1) {  //若後面還有
                        back = expr.substr(n + 1);  //接到最後面
                        front.append(back); //再接在一起
                    }
                    expr.clear();   //清空expr
                    expr = std::move(front);   //讓結果給expr
                    while (i > 1 && this->is_space(expr[i])) {  //要退回左括號的前面並跳過空格，但也不能少於0
                        i--;
                    }
                }
            }
                break;
        }
    }
}

//合併不需要的正負號
void Calculator::combine_add_sub(std::string &expr, std::string &front, std::string &back, int before, int after, char op) {
    front = expr.substr(0, before); //從0分割到before
    back = expr.substr(after + 1);  //從after+1分割到最後
    front.push_back(op);    //接上op
    front.append(back); //接上尾端
    expr.clear();   //清空expr
    expr = std::move(front);    //結果給回expr
}

//把輸入的運算式拆解成後綴式
unsigned Calculator::infix_to_postfix(std::vector<std::string> &result, const std::string &expr, const c_type &ans, const unsigned start, const char input_op) {
    std::stack<char> op;    //利用stack儲存運算子
    unsigned i;
    for (i = start; i < expr.size(); i++) {
        //如果是數字或(整數且(op不為空或非(result不為空且result最後一個長度為1且result最後一個為'|')))，那就為一整數
        if (this->is_number(expr[i]) || (this->is_integer(expr[i], (i > 0)? expr[i-1]: '\0', expr[i+1]) && (!op.empty() || input_op == '|' || !(!result.empty() && result.back().size() == 1 && result.back()[0] == '|')))) {
            std::string num("");
            do {
                num.push_back(expr[i++]);   //把數字集合在一個string裡
            } while (this->is_number(expr[i]));
            i--;    //因迴圈會加一，所以要先減一
            result.push_back(num);  //把num加到result尾端
        }
        else if (expr[i] == 'a' && expr.substr(i, 3) == "ans") {    //如果這一段為"ans"
            result.push_back(std::to_string(ans));  //把上一個的運算結果加到result尾端
            i += 2; //略過"ans"
        }
        else if (is_space(expr[i])) {   //是空白的話跳過
            continue;
        }
        else if (expr[i] == '(') {  //是左括號的話呼叫自身且start = i + 1, input_op = '('
            i = this->infix_to_postfix(result, expr, ans, i+1, '(');
        }
        else if (expr[i] == ')') {  //是右括號的話
            if (input_op == '(') {  //input_op為左括號的話
                while (!op.empty()) {   //把所有收集到的運算子提出加到result尾端
                    result.push_back(std::string(1, op.top()));
                    op.pop();   //把已加入result的取出
                }
                break;  //跳出for迴圈，以回到上一層
            }
            else {  //括號與預期的輸入不同，ex:")(","|(|)"
                throw "要不要檢查一下，括號好像怪怪的";
            }
        }
        else if (expr[i] == '|') {  //是絕對值的話
            if (input_op == '|') {  //input_op為絕對值的話
                unsigned n = start; //從start開始數
                while (n < i && this->is_space(expr[i])) {
                    //當n小於i且中間為空白，n加一
                    n++;
                }
                if (n != i) {   //n不等於i的話(兩絕對值中間有別的東西)
                    while (!op.empty()) {   //提取op裡的內容加到result尾端
                        result.push_back(std::string(1, op.top()));
                        op.pop();   //把已加入result的取出
                    }
                    result.push_back(std::string(1, '|'));  //再加入絕對值以運算
                    break;  //跳出for迴圈，以回到上一層
                }
                else {  //n等於i(兩絕對值中間沒東西，ex:"||...||")
                    //呼叫自身且start = i + 1, input_op = '|'
                    i = this->infix_to_postfix(result, expr, ans, i+1, '|');
                }
            }
            else {  //呼叫自身且start = i + 1, input_op = '|'
                i = this->infix_to_postfix(result, expr, ans, i+1, '|');
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
    for (std::string expr : exprs) {    //輪過每個exprs的元素
        if (this->is_operator(expr) && (this->operand[expr[0]] == 2)) {
            //expr為運算子且該需要的運算元為2
            c_type back = 0, front = 0;
            if (!numbers.empty()) { //確保不會超抽
                back = numbers.top();   //取出最上面的數儲存
                numbers.pop();  //抽出
            }
            if (!numbers.empty()) {
                front = numbers.top();  //再取一次
                numbers.pop();
            }
            numbers.push(this->op_calculate(expr[0], front, back)); //運算(後抽出的在前面(stack特性))並存回去
        }
        else if (this->is_operator(expr) && (this->operand[expr[0]] == 1)) {
            //expr為運算子且該需要的運算元為1
            c_type num = 0;
            if (!numbers.empty()) {
                num = numbers.top();
                numbers.pop();
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
        throw "階層超過可以接受的上限了";
    }
    if (num == 0) return 1; //0! == 1
    c_type res = 1;
    for (c_type i = 2; i < this->op_calculate('|', num); i++) { //從2開始乘
        res *= i;
    }
    res *= num;
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

//階層是否超出上限
bool Calculator::is_factorial_overflow(const c_type &num) {
    c_type number = this->op_calculate('|', num);
    switch(sizeof(c_type)) {
        case sizeof(long long): //實測如果為8bytes，21!以上會超過上限
            if (number > 20) {
                return true;
            }
            break;
        case sizeof(int):   //實測如果為4bytes，21!以上會超過上限
            if (number > 10) {
                return true;
            }
    }
    return false;
}

//是否為特殊字句
bool Calculator::is_special(const std::string &input, Special &s) {
    bool special = false;
    if (spes.find(input) != spes.end()) {   //試找spes裡有無特殊句
        s = spes[input];    //有的話s為特數句的引索
        special = true; //有特殊句
    }
    return special;
}

//印出特殊字句的特殊輸出
void Calculator::print_special(const Special &s) {
    switch(s) {
        case Shang_xiang:
            std::cout << R"(\|/)" << std::flush;
            this->m_delay(1000);    //延遲1秒
            for (int i = 0; i < 3; i++) {   //輸出'.'並延遲1秒三次
                std::cout << '.' << std::flush;
                this->m_delay(1000);
            }
            std::cout << std::string(6, '\b') << R"(\|_   )" << "\b\b\b" << std::flush;
            this->m_delay(500); //延遲0.5秒
            std::cout << "\t阿" << std::flush;
            for (int i = 0; i < 3; i++) {   //輸出'.'並延遲0.5秒三次
                this->m_delay(500);
                std::cout << '.' << std::flush;
            }
            this->m_delay(1000);    //延遲1秒
            std::cout << "被風吹倒了" << std::flush;
            for (int i = 0; i < 3; i++) {   //輸出'.'並延遲0.5秒三次
                this->m_delay(500);
                std::cout << '.' << std::flush;
            }
            this->m_delay(1000);    //延遲1秒
            break;
        case TK888:
            std::cout << "這裡沒有魔關羽喔!";
            break;
        case MIKU:
            std::cout << "miku最高!!!";
            break;
        case MIKU3939:
            std::cout << "把你咪哭咪哭掉喔";
            break;
        case OuO:
            std::cout << "OuO";
            break;
        case C8763:
            std::cout << "在這裡也想星爆是不是搞錯了什麼" << std::flush;
            for (int i = 0; i < 10; i++) {  //輸出'.'並延遲1秒十次
                std::cout << '.' << std::flush;
                this->m_delay(1000);
            }
            std::cout << std::endl << std::endl << "星" << std::flush;
            this->m_delay(900); //延遲0.9秒
            std::cout << "爆" << std::flush;
            this->m_delay(900); //延遲0.9秒
            std::cout << "氣" << std::flush;
            this->m_delay(400); //延遲0.4秒
            std::cout << "流" << std::flush;
            this->m_delay(400); //延遲0.4秒
            std::cout << "斬!" << std::flush;
            this->m_delay(600); //延遲0.6秒
            std::cout << "\t\t騙人的吧";
            break;
        case NEKOPARA:
            std::cout << R"(在這裡也想要%%%是否搞錯了什麼...)" << std::flush;
            this->m_delay(1000);
            std::cout << std::endl << std::endl << R"(我就%!)";
            break;
    }
    std::cout << std::endl << std::endl;
}

//以毫秒為單位的延遲
void Calculator::m_delay(int time) {
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
