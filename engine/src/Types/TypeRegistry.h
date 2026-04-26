#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <any>
#include <memory>

namespace Engine
{
    class Entity;
    
    // --- Types ---

    // Describes a single settable property on a registered type.
    struct PropertyDescriptor
    {
        std::string name;
        std::function<void(Entity *, const std::any &)> setter;
    };

    // Describes a registered entity type: its name, its base class, a factory, and own properties.
    struct TypeDescriptor
    {
        std::string name;
        std::string parentName;
        std::function<Entity *()> factory;
        std::vector<PropertyDescriptor> properties;
    };

    using PropertyMap = std::unordered_map<std::string, std::any>;

    // Singleton registry mapping type names to factories + property descriptors.
    // Types self-register via static initializers created by the registration macros.
    class TypeRegistry
    {
        // --- Fields ---
        private:
            std::unordered_map<std::string, TypeDescriptor> m_types;

        // --- Constructors & Destructors ---
        private:
            TypeRegistry() = default;

        // --- Singleton ---
        public:
            static TypeRegistry &Get();

        // --- Registration ---
        public:
            void RegisterType(const TypeDescriptor &descriptor);

        // --- Factory ---
        public:
            Entity *Create(const std::string &typeName) const;

        // --- Property Access ---
        public:
            bool SetProperty(Entity *entity, const std::string &typeName,
                            const std::string &propertyName, const std::any &value) const;
            void SetProperties(Entity *entity, const std::string &typeName,
                            const PropertyMap &properties) const;

        // --- Queries ---
        public:
            bool IsTypeRegistered(const std::string &typeName) const;
            std::vector<std::string> GetRegisteredTypes() const;
        };
}
