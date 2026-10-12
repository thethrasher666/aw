//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#if defined(_WIN32)
#    define AW_BASE_EXPORT            __declspec(dllexport) ///< Export symbol visibility.
#    define AW_BASE_HIDDEN                                  ///< Hidden symbol visibility.
#    define AW_BASE_IMPORT            __declspec(dllimport) ///< Import symbol visibility.
#    define AW_BASE_EXPORT_TEMPLATE   AW_BASE_EXPORT       ///< Export attribute for explicit template instantiations.
#    define AW_BASE_IMPORT_TEMPLATE   AW_BASE_IMPORT       ///< Import attribute for extern explicit template instantiations.
#    define AW_BASE_PLATFORM_IS_WIN32                       /// Platform identifier.
#elif defined(__APPLE__)
#    define AW_BASE_EXPORT             __attribute__((visibility("default"))) ///< Export symbol visibility.
#    define AW_BASE_HIDDEN             __attribute__((visibility("hidden")))  ///< Hidden symbol visibility.
#    define AW_BASE_IMPORT                                                    ///< Import symbol visibility.
#    define AW_BASE_EXPORT_TEMPLATE                                           ///< Export attribute for explicit template instantiations.
#    define AW_BASE_IMPORT_TEMPLATE                                           ///< Import attribute for extern explicit template instantiations.
#    define AW_BASE_PLATFORM_IS_DARWIN                                        /// Platform identifier.
#elif defined(__linux__)
#    define AW_BASE_EXPORT            __attribute__((visibility("default"))) ///< Export symbol visibility.
#    define AW_BASE_HIDDEN            __attribute__((visibility("hidden")))  ///< Hidden symbol visibility.
#    define AW_BASE_IMPORT                                                   ///< Import symbol visibility.
#    define AW_BASE_EXPORT_TEMPLATE                                          ///< Export attribute for explicit template instantiations.
#    define AW_BASE_IMPORT_TEMPLATE                                          ///< Import attribute for extern explicit template instantiations.
#    define AW_BASE_PLATFORM_IS_LINUX                                        /// Platform identifier.
#else
#    define AW_BASE_EXPORT
#    define AW_BASE_HIDDEN
#    define AW_BASE_IMPORT
#    define AW_BASE_EXPORT_TEMPLATE
#    define AW_BASE_IMPORT_TEMPLATE
#endif // #if defined(WIN32)
