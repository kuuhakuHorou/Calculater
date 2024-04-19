#include <iostream>
#include "class_cal.h"

bool calculate(Calculator &); //嘗試運算

int main(void) {
    Calculator c;
    while (!c.is_exit()) {  //沒有要離開時
        std::cout << "Expression: " << std::flush;
        if (calculate(c) && !c.is_exit()) { //如果有運算成功且沒有要離開
            std::cout << std::endl << "result: " << c.get_answer() << std::endl;
        }
        if (!c.is_exit()) {
            Calculator::print_dash();   //印出結果和分隔線
        }
    }
    return 0;
}

bool calculate(Calculator &c) {    //嘗試運算
    static std::string input;
    input.clear();  //清理上次的輸入
    std::getline(std::cin, input);   //取得輸入(一行)
    return c.string_calculate(input);  //呼叫Calculator的string_calculate()
}
