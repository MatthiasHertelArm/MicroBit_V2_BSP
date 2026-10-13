#ifndef REGIONS_MICROBIT_V2_H
#define REGIONS_MICROBIT_V2_H


//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

// <n>Device pack: NordicSemiconductor::nRF_DeviceFamilyPack@8.44.1
// <i>Device pack used to generate this file

// <h>ROM Configuration
// =======================
// <h> __ROM0 (is rx memory: IROM1 from DFP)
//   <o> Base address <0x0-0xFFFFFFFF:8>
//   <i> Defines base address of memory region.
//   <i> Default: 0x00000000
#define __ROM0_BASE 0x00000000
//   <o> Region size [bytes] <0x0-0xFFFFFFFF:8>
//   <i> Defines size of memory region.
//   <i> Default: 0x00080000
#define __ROM0_SIZE 0x00080000
// </h>

// </h>

// <h>RAM Configuration
// =======================
// <h> __RAM0 (is rwx memory: IRAM1 from DFP)
//   <o> Base address <0x0-0xFFFFFFFF:8>
//   <i> Defines base address of memory region.
//   <i> Default: 0x20000000
#define __RAM0_BASE 0x20000000
//   <o> Region size [bytes] <0x0-0xFFFFFFFF:8>
//   <i> Defines size of memory region.
//   <i> Default: 0x00020000
#define __RAM0_SIZE 0x00020000
// </h>

// </h>

// <h>Stack / Heap Configuration
//   <o0> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
//   <o1> Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
#define __STACK_SIZE 0x00000200
#define __HEAP_SIZE 0x00000C00
// </h>


#endif /* REGIONS_MICROBIT_V2_H */
