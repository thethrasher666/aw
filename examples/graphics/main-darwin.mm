//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "entry-point.hxx"

#include <Cocoa/Cocoa.h>

auto main(int32_t argc, char* argv[]) -> int32_t
{
    @autoreleasepool
    {
        NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
        const auto rc = aw::examples::entry_point({argv, argv + argc});
        [pool release];
        return rc ? 0 : -1;
    }
}
