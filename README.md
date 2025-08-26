# OpenFlash

**OpenFlash** is an Open and Agile Developing and Evaluating Platform for Solid State Drive (SSD), which is developed by the storage group in the Institute of Computing Technology, Chinese Academy of Sciences. 

## 1. Main Components

OpenFlash mainly consists of 
- FPGA-based SSD controller
- FTL (Flash Translation Layer) firmware
- AIC-formed evaluation board with attached NAND chips.

## 2. Hardware Specifications

The current specifications of OpenFlash are as follows.

### SSD Controller
- **FPGA**: Xilinx XCKU15P-2ffva1156e
- **ARM core**: 125MHz
- **RISC-V core**: 62.5MHz
- **Microcode** for each ONFI channel

### Memory
- **SRAM**: 2MB
- **DDR4 DRAM**: 2~4GB
- **Speed**: 1600MT/s

### NAND Chips
- **Interface**: ONFI 4
- **Configuration**: 8 channel, 4 CE
- **Capacity**: 2TB (512GB per NAND chip)

### Host Interfaces
- PCIe Gen3x4 End-Point (AIC form)

### Debug Interfaces
- 1 Xilinx download JTAG interface
- 1 ARM/RISCV JTAG interface
- 1 UART interface


## 3. Evaluation Board

The AIC-formed evaluation board is shown below.

<img src="https://github.com/user-attachments/assets/8499c03c-0439-4414-adf5-b5f8cc70fcd7" alt="OpenFlash AIC EVB" width="60%">


## 4. Performance

The table below shows the performance of OpenFlash using fio. 

| IO modes | Bandwidth（GB/s） | IOPS (K) | Latency（us，QD=1，jobs=1） |
|---------|---------|---------|---------|
| 64KB Seq. Write  | 1.8  | 27.7  | 43  |
| 64KB Seq. Read  | 2.1  | 33.5  | 258  |
| 4KB Rand. Write  | 1.1  | 275  | 7  |
| 4KB Rand. Read  | 1.5  | 367  | 109  |

## 5. How to Use OpenFlash

If you are interested in using OpenFlash for you research work, please feel free to contact：jiangdejun@ict.ac.cn or qinxiaowan@ict.ac.cn. We will provide you the OpenFlash evaluation board as soon as possible.



