//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "entry-point.hxx"

#include <cstdint>

auto WinMain(void*, void*, char*, int32_t) -> int32_t
{
    return aw::examples::entry_point({ __argv, __argv + __argc }) ? 0 : -1;
}
