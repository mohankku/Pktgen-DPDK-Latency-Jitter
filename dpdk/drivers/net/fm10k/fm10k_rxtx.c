/*-
 *   BSD LICENSE
 *
 *   Copyright(c) 2013-2015 Intel Corporation. All rights reserved.
 *   All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *     * Neither the name of Intel Corporation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <inttypes.h>

#include <rte_ethdev.h>
#include <rte_common.h>
#include "fm10k.h"
#include "base/fm10k_type.h"

#ifdef RTE_PMD_PACKET_PREFETCH
#define rte_packet_prefetch(p)  rte_prefetch1(p)
#else
#define rte_packet_prefetch(p)  do {} while (0)
#endif

#ifdef RTE_LIBRTE_FM10K_DEBUG_RX
static inline void dump_rxd(union fm10k_rx_desc *rxd)
{
	PMD_RX_LOG(DEBUG, "+----------------|----------------+");
	PMD_RX_LOG(DEBUG, "|     GLORT      | PKT HDR & TYPE |");
	PMD_RX_LOG(DEBUG, "|   0x%08x   |   0x%08x   |", rxd->d.glort,
			rxd->d.data);
	PMD_RX_LOG(DEBUG, "+----------------|----------------+");
	PMD_RX_LOG(DEBUG, "|   VLAN & LEN   |     STATUS     |");
	PMD_RX_LOG(DEBUG, "|   0x%08x   |   0x%08x   |", rxd->d.vlan_len,
			rxd->d.staterr);
	PMD_RX_LOG(DEBUG, "+----------------|----------------+");
	PMD_RX_LOG(DEBUG, "|    RESERVED    |    RSS_HASH    |");
	PMD_RX_LOG(DEBUG, "|   0x%08x   |   0x%08x   |", 0, rxd->d.rss);
	PMD_RX_LOG(DEBUG, "+----------------|----------------+");
	PMD_RX_LOG(DEBUG, "|            TIME TAG             |");
	PMD_RX_LOG(DEBUG, "|       0x%016"PRIx64"        |", rxd->q.timestamp);
	PMD_RX_LOG(DEBUG, "+----------------|----------------+");
}
#endif

static inline void
rx_desc_to_ol_flags(struct rte_mbuf *m, const union fm10k_rx_desc *d)
{
#ifdef RTE_NEXT_ABI
	static const uint32_t
		ptype_table[FM10K_RXD_PKTTYPE_MASK >> FM10K_RXD_PKTTYPE_SHIFT]
			__rte_cache_aligned = {
		[FM10K_PKTTYPE_OTHER] = RTE_PTYPE_L2_ETHER,
		[FM10K_PKTTYPE_IPV4] = RTE_PTYPE_L2_ETHER | RTE_PTYPE_L3_IPV4,
		[FM10K_PKTTYPE_IPV4_EX] = RTE_PTYPE_L2_ETHER |
			RTE_PTYPE_L3_IPV4_EXT,
		[FM10K_PKTTYPE_IPV6] = RTE_PTYPE_L2_ETHER | RTE_PTYPE_L3_IPV6,
		[FM10K_PKTTYPE_IPV6_EX] = RTE_PTYPE_L2_ETHER |
			RTE_PTYPE_L3_IPV6_EXT,
		[FM10K_PKTTYPE_IPV4 | FM10K_PKTTYPE_TCP] = RTE_PTYPE_L2_ETHER |
			RTE_PTYPE_L3_IPV4 | RTE_PTYPE_L4_TCP,
		[FM10K_PKTTYPE_IPV6 | FM10K_PKTTYPE_TCP] = RTE_PTYPE_L2_ETHER |
			RTE_PTYPE_L3_IPV6 | RTE_PTYPE_L4_TCP,
		[FM10K_PKTTYPE_IPV4 | FM10K_PKTTYPE_UDP] = RTE_PTYPE_L2_ETHER |
			RTE_PTYPE_L3_IPV4 | RTE_PTYPE_L4_UDP,
		[FM10K_PKTTYPE_IPV6 | FM10K_PKTTYPE_UDP] = RTE_PTYPE_L2_ETHER |
			RTE_PTYPE_L3_IPV6 | RTE_PTYPE_L4_UDP,
	};

	m->packet_type = ptype_table[(d->w.pkt_info & FM10K_RXD_PKTTYPE_MASK)
						>> FM10K_RXD_PKTTYPE_SHIFT];
#else /* RTE_NEXT_ABI */
	uint16_t ptype;
	static const uint16_t pt_lut[] = { 0,
		PKT_RX_IPV4_HDR, PKT_RX_IPV4_HDR_EXT,
		PKT_RX_IPV6_HDR, PKT_RX_IPV6_HDR_EXT,
		0, 0, 0
	};
