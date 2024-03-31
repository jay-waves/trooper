#include <stdio.h>

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

// 结构体包含函数指针
struct Operations {
    operation add_one_ptr;
    operation multiply_two_ptr;
    operation subtract_one_ptr;
};

int main() {
    int input = 10;

    // 创建结构体并初始化
    struct Operations ops = {
        .add_one_ptr = add_one,
        .multiply_two_ptr = multiply_two,
        .subtract_one_ptr = subtract_one
    };

    // 通过结构体调用函数指针
    int result1 = ops.add_one_ptr(input);
    printf("result of add_one: %d\n", result1);

    int result2 = ops.multiply_two_ptr(input);
    printf("result of multiply_two: %d\n", result2);

    int result3 = ops.subtract_one_ptr(input);
    printf("result of subtract_one: %d\n", result3);

    return 0;
}

