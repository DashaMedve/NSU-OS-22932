//10. ��� ���������� �������
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
    
int main(int argc, char* argv[]) {
    if (argc == 1)  // ���� ���������� ���, �� ���������� ���������
        return 1;

    pid_t pid = fork();  // �������� ����������� � ���������� pid
    if (pid == -1)  // ���� fork �� ������, �.�. pid ����� -1, �� ���������� ���������
        return 1;

    if (pid == 0) {  // ���� fork ������, �.�. pid ����� 0,
        execvp(argv[1], argv + 1);  // �� execvp ��������� ������� argv[1] � �����������, ������� � argv + 1
        return 1;
    } else {  // �����
        int stat_loc;
        wait(&stat_loc);  // ���� ���������� �����������

        if (WIFEXITED(stat_loc))  // ���� ���������� ����������, �� ������� ��������� �� ����
            printf("\nSubprocess pid: %d finished with exit code %d\n", pid, WEXITSTATUS(stat_loc));
    }

    return 0;
}
// ./lb10 printf "Hello, World!"
