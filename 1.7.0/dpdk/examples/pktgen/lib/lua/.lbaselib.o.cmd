cmd_lbaselib.o = gcc -Wp,-MD,./.lbaselib.o.d.tmp -m64 -pthread  -march=native -DRTE_MACHINE_CPUFLAG_SSE -DRTE_MACHINE_CPUFLAG_SSE2 -DRTE_MACHINE_CPUFLAG_SSE3 -DRTE_MACHINE_CPUFLAG_SSSE3 -DRTE_MACHINE_CPUFLAG_SSE4_1 -DRTE_MACHINE_CPUFLAG_SSE4_2 -DRTE_MACHINE_CPUFLAG_AES -DRTE_MACHINE_CPUFLAG_PCLMULQDQ -DRTE_MACHINE_CPUFLAG_AVX -DRTE_COMPILE_TIME_CPUFLAGS=RTE_CPUFLAG_SSE,RTE_CPUFLAG_SSE2,RTE_CPUFLAG_SSE3,RTE_CPUFLAG_SSSE3,RTE_CPUFLAG_SSE4_1,RTE_CPUFLAG_SSE4_2,RTE_CPUFLAG_AES,RTE_CPUFLAG_PCLMULQDQ,RTE_CPUFLAG_AVX  -I/root/Pktgen-DPDK-master/dpdk/examples/pktgen/lib/lua/lua/x86_64-pktgen-linuxapp-gcc/include -I/root/Pktgen-DPDK-master/dpdk/x86_64-pktgen-linuxapp-gcc/include -include /root/Pktgen-DPDK-master/dpdk/x86_64-pktgen-linuxapp-gcc/include/rte_config.h -W -Wall -Wpointer-arith -Wcast-align -Wnested-externs -I/root/Pktgen-DPDK-master/dpdk/examples/pktgen/lib/lua/root/Pktgen-DPDK-master/dpdk/examples/pktgen/lib/lua -O3 -g -fno-stack-protector   -o lbaselib.o -c lbaselib.c 
