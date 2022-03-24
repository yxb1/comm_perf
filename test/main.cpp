#include <unistd.h>
#include "comm_performance.h"

int main() {
    DSCommPerf *tmp = new DSCommPerf("adcanif");
    while(1) {
		//recv_thread(char *buff);
		//uint64_t timestamp = *(uint64_t*)buff;
		//uint32_t curr_seq = *(uint32_t*)(buff+8);
		//tmp->DSCommPerfWrite(timestamp,curr_seq);
        tmp->DSCommPerfWrite(1,1);
        sleep(1);
    }
    return 0;
}