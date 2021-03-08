#include <Windows.h>
#include "Core/Log.h"
#include "Platform/Windows/WindowsProcessorInfo.h"

typedef BOOL(WINAPI* LPFN_GLPI)(
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION,
    PDWORD);

// Helper function to count set bits in the processor mask.
DWORD CountSetBits(ULONG_PTR bitMask)
{
    DWORD LSHIFT = sizeof(ULONG_PTR) * 8 - 1;
    DWORD bitSetCount = 0;
    ULONG_PTR bitTest = (ULONG_PTR)1 << LSHIFT;
    DWORD i;

    for (i = 0; i <= LSHIFT; ++i)
    {
        bitSetCount += ((bitMask & bitTest) ? 1 : 0);
        bitTest >= 2; //shift right
    }

    return bitSetCount;
}

void WindowsProcessorInfo::Display()
{
    CORE_INFO("Processer Info:");
    CORE_INFO("==================");

    LPFN_GLPI glpi;
    BOOL done = 0;
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer = nullptr;
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ptr = nullptr;
    DWORD returnLength = 0;
    DWORD logicalProcessorCount = 0;
    DWORD numaNodeCount = 0;
    DWORD processorCoreCount = 0;
    DWORD processorL1CacheCount = 0;
    DWORD processorL2CacheCount = 0;
    DWORD processorL3CacheCount = 0;
    DWORD processorPackageCount = 0;
    DWORD byteOffset = 0;
    PCACHE_DESCRIPTOR Cache;

    glpi = (LPFN_GLPI)GetProcAddress(
        GetModuleHandle("kernel32"),
        "GetLogicalProcessorInformation");

    if (glpi == nullptr)
    {
        CORE_INFO("GetLogicalProcessorInformation is not supported.");
        return;
    }

    while (!done)
    {
        DWORD rc = glpi(buffer, &returnLength);

        if (FALSE == rc)
        {
            if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
            {
                if (buffer) free(buffer);

                buffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc(returnLength);

                if (nullptr == buffer)
                {
                    CORE_INFO("Error: Allocation failure");
                    return;
                }
            }
            else
            {
                CORE_INFO("Error {0}", GetLastError());
                return;
            }
        }
        else
        {
            done = TRUE;
        }
    }

    ptr = buffer;

    while (byteOffset + sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION) <= returnLength)
    {
        if (ptr != nullptr)
        {
            switch (ptr->Relationship)
            {
                case RelationNumaNode:
                    // Non-NUMA systems report a single record of this type.
                    numaNodeCount++;
                    break;

                case RelationProcessorCore:
                    processorCoreCount++;

                    // A hyperthreaded core supplies more than one logical processor.
                    logicalProcessorCount += CountSetBits(ptr->ProcessorMask);
                    break;

                case RelationCache:
                    // Cache data is in ptr->Cache, one CACHE_DESCRIPTOR structure for each cache. 
                    Cache = &ptr->Cache;
                    if (Cache->Level == 1)
                    {
                        processorL1CacheCount++;
                        CORE_INFO("L1 Cache Line Size: {0} bytes", Cache->LineSize);
                        CORE_INFO("L1 Cache Block Size: {0} bytes", Cache->Size);
                    }
                    else if (Cache->Level == 2)
                    {
                        processorL2CacheCount++;
                        CORE_INFO("L2 Cache Line Size: {0} bytes", Cache->LineSize);
                        CORE_INFO("L2 Cache Block Size: {0} bytes", Cache->Size);
                    }
                    else if (Cache->Level == 3)
                    {
                        processorL3CacheCount++;
                        CORE_INFO("L3 Cache Line Size: {0} bytes", Cache->LineSize);
                        CORE_INFO("L3 Cache Block Size: {0} bytes", Cache->Size);
                    }
                    CORE_INFO("==========================");
                    break;

                case RelationProcessorPackage:
                    // Logical processors share a physical package.
                    processorPackageCount++;
                    break;

                default:
                    CORE_INFO("Error: Unsupported LOGICAL_PROCESSOR_RELATIONSHIP value.");
                    break;
            }
            byteOffset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
            ptr++;
        }
        else
        {
            free(buffer);
            return;
        }
    }

    CORE_INFO("Number of NUMA nodes: {0}",
        numaNodeCount);
    CORE_INFO("Number of physical processor packages: {0}",
        processorPackageCount);
    CORE_INFO("Number of processor cores: {0}",
        processorCoreCount);
    CORE_INFO("Number of logical processors: {0}",
        logicalProcessorCount);
    CORE_INFO("Number of processor L1/L2/L3 caches: {0}/{1}/{2}",
        processorL1CacheCount,
        processorL2CacheCount,
        processorL3CacheCount);

    free(buffer);
}