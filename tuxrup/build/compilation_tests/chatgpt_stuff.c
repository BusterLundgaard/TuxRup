#include <stdio.h>
#include <string.h>

int main() {
    char line[512];
    char identifier[128];

    while (fgets(line, sizeof(line), stdin)) {
		if (strstr(line, "called object")) {
        if (sscanf(line, "%*[^‘]‘%127[^’]’", identifier) == 1) {
            printf("Found identifier: %s\n", identifier);
        }
		}
    }

    return 0;
}

