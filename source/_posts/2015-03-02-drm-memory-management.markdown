---
layout: post
title: "DRM memory management - 最好的GEM/TTM/PRIME解释"
date: 2015-03-02 10:37
comments: true
categories: linux-kernel repost
---

本文转载自：[https://www.kernel.org/doc/htmldocs/drm/drm-memory-management.html](https://www.kernel.org/doc/htmldocs/drm/drm-memory-management.html)

**Memory management**

- The Translation Table Manager (TTM)
- The Graphics Execution Manager (GEM)
- VMA Offset Manager
- PRIME Buffer Sharing
- PRIME Function References
- DRM MM Range Allocator
- DRM MM Range Allocator Function References
- CMA Helper Functions Reference

<!-- more -->

Modern Linux systems require large amount of graphics memory to store frame buffers, textures, vertices and other graphics-related data. Given the very dynamic nature of many of that data, managing graphics memory efficiently is thus crucial for the graphics stack and plays a central role in the DRM infrastructure.

The DRM core includes two memory managers, namely Translation Table Maps (TTM) and Graphics Execution Manager (GEM). TTM was the first DRM memory manager to be developed and tried to be a one-size-fits-them all solution. It provides a single userspace API to accommodate the need of all hardware, supporting both Unified Memory Architecture (UMA) devices and devices with dedicated video RAM (i.e. most discrete video cards). This resulted in a large, complex piece of code that turned out to be hard to use for driver development.

GEM started as an Intel-sponsored project in reaction to TTM's complexity. Its design philosophy is completely different: instead of providing a solution to every graphics memory-related problems, GEM identified common code between drivers and created a support library to share it. GEM has simpler initialization and execution requirements than TTM, but has no video RAM management capabilities and is thus limited to UMA devices.

**The Translation Table Manager (TTM)**

TTM initialization

Drivers wishing to support TTM must fill out a drm_bo_driver structure. The structure contains several fields with function pointers for initializing the TTM, allocating and freeing memory, waiting for command completion and fence synchronization, and memory migration. See the radeon_ttm.c file for an example of usage.

The ttm_global_reference structure is made up of several fields:

``` cpp
struct ttm_global_reference {
	enum ttm_global_types global_type;
	size_t size;
	void *object;
	int (*init) (struct ttm_global_reference *);
	void (*release) (struct ttm_global_reference *);
};
```
        
There should be one global reference structure for your memory manager as a whole, and there will be others for each object created by the memory manager at runtime. Your global TTM should have a type of TTM_GLOBAL_TTM_MEM. The size field for the global object should be sizeof(struct ttm_mem_global), and the init and release hooks should point at your driver-specific init and release routines, which probably eventually call ttm_mem_global_init and ttm_mem_global_release, respectively.

Once your global TTM accounting structure is set up and initialized by calling ttm_global_item_ref() on it, you need to create a buffer object TTM to provide a pool for buffer object allocation by clients and the kernel itself. The type of this object should be TTM_GLOBAL_TTM_BO, and its size should be sizeof(struct ttm_bo_global). Again, driver-specific init and release functions may be provided, likely eventually calling ttm_bo_global_init() and ttm_bo_global_release(), respectively. Also, like the previous object, ttm_global_item_ref() is used to create an initial reference count for the TTM, which will call your initialization function.

**The Graphics Execution Manager (GEM)**

GEM Initialization
GEM Objects Creation
GEM Objects Lifetime
GEM Objects Naming
GEM Objects Mapping
Memory Coherency
Command Execution
GEM Function Reference

The GEM design approach has resulted in a memory manager that doesn't provide full coverage of all (or even all common) use cases in its userspace or kernel API. GEM exposes a set of standard memory-related operations to userspace and a set of helper functions to drivers, and let drivers implement hardware-specific operations with their own private API.

The GEM userspace API is described in the GEM - the Graphics Execution Manager article on LWN. While slightly outdated, the document provides a good overview of the GEM API principles. Buffer allocation and read and write operations, described as part of the common GEM API, are currently implemented using driver-specific ioctls.

GEM is data-agnostic. It manages abstract buffer objects without knowing what individual buffers contain. APIs that require knowledge of buffer contents or purpose, such as buffer allocation or synchronization primitives, are thus outside of the scope of GEM and must be implemented using driver-specific ioctls.

On a fundamental level, GEM involves several operations:

- Memory allocation and freeing
- Command execution
- Aperture management at command execution time

Buffer object allocation is relatively straightforward and largely provided by Linux's shmem layer, which provides memory to back each object.

Device-specific operations, such as command execution, pinning, buffer read & write, mapping, and domain ownership transfers are left to driver-specific ioctls.

**GEM Initialization**

Drivers that use GEM must set the DRIVER_GEM bit in the struct drm_driver driver_features field. The DRM core will then automatically initialize the GEM core before calling the load operation. Behind the scene, this will create a DRM Memory Manager object which provides an address space pool for object allocation.

In a KMS configuration, drivers need to allocate and initialize a command ring buffer following core GEM initialization if required by the hardware. UMA devices usually have what is called a "stolen" memory region, which provides space for the initial framebuffer and large, contiguous memory regions required by the device. This space is typically not managed by GEM, and must be initialized separately into its own DRM MM object.

**GEM Objects Creation**

GEM splits creation of GEM objects and allocation of the memory that backs them in two distinct operations.

GEM objects are represented by an instance of struct drm_gem_object. Drivers usually need to extend GEM objects with private information and thus create a driver-specific GEM object structure type that embeds an instance of struct drm_gem_object.

To create a GEM object, a driver allocates memory for an instance of its specific GEM object type and initializes the embedded struct drm_gem_object with a call to drm_gem_object_init. The function takes a pointer to the DRM device, a pointer to the GEM object and the buffer object size in bytes.

GEM uses shmem to allocate anonymous pageable memory. drm_gem_object_init will create an shmfs file of the requested size and store it into the struct drm_gem_object filp field. The memory is used as either main storage for the object when the graphics hardware uses system memory directly or as a backing store otherwise.

Drivers are responsible for the actual physical pages allocation by calling shmem_read_mapping_page_gfp for each page. Note that they can decide to allocate pages when initializing the GEM object, or to delay allocation until the memory is needed (for instance when a page fault occurs as a result of a userspace memory access or when the driver needs to start a DMA transfer involving the memory).

Anonymous pageable memory allocation is not always desired, for instance when the hardware requires physically contiguous system memory as is often the case in embedded devices. Drivers can create GEM objects with no shmfs backing (called private GEM objects) by initializing them with a call to drm_gem_private_object_init instead of drm_gem_object_init. Storage for private GEM objects must be managed by drivers.

Drivers that do not need to extend GEM objects with private information can call the drm_gem_object_alloc function to allocate and initialize a struct drm_gem_object instance. The GEM core will call the optional driver gem_init_object operation after initializing the GEM object with drm_gem_object_init.

int (*gem_init_object) (struct drm_gem_object *obj);

No alloc-and-init function exists for private GEM objects.

**GEM Objects Lifetime**

All GEM objects are reference-counted by the GEM core. References can be acquired and release by calling drm_gem_object_reference and drm_gem_object_unreference respectively. The caller must hold the drm_device struct_mutex lock. As a convenience, GEM provides the drm_gem_object_reference_unlocked and drm_gem_object_unreference_unlocked functions that can be called without holding the lock.

When the last reference to a GEM object is released the GEM core calls the drm_driver gem_free_object operation. That operation is mandatory for GEM-enabled drivers and must free the GEM object and all associated resources.

void (*gem_free_object) (struct drm_gem_object *obj);

Drivers are responsible for freeing all GEM object resources, including the resources created by the GEM core. If an mmap offset has been created for the object (in which case drm_gem_object::map_list::map is not NULL) it must be freed by a call to drm_gem_free_mmap_offset. The shmfs backing store must be released by calling drm_gem_object_release (that function can safely be called if no shmfs backing store has been created).

**GEM Objects Naming**

Communication between userspace and the kernel refers to GEM objects using local handles, global names or, more recently, file descriptors. All of those are 32-bit integer values; the usual Linux kernel limits apply to the file descriptors.

GEM handles are local to a DRM file. Applications get a handle to a GEM object through a driver-specific ioctl, and can use that handle to refer to the GEM object in other standard or driver-specific ioctls. Closing a DRM file handle frees all its GEM handles and dereferences the associated GEM objects.

To create a handle for a GEM object drivers call drm_gem_handle_create. The function takes a pointer to the DRM file and the GEM object and returns a locally unique handle. When the handle is no longer needed drivers delete it with a call to drm_gem_handle_delete. Finally the GEM object associated with a handle can be retrieved by a call to drm_gem_object_lookup.

Handles don't take ownership of GEM objects, they only take a reference to the object that will be dropped when the handle is destroyed. To avoid leaking GEM objects, drivers must make sure they drop the reference(s) they own (such as the initial reference taken at object creation time) as appropriate, without any special consideration for the handle. For example, in the particular case of combined GEM object and handle creation in the implementation of the dumb_create operation, drivers must drop the initial reference to the GEM object before returning the handle.

GEM names are similar in purpose to handles but are not local to DRM files. They can be passed between processes to reference a GEM object globally. Names can't be used directly to refer to objects in the DRM API, applications must convert handles to names and names to handles using the DRM_IOCTL_GEM_FLINK and DRM_IOCTL_GEM_OPEN ioctls respectively. The conversion is handled by the DRM core without any driver-specific support.

GEM also supports buffer sharing with dma-buf file descriptors through PRIME. GEM-based drivers must use the provided helpers functions to implement the exporting and importing correctly. See the section called “PRIME Buffer Sharing”. Since sharing file descriptors is inherently more secure than the easily guessable and global GEM names it is the preferred buffer sharing mechanism. Sharing buffers through GEM names is only supported for legacy userspace. Furthermore PRIME also allows cross-device buffer sharing since it is based on dma-bufs.

**GEM Objects Mapping**

Because mapping operations are fairly heavyweight GEM favours read/write-like access to buffers, implemented through driver-specific ioctls, over mapping buffers to userspace. However, when random access to the buffer is needed (to perform software rendering for instance), direct access to the object can be more efficient.

The mmap system call can't be used directly to map GEM objects, as they don't have their own file handle. Two alternative methods currently co-exist to map GEM objects to userspace. The first method uses a driver-specific ioctl to perform the mapping operation, calling do_mmap under the hood. This is often considered dubious, seems to be discouraged for new GEM-enabled drivers, and will thus not be described here.

The second method uses the mmap system call on the DRM file handle.

``` cpp
void *mmap(void *addr, size_t length, int prot, int flags, int fd,
             off_t offset);
```

DRM identifies the GEM object to be mapped by a fake offset passed through the mmap offset argument. Prior to being mapped, a GEM object must thus be associated with a fake offset. To do so, drivers must call drm_gem_create_mmap_offset on the object. The function allocates a fake offset range from a pool and stores the offset divided by PAGE_SIZE in obj->map_list.hash.key. Care must be taken not to call drm_gem_create_mmap_offset if a fake offset has already been allocated for the object. This can be tested by obj->map_list.map being non-NULL.

Once allocated, the fake offset value (obj->map_list.hash.key << PAGE_SHIFT) must be passed to the application in a driver-specific way and can then be used as the mmap offset argument.

The GEM core provides a helper method drm_gem_mmap to handle object mapping. The method can be set directly as the mmap file operation handler. It will look up the GEM object based on the offset value and set the VMA operations to the drm_driver gem_vm_ops field. Note that drm_gem_mmap doesn't map memory to userspace, but relies on the driver-provided fault handler to map pages individually.

To use drm_gem_mmap, drivers must fill the struct drm_driver gem_vm_ops field with a pointer to VM operations.

``` cpp
struct vm_operations_struct *gem_vm_ops

  struct vm_operations_struct {
          void (*open)(struct vm_area_struct * area);
          void (*close)(struct vm_area_struct * area);
          int (*fault)(struct vm_area_struct *vma, struct vm_fault *vmf);
  };
```

The open and close operations must update the GEM object reference count. Drivers can use the drm_gem_vm_open and drm_gem_vm_close helper functions directly as open and close handlers.

The fault operation handler is responsible for mapping individual pages to userspace when a page fault occurs. Depending on the memory allocation scheme, drivers can allocate pages at fault time, or can decide to allocate memory for the GEM object at the time the object is created.

Drivers that want to map the GEM object upfront instead of handling page faults can implement their own mmap file operation handler.

**Memory Coherency**

When mapped to the device or used in a command buffer, backing pages for an object are flushed to memory and marked write combined so as to be coherent with the GPU. Likewise, if the CPU accesses an object after the GPU has finished rendering to the object, then the object must be made coherent with the CPU's view of memory, usually involving GPU cache flushing of various kinds. This core CPU<->GPU coherency management is provided by a device-specific ioctl, which evaluates an object's current domain and performs any necessary flushing or synchronization to put the object into the desired coherency domain (note that the object may be busy, i.e. an active render target; in that case, setting the domain blocks the client and waits for rendering to complete before performing any necessary flushing operations).

**Command Execution**

Perhaps the most important GEM function for GPU devices is providing a command execution interface to clients. Client programs construct command buffers containing references to previously allocated memory objects, and then submit them to GEM. At that point, GEM takes care to bind all the objects into the GTT, execute the buffer, and provide necessary synchronization between clients accessing the same buffers. This often involves evicting some objects from the GTT and re-binding others (a fairly expensive operation), and providing relocation support which hides fixed GTT offsets from clients. Clients must take care not to submit command buffers that reference more objects than can fit in the GTT; otherwise, GEM will reject them and no rendering will occur. Similarly, if several objects in the buffer require fence registers to be allocated for correct rendering (e.g. 2D blits on pre-965 chips), care must be taken not to require more fence registers than are available to the client. Such resource management should be abstracted from the client in libdrm.

**GEM Function Reference**

VMA Offset Manager

``` cpp
drm_vma_offset_manager_init — Initialize new offset-manager
drm_vma_offset_manager_destroy — Destroy offset manager
drm_vma_offset_lookup — Find node in offset space
drm_vma_offset_lookup_locked — Find node in offset space
drm_vma_offset_add — Add offset node to manager
drm_vma_offset_remove — Remove offset node from manager
drm_vma_node_allow — Add open-file to list of allowed users
drm_vma_node_revoke — Remove open-file from list of allowed users
drm_vma_node_is_allowed — Check whether an open-file is granted access
drm_vma_offset_exact_lookup — Look up node by exact address
drm_vma_offset_lock_lookup — Lock lookup for extended private use
drm_vma_offset_unlock_lookup — Unlock lookup for extended private use
drm_vma_node_reset — Initialize or reset node object
drm_vma_node_start — Return start address for page-based addressing
drm_vma_node_size — Return size (page-based)
drm_vma_node_has_offset — Check whether node is added to offset manager
drm_vma_node_offset_addr — Return sanitized offset for user-space mmaps
drm_vma_node_unmap — Unmap offset node
drm_vma_node_verify_access — Access verification helper for TTM
```

The vma-manager is responsible to map arbitrary driver-dependent memory regions into the linear user address-space. It provides offsets to the caller which can then be used on the address_space of the drm-device. It takes care to not overlap regions, size them appropriately and to not confuse mm-core by inconsistent fake vm_pgoff fields. Drivers shouldn't use this for object placement in VMEM. This manager should only be used to manage mappings into linear user-space VMs.

We use drm_mm as backend to manage object allocations. But it is highly optimized for alloc/free calls, not lookups. Hence, we use an rb-tree to speed up offset lookups.

You must not use multiple offset managers on a single address_space. Otherwise, mm-core will be unable to tear down memory mappings as the VM will no longer be linear.

This offset manager works on page-based addresses. That is, every argument and return code (with the exception of drm_vma_node_offset_addr) is given in number of pages, not number of bytes. That means, object sizes and offsets must always be page-aligned (as usual). If you want to get a valid byte-based user-space address for a given offset, please see drm_vma_node_offset_addr.

Additionally to offset management, the vma offset manager also handles access management. For every open-file context that is allowed to access a given node, you must call drm_vma_node_allow. Otherwise, an mmap call on this open-file with the offset of the node will fail with -EACCES. To revoke access again, use drm_vma_node_revoke. However, the caller is responsible for destroying already existing mappings, if required.

**PRIME Buffer Sharing**

Overview and Driver Interface
PRIME Helper Functions
PRIME is the cross device buffer sharing framework in drm, originally created for the OPTIMUS range of multi-gpu platforms. To userspace PRIME buffers are dma-buf based file descriptors.

**Overview and Driver Interface**

Similar to GEM global names, PRIME file descriptors are also used to share buffer objects across processes. They offer additional security: as file descriptors must be explicitly sent over UNIX domain sockets to be shared between applications, they can't be guessed like the globally unique GEM names.

Drivers that support the PRIME API must set the DRIVER_PRIME bit in the struct drm_driver driver_features field, and implement the prime_handle_to_fd and prime_fd_to_handle operations.

``` cpp
int (*prime_handle_to_fd)(struct drm_device *dev,
                          struct drm_file *file_priv, uint32_t handle,
                          uint32_t flags, int *prime_fd);
int (*prime_fd_to_handle)(struct drm_device *dev,
                          struct drm_file *file_priv, int prime_fd,
                          uint32_t *handle);
```

Those two operations convert a handle to a PRIME file descriptor and vice versa. Drivers must use the kernel dma-buf buffer sharing framework to manage the PRIME file descriptors. Similar to the mode setting API PRIME is agnostic to the underlying buffer object manager, as long as handles are 32bit unsigned integers.

While non-GEM drivers must implement the operations themselves, GEM drivers must use the drm_gem_prime_handle_to_fd and drm_gem_prime_fd_to_handle helper functions. Those helpers rely on the driver gem_prime_export and gem_prime_import operations to create a dma-buf instance from a GEM object (dma-buf exporter role) and to create a GEM object from a dma-buf instance (dma-buf importer role).

``` cpp
struct dma_buf * (*gem_prime_export)(struct drm_device *dev,
                             struct drm_gem_object *obj,
                             int flags);
struct drm_gem_object * (*gem_prime_import)(struct drm_device *dev,
                                            struct dma_buf *dma_buf);
```

These two operations are mandatory for GEM drivers that support PRIME.

**PRIME Helper Functions**

Drivers can implement gem_prime_export and gem_prime_import in terms of simpler APIs by using the helper functions drm_gem_prime_export and drm_gem_prime_import. These functions implement dma-buf support in terms of five lower-level driver callbacks:

Export callbacks:

``` cpp
gem_prime_pin (optional): prepare a GEM object for exporting
gem_prime_get_sg_table: provide a scatter/gather table of pinned pages
gem_prime_vmap: vmap a buffer exported by your driver
gem_prime_vunmap: vunmap a buffer exported by your driver
```

Import callback:

```cpp
gem_prime_import_sg_table (import): produce a GEM object from another driver's scatter/gather table
```

**PRIME Function References**

``` cpp
drm_gem_dmabuf_release — dma_buf release implementation for GEM
drm_gem_prime_export — helper library implementation of the export callback
drm_gem_prime_handle_to_fd — PRIME export function for GEM drivers
drm_gem_prime_import — helper library implementation of the import callback
drm_gem_prime_fd_to_handle — PRIME import function for GEM drivers
drm_prime_pages_to_sg — converts a page array into an sg list
drm_prime_sg_to_page_addr_arrays — convert an sg table into a page array
drm_prime_gem_destroy — helper to clean up a PRIME-imported GEM object
```

**DRM MM Range Allocator**

Overview
LRU Scan/Eviction Support
Overview

drm_mm provides a simple range allocator. The drivers are free to use the resource allocator from the linux core if it suits them, the upside of drm_mm is that it's in the DRM core. Which means that it's easier to extend for some of the crazier special purpose needs of gpus.

The main data struct is drm_mm, allocations are tracked in drm_mm_node. Drivers are free to embed either of them into their own suitable datastructures. drm_mm itself will not do any allocations of its own, so if drivers choose not to embed nodes they need to still allocate them themselves.

The range allocator also supports reservation of preallocated blocks. This is useful for taking over initial mode setting configurations from the firmware, where an object needs to be created which exactly matches the firmware's scanout target. As long as the range is still free it can be inserted anytime after the allocator is initialized, which helps with avoiding looped depencies in the driver load sequence.

drm_mm maintains a stack of most recently freed holes, which of all simplistic datastructures seems to be a fairly decent approach to clustering allocations and avoiding too much fragmentation. This means free space searches are O(num_holes). Given that all the fancy features drm_mm supports something better would be fairly complex and since gfx thrashing is a fairly steep cliff not a real concern. Removing a node again is O(1).

drm_mm supports a few features: Alignment and range restrictions can be supplied. Further more every drm_mm_node has a color value (which is just an opaqua unsigned long) which in conjunction with a driver callback can be used to implement sophisticated placement restrictions. The i915 DRM driver uses this to implement guard pages between incompatible caching domains in the graphics TT.

Two behaviors are supported for searching and allocating: bottom-up and top-down. The default is bottom-up. Top-down allocation can be used if the memory area has different restrictions, or just to reduce fragmentation.

Finally iteration helpers to walk all nodes and all holes are provided as are some basic allocator dumpers for debugging.

**LRU Scan/Eviction Support**

Very often GPUs need to have continuous allocations for a given object. When evicting objects to make space for a new one it is therefore not most efficient when we simply start to select all objects from the tail of an LRU until there's a suitable hole: Especially for big objects or nodes that otherwise have special allocation constraints there's a good chance we evict lots of (smaller) objects unecessarily.

The DRM range allocator supports this use-case through the scanning interfaces. First a scan operation needs to be initialized with drm_mm_init_scan or drm_mm_init_scan_with_range. The the driver adds objects to the roaster (probably by walking an LRU list, but this can be freely implemented) until a suitable hole is found or there's no further evitable object.

The the driver must walk through all objects again in exactly the reverse order to restore the allocator state. Note that while the allocator is used in the scan mode no other operation is allowed.

Finally the driver evicts all objects selected in the scan. Adding and removing an object is O(1), and since freeing a node is also O(1) the overall complexity is O(scanned_objects). So like the free stack which needs to be walked before a scan operation even begins this is linear in the number of objects. It doesn't seem to hurt badly.

**DRM MM Range Allocator Function References**

``` cpp
drm_mm_reserve_node — insert an pre-initialized node
drm_mm_insert_node_generic — search for space and insert node
drm_mm_insert_node_in_range_generic — ranged search for space and insert node
drm_mm_remove_node — Remove a memory node from the allocator.
drm_mm_replace_node — move an allocation from old to new
drm_mm_init_scan — initialize lru scanning
drm_mm_init_scan_with_range — initialize range-restricted lru scanning
drm_mm_scan_add_block — add a node to the scan list
drm_mm_scan_remove_block — remove a node from the scan list
drm_mm_clean — checks whether an allocator is clean
drm_mm_init — initialize a drm-mm allocator
drm_mm_takedown — clean up a drm_mm allocator
drm_mm_debug_table — dump allocator state to dmesg
drm_mm_dump_table — dump allocator state to a seq_file
drm_mm_node_allocated — checks whether a node is allocated
drm_mm_initialized — checks whether an allocator is initialized
drm_mm_hole_node_start — computes the start of the hole following node
drm_mm_hole_node_end — computes the end of the hole following node
drm_mm_for_each_node — iterator to walk over all allocated nodes
drm_mm_for_each_hole — iterator to walk over all holes
drm_mm_insert_node — search for space and insert node
drm_mm_insert_node_in_range — ranged search for space and insert node
```

**CMA Helper Functions Reference**

``` cpp
drm_gem_cma_create — allocate an object with the given size
drm_gem_cma_free_object — free resources associated with a CMA GEM object
drm_gem_cma_dumb_create_internal — create a dumb buffer object
drm_gem_cma_dumb_create — create a dumb buffer object
drm_gem_cma_dumb_map_offset — return the fake mmap offset for a CMA GEM object
drm_gem_cma_mmap — memory-map a CMA GEM object
drm_gem_cma_describe — describe a CMA GEM object for debugfs
drm_gem_cma_prime_get_sg_table — provide a scatter/gather table of pinned pages for a CMA GEM object
drm_gem_cma_prime_import_sg_table — produce a CMA GEM object from another driver's scatter/gather table of pinned pages
drm_gem_cma_prime_mmap — memory-map an exported CMA GEM object
drm_gem_cma_prime_vmap — map a CMA GEM object into the kernel's virtual address space
drm_gem_cma_prime_vunmap — unmap a CMA GEM object from the kernel's virtual address space
struct drm_gem_cma_object — GEM object backed by CMA memory allocations
```

The Contiguous Memory Allocator reserves a pool of memory at early boot that is used to service requests for large blocks of contiguous memory.

The DRM GEM/CMA helpers use this allocator as a means to provide buffer objects that are physically contiguous in memory. This is useful for display drivers that are unable to map scattered buffers via an IOMMU.Memory management
