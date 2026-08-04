/****************************************************************************
 * drivers/vhost/vhost-net.c
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Device-role virtio network driver ("vhost-net"): implements the DEVICE
 * end of a virtio-net link, so a peer processor running a stock virtio-net
 * DRIVER (e.g. Linux via remoteproc/rproc-virtio) sees this side as a
 * network card.  Registers a NuttX netdev lowerhalf (ethN).
 *
 * Ring layout is fixed by the peer driver's point of view:
 *   vq[0] = peer driver RX queue: the peer posts empty buffers; WE fill
 *           them to transmit toward the peer.
 *   vq[1] = peer driver TX queue: the peer posts filled buffers; WE
 *           harvest them as our receive path.
 *
 * No virtio-net features are negotiated (the resource table advertises
 * none), so every packet is prefixed by the legacy 10-byte
 * struct virtio_net_hdr with all fields zero (gso_type NONE).
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <debug.h>
#include <string.h>
#include <sys/param.h>

#include <nuttx/kmalloc.h>
#include <nuttx/net/netdev_lowerhalf.h>
#include <nuttx/vhost/vhost.h>

#include "vhost-net.h"

/* Peer buffers are referenced by 64-bit guest physical addresses that may
 * exceed the CPU's direct reach; map them through an arch-provided
 * translation window.  On AM67 this is the R5F RAT sliding window, which
 * is also mapped non-cacheable so no cache maintenance is needed here.
 *
 * TODO: replace this direct arch hook with a proper interface before
 * upstreaming.
 */

