// 21. �������
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

int count = 0;  // ������� ��������

void handleSIGQUIT() {
    printf("\nSignal sound times: %d", count);
    exit(0);
}

void handleSIGINT() {
    printf("\a");
    fflush(NULL);
    count++;
}

int main() {
    signal(SIGINT, &handleSIGINT);  // ������ �������� ������ �� ����� ��������� ������ ���, ����� �� ������� ������, �� ������� � ��� ��������� ������� ������� SIGINT
    signal(SIGQUIT, &handleSIGQUIT);  // ��� ��������� SIGQUIT, ��� ������ ������� ���������, ���������, ������� ��� ��������� ������, � �����������.
    while (1);
}