//***************************************************************************************/
// 
// File name: Photoneo_PhoXi-3D-Scanner_MXSP4.cpp  
//
// Synopsis:  This program contains an example of 3d reconstruction by interfacing with
//            a Photoneo Phoxi scanner.
//            See the PrintHeader() function below for detailed description.
//
// Copyright © 1992-2024 Zebra Technologies Corp. and/or its affiliates
// All Rights Reserved
//***************************************************************************************/

#include <mil.h>
#include <vector>

#include "MainTemplate.h"

// Once the PhoxiControl SDK is installed and the project is configured correctly (see
// PrintHeader()), set PHOXICONTROLSDK_INSTALLED to 1 to enable the Phoxi-specific code.
// You need an actual Phoxi connected to your computer.
#define PHOXICONTROLSDK_INSTALLED  0

//****************************************************************************
// Example description.
//****************************************************************************
void PrintHeader()   
   {
   MosPrintf(MIL_TEXT("[EXAMPLE NAME]\n")
             MIL_TEXT("Photoneo_PhoXi-3D-Scanner_MXSP4\n\n")

             MIL_TEXT("[SYNOPSIS]\n")
             MIL_TEXT("This program acquires a 3d point cloud using an Photoneo Phoxi sensor\n")
             MIL_TEXT("with the PhoxiControl API. It then converts the point cloud to the MIL\n")
             MIL_TEXT("format and displays the result.\n\n")

             MIL_TEXT("[MODULES USED]\n")
             MIL_TEXT("Modules used: application, system, buffer, 3D display,\n")
             MIL_TEXT("              3D graphicd, 3D processing.\n\n"));
   }

//*****************************************************************************
// Phoxi-specific header, constants
//*****************************************************************************

#if PHOXICONTROLSDK_INSTALLED

   #if defined(WIN32)
      #error "PhoXi api cannot be used in a 32-bit application."
   #endif

// The project settings might need be modified so that the libs are reachable.
// Corresponding 2 DLLs need to be copied in the same folder where the executables are.
   #if _DEBUG
      #pragma comment(lib, "PhoXi_API_msvc14_Debug_1.2.14.lib")
   #else
      #pragma comment(lib, "PhoXi_API_msvc14_Release_1.2.14.lib")
   #endif

// Remove warnings that are given when their head file is compiled
#ifdef _WIN32  
	#undef _WIN32
	#pragma warning(push, 2)

// The project settings might need be modified so that this header is reachable.
	#include <Phoxi.h>

	#pragma warning(pop)
	#define _WIN32
#else
	#include <Phoxi.h>
#endif

   // Timeout related to acquisition time.
   static const int RECEIVE_TIMEOUT = 5000;  // in ms

   // Singleton phoxi device.
   struct SPhoXiObjects
      {
      pho::api::PPhoXi PhoXiDevice;
      };
    


//*****************************************************************************
// Helper function prototypes
//*****************************************************************************


// 3d camera interface functions.
template <> bool Init(SPhoXiObjects* pSensor3d);
template <> bool SetupForExample(SPhoXiObjects* pSensor3d);
template <> bool Grab(SPhoXiObjects* pSensor3d, MIL_ID MilContainer);
template <> bool Free(SPhoXiObjects* pSensor3d);

// Function for example code.
template <class T>
void ExtractPhoXiComponent(MIL_ID MilSystem, MIL_ID MilContainer, MIL_INT ComponentIdFlag, T& rPhoXiComponent,
                           MIL_INT OutNbBands, MIL_INT OutSizeX, MIL_INT OutSizeY, MIL_INT OutType, MIL_INT OutAttribute, MIL_INT InFormat);

