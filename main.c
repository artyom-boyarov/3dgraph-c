#include "graph.h"
#include "3d.h"
#include <time.h>

int main(int argc, char* argv[]) {
    clock_t t1 = clock();
	if (graph_init(320, 240) < 0)
        return 1;

    init();
    clock_t t2 = clock();
    float elapsed = (t2 - t1)/CLOCKS_PER_SEC;
    t1 = clock();
	while (update(elapsed)) {
		if (graph_process_events() < 0)
            break;
        t2 = clock();
        elapsed = (t2 - t1)/CLOCKS_PER_SEC;

        t1 = clock();
        delay(10);
	}
	graph_close();
}
