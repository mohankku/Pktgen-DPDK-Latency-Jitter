cmd_pktgen.o = gcc -Wp,-MD,./.pktgen.o.d.tmp -m64 -pthread  -march=native -DRTE_MACHINE_CPUFLAG_SSE -DRTE_MACHINE_CPUFLAG_SSE2 -DRTE_MACHINE_CPUFLAG_SSE3 -DRTE_MACHINE_CPUFLAG_SSSE3 -DRTE_MACHINE_CPUFLAG_SSE4_1 -DRTE_MACHINE_CPUFLAG_SSE4_2 -DRTE_MACHINE_CPUFLAG_AES -DRTE_MACHINE_CPUFLAG_PCLMULQDQ -DRTE_MACHINE_CPUFLAG_AVX -DRTE_COMPILE_TIME_CPUFLAGS=RTE_CPUFLAG_SSE,RTE_CPUFLAG_SSE2,RTE_CPUFLAG_SSE3,RTE_CPUFLAG_SSSE3,RTE_CPUFLAG_SSE4_1,RTE_CPUFLAG_SSE4_2,RTE_CPUFLAG_AES,RTE_CPUFLAG_PCLMULQDQ,RTE_CPUFLAG_AVX  -I/root/FLOSS/Pktgen-DPDK-Latency-Jitter/1.7.0/dpdk/examples/pktgen/app/build/include -I/root/FLOSS/Pktgen-DPDK-Latency-Jitter/1.7.0/dpdk/x86_64-pktgen-linuxapp-gcc/include -include /root/FLOSS/Pktgen-DPDK-Latency-Jitter/1.7.0/dpdk/x86_64-pktgen-linuxapp-gcc/include/rte_config.h -O3 -g -fno-stack-protector -I../lib/common -I../lib/scrn -I../lib/lua   -o pktgen.o -c pktgen.c 
