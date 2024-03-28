#include <iostream>

// 函数：加一
int add_one(int x) {
    return x + 1;
}

// 函数：乘二
int multiply_two(int x) {
    return x * 2;
}

// 函数：减一
int subtract_one(int x) {
    return x - 1;
}

// 函数指针类型
typedef int (*operation)(int);

// 结构体包含函数指针, 强制使用间接调用, 禁止内联优化
struct Operations {
    operation add_one_ptr = add_one;
    operation multiply_two_ptr = multiply_two;
    operation subtract_one_ptr = subtract_one;
};

int main() {
    int input = 10;

    // 创建结构体
    Operations ops;

    // 通过结构体调用函数指针
    int result1 = ops.add_one_ptr(input);
    std::cout << "Result of operation add_one: " << result1 << std::endl;

    int result2 = ops.multiply_two_ptr(input);
    std::cout << "Result of operation multiply_two: " << result2 << std::endl;

    int result3 = ops.subtract_one_ptr(input);
    std::cout << "Result of operation subtract_one: " << result3 << std::endl;

    return 0;
}

