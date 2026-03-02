#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

unsigned long long compute_pi_digits(int digits) {
    int len = 10 * digits / 3 + 1;
    int *a = (int*)malloc(len * sizeof(int));
    if (!a) return 0;
    for (int i = 0; i < len; i++) a[i] = 2;
    int nines = 0, predigit = 0;
    unsigned long long checksum = 0;
    for (int j = 1; j <= digits; j++) {
        int q = 0;
        for (int i = len - 1; i >= 0; i--) {
            a[i] = a[i] * 10 + q;
            q = a[i] / (2 * i + 1);
            a[i] %= (2 * i + 1);
        }
        a[0] = q % 10;
        q = q / 10;
        if (q == 9) nines++;
        else if (q == 10) { checksum += predigit + 1; predigit = 0; nines = 0; }
        else {
            if (j > 1) checksum += predigit;
            if (nines > 0) checksum += nines * 9;
            predigit = q;
            nines = 0;
        }
    }
    checksum += predigit;
    free(a);
    return checksum;
}

int main() {
    const int digits = 10000;
    const int repeats = 20;
    LARGE_INTEGER freq, start, end;
    double total_time = 0.0;
    unsigned long long total_checksum = 0;

    FILE *log_file = fopen("print", "wb");
    if (!log_file) return 1;
    unsigned char bom[] = {0xEF, 0xBB, 0xBF};
    fwrite(bom, 1, 3, log_file);

    if (!QueryPerformanceFrequency(&freq)) {
        fclose(log_file);
        return 1;
    }

    // 启动提示
    fprintf(log_file,
        "\xE6\xAD\xA3\xE5\x9C\xA8\xE8\xAE\xA1\xE7\xAE\x97\xCF\x80\xE7\x9A\x84\xE5\x89\x8D%d\xE4\xBD\x8D\xEF\xBC\x8C\xE9\x87\x8D\xE5\xA4\x8D%d\xE6\xAC\xA1\xEF\xBC\x8C\xE8\xAF\xB7\xE7\xA8\x8D\xE5\x80\x99...\n\n",
        digits, repeats);
    fflush(log_file);

    for (int i = 0; i < repeats; i++) {
        QueryPerformanceCounter(&start);
        unsigned long long cs = compute_pi_digits(digits);
        QueryPerformanceCounter(&end);

        total_checksum += cs;
        double elapsed = (double)(end.QuadPart - start.QuadPart) / (double)freq.QuadPart;
        total_time += elapsed;

        // 进度行：第%2d次：%.6f 秒（校验和 = %llu）
        fprintf(log_file,
            "\xE7\xAC\xAC%2d\xE6\xAC\xA1\xEF\xBC\x9A%.6f \xE7\xA7\x92\xEF\xBC\x88\xE6\xA0\xA1\xE9\xAA\x8C\xE5\x92\x8C\xEF\xBC\x9D%llu\xEF\xBC\x89\n",
            i + 1, elapsed, cs);
        fflush(log_file);
    }

    double avg_time = total_time / repeats;
    double score = 100000.0 / avg_time;

    // 结果
    fprintf(log_file,
        "\n%d\xE6\xAC\xA1\xE5\xB9\xB3\xE5\x9D\x87\xE6\x97\xB6\xE9\x97\xB4\xEF\xBC\x9A%.6f \xE7\xA7\x92\n"
        "\xE6\x80\xA7\xE8\x83\xBD\xE5\xBE\x97\xE5\x88\x86\xEF\xBC\x88\xE6\x97\xB6\xE9\x97\xB4\xE5\x80\x92\xE6\x95\xB0\xC3\x97\x31\x30\x30\x30\x30\x30\xEF\xBC\x89\xEF\xBC\x9A%.2f\n"
        "\xE6\x80\xBB\xE6\xA0\xA1\xE9\xAA\x8C\xE5\x92\x8C\xEF\xBC\x9A%llu\xEF\xBC\x88\xE7\x94\xA8\xE4\xBA\x8E\xE9\xAA\x8C\xE8\xAF\x81\xE8\xAE\xA1\xE7\xAE\x97\xE5\xAE\x8C\xE6\x95\xB4\xE6\x80\xA7\xEF\xBC\x89\n",
        repeats, avg_time, score, total_checksum);
    fflush(log_file);

    fclose(log_file);
    return 0;
}
