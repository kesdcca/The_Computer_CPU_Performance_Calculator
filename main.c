#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

// 计算π的前digits位数字，返回所有输出数字的累加和（校验用）
unsigned long long compute_pi_digits(int digits) {
    int len = 10 * digits / 3 + 1;               // 算法需要的数组长度
    int *a = (int*)malloc(len * sizeof(int));
    if (!a) {
        fprintf(stderr, "内存分配失败\n");
        return 0;
    }

    // 初始化数组全为2（对应连分数的初始值）
    for (int i = 0; i < len; i++) a[i] = 2;

    int nines = 0;        // 连续9的个数
    int predigit = 0;      // 前一位数字
    unsigned long long checksum = 0;   // 所有输出数字之和

    for (int j = 1; j <= digits; j++) {
        int q = 0;
        // 从后向前更新数组
        for (int i = len - 1; i >= 0; i--) {
            a[i] = a[i] * 10 + q;
            q = a[i] / (2 * i + 1);
            a[i] %= (2 * i + 1);
        }
        a[0] = q % 10;          // 当前位数字
        q = q / 10;             // 可能的进位

        if (q == 9) {
            nines++;
        } else if (q == 10) {
            checksum += predigit + 1;   // 输出 predigit+1
            predigit = 0;
            nines = 0;
        } else {
            if (j > 1) checksum += predigit;      // 输出前一位数字
            if (nines > 0) checksum += nines * 9;  // 输出所有连续的9
            predigit = q;
            nines = 0;
        }
    }
    checksum += predigit;   // 输出最后一个数字
    free(a);
    return checksum;
}

int main() {
    const int digits = 10000;        // 计算1万位
    const int repeats = 20;           // 重复20次
    LARGE_INTEGER freq, start, end;
    double total_time = 0.0;
    unsigned long long total_checksum = 0;

    if (!QueryPerformanceFrequency(&freq)) {
        fprintf(stderr, "系统不支持高精度计时器\n");
        return 1;
    }

    printf("正在计算π的前%d位，重复%d次，请稍候...\n\n", digits, repeats);

    for (int i = 0; i < repeats; i++) {
        QueryPerformanceCounter(&start);
        unsigned long long cs = compute_pi_digits(digits);
        QueryPerformanceCounter(&end);

        total_checksum += cs;
        double elapsed = (double)(end.QuadPart - start.QuadPart) / (double)freq.QuadPart;
        total_time += elapsed;

        printf("第%2d次：%.6f 秒（校验和 = %llu）\n", i + 1, elapsed, cs);
    }

    double avg_time = total_time / repeats;
    double score = 100000.0 / avg_time;   // 得分 = 100000 / 平均时间（秒）

    printf("\n%d次平均时间：%.6f 秒\n", repeats, avg_time);
    printf("性能得分（时间倒数×100000）：%.2f\n", score);
    printf("总校验和：%llu（用于验证计算完整性）\n", total_checksum);

    return 0;
}