#endif /* RTE_NEXT_ABI */

	if (d->w.pkt_info & FM10K_RXD_RSSTYPE_MASK)
		m->ol_flags |= PKT_RX_RSS_HASH;

	if (unlikely((d->d.staterr &
		(FM10K_RXD_STATUS_IPCS | FM10K_RXD_STATUS_IPE)) ==
		(FM10K_RXD_STATUS_IPCS | FM10K_RXD_STATUS_IPE)))
		m->ol_flags |= PKT_RX_IP_CKSUM_BAD;

	if (unlikely((d->d.staterr &
		(FM10K_RXD_STATUS_L4CS | FM10K_RXD_STATUS_L4E)) ==
		(FM10K_RXD_STATUS_L4CS | FM10K_RXD_STATUS_L4E)))
		m->ol_flags |= PKT_RX_L4_CKSUM_BAD;

	if (d->d.staterr & FM10K_RXD_STATUS_VEXT)
		m->ol_flags |= PKT_RX_VLAN_PKT;

	if (unlikely(d->d.staterr & FM10K_RXD_STATUS_HBO))
		m->ol_flags |= PKT_RX_HBUF_OVERFLOW;

	if (unlikely(d->d.staterr & FM10K_RXD_STATUS_RXE))
		m->ol_flags |= PKT_RX_RECIP_ERR;

#ifndef RTE_NEXT_ABI
	ptype = (d->d.data & FM10K_RXD_PKTTYPE_MASK_L3) >>
						FM10K_RXD_PKTTYPE_SHIFT;
	m->ol_flags |= pt_lut[(uint8_t)ptype];
#endif
}

