#include <stdlib.h>
#include <stdio.h>

struct xy {
int x;
double y;
};

static struct xy* x;

int main() {
	int y;
	scanf("%d", &y);
	x = malloc(y * sizeof(struct xy));

	free(x);
}
