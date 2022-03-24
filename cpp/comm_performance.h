#include <string>
#include <thread>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
//#include "internal_matrix.h"
//#define _TEST_SWITCH_
//#define _TEST_PRINT_

#pragma pack(push,1)
typedef struct {
    uint32_t curr_seq;
    uint32_t total_package_loss_num;
    uint32_t total_package_disorder_num;
    uint32_t total_package_repeated_num;
    uint32_t curr_package_loss_num;
    uint32_t curr_package_disorder_num;
    uint32_t curr_is_package_repeated;
    uint32_t total_frames;
    int64_t time_delay;
    uint32_t time_jump;
    uint64_t curr_time;
    uint64_t mcu_time;
}_CommPerfCal;
#pragma pack(pop)

class DSCommPerf {
public:
    DSCommPerf(const char * file_name);
    virtual ~DSCommPerf();

    void DSCommPerfInit(std::string path = "/opt/haomo/data/");
    void DSCommPerfCalc();
    void DSCommPerfWrite(uint64_t timestamp, uint32_t curr_seq);
    void DSCommPerfDeInit();

private:
    bool first;
    int fd_rd;
    uint16_t invalid_tick;
    uint32_t last_seq;
    _CommPerfCal perf_info;
    std::string file_name;
    //std::ofstream fd_rd;
    std::vector<uint32_t> lost;
};