uint16_t
fm10k_recv_pkts(void *rx_queue, struct rte_mbuf **rx_pkts,
	uint16_t nb_pkts)
{
	struct rte_mbuf *mbuf;
	union fm10k_rx_desc desc;
	struct fm10k_rx_queue *q = rx_queue;
	uint16_t count = 0;
	int alloc = 0;
	uint16_t next_dd;
	int ret;

	next_dd = q->next_dd;

	nb_pkts = RTE_MIN(nb_pkts, q->alloc_thresh);
	for (count = 0; count < nb_pkts; ++count) {
		mbuf = q->sw_ring[next_dd];
		desc = q->hw_ring[next_dd];
		if (!(desc.d.staterr & FM10K_RXD_STATUS_DD))
			break;
#ifdef RTE_LIBRTE_FM10K_DEBUG_RX
		dump_rxd(&desc);
#endif
		rte_pktmbuf_pkt_len(mbuf) = desc.w.length;
		rte_pktmbuf_data_len(mbuf) = desc.w.length;

		mbuf->ol_flags = 0;
#ifdef RTE_LIBRTE_FM10K_RX_OLFLAGS_ENABLE
		rx_desc_to_ol_flags(mbuf, &desc);
#endif

		mbuf->hash.rss = desc.d.rss;

		rx_pkts[count] = mbuf;
		if (++next_dd == q->nb_desc) {
			next_dd = 0;
			alloc = 1;
		}

		/* Prefetch next mbuf while processing current one. */
		rte_prefetch0(q->sw_ring[next_dd]);

		/*
		 * When next RX descriptor is on a cache-line boundary,
		 * prefetch the next 4 RX descriptors and the next 8 pointers
		 * to mbufs.
		 */
		if ((next_dd & 0x3) == 0) {
			rte_prefetch0(&q->hw_ring[next_dd]);
			rte_prefetch0(&q->sw_ring[next_dd]);
		}
	}

	q->next_dd = next_dd;

	if ((q->next_dd > q->next_trigger) || (alloc == 1)) {
		ret = rte_mempool_get_bulk(q->mp,
					(void **)&q->sw_ring[q->next_alloc],
					q->alloc_thresh);

		if (unlikely(ret != 0)) {
			uint8_t port = q->port_id;
			PMD_RX_LOG(ERR, "Failed to alloc mbuf");
			/*
			 * Need to restore next_dd if we cannot allocate new
			 * buffers to replenish the old ones.
			 */
			q->next_dd = (q->next_dd + q->nb_desc - count) %
								q->nb_desc;
			rte_eth_devices[port].data->rx_mbuf_alloc_failed++;
			return 0;
		}

		for (; q->next_alloc <= q->next_trigger; ++q->next_alloc) {
			mbuf = q->sw_ring[q->next_alloc];

			/* setup static mbuf fields */
			fm10k_pktmbuf_reset(mbuf, q->port_id);

			/* write descriptor */
			desc.q.pkt_addr = MBUF_DMA_ADDR_DEFAULT(mbuf);
			desc.q.hdr_addr = MBUF_DMA_ADDR_DEFAULT(mbuf);
			q->hw_ring[q->next_alloc] = desc;
		}
		FM10K_PCI_REG_WRITE(q->tail_ptr, q->next_trigger);
		q->next_trigger += q->alloc_thresh;
		if (q->next_trigger >= q->nb_desc) {
			q->next_trigger = q->alloc_thresh - 1;
			q->next_alloc = 0;
		}
	}

	return count;
}