#ifdef CONFIG_ARCH_CHIP_AM67
FAR void *am67_rat_map(uint64_t pa, FAR size_t *avail);
#  define vhost_net_map(pa, avl) am67_rat_map((pa), (avl))
#else
static inline FAR void *vhost_net_map(uint64_t pa, FAR size_t *avail)
{
  if (avail != NULL)
    {
      *avail = SIZE_MAX;
    }

  return (FAR void *)(uintptr_t)pa;
}
#endif

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Queue indices (peer driver's numbering, see file header) */

#define VHOST_NET_PEER_RXQ   0    /* our transmit lane */
#define VHOST_NET_PEER_TXQ   1    /* our receive lane */
#define VHOST_NET_NUM        2

/* Legacy struct virtio_net_hdr (no VIRTIO_NET_F_MRG_RXBUF): flags(1) +
 * gso_type(1) + hdr_len(2) + gso_size(2) + csum_start(2) + csum_offset(2)
 */

#define VHOST_NET_HDRSIZE    10

/* netpkt quota per direction and the longest peer descriptor chain we
 * accept on receive (Linux commonly splits header and payload).
 */

#define VHOST_NET_NPKTS      8
#define VHOST_NET_MAXCHAIN   8

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct vhost_net_priv_s
{
  struct netdev_lowerhalf_s lower;      /* Must be first for casts */
  FAR struct vhost_device  *hdev;
  FAR struct virtqueue     *txq;        /* peer RX ring (we fill) */
  FAR struct virtqueue     *rxq;        /* peer TX ring (we drain) */
};

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static int vhost_net_ifup(FAR struct netdev_lowerhalf_s *dev);
static int vhost_net_ifdown(FAR struct netdev_lowerhalf_s *dev);
static int vhost_net_transmit(FAR struct netdev_lowerhalf_s *dev,
                              FAR netpkt_t *pkt);
static FAR netpkt_t *vhost_net_receive(FAR struct netdev_lowerhalf_s *dev);
static int vhost_net_probe(FAR struct vhost_device *hdev);
static void vhost_net_remove(FAR struct vhost_device *hdev);

/****************************************************************************
 * Private Data
 ****************************************************************************/

static const struct netdev_ops_s g_vhost_net_ops =
{
  vhost_net_ifup,      /* ifup */
  vhost_net_ifdown,    /* ifdown */
  vhost_net_transmit,  /* transmit */
  vhost_net_receive,   /* receive */
#ifdef CONFIG_NET_MCASTGROUP
  NULL,                /* addmac */
  NULL,                /* rmmac */
#endif
#ifdef CONFIG_NETDEV_IOCTL
  NULL,                /* ioctl */
#endif
  NULL                 /* reclaim */
};

static struct vhost_driver g_vhost_net_driver =
{
  LIST_INITIAL_VALUE(g_vhost_net_driver.node),  /* node */
  VIRTIO_ID_NETWORK,                            /* device id */
  vhost_net_probe,                              /* probe */
  vhost_net_remove,                             /* remove */
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: vhost_net_rxready / vhost_net_txdone
 *
 * Description:
 *   Virtqueue kick callbacks (run in the transport's notification
 *   context, thread level).  A kick on the peer TX ring means frames
 *   await harvest; a kick on the peer RX ring means the peer replenished
 *   the buffers we transmit into.  Just poke the upper half; all ring
 *   work happens in transmit()/receive() on the netdev work thread.
 *
 ****************************************************************************/

static void vhost_net_rxready(FAR struct virtqueue *vq)
{
  FAR struct vhost_net_priv_s *priv = vq->vq_dev->priv;

  netdev_lower_rxready(&priv->lower);
}

static void vhost_net_txdone(FAR struct virtqueue *vq)
{
  FAR struct vhost_net_priv_s *priv = vq->vq_dev->priv;

  netdev_lower_txdone(&priv->lower);
}

/****************************************************************************
 * Name: vhost_net_ifup / vhost_net_ifdown
 ****************************************************************************/

static int vhost_net_ifup(FAR struct netdev_lowerhalf_s *dev)
{
  netdev_lower_carrier_on(dev);
  return OK;
}

static int vhost_net_ifdown(FAR struct netdev_lowerhalf_s *dev)
{
  netdev_lower_carrier_off(dev);
  return OK;
}

/****************************************************************************
 * Name: vhost_net_transmit
 *
 * Description:
 *   Fill one peer-posted RX buffer with the frame and complete it.
 *   Copy-based: the peer buffer lives in shared memory and is returned
 *   immediately, so the netpkt can be freed here (synchronous
 *   completion).
 *
 ****************************************************************************/

static int vhost_net_transmit(FAR struct netdev_lowerhalf_s *dev,
                              FAR netpkt_t *pkt)
{
  FAR struct vhost_net_priv_s *priv = (FAR struct vhost_net_priv_s *)dev;
  struct vhost_buf_s vb[1];
  unsigned int len;
  unsigned int pos;
  size_t cnt;
  int head;

  head = vhost_get_vq_buffers_pa(priv->txq, vb, nitems(vb), &cnt);
  if (head < 0)
    {
      /* Peer has not posted buffers (yet); retry on the next kick */

      virtqueue_enable_cb(priv->txq);
      return -ENOBUFS;
    }

  len = netpkt_getdatalen(dev, pkt);
  if (len + VHOST_NET_HDRSIZE > vb[0].len)
    {
      /* Frame cannot fit the peer's buffer: complete it empty (drop) */

      vhosterr("frame %u exceeds peer buffer %u, dropped\n",
               len, vb[0].len);
      len = 0;
    }
  else
    {
      /* Serialize the zero header + frame into the peer buffer through
       * the translation window, honoring window-boundary splits.
       */

      for (pos = 0; pos < len + VHOST_NET_HDRSIZE; )
        {
          size_t avail;
          FAR uint8_t *dst = vhost_net_map(vb[0].addr + pos, &avail);
          unsigned int chunk = MIN(len + VHOST_NET_HDRSIZE - pos, avail);

          if (pos < VHOST_NET_HDRSIZE)
            {
              unsigned int hz = MIN(VHOST_NET_HDRSIZE - pos, chunk);

              memset(dst, 0, hz);
              if (chunk > hz)
                {
                  netpkt_copyout(dev, dst + hz, pkt, chunk - hz, 0);
                }
            }
          else
            {
              netpkt_copyout(dev, dst, pkt, chunk,
                             pos - VHOST_NET_HDRSIZE);
            }

          pos += chunk;
        }
    }

  virtqueue_add_consumed_buffer(priv->txq, head,
                                len ? len + VHOST_NET_HDRSIZE : 0);
  virtqueue_kick(priv->txq);

  netpkt_free(dev, pkt, NETPKT_TX);
  netdev_lower_txdone(dev);
  return OK;
}

/****************************************************************************
 * Name: vhost_net_receive
 *
 * Description:
 *   Harvest one frame (possibly a descriptor chain) from the peer TX
 *   ring, copy it into a fresh netpkt (stripping the virtio-net header)
 *   and return the buffers to the peer.
 *
 ****************************************************************************/

static FAR netpkt_t *vhost_net_receive(FAR struct netdev_lowerhalf_s *dev)
{
  FAR struct vhost_net_priv_s *priv = (FAR struct vhost_net_priv_s *)dev;
  struct vhost_buf_s vb[VHOST_NET_MAXCHAIN];
  FAR netpkt_t *pkt = NULL;
  unsigned int total = 0;
  unsigned int skip = VHOST_NET_HDRSIZE;
  int offset = 0;
  size_t cnt;
  size_t i;
  int head;

  head = vhost_get_vq_buffers_pa(priv->rxq, vb, nitems(vb), &cnt);
  if (head < 0)
    {
      virtqueue_enable_cb(priv->rxq);
      return NULL;
    }

  for (i = 0; i < cnt; i++)
    {
      total += vb[i].len;
    }

  if (total > skip)
    {
      pkt = netpkt_alloc(dev, NETPKT_RX);
    }

  if (pkt != NULL &&
      netpkt_setdatalen(dev, pkt, total - skip) < total - skip)
    {
      vhosterr("rx dropped: cannot size netpkt to %u\n", total - skip);
      netpkt_free(dev, pkt, NETPKT_RX);
      pkt = NULL;
    }

  if (pkt != NULL)
    {
      for (i = 0; i < cnt; i++)
        {
          uint64_t pa = vb[i].addr;
          uint32_t blen = vb[i].len;

          if (skip > 0)
            {
              uint32_t d = MIN(skip, blen);

              pa   += d;
              blen -= d;
              skip -= d;
            }

          /* Copy through the translation window, honoring
           * window-boundary splits.
           */

          while (blen > 0)
            {
              size_t avail;
              FAR const uint8_t *src = vhost_net_map(pa, &avail);
              uint32_t chunk = MIN(blen, avail);

              netpkt_copyin(dev, pkt, src, chunk, offset);
              offset += chunk;
              pa     += chunk;
              blen   -= chunk;
            }
        }
    }
  else
    {
      vhosterr("rx dropped: total=%u (no netpkt)\n", total);
    }

  /* Hand the buffers back to the peer either way */

  virtqueue_add_consumed_buffer(priv->rxq, head, total);
  virtqueue_kick(priv->rxq);

  return pkt;
}

/****************************************************************************
 * Name: vhost_net_probe
 ****************************************************************************/

static int vhost_net_probe(FAR struct vhost_device *hdev)
{
  FAR struct vhost_net_priv_s *priv;
  FAR const char *vqnames[VHOST_NET_NUM];
  vq_callback callbacks[VHOST_NET_NUM];
  FAR uint8_t *mac;
  int ret;

  priv = kmm_zalloc(sizeof(*priv));
  if (priv == NULL)
    {
      return -ENOMEM;
    }

  priv->hdev = hdev;
  hdev->priv = priv;

  vqnames[VHOST_NET_PEER_RXQ]   = "vhost_net_peer_rx";
  vqnames[VHOST_NET_PEER_TXQ]   = "vhost_net_peer_tx";
  callbacks[VHOST_NET_PEER_RXQ] = vhost_net_txdone;
  callbacks[VHOST_NET_PEER_TXQ] = vhost_net_rxready;
  ret = vhost_create_virtqueues(hdev, 0, VHOST_NET_NUM, vqnames,
                                callbacks, NULL);
  if (ret < 0)
    {
      vhosterr("vhost_create_virtqueues failed, ret=%d\n", ret);
      goto err_with_priv;
    }

  priv->txq = hdev->vrings_info[VHOST_NET_PEER_RXQ].vq;
  priv->rxq = hdev->vrings_info[VHOST_NET_PEER_TXQ].vq;

  priv->lower.quota[NETPKT_RX] = VHOST_NET_NPKTS;
  priv->lower.quota[NETPKT_TX] = VHOST_NET_NPKTS;
  priv->lower.ops = &g_vhost_net_ops;

  /* Fixed locally-administered MAC for the device side */

  mac = priv->lower.netdev.d_mac.ether.ether_addr_octet;
  mac[0] = 0x02;
  mac[1] = 0x54;
  mac[2] = 0x33;
  mac[3] = 0x00;
  mac[4] = 0x00;
  mac[5] = 0x01;

  ret = netdev_lower_register(&priv->lower, NET_LL_ETHERNET);
  if (ret < 0)
    {
      vhosterr("netdev_lower_register failed, ret=%d\n", ret);
      goto err_with_vqs;
    }

  return OK;

err_with_vqs:
  vhost_delete_virtqueues(hdev);
err_with_priv:
  kmm_free(priv);
  hdev->priv = NULL;
  return ret;
}

/****************************************************************************
 * Name: vhost_net_remove
 ****************************************************************************/

static void vhost_net_remove(FAR struct vhost_device *hdev)
{
  FAR struct vhost_net_priv_s *priv = hdev->priv;

  netdev_lower_unregister(&priv->lower);
  vhost_delete_virtqueues(hdev);
  kmm_free(priv);
  hdev->priv = NULL;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: vhost_register_net_driver
 ****************************************************************************/

int vhost_register_net_driver(void)
{
  return vhost_register_driver(&g_vhost_net_driver);
}