#endif
//*****************************************************************************
// Main.
//*****************************************************************************
int MosMain()
   {
   PrintHeader();

#if !PHOXICONTROLSDK_INSTALLED
   MosPrintf(MIL_TEXT("This example is designed to be used with an Photoneo PhoXi sensor and\n"));
   MosPrintf(MIL_TEXT("the PhoXi API. To run the example:\n"));
   MosPrintf(MIL_TEXT("\n"));
   MosPrintf(MIL_TEXT("- Install the PhoXi Control application.\n"));
   MosPrintf(MIL_TEXT("\n"));
   MosPrintf(MIL_TEXT("- Follow the PhoXi Control Quick Start Guide:\n"));
   MosPrintf(MIL_TEXT("  - Connect the PhoXi sensor to your computer.\n"));
   MosPrintf(MIL_TEXT("  - Run the PhoXi Control application.\n"));
   MosPrintf(MIL_TEXT("  - Connect to the PhoXi discovered by the application.\n"));
   MosPrintf(MIL_TEXT("  - If in FreeRun, put the acquisition to SoftwareTrigger (Trigger Scan).\n"));
   MosPrintf(MIL_TEXT("  - If not started, start the acquisition.\n"));
   MosPrintf(MIL_TEXT("  - Press the SoftTrig button to verify that it acquires correctly.\n"));
   MosPrintf(MIL_TEXT("\n"));
   MosPrintf(MIL_TEXT("- Configure the properties of the camera until a satisfactory point cloud is generated.\n"));
   MosPrintf(MIL_TEXT("-\n"));

   MosPrintf(MIL_TEXT("- An environment variable named Phoxi_control_path should exist, with its value corresponding\n"));
   MosPrintf(MIL_TEXT("  to the path of the Photoneo_PhoXi-3D-Scanner_MXSP4Control folder.\n"));
   MosPrintf(MIL_TEXT("\n"));
   MosPrintf(MIL_TEXT("- Copy the shared libraries to your system path. Under Windows, copy\n"));
   MosPrintf(MIL_TEXT("    %%Phoxi_control_path%%\\Api\\bin\\PhoXi_API_msvc14_Release_1.2.14\n"));
   MosPrintf(MIL_TEXT("    %%Phoxi_control_path%%\\Api\\bin\\PhoXi_API_msvc14_Debug_1.2.14\n"));
   MosPrintf(MIL_TEXT("  to\n"));
   MosPrintf(MIL_TEXT("    %%SYSTEMROOT%%\\System32\\   (usually C:\\Windows\\System32\\)\n"));
   MosPrintf(MIL_TEXT("\n"));
   MosPrintf(MIL_TEXT("- Add the paths to the header files and library files of the Photoneo_PhoXi-3D-Scanner_MXSP4 SDK to\n"));
   MosPrintf(MIL_TEXT("  the example project files. If you are using Visual Studio, open the Property\n"));
   MosPrintf(MIL_TEXT("  Pages of the Photoneo_PhoXi-3D-Scanner_MXSP4 project. Then, under Configuration Properties,\n"));
   MosPrintf(MIL_TEXT("  you must:\n"));
   MosPrintf(MIL_TEXT("  - Add\n"));
   MosPrintf(MIL_TEXT("      $(Phoxi_control_path)\\API\\include\n"));
   MosPrintf(MIL_TEXT("    to\n"));
   MosPrintf(MIL_TEXT("      C/C++->General->Additional Include Directories\n"));
   MosPrintf(MIL_TEXT("  - Add \n"));
   MosPrintf(MIL_TEXT("      $(Phoxi_control_path)\\API\\lib\n"));
   MosPrintf(MIL_TEXT("    to \n"));
   MosPrintf(MIL_TEXT("      Linker->General->Additional Library Directories\n"));
   MosPrintf(MIL_TEXT("\n"));
   
   MosPrintf(MIL_TEXT("- Update the example code:\n"));
   MosPrintf(MIL_TEXT("  - Set the PHOXICONTROLSDK_INSTALLED define to 1.\n"));
   MosPrintf(MIL_TEXT("  - Recompile the example.\n"));
   MosPrintf(MIL_TEXT("\n\n"));
   MosPrintf(MIL_TEXT("The example has been tested with the following setups:\n"));
   MosPrintf(MIL_TEXT("- Windows 7 SP1 64-bit and VS2017.\n"));
   MosPrintf(MIL_TEXT("- MIL X Version 1911 (build 4892) 64-bit, SP4 build 405 EA.\n"));
   MosPrintf(MIL_TEXT("- PhoXi Control application (Phoxi Control v1.1.62- WIn64(7, 10)).\n"));
   MosPrintf(MIL_TEXT("\n"));
   MosPrintf(MIL_TEXT("Press <Enter> to end.\n"));
   MosGetch();
#else
   MosPrintf(MIL_TEXT("Press <Enter> to start.\n"));
   MosGetch();

   SPhoXiObjects Sensor3d;
   Interface3dCameraExample(&Sensor3d, 0);
   
#endif
   return 0;
   }