uint16_t
fm10k_recv_scattered_pkts(void *rx_queue, struct rte_mbuf **rx_pkts,
				uint16_t nb_pkts)
{
	struct rte_mbuf *mbuf;
	union fm10k_rx_desc desc;
	struct fm10k_rx_queue *q = rx_queue;
	uint16_t count = 0;
	uint16_t nb_rcv, nb_seg;
	int alloc = 0;
	uint16_t next_dd;
	struct rte_mbuf *first_seg = q->pkt_first_seg;
	struct rte_mbuf *last_seg = q->pkt_last_seg;
	int ret;

	next_dd = q->next_dd;
	nb_rcv = 0;

	nb_seg = RTE_MIN(nb_pkts, q->alloc_thresh);
	for (count = 0; count < nb_seg; count++) {
		mbuf = q->sw_ring[next_dd];
		desc = q->hw_ring[next_dd];
		if (!(desc.d.staterr & FM10K_RXD_STATUS_DD))
			break;
#ifdef RTE_LIBRTE_FM10K_DEBUG_RX
		dump_rxd(&desc);
#endif

		if (++next_dd == q->nb_desc) {
			next_dd = 0;
			alloc = 1;
		}

		/* Prefetch next mbuf while processing current one. */
		rte_prefetch0(q->sw_ring[next_dd]);

		/*
		 * When next RX descriptor is on a cache-line boundary,
		 * prefetch the next 4 RX descriptors and the next 8 pointers
		 * to mbufs.
		 */
		if ((next_dd & 0x3) == 0) {
			rte_prefetch0(&q->hw_ring[next_dd]);
			rte_prefetch0(&q->sw_ring[next_dd]);
		}

		/* Fill data length */
		rte_pktmbuf_data_len(mbuf) = desc.w.length;

		/*
		 * If this is the first buffer of the received packet,
		 * set the pointer to the first mbuf of the packet and
		 * initialize its context.
		 * Otherwise, update the total length and the number of segments
		 * of the current scattered packet, and update the pointer to
		 * the last mbuf of the current packet.
		 */
		if (!first_seg) {
			first_seg = mbuf;
			first_seg->pkt_len = desc.w.length;
		} else {
			first_seg->pkt_len =
					(uint16_t)(first_seg->pkt_len +
					rte_pktmbuf_data_len(mbuf));
			first_seg->nb_segs++;
			last_seg->next = mbuf;
		}

		/*
		 * If this is not the last buffer of the received packet,
		 * update the pointer to the last mbuf of the current scattered
		 * packet and continue to parse the RX ring.
		 */
		if (!(desc.d.staterr & FM10K_RXD_STATUS_EOP)) {
			last_seg = mbuf;
			continue;
		}

		first_seg->ol_flags = 0;
#ifdef RTE_LIBRTE_FM10K_RX_OLFLAGS_ENABLE
		rx_desc_to_ol_flags(first_seg, &desc);
#endif
		first_seg->hash.rss = desc.d.rss;

		/* Prefetch data of first segment, if configured to do so. */
		rte_packet_prefetch((char *)first_seg->buf_addr +
			first_seg->data_off);

		/*
		 * Store the mbuf address into the next entry of the array
		 * of returned packets.
		 */
		rx_pkts[nb_rcv++] = first_seg;

		/*
		 * Setup receipt context for a new packet.
		 */
		first_seg = NULL;
	}

	q->next_dd = next_dd;

	if ((q->next_dd > q->next_trigger) || (alloc == 1)) {
		ret = rte_mempool_get_bulk(q->mp,
					(void **)&q->sw_ring[q->next_alloc],
					q->alloc_thresh);

		if (unlikely(ret != 0)) {
			uint8_t port = q->port_id;
			PMD_RX_LOG(ERR, "Failed to alloc mbuf");
			/*
			 * Need to restore next_dd if we cannot allocate new
			 * buffers to replenish the old ones.
			 */
			q->next_dd = (q->next_dd + q->nb_desc - count) %
								q->nb_desc;
			rte_eth_devices[port].data->rx_mbuf_alloc_failed++;
			return 0;
		}

		for (; q->next_alloc <= q->next_trigger; ++q->next_alloc) {
			mbuf = q->sw_ring[q->next_alloc];

			/* setup static mbuf fields */
			fm10k_pktmbuf_reset(mbuf, q->port_id);

			/* write descriptor */
			desc.q.pkt_addr = MBUF_DMA_ADDR_DEFAULT(mbuf);
			desc.q.hdr_addr = MBUF_DMA_ADDR_DEFAULT(mbuf);
			q->hw_ring[q->next_alloc] = desc;
		}
		FM10K_PCI_REG_WRITE(q->tail_ptr, q->next_trigger);
		q->next_trigger += q->alloc_thresh;
		if (q->next_trigger >= q->nb_desc) {
			q->next_trigger = q->alloc_thresh - 1;
			q->next_alloc = 0;
		}
	}

	q->pkt_first_seg = first_seg;
	q->pkt_last_seg = last_seg;

	return nb_rcv;
}

static inline void tx_free_descriptors(struct fm10k_tx_queue *q)
{
	uint16_t next_rs, count = 0;

	next_rs = fifo_peek(&q->rs_tracker);
	if (!(q->hw_ring[next_rs].flags & FM10K_TXD_FLAG_DONE))
		return;

	/* the DONE flag is set on this descriptor so remove the ID
	 * from the RS bit tracker and free the buffers */
	fifo_remove(&q->rs_tracker);

	/* wrap around? if so, free buffers from last_free up to but NOT
	 * including nb_desc */
	if (q->last_free > next_rs) {
		count = q->nb_desc - q->last_free;
		while (q->last_free < q->nb_desc) {
			rte_pktmbuf_free_seg(q->sw_ring[q->last_free]);
			q->sw_ring[q->last_free] = NULL;
			++q->last_free;
		}
		q->last_free = 0;
	}

	/* adjust free descriptor count before the next loop */
	q->nb_free += count + (next_rs + 1 - q->last_free);

	/* free buffers from last_free, up to and including next_rs */
	while (q->last_free <= next_rs) {
		rte_pktmbuf_free_seg(q->sw_ring[q->last_free]);
		q->sw_ring[q->last_free] = NULL;
		++q->last_free;
	}

	if (q->last_free == q->nb_desc)
		q->last_free = 0;
}

