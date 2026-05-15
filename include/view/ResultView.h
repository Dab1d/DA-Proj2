#ifndef DA_PROJ2_RESULTVIEW_H
#define DA_PROJ2_RESULTVIEW_H

#include "controllers/RegisterAllocatorController.h"
#include <string>

std::string promptFilename(const std::string& label, const char* preferredSub = nullptr);
void printAllocationResult(const RegisterAllocatorController& ctrl);

#endif // DA_PROJ2_RESULTVIEW_H