#if PHOXICONTROLSDK_INSTALLED

//*****************************************************************************
// Creates and initializes the PhoXi.
//*****************************************************************************
template <> bool Init(SPhoXiObjects* p3dSensor)
   {
   // Create the factory.
   pho::api::PhoXiFactory Factory;

   //Check if the PhoXi Control Software is running
   if(!Factory.isPhoXiControlRunning())
      {
      MosPrintf(MIL_TEXT("PhoXi: PhoXi Control Software is not running.\n\n"));
      return false;
      }
   else
      MosPrintf(MIL_TEXT("PhoXi: PhoXi Control Software is running.\n"));

   // Get List of available devices on the network.
   std::vector< pho::api::PhoXiDeviceInformation > DeviceList = Factory.GetDeviceList();
   MosPrintf(MIL_TEXT("PhoXi: PhoXi Factory found %i devices.\n"), DeviceList.size());

   // Try to connect Device opened in PhoXi Control.
   p3dSensor->PhoXiDevice = Factory.CreateAndConnectFirstAttached();

   if (!p3dSensor->PhoXiDevice || !p3dSensor->PhoXiDevice->isConnected())
      {
      MosPrintf(MIL_TEXT("PhoXi: Unable to connect to first PhoXi device.\n\n"));
      return false;
      }

   if (p3dSensor->PhoXiDevice->isAcquiring())
      p3dSensor->PhoXiDevice->StopAcquisition();

   // Setup the acquisition.
   p3dSensor->PhoXiDevice->TriggerMode = pho::api::PhoXiTriggerMode::Software;
   p3dSensor->PhoXiDevice->ClearBuffer();
   return true;
   }

//*****************************************************************************
// Setup the Phoxi for the example.
//*****************************************************************************
template <> bool SetupForExample(SPhoXiObjects* p3dSensor)
   {
   p3dSensor->PhoXiDevice->OutputSettings->SendPointCloud = true;
   p3dSensor->PhoXiDevice->OutputSettings->SendTexture = true;
   return true;
   }

//*****************************************************************************
// Frees the device.
//*****************************************************************************
template <> bool Free(SPhoXiObjects* p3dSensor)
	{
   // Stop the acquisition
   if(!p3dSensor->PhoXiDevice->StopAcquisition())
      {
      MosPrintf(MIL_TEXT("PhoXi: Unable to stop the acquisition.\n"));
      return false;
      }

   // Disconnect the device.
   if(!p3dSensor->PhoXiDevice->Disconnect())
      {
      MosPrintf(MIL_TEXT("PhoXi: Unable to disconnect.\n"));
      return false;
      }

   return true;
	}

