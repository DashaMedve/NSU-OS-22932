// 9. �������� ���� ���������
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>

int main() {
    pid_t pid = fork();  // �������� �����������
    if (pid == -1)  // ���� fork �� ������, ���������� -1
        return 1;
    if (pid == 0) {  // ���� ������� ����� �������������� 0, �� ����������� cat ��� task.c
        execlp("cat", "cat", "file.txt", NULL);
        return 1;
    }
    else {  // �����, ���� ������� �� �������� ������������, �� �� ���� ��������� ����������
        if (wait(NULL) != -1)  // � ������� wait
            printf("\nChild process (pid: %d) finished\n", pid);
    }

    return 0;
}
