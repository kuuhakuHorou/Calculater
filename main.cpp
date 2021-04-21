#include <iostream>
#include "class_cal.h"

bool calculate(Calculator &, c_type &); //嘗試運算

int main(void) {
    c_type result = 0;
    Calculator c;
    while (!c.is_exit()) {  //沒有要離開時
        std::cout << "Expression: " << std::flush;
        if (calculate(c, result) && !c.is_exit()) { //如果有運算成功且沒有要離開
            std::cout << std::endl << "result: " << result << std::endl;
            Calculator::print_dash();   //印出結果和分隔線
        }
    }
    return 0;
}

bool calculate(Calculator &c, c_type &res) {    //嘗試運算
    static std::string input;
    input.clear();  //清理上次的輸入
    getline(std::cin, input);   //取得輸入(一行)
    return c.string_calculate(input, res);  //呼叫Calculator的string_calculate()
}
