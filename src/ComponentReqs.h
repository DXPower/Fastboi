#pragma once

#include "Gameobject.h"
#include "ctti/type_id.hpp"
#include <string>
#include <sstream>

namespace Fastboi {
    template <typename... Types>
    struct RequiredComponents final {
        public:
        RequiredComponents() = delete; // Not constructible

        static bool HasRequiredComponents(const Gameobject& go) {
            if constexpr (sizeof...(Types) == 0)
                return true;
            
            return (go.HasComponent<Types>() && ...); // Fold expression over each required component
        };


        private:
        template <typename Check>
        static void WalkTypes(const Gameobject& go, std::vector<std::string>& missing) {
            if (!go.HasComponent<Check>())
                missing.push_back(ctti::type_id<Check>().name().str());
        }

        public:
        static std::vector<std::string> GetMissingNamesList(const Gameobject& go) {
            std::vector<std::string> missing;

            if constexpr (sizeof...(Types) == 0)
                return missing;
            
            (WalkTypes<Types>(go, missing), ...);

            return missing;
        }

        static std::string GetMissingNamesString(const Gameobject& go) {
            std::stringstream s;

            auto missing = GetMissingNamesList(go);

            for (unsigned int i = 0; i < missing.size(); i++) {
                s << missing[i];

                if (i != missing.size() - 1)
                    s << ' ';
            }

            return s.str();
        }

        static void StandardThrowingCheck(const Gameobject& go) {
            if (!HasRequiredComponents(go)) {
                const std::string msg = " missing reqs";
                char* buffer = new char[msg.size() + go.name.size() + 1];
                sprintf(buffer, "%s%s", go.name.c_str(), msg.c_str());

                Application::ThrowRuntimeException(
                    msg.c_str()
                    , Application::REQUIREMENTS_NOT_FULFILLED
                    , GetMissingNamesString(go).c_str()
                );
            }
        }
    };

};

