#include "include/acpi_override.h"

platform_mpam_cfg acpi_override_cfg = {

    // Cache Count
    17,
   {
    //System Cache
    {
        //MPIDR
        0xFFFFFFFFFFFFFFFF,
        1024,             //Line size
        2048,           //Cache size
        {0, 0, 0, 0},   //neighbours
        {0x100000, 0x2},  //Node Info
        {0, 0, 0, 0},   //Attributes
        0x0,            //Base Address
        0x0,            //not_ready_us
        {0, 0, 0, 1},      //Interrupt info
    },



    //Cluster-0
    {
        //MPIDR
        0x0000000000000000,
        1024,             //Line size
        2048,           //Cache size
        {0, 0, 0, 0},   //neighbours
        {0x100000, 0x1},//Node Info
        {0, 0, 0, 0},   //Attributes
        0x0,            //Base Address
        0x0,            //not_ready_us
        {0, 0, 0, 1},      //Interrupt info
    },


    //Cluster-1
    {
        //MPIDR
        0x0000000000000100,
        1024,             //Line size
        2048,           //Cache size
        {0, 0, 0, 0},   //neighbours
        {0, 0x1},       //Node Info
        {0, 0, 0, 0},   //Attributes
        0x0,            //Base Address
        0x0,            //not_ready_us
        {0, 0, 0, 1},      //Interrupt info
    },




    //Cluster-2
    {
        //MPIDR
        0x0000000000000200,
        1024,             //Line size
        2048,           //Cache size
        {0, 0, 0, 0},   //neighbours
        {0, 0x1},       //Node Info
        {0, 0, 0, 0},   //Attributes
        0x0,            //Base Address
        0x0,            //not_ready_us
        {0, 0, 0, 1},      //Interrupt info
    },

    //Cluster-3
    {
        //MPIDR
        0x0000000000000300,
        1024,             //Line size
        2048,           //Cache size
        {0, 0, 0, 0},   //neighbours
        {0, 0x1},       //Node Info
        {0, 0, 0, 0},   //Attributes
        0x0,            //Base Address
        0x0,            //not_ready_us
        {0, 0, 0, 1},      //Interrupt info
    },



    //Cluster-4
    {
        //MPIDR
        0x0000000000010000,
        1024,             //Line size
        2048,           //Cache size
        {0, 0, 0, 0},   //neighbours
        {0, 0x1},       //Node Info
        {0, 0, 0, 0},   //Attributes
        0x0,            //Base Address
        0x0,            //not_ready_us
        {0, 0, 0, 1},      //Interrupt info
    },

    //Cluster-5
    {
        //MPIDR
        0x0000000000010100,
        1024,             //Line size
        2048,           //Cache size
        {0, 0, 0, 0},   //neighbours
        {0, 0x1},       //Node Info
        {0, 0, 0, 0},   //Attributes
        0x0,            //Base Address
        0x0,            //not_ready_us
        {0, 0, 0, 1},      //Interrupt info
    },

    //Cluster-6
    {
        //MPIDR
        0x0000000000010200,
        1024,             //Line size
        2048,           //Cache size
        {0, 0, 0, 0},   //neighbours
        {0, 0x1},       //Node Info
        {0, 0, 0, 0},   //Attributes
        0x0,            //Base Address
        0x0,            //not_ready_us
        {0, 0, 0, 1},      //Interrupt info
    },



    //Cluster-7
    {
        //MPIDR
        0x0000000000010300,
        1024,             //Line size
        2048,           //Cache size
        {0, 0, 0, 0},   //neighbours
        {0, 0x1},       //Node Info
        {0, 0, 0, 0},   //Attributes
        0x0,            //Base Address
        0x0,            //not_ready_us
        {0, 0, 0, 1},      //Interrupt info
    },






    //PE-0
    {
        //MPIDR
        0x0000000000000000,
        1024,             //Line size
        2048,           //Cache size
        {0, 0, 0, 0},   //neighbours
        {0x100000, 0x0},//Node Info
        {2, 1, 1, 0},   //Attributes
        0x0,            //Base Address
        0x0,            //not_ready_us
        {0, 0, 0, 1},      //Interrupt info
    },


    //PE-1
    {
        //MPIDR
        0x0000000000000100,
        1024,             //Line size
        2048,           //Cache size
        {0, 0, 0, 0},   //neighbours
        {0, 0x0},       //Node Info
        {0, 0, 0, 0},   //Attributes
        0x0,            //Base Address
        0x0,            //not_ready_us
        {0, 0, 0, 1},      //Interrupt info
    },




    //PE-2
    {
        //MPIDR
        0x0000000000000200,
        1024,             //Line size
        2048,           //Cache size
        {0, 0, 0, 0},   //neighbours
        {0, 0x0},       //Node Info
        {0, 0, 0, 0},   //Attributes
        0x0,            //Base Address
        0x0,            //not_ready_us
        {0, 0, 0, 1},      //Interrupt info
    },

    //PE-3
    {
        //MPIDR
        0x0000000000000300,
        1024,             //Line size
        2048,           //Cache size
        {0, 0, 0, 0},   //neighbours
        {0, 0x0},       //Node Info
        {0, 0, 0, 0},   //Attributes
        0x0,            //Base Address
        0x0,            //not_ready_us
        {0, 0, 0, 1},      //Interrupt info
    },



    //PE-4
    {
        //MPIDR
        0x0000000000010000,
        1024,             //Line size
        2048,           //Cache size
        {0, 0, 0, 0},   //neighbours
        {0, 0x0},       //Node Info
        {0, 0, 0, 0},   //Attributes
        0x0,            //Base Address
        0x0,            //not_ready_us
        {0, 0, 0, 1},      //Interrupt info
    },

    //PE-5
    {
        //MPIDR
        0x0000000000010100,
        1024,             //Line size
        2048,           //Cache size
        {0, 0, 0, 0},   //neighbours
        {0, 0x0},       //Node Info
        {0, 0, 0, 0},   //Attributes
        0x0,            //Base Address
        0x0,            //not_ready_us
        {0, 0, 0, 1},      //Interrupt info
    },

    //PE-6
    {
        //MPIDR
        0x0000000000010200,
        1024,             //Line size
        2048,           //Cache size
        {0, 0, 0, 0},   //neighbours
        {0, 0x0},       //Node Info
        {0, 0, 0, 0},   //Attributes
        0x0,            //Base Address
        0x0,            //not_ready_us
        {0, 0, 0, 1},      //Interrupt info
    },



    //PE-7
    {
        //MPIDR
        0x0000000000010300,
        1024,             //Line size
        2048,           //Cache size
        {0, 0, 0, 0},   //neighbours
        {0, 0x0},       //Node Info
        {0, 0, 0, 0},   //Attributes
        0x0,            //Base Address
        0x0,            //not_ready_us
        {0, 0, 0, 1},      //Interrupt info

    },

   },

    2,   // Number of Memory Nodes

    {

       {
         5,          // Proximity Domain
         0x123456,   // Base Address
         0x1024,    // Length
         10,         // Flags
         0x11111,    // Hardware register address
         0x11111,    // Not Ready Max
         {0,0,0,1},  // Intr Info
       },

       {
         6,          // Proximity Domain
         0x12345,    // Base Address
         0x1024,    // Length
         20,         // Flags
         0x22222,    // Hardware register address
         0x22222,    // Not Ready Max
         {0,0,0,1},  // Intr Info
       },
    }

};

