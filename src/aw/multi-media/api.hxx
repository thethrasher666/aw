//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <aw/base/definitions.hxx>

#if defined(AW_BUILD_MULTIMEDIA)
#    define AW_MULTIMEDIA_API AW_BASE_EXPORT
#else
#    define AW_MULTIMEDIA_API AW_BASE_IMPORT
#endif
