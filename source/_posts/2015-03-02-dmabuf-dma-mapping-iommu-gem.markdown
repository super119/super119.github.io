---
layout: post
title: "有关DMABUF/DMA Mapping/IOMMU/CMA/TTM/GEM/GART/SMMU"
date: 2015-03-02 11:06
comments: true
categories: linux-kernel
---

1. DMABUF can be used as a wrapper to encapsulate other memory management frameworks. All these memory management framework(I mean mostly for graphics), buffer is the keypoint. DMABUF defines a standard buffer structure. So DMABUF can be used as a wrapper for TTM/GEM/Android ION... and etc. Notice DMABUF can't replace these things, cause it doesn't cover everything. E.g: DMABUF has no userspace interfaces, right now only kernel interfaces(can be used in device driver).

2. Kernel has DMA mapping API from origin. ARM defines IOMMU which can be used to connect scattered physical memory as a continuous region for devices which needs continue address to work(e.g: DMA). So IOMMU implementations & CMA should work behind kernel DMA mapping API. E.g: dma_alloc_from_contiguous can be implemented by CMA; dma_alloc_coherent can be implemented by IOMMU or by the normal case(just call __get_free_pages). So for device drivers need dma buffers, we should use dma mapping APIs, not call iommu api directly.

3. For tegra, GART & SMMU can be used to implement IOMMU apis.
