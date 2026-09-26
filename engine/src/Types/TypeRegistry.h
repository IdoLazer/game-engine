#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <any>
#include <string_view>
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
        std::function<std::any(std::string_view)> parser;
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

            // Empty when the type has no such property, or when the text doesn't parse as its type.
            std::any ParseProperty(const std::string &typeName, const std::string &propertyName,
                            std::string_view text) const;
            bool HasProperty(const std::string &typeName, const std::string &propertyName) const;

        // --- Queries ---
        public:
            bool IsTypeRegistered(const std::string &typeName) const;
            std::vector<std::string> GetRegisteredTypes() const;

        // --- Internal ---
        private:
            // Walks the parent chain, so inherited properties resolve too.
            const PropertyDescriptor *FindProperty(const std::string &typeName,
                            const std::string &propertyName) const;
        };
}
