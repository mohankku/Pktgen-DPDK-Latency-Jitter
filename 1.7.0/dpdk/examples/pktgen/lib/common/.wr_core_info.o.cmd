cmd_wr_core_info.o = gcc -Wp,-MD,./.wr_core_info.o.d.tmp -m64 -pthread  -march=native -DRTE_MACHINE_CPUFLAG_SSE -DRTE_MACHINE_CPUFLAG_SSE2 -DRTE_MACHINE_CPUFLAG_SSE3 -DRTE_MACHINE_CPUFLAG_SSSE3 -DRTE_MACHINE_CPUFLAG_SSE4_1 -DRTE_MACHINE_CPUFLAG_SSE4_2 -DRTE_MACHINE_CPUFLAG_AES -DRTE_MACHINE_CPUFLAG_PCLMULQDQ -DRTE_MACHINE_CPUFLAG_AVX -DRTE_COMPILE_TIME_CPUFLAGS=RTE_CPUFLAG_SSE,RTE_CPUFLAG_SSE2,RTE_CPUFLAG_SSE3,RTE_CPUFLAG_SSSE3,RTE_CPUFLAG_SSE4_1,RTE_CPUFLAG_SSE4_2,RTE_CPUFLAG_AES,RTE_CPUFLAG_PCLMULQDQ,RTE_CPUFLAG_AVX  -I/root/Pktgen-DPDK-master/dpdk/examples/pktgen/lib/common/common/x86_64-pktgen-linuxapp-gcc/include -I/root/Pktgen-DPDK-master/dpdk/x86_64-pktgen-linuxapp-gcc/include -include /root/Pktgen-DPDK-master/dpdk/x86_64-pktgen-linuxapp-gcc/include/rte_config.h -W -Wall -Werror -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wold-style-definition -Wpointer-arith -Wcast-align -Wnested-externs -Wcast-qual -Wformat-nonliteral -Wformat-security -Wundef -Wwrite-strings -I/root/Pktgen-DPDK-master/dpdk/examples/pktgen/lib/common/root/Pktgen-DPDK-master/dpdk/examples/pktgen/lib/common -O3 -I../scrn   -o wr_core_info.o -c wr_core_info.c 
