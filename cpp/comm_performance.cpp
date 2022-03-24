#include "comm_performance.h"

DSCommPerf::DSCommPerf(const char * file_name):file_name(file_name) {
    DSCommPerfInit();
    std::cout << "DSCommPerf: struct total size - " << sizeof(_CommPerfCal) << std::endl;
    std::cout << "DSCommPerf: enter constructor" << std::endl;
}

DSCommPerf::~DSCommPerf() {
    DSCommPerfDeInit();
    std::cout << "DSCommPerf: enter destructor" << std::endl;
}

void DSCommPerf::DSCommPerfInit(std::string path) {
    first = true;
    invalid_tick = 1000;
    memset(&perf_info, 0, sizeof(_CommPerfCal));
    std::string full_path;
    full_path.append(path);
    full_path.append(file_name);
    fd_rd = open(full_path.c_str(), O_WRONLY | O_CREAT);
    if(fd_rd == -1) {
        std::cout << "DSCommPerf: open file failed - " << strerror(errno) << std::endl;
    }
}

void DSCommPerf::DSCommPerfWrite(uint64_t timestamp, uint32_t curr_seq) {
    if(invalid_tick > 0) {
        invalid_tick--;  //ignore 1000 frames after systen run
        return;
    }

    //uint64_t timeHi = ipc_header->StbmTs.secondsHi << 16;
    //uint64_t timestamp =((timeHi+ipc_header->StbmTs.seconds)*1000*1000*1000)+ipc_header->StbmTs.nanoseconds;
    //uint32_t curr_seq = ipc_header->e2e.Counter;

    uint64_t soc_timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();

    //std::cout << "DSCommPerf: soc_timestamp - " << soc_timestamp << std::endl;
    //std::cout << "DSCommPerf: " << timestamp << " " << curr_seq << std::endl;
    perf_info.time_delay = 0xFFFFFFFF;
    perf_info.curr_seq = curr_seq;
    perf_info.curr_time = soc_timestamp;
    perf_info.mcu_time = timestamp;
    perf_info.time_jump = 0;
    perf_info.time_delay = (int64_t)(soc_timestamp - timestamp);
    if(soc_timestamp < timestamp) {
        perf_info.time_jump = 1;
        std::cout << "DSCommPerf: soc_timestamp is later than mcu timestamp." << soc_timestamp << " " << timestamp << std::endl;
    }
    if(true == first) {
        first = false;
        last_seq = curr_seq;
        return;  //don't calculate first bag that received.
    }
    else {
        if(curr_seq != (last_seq + 1)) {
            if(curr_seq > last_seq) {
                perf_info.curr_package_loss_num   = (curr_seq - last_seq - 1);
                perf_info.total_package_loss_num += perf_info.curr_package_loss_num;
                for(uint32_t lost_seq = (last_seq + 1); lost_seq < curr_seq; lost_seq++) {
                    lost.push_back(lost_seq);
                }
            }
            else if(curr_seq == last_seq){
                perf_info.curr_is_package_repeated = 1;
                perf_info.total_package_repeated_num++;
            }
            else {
                perf_info.curr_package_disorder_num = 1;
                perf_info.total_package_disorder_num++;

                std::vector<uint32_t>::iterator it;
                for(it = lost.begin(); it != lost.end();) {
                    if(*it == curr_seq) {
                        lost.erase(it);
                        std::cout << "DSCommPerf: found a disorder frame, erase it in lost vector - " << curr_seq << std::endl;
                        perf_info.total_package_loss_num--;
                    }
                }
            }
        }
        last_seq = curr_seq;
    }
    perf_info.total_frames++;
#ifdef _TEST_SWITCH_
    perf_info.curr_seq = 0x01020304;
    perf_info.total_package_loss_num = 0x05060708;
    perf_info.total_package_disorder_num = 0x01020304;
    perf_info.total_package_repeated_num = 0x05060708;
    perf_info.curr_package_loss_num = 0x01020304;
    perf_info.curr_package_disorder_num = 0x05060708;
    perf_info.curr_is_package_repeated = 0x01020304;
    perf_info.total_frames = 0x05060708;
    perf_info.time_delay = 0x01020304;
    perf_info.time_jump = 0x01;
    perf_info.curr_time = 0x0102030405060708;
#endif
#ifdef _TEST_PRINT_
    std::cout << "curr_seq:                    " << perf_info.curr_seq << std::endl;
    std::cout << "total_package_loss_num:      " << perf_info.total_package_loss_num << std::endl;
    std::cout << "total_package_disorder_num:  " << perf_info.total_package_disorder_num << std::endl;
    std::cout << "total_package_repeated_num:  " << perf_info.total_package_repeated_num << std::endl;
    std::cout << "curr_package_loss_num:       " << perf_info.curr_package_loss_num << std::endl;
    std::cout << "curr_package_disorder_num:   " << perf_info.curr_package_disorder_num << std::endl;
    std::cout << "curr_is_package_repeated:    " << perf_info.curr_is_package_repeated << std::endl;
    std::cout << "total_frames:                " << perf_info.total_frames << std::endl;
    std::cout << "time_delay:                  " << perf_info.time_delay << std::endl;
    std::cout << "time_jump:                   " << perf_info.time_jump << std::endl;
    std::cout << "curr_time:                   " << perf_info.curr_time << std::endl;
#endif
    //std::string content = std::string((const char *)&perf_info, sizeof(_CommPerfCal));
    //fd_rd << content << std::endl;
    if(int ret = write(fd_rd, &perf_info, sizeof(_CommPerfCal)) != sizeof(_CommPerfCal)) {
        std::cout << "DSCommPerf: write file error - " << ret << std::endl;
    }
}

void DSCommPerf::DSCommPerfDeInit() {
    close(fd_rd);
}