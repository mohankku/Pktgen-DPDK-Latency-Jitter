cmd_lua-socket.o = gcc -Wp,-MD,./.lua-socket.o.d.tmp -m64 -pthread  -march=native -DRTE_MACHINE_CPUFLAG_SSE -DRTE_MACHINE_CPUFLAG_SSE2 -DRTE_MACHINE_CPUFLAG_SSE3 -DRTE_MACHINE_CPUFLAG_SSSE3 -DRTE_MACHINE_CPUFLAG_SSE4_1 -DRTE_MACHINE_CPUFLAG_SSE4_2 -DRTE_MACHINE_CPUFLAG_AES -DRTE_MACHINE_CPUFLAG_PCLMULQDQ -DRTE_MACHINE_CPUFLAG_AVX -DRTE_COMPILE_TIME_CPUFLAGS=RTE_CPUFLAG_SSE,RTE_CPUFLAG_SSE2,RTE_CPUFLAG_SSE3,RTE_CPUFLAG_SSSE3,RTE_CPUFLAG_SSE4_1,RTE_CPUFLAG_SSE4_2,RTE_CPUFLAG_AES,RTE_CPUFLAG_PCLMULQDQ,RTE_CPUFLAG_AVX  -I/root/FLOSS/Pktgen-DPDK/dpdk/examples/pktgen/lib/lua-5.2.3/src/lua/src/x86_64-ivshmem-linuxapp-gcc/include -I/root/ovs_dpdk/DPDK/x86_64-ivshmem-linuxapp-gcc/include -include /root/ovs_dpdk/DPDK/x86_64-ivshmem-linuxapp-gcc/include/rte_config.h -O3 -g -DLUA_USE_LINUX -DUSE_64_BIT -W -Wall -Wpointer-arith -Wcast-align -Wnested-externs -Wno-cast-align -I/root/FLOSS/Pktgen-DPDK/dpdk/examples/pktgen/lib/lua-5.2.3/src/root/FLOSS/Pktgen-DPDK/dpdk/examples/pktgen/lib/lua-5.2.3/src -fno-stack-protector   -o lua-socket.o -c lua-socket.c 
