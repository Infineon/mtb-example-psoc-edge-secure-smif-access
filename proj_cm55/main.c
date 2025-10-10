/*******************************************************************************
* File Name        : main.c
*
* Description      : This source file contains the main routine for CM55 CPU
*
* Related Document : See README.md
*
********************************************************************************
 (c) 2023-2025, Infineon Technologies AG, or an affiliate of Infineon Technologies AG. All rights reserved.
 This software, associated documentation and materials ("Software") is owned by
 Infineon Technologies AG or one of its affiliates ("Infineon") and is protected
 by and subject to worldwide patent protection, worldwide copyright laws, and
 international treaty provisions. Therefore, you may use this Software only as
 provided in the license agreement accompanying the software package from which
 you obtained this Software. If no license agreement applies, then any use,
 reproduction, modification, translation, or compilation of this Software is
 prohibited without the express written permission of Infineon.
 Disclaimer: UNLESS OTHERWISE EXPRESSLY AGREED WITH INFINEON, THIS SOFTWARE
 IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING,
 BUT NOT LIMITED TO, ALL WARRANTIES OF NON-INFRINGEMENT OF THIRD-PARTY RIGHTS AND
 IMPLIED WARRANTIES SUCH AS WARRANTIES OF FITNESS FOR A SPECIFIC USE/PURPOSE OR
 MERCHANTABILITY. Infineon reserves the right to make changes to the Software
 without notice. You are responsible for properly designing, programming, and
 testing the functionality and safety of your intended application of the
 Software, as well as complying with any legal requirements related to its
 use. Infineon does not guarantee that the Software will be free from intrusion,
 data theft or loss, or other breaches ("Security Breaches"), and Infineon
 shall have no liability arising out of any Security Breaches. Unless otherwise
 explicitly approved by Infineon, the Software may not be used in any application
 where a failure of the Product or any consequences of the use thereof can
 reasonably be expected to result in personal injury.

*******************************************************************************/

/*******************************************************************************
* Header File
*******************************************************************************/

#include "cybsp.h"
#include "mtb_serial_memory.h"
#include "retarget_io_init.h"

#define PACKET_SIZE (64u)

_MTB_SRF_DATA_ALIGN uint8_t rx_buf[PACKET_SIZE];
_MTB_SRF_DATA_ALIGN uint8_t tx_buf[PACKET_SIZE];
_MTB_SRF_DATA_ALIGN const cy_stc_smif_mem_config_t* mem_config;
_MTB_SRF_DATA_ALIGN mtb_serial_memory_t serial_memory_obj;
_MTB_SRF_DATA_ALIGN cy_stc_smif_mem_context_t smif_mem_context;
_MTB_SRF_DATA_ALIGN cy_stc_smif_mem_info_t smif_mem_info;

uint32_t test_start_address = 0;


/********************************************************************************
 * Function Name: non_secure_memory_write
 ********************************************************************************
 * Summary:
 * Read and write to non secure ( MPC) region from CM55 application
 *
 * Parameters:
 *  void
 *
 * Return:
 *  void
 *
 *******************************************************************************/
cy_rslt_t non_secure_memory_write(mtb_serial_memory_t* serial_memory_obj)
{
    cy_rslt_t result;    
    size_t sector_size;
    test_start_address = CYMEM_CM33_0_test_nvm_ns_OFFSET;
    
    memset(rx_buf, 0, PACKET_SIZE);
    
    printf("\r\n*****************************************************\r\n");
    printf("\r\nAccessing non-secure memory from CM55 \r\n");
    printf("\r\n*****************************************************\r\n");
      
    sector_size = mtb_serial_memory_get_erase_size(serial_memory_obj, 
                                                   test_start_address);
          
    printf("\r\n1. Erasing %d bytes from offset address 0x%"PRIx32"\r\n", 
                                        sector_size, test_start_address);

    result = mtb_serial_memory_erase(serial_memory_obj, 
                                     test_start_address, 
                                     sector_size);

    check_status("Erasing memory failed", result);
    
    /*Read back to verify erase*/
    result = mtb_serial_memory_read(serial_memory_obj, test_start_address, PACKET_SIZE, rx_buf);
    check_status("Reading memory failed", result);

    for (uint32_t index = 0; index < PACKET_SIZE; index++)
    {
       CY_ASSERT(rx_buf[index] == 0xFFU);
    }
 
    /*prepare write buffer*/
    for (uint32_t index = 0; index < PACKET_SIZE; index++)
    {
        tx_buf[index] = (uint8_t)index;
    }
            
            
    printf("\r\n2. Writing data to non secure memory at offset 0x%"PRIx32"\r\n",test_start_address) ;
    result = mtb_serial_memory_write(serial_memory_obj, test_start_address, PACKET_SIZE, tx_buf);
    check_status("Writing to memory failed", result);
        
    print_array("Write data", tx_buf, PACKET_SIZE);

    /* Read back after Write for verification */
    printf("\r\n3. Reading back for verification\r\n");
    result = mtb_serial_memory_read(serial_memory_obj, test_start_address, PACKET_SIZE, rx_buf);
    CY_ASSERT(result == CY_RSLT_SUCCESS);
    check_status("Read data does not match with written data. "
                     "Read/Write operation failed.",
                       memcmp(tx_buf, rx_buf, PACKET_SIZE));
        
    print_array("Read buffer", rx_buf, PACKET_SIZE);
     
     memset(rx_buf, 0, PACKET_SIZE);
     
    /*Check XIP access*/   
    mem_config = smif0MemConfigs[0]; 
    printf("\r\n4. Reading back for XIP verification\r\n");    
    for (uint32_t index = 0; index < PACKET_SIZE; index++)
    {
        rx_buf[index] = CY_GET_REG8(GET_ALIAS_ADDRESS(mem_config->baseAddress + test_start_address + index));    
    }
    
    check_status("Read data does not match with written data. "
                     "Read/Write operation failed.",
                       memcmp(tx_buf, rx_buf, PACKET_SIZE));
    print_array("Read buffer", rx_buf, PACKET_SIZE);
        
    return result;
}