//*****************************************************************************
// Triggers and acquires the data and put it in a MIL container.
//*****************************************************************************
template <> bool Grab(SPhoXiObjects* p3dSensor, MIL_ID MilContainer)
   {
   bool Success = false;

   // Start the acquisition.
   p3dSensor->PhoXiDevice->StartAcquisition();

   if(p3dSensor->PhoXiDevice->isAcquiring())
      {
      // Send the trigger.
      int FrameID = p3dSensor->PhoXiDevice->TriggerFrame();
      if(FrameID < 0)
         {
         MosPrintf(MIL_TEXT("PhoXi: Trigger was unsuccessful!\n"));
         }
      else
         {
         MosPrintf(MIL_TEXT("PhoXi: Acquiring 3d point cloud data... "));
         pho::api::PFrame Frame = p3dSensor->PhoXiDevice->GetFrame(RECEIVE_TIMEOUT);
         if(Frame)
            {
            MosPrintf(MIL_TEXT("OK.\n\n"));

            // Allocate the component buffers if necessary.
            MIL_ID MilSystem = MbufInquire(MilContainer, M_OWNER_SYSTEM, M_NULL);

            // Get the point cloud component.
            ExtractPhoXiComponent(MilSystem, MilContainer, M_COMPONENT_RANGE,
                                  Frame->PointCloud, 3, Frame->PointCloud.Size.Width, Frame->PointCloud.Size.Height,
                                  32 + M_FLOAT, M_IMAGE + M_PROC + M_PACKED + M_RGB96,
                                  M_RGB96 + M_PACKED);
                        
            // Get the "depth image" component into the disparity.
            ExtractPhoXiComponent(MilSystem, MilContainer, M_COMPONENT_DISPARITY,
                                  Frame->DepthMap, 1, Frame->DepthMap.Size.Width, Frame->DepthMap.Size.Height,
                                  32 + M_FLOAT, M_IMAGE + M_PROC,
                                  M_SINGLE_BAND);

            // Get the "reflectance" component.
            ExtractPhoXiComponent(MilSystem, MilContainer, M_COMPONENT_REFLECTANCE,
                                  Frame->Texture, 1, Frame->Texture.Size.Width, Frame->Texture.Size.Height,
                                  32 + M_FLOAT, M_IMAGE + M_PROC,
                                  M_SINGLE_BAND);

            // Get the "confidence" component.
            ExtractPhoXiComponent(MilSystem, MilContainer, M_COMPONENT_CONFIDENCE,
                                  Frame->ConfidenceMap, 1, Frame->ConfidenceMap.Size.Width, Frame->ConfidenceMap.Size.Height,
                                  32 + M_FLOAT, M_IMAGE + M_PROC,
                                  M_SINGLE_BAND);

            // Get the "normal" component.
            ExtractPhoXiComponent(MilSystem, MilContainer, M_COMPONENT_NORMALS_MIL,
                                  Frame->NormalMap, 3, Frame->NormalMap.GetElementsCount(), 1,
                                  32 + M_FLOAT, M_IMAGE + M_PROC + M_PACKED + M_RGB96,
                                  M_RGB96 + M_PACKED);

            Success = true;
            }
         }
      }
   return Success;
   }


//*****************************************************************************
// Extract an individual component from a PhoXi output to MIL container.
// Allocates a component if it is not present.
//*****************************************************************************
template <class T>
void ExtractPhoXiComponent(MIL_ID MilSystem, MIL_ID MilContainer, MIL_INT ComponentIdFlag, T& rPhoXiComponent,
                           MIL_INT OutNbBands, MIL_INT OutSizeX, MIL_INT OutSizeY, MIL_INT OutType, MIL_INT OutAttribute, MIL_INT InFormat)
   {
   if(!rPhoXiComponent.Empty())
      {
      MIL_ID MilComponent = MbufInquireContainer(MilContainer, ComponentIdFlag, M_COMPONENT_ID, M_NULL);
 
      if(!MilComponent)
         {
         MbufAllocComponent(MilContainer, OutNbBands, OutSizeX, OutSizeY, OutType, OutAttribute, ComponentIdFlag, &MilComponent);
         }
		MbufPutColor(MilComponent, InFormat, M_ALL_BANDS, rPhoXiComponent.GetDataPtr());
      }
   }

#endif
