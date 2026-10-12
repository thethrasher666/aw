//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "entry-point.hxx"

#include <cstdint>

auto main(int32_t argc, char* argv[]) -> int32_t
{
    return aw::examples::entry_point({argv, argv + argc}) ? 0 : -1;
}
