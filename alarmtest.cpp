#include<stdio.h>
#include<unistd.h>
#include<signal.h>

int main() {
    alarm(2);
    sleep(3);
    return 0;
}
