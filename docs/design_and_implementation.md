[Click here](../README.md) to view the README.

## Design and implementation

The design of this application is minimalistic to get started with code examples on PSOC&trade; Edge MCU devices. All PSOC&trade; Edge E84 MCU applications have a dual-CPU three-project structure to develop code for the CM33 and CM55 cores. The CM33 core has two separate projects for the secure processing environment (SPE) and non-secure processing environment (NSPE). A project folder consists of various subfolders, each denoting a specific aspect of the project. The three project folders are as follows:

**Table 1. Application projects**

Project | Description
--------|------------------------
*proj_cm33_s* | Project for CM33 secure processing environment (SPE)
*proj_cm33_ns* | Project for CM33 non-secure processing environment (NSPE)
*proj_cm55* | CM55 project

<br>

In this code example, at device reset, the boot process starts from the ROM boot with the secure enclave (SE) as the root of trust (RoT). From the secure enclave, the boot flow is passed on to the system CPU subsystem where the secure CM33 application starts.

Secure CM33 application performs the SMIF0 setup and initializes "MemNum" required for access to secure SMIF from non-secure domain. Secure application then applies the peripheral protection configurations as per the device configurations, which makes the SMIF0 peripheral secure. See the Peripheral Protection Controllers security configurations applied by this code example under the **System** tab of the Device Configurator.

> **Note:** Configure the following SMIF0 peripherals as 'secure' to enable the SRF integration. See the *cy_smif_srf.h* file for implementation details.

- SMIF0_CORE_MAIN 
- SMIF0_CORE_CRYPTO
- SMIF0_CORE_MAIN2
- SMIF0_CORE_DEVICE

In this code example, two memory regions are used "test_nvm_sec" region, which is in the secure MPC region and "test_nvm_ns" region, which is in the non-secure MPC region. 

The CM33 non-secure application demonstrates how to successfully access the "test_nvm_ns" region of the secure SMIF peripheral using the Secure Request Framework. Access to "test_nvm_sec" from non-secure application is not allowed.

The CM55 non-secure application demonstrates how to successfully access the "test_nvm_ns" region of the secure SMIF peripheral using the Secure Request Framework. Access to "test_nvm_sec" from non-secure application is not allowed.

<br>