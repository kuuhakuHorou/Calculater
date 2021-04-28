# Calculator
**注意**:這是一個**整數**的計算機，所以**不會**有浮點數(小數)。且因為如此，次方的指數如果是負數的話會出現0。

## 說明
這是一個簡易的計算機，能夠算四則運算、取餘數(%)、次方(^)、階層(!)、排列組合(c,p)、絕對值(|)。在其中也能使用括號(只有小的)，就像一般的算式一樣!

| 運算子 |   說明   |  範例   | 輸出  |
|:------:|:--------:|:-------:|:-----:|
|   +    |   加法   |   5+6   |  11   |
|   -    |   減法   |   9-5   |   4   |
|   *    |   乘法   |   2*9   |  18   |
|   /    |   除法   |  15/5   |   3   |
|   %    |  取餘數  |   9%2   |   1   |
|   ^    |   次方   |   2^3   |   8   |
|   e    | 科學記號 |   5e4   | 50000 |
|   !    |   階層   |   5!    |  120  |
|   c    |   組合   |   5c2   |  10   |
|   p    |   排列   |   5p2   |  20   |
|  ( )   | 優先運算 | 5*(1+2) |  15   |
|   \|   |  絕對值  | \|-9\|  |   9   |

對了是真的可以輸入算式，就像"8+5*(8+4)/2"一樣!
![算式範例](https://cdn.discordapp.com/attachments/834440816050831390/834440992555008051/2021-04-21_22-49-37.jpg "算式範例")

還可以使用`ans`取得上次的運算結果，或者是`exit`離開計算機，還有`clear`清理畫面。

不過要小心的是，不小心**輸入錯誤**的話就會跳出**錯誤訊息**喔!

|          範例           | 應有的錯誤訊息             |
|:-----------------------:| -------------------------- |
|       5/0 or 5%0        | 有除以0，請更正!!          |
|          \|-5           | 絕對值的部分好像有錯       |
|           (-5           | 括號數有錯，請更正!!       |
|           5+            | 好像少了些什麼?            |
|           /+            | 這不是個算式!!             |
|           5&2           | 有奇怪的東西混進來了       |
|          ()+5           | 有空的括號喔!              |
| 當開啟第一個輸入有`ans` | 第一次算並沒有先前的答案!! |

想要試試看的話，這裡有個[線上的編輯器](https://replit.com/@kuuhakuHorou/Calculator "replit.com")，不用下載，打開網站後點Run就可以輸入了。

## Bug report
有發現的bug需要把輸入的運算式一起回報，這樣找起來才會更快。

### 已知Bug
* 無

# Changelog
## [1.0.9] - 2021-04-29 00:05
* 新增接在括號前後的數字以乘法做運算
* 新增絕對值的判斷
   * 提醒缺少了多少的左/右絕對值
   * 前後缺少運算子的判斷
* 新增絕對值前正負號的運算和前後數字的乘法運算
* 再次更改兩個加減的合併方式
* 更改錯誤訊息的收集方法
* 更改括號前正負號的運算方法
* 更改整數的判斷
* 移除合併無用的括號作用
## [1.0.8] - 2021-04-24 02:16
* 更改正負號的合併方式
* 更改括號和絕對值前正負號的判斷方法
* 更改括號前正負號的運算方法
## [1.0.7] - 2021-04-23 00:05
* 修正括號前後沒有運算子的判斷
* 修正合併括號時可能超出讀取範圍的情況
## [1.0.6] - 2021-04-22 15:10
* 修正負的階層
* 修正除正負外其他運算子的錯誤狀況
## [1.0.5] - 2021-04-22 14:27
* 修正前面有空白後面接正負有錯誤的情況
## [1.0.4] - 2021-04-22 00:10
* 修正跳過的判斷
## [1.0.3] - 2021-04-21 23:55
* 更換清除無用的括號算法
* 修正使用負號的結果
## [1.0.2] - 2021-04-20 23:50
* 修正對於空括號的判斷
* 修正階層數字過大的問題
## [1.0.1] - 2021-04-20 19:30
* 修正絕對值的判斷標準
## [1.0.0] - 2021-04-19 22:50
* 計算機完成!
