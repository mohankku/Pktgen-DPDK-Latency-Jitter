cmd_eal_common_pci.o = gcc -Wp,-MD,./.eal_common_pci.o.d.tmp -m64 -pthread  -march=native -DRTE_MACHINE_CPUFLAG_SSE -DRTE_MACHINE_CPUFLAG_SSE2 -DRTE_MACHINE_CPUFLAG_SSE3 -DRTE_MACHINE_CPUFLAG_SSSE3 -DRTE_MACHINE_CPUFLAG_SSE4_1 -DRTE_MACHINE_CPUFLAG_SSE4_2 -DRTE_MACHINE_CPUFLAG_AES -DRTE_MACHINE_CPUFLAG_PCLMULQDQ -DRTE_MACHINE_CPUFLAG_AVX -DRTE_COMPILE_TIME_CPUFLAGS=RTE_CPUFLAG_SSE,RTE_CPUFLAG_SSE2,RTE_CPUFLAG_SSE3,RTE_CPUFLAG_SSSE3,RTE_CPUFLAG_SSE4_1,RTE_CPUFLAG_SSE4_2,RTE_CPUFLAG_AES,RTE_CPUFLAG_PCLMULQDQ,RTE_CPUFLAG_AVX  -I/root/FLOSS/Pktgen-DPDK-Latency-Jitter/1.7.0/dpdk/x86_64-pktgen-linuxapp-gcc/include -include /root/FLOSS/Pktgen-DPDK-Latency-Jitter/1.7.0/dpdk/x86_64-pktgen-linuxapp-gcc/include/rte_config.h -I/root/FLOSS/Pktgen-DPDK-Latency-Jitter/1.7.0/dpdk/lib/librte_eal/linuxapp/eal/include -I/root/FLOSS/Pktgen-DPDK-Latency-Jitter/1.7.0/dpdk/lib/librte_eal/common/include -I/root/FLOSS/Pktgen-DPDK-Latency-Jitter/1.7.0/dpdk/lib/librte_ring -I/root/FLOSS/Pktgen-DPDK-Latency-Jitter/1.7.0/dpdk/lib/librte_mempool -I/root/FLOSS/Pktgen-DPDK-Latency-Jitter/1.7.0/dpdk/lib/librte_malloc -I/root/FLOSS/Pktgen-DPDK-Latency-Jitter/1.7.0/dpdk/lib/librte_ether -I/root/FLOSS/Pktgen-DPDK-Latency-Jitter/1.7.0/dpdk/lib/librte_ivshmem -I/root/FLOSS/Pktgen-DPDK-Latency-Jitter/1.7.0/dpdk/lib/librte_pmd_ring -I/root/FLOSS/Pktgen-DPDK-Latency-Jitter/1.7.0/dpdk/lib/librte_pmd_pcap -I/root/FLOSS/Pktgen-DPDK-Latency-Jitter/1.7.0/dpdk/lib/librte_pmd_xenvirt -W -Wall -Werror -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wold-style-definition -Wpointer-arith -Wcast-align -Wnested-externs -Wcast-qual -Wformat-nonliteral -Wformat-security -Wundef -Wwrite-strings -O3   -o eal_common_pci.o -c /root/FLOSS/Pktgen-DPDK-Latency-Jitter/1.7.0/dpdk/lib/librte_eal/common/eal_common_pci.c 
