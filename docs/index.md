# Applicaton Framework Overview

Application Framework is organized as a layered C++ framework with increasing capability at each layer.

## Layer Summary

- `base`: shared definitions and versioning metadata.
- `core`: object model and event loop for non-graphical applications.
- `gui`: graphical application support such as windows and input.
- `widgets`: higher-level GUI abstractions and reusable controls.
- `vulkan-widgets`: higher-level Vulkan abstractions and reusable controls.
- `tools`: provides the embedding of resource localisation strings.

## Design Principles

- Stable upward dependency direction from lower-level modules to higher-level modules.
- Clear public headers per module with private platform details hidden behind forward declarations and source files.
- Small API surfaces that make ownership and lifetime visible.
- Cross-platform seams isolated in platform-specific source folders.

## Include and Naming Policy

This repository intentionally allows repeated leaf filenames such as `application.hxx` across modules.

Headers are included by full module path, for example:

- `#include <aw/core/application.hxx>`
- `#include <aw/gui/application.hxx>`
- `#include <aw/widgets/application.hxx>`

### Rationale

- The include path expresses module ownership and layer context.
- Module-local naming stays concise and predictable.
- New modules can reuse familiar filenames without global naming inflation.

## Application Lifecycle

This document describes the intended lifecycle across framework layers.

### Construction

1. Collect command line arguments from the platform entry point.
2. Construct the highest-level application type needed by the program:
   - `CoreApplication` for non-GUI applications.
   - `GuiApplication` for windowed applications.
   - `WidgetsApplication` when using widget-level abstractions.

### Runtime

- The application object owns process-level state and runtime integration.
- `run()` enters the event loop for the selected layer.
- Layer-specific behavior should be composed above shared core runtime behavior.

### Shutdown

- Exiting the event loop should return control cleanly to the entry point.
- Destruction should release platform resources in reverse construction order.

### Architectural Notes

- Keep lifecycle semantics consistent across all application types.
- Prefer extending behavior by layering rather than duplicating loop logic.
- Ensure platform details stay behind internal implementation boundaries.

## API Overview

- [Class List](annotated.html)
- [Class Hierarchy](hierarchy.html)
- [Namespace List](namespaces.html)
- [File List](files.html)