static inline void tx_xmit_pkt(struct fm10k_tx_queue *q, struct rte_mbuf *mb)
{
	uint16_t last_id;
	uint8_t flags;

	/* always set the LAST flag on the last descriptor used to
	 * transmit the packet */
	flags = FM10K_TXD_FLAG_LAST;
	last_id = q->next_free + mb->nb_segs - 1;
	if (last_id >= q->nb_desc)
		last_id = last_id - q->nb_desc;

	/* but only set the RS flag on the last descriptor if rs_thresh
	 * descriptors will be used since the RS flag was last set */
	if ((q->nb_used + mb->nb_segs) >= q->rs_thresh) {
		flags |= FM10K_TXD_FLAG_RS;
		fifo_insert(&q->rs_tracker, last_id);
		q->nb_used = 0;
	} else {
		q->nb_used = q->nb_used + mb->nb_segs;
	}

	q->nb_free -= mb->nb_segs;

	q->hw_ring[q->next_free].flags = 0;
	/* set checksum flags on first descriptor of packet. SCTP checksum
	 * offload is not supported, but we do not explicitly check for this
	 * case in favor of greatly simplified processing. */
	if (mb->ol_flags & (PKT_TX_IP_CKSUM | PKT_TX_L4_MASK))
		q->hw_ring[q->next_free].flags |= FM10K_TXD_FLAG_CSUM;

	/* set vlan if requested */
	if (mb->ol_flags & PKT_TX_VLAN_PKT)
		q->hw_ring[q->next_free].vlan = mb->vlan_tci;

	q->sw_ring[q->next_free] = mb;
	q->hw_ring[q->next_free].buffer_addr =
			rte_cpu_to_le_64(MBUF_DMA_ADDR(mb));
	q->hw_ring[q->next_free].buflen =
			rte_cpu_to_le_16(rte_pktmbuf_data_len(mb));
	if (++q->next_free == q->nb_desc)
		q->next_free = 0;

	/* fill up the rings */
	for (mb = mb->next; mb != NULL; mb = mb->next) {
		q->sw_ring[q->next_free] = mb;
		q->hw_ring[q->next_free].buffer_addr =
				rte_cpu_to_le_64(MBUF_DMA_ADDR(mb));
		q->hw_ring[q->next_free].buflen =
				rte_cpu_to_le_16(rte_pktmbuf_data_len(mb));
		q->hw_ring[q->next_free].flags = 0;
		if (++q->next_free == q->nb_desc)
			q->next_free = 0;
	}

	q->hw_ring[last_id].flags = flags;
}

uint16_t
fm10k_xmit_pkts(void *tx_queue, struct rte_mbuf **tx_pkts,
	uint16_t nb_pkts)
{
	struct fm10k_tx_queue *q = tx_queue;
	struct rte_mbuf *mb;
	uint16_t count;

	for (count = 0; count < nb_pkts; ++count) {
		mb = tx_pkts[count];

		/* running low on descriptors? try to free some... */
		if (q->nb_free < q->free_thresh)
			tx_free_descriptors(q);

		/* make sure there are enough free descriptors to transmit the
		 * entire packet before doing anything */
		if (q->nb_free < mb->nb_segs)
			break;

		/* sanity check to make sure the mbuf is valid */
		if ((mb->nb_segs == 0) ||
		    ((mb->nb_segs > 1) && (mb->next == NULL)))
			break;

		/* process the packet */
		tx_xmit_pkt(q, mb);
	}

	/* update the tail pointer if any packets were processed */
	if (likely(count > 0))
		FM10K_PCI_REG_WRITE(q->tail_ptr, q->next_free);

	return count;
}