/********************************************************************************
 * Function Name: secure_memory_write
 ********************************************************************************
 * Summary:
 *  Read and write to secure ( MPC) region from CM55 application
 *
 * Parameters:
 *  void
 *
 * Return:
 *  void
 *
 *******************************************************************************/
cy_rslt_t secure_memory_write(mtb_serial_memory_t* serial_memory_obj)
{
    cy_rslt_t result;
    size_t sector_size;
    test_start_address = CYMEM_CM33_0_S_test_nvm_sec_OFFSET;
    
    memset(rx_buf, 0, PACKET_SIZE);
    
    printf("\r\n*****************************************************\r\n");
    printf("\r\nAccessing secure memory from CM55 \r\n");
    printf("\r\n*****************************************************\r\n");

    sector_size = mtb_serial_memory_get_erase_size(serial_memory_obj, 
                                                   test_start_address);

    printf("\r\n1. Erasing %d bytes from offset address 0x%"PRIx32"\r\n", 
                                        sector_size, test_start_address);
                                        
    result = mtb_serial_memory_erase(serial_memory_obj, 
                                     test_start_address, 
                                     sector_size);


    printf("\r\nErasing secure memory from CM55 not allowed !!!\r\n") ;
    
    CY_ASSERT(result == CY_SMIF_SECURITY_POLICY_VIOLATION);
    
    printf("\r\n2. Reading data from CM55 at offset 0x%"PRIx32"\r\n",test_start_address) ;
    
    result = mtb_serial_memory_read(serial_memory_obj, test_start_address, PACKET_SIZE, rx_buf);
    
    CY_ASSERT(result == CY_SMIF_SECURITY_POLICY_VIOLATION);
    
    printf("\r\nRead secure memory from CM55 not allowed !!!\r\n") ;
         
    for (uint32_t index = 0; index < PACKET_SIZE; index++)
    {
        tx_buf[index] = (uint8_t)index;
    }
 
    printf("\r\n3. Writing data to CM55 at offset 0x%"PRIx32"\r\n",test_start_address) ;            
    result = mtb_serial_memory_write(serial_memory_obj, test_start_address, PACKET_SIZE, tx_buf);
    CY_ASSERT(result == CY_SMIF_SECURITY_POLICY_VIOLATION);
    
    printf("\r\nWrite to secure memory from CM55 not allowed !!!\r\n") ;
        
    return result;
}

/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
* This is the main function for CM55 application. 
* 
* It demonstrates how to set up and access the secure SMIF peripheral from the  
* CM55 application. 
* 
* Parameters:
*  void
*
* Return:
*  int
*
*******************************************************************************/
int main(void)
{
    cy_rslt_t result;

    /* Initialize the device and board peripherals. */
    result = cybsp_init();

    /* Board init failed. Stop program execution. */
    if (CY_RSLT_SUCCESS != result)
    {
        /* Disable all interrupts. */
        __disable_irq();

        CY_ASSERT(0);

        /* Infinite loop */
        while(true);
    }

    /* Enable global interrupts. */
    __enable_irq();
    
        /* Initialize retarget-io middleware */
    init_retarget_io();

    /*Initialize smif from non secure domain*/ 
    result = mtb_serial_memory_setup_nonsecure(&serial_memory_obj, 
                                               MTB_SERIAL_MEMORY_CHIP_SELECT_1, 
                                               SMIF0_CORE, 
                                               &smif_mem_context,
                                               &smif_mem_info);
                                               
    
    check_status("Serial memory setup failed", result);
                                    
    //Non-secure memory access
    non_secure_memory_write(&serial_memory_obj);
    
    //secure memory access
    secure_memory_write(&serial_memory_obj);

    /* Put the CPU to Deep Sleep. */
    for (;;)
    {
        Cy_SysPm_CpuEnterDeepSleep(CY_SYSPM_WAIT_FOR_INTERRUPT);
    }
}

/* [] END OF FILE */
