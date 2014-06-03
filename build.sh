rm ./MYCPU.out
g++ mycpu.cpp main.cpp program.cpp cpu.cpp pipeline_reg.cpp control_signal.cpp data_memory.cpp instruction.cpp register.cpp -o MYCPU.out
./MYCPU.out