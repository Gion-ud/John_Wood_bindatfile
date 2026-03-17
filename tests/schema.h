#pragma once

#include <packed_ser_types.h>

__attribute__((packed))
struct data_entry {
    float   salt_conc;
    float3  gms_mass_before;
    float3  gms_mass_after;
};

