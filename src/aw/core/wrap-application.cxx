// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "wrap.hxx"

#include <aw/core/application.hxx>

namespace aw::bind::core
{
    void wrapApplication(py::module_& m)
    {
        py::class_<aw::core::CoreApplication> cls(m, "CoreApplication");
        cls.def(py::init<std::vector<std::string> const&>(), py::arg("arguments"), "Constructor.");
        cls.def("arguments", &aw::core::CoreApplication::arguments, "Get the command line arguments passed to the application.");
        cls.def("is_continuous_update_enabled",
                &aw::core::CoreApplication::isContinuousUpdateEnabled,
                "Determine whether the application drives a frame after every pass over the event queue.");
        // instance() is static, so it needs def_static rather than def (a plain def would implicitly
        // expect a `self` receiver that instance()'s zero-argument signature can't satisfy). The
        // returned CoreApplication* is a non-owning observer into a global, set by whichever object
        // was constructed - py::return_value_policy::reference is required so Python doesn't think it
        // owns (and may destroy) the singleton, mirroring Object::parent() in wrap-object.cxx. A null
        // return (no instance constructed yet) is converted to Python None automatically.
        cls.def_static("instance", &aw::core::CoreApplication::instance, py::return_value_policy::reference, "Get the singleton instance of the application.");
        cls.def("locale", &aw::core::CoreApplication::locale, "Get the locale of the application.");
        cls.def("run", &aw::core::CoreApplication::run, "Run the application.");
        cls.def("set_continuous_update_enabled", &aw::core::CoreApplication::setContinuousUpdateEnabled, py::arg("enabled"), "Enable or disable continuous updates.");
    }
} // namespace aw::bind::core
