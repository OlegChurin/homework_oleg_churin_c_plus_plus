#include <cstdio>
int main() {
    const char* s =
        "#include <cstdio>%cint main() {%c    const char* s = %c%s%c;%c    "
        "printf(s, 10, 10, 34, s, 34);%c    return 0;%c}%c";
    printf(s, 10, 10, 34, s, 34);
    return 0;
}