#include "TypeRegistry.h"

namespace Engine
{

// --- Singleton ---

TypeRegistry &TypeRegistry::Get()
{
    static TypeRegistry instance;
    return instance;
}

// --- Registration ---

void TypeRegistry::RegisterType(const TypeDescriptor &descriptor)
{
    if (m_types.find(descriptor.name) == m_types.end())
        m_types[descriptor.name] = descriptor;
}

// --- Factory ---

Entity *TypeRegistry::Create(const std::string &typeName) const
{
    auto it = m_types.find(typeName);
    if (it == m_types.end())
        return nullptr;
    return it->second.factory();
}

// --- Property Access ---

bool TypeRegistry::SetProperty(Entity *entity, const std::string &typeName,
                               const std::string &propertyName, const std::any &value) const
{
    std::string currentType = typeName;
    while (!currentType.empty())
    {
        auto typeIt = m_types.find(currentType);
        if (typeIt == m_types.end())
            break;

        for (const auto &prop : typeIt->second.properties)
        {
            if (prop.name == propertyName)
            {
                prop.setter(entity, value);
                return true;
            }
        }
        currentType = typeIt->second.parentName;
    }
    return false;
}

void TypeRegistry::SetProperties(Entity *entity, const std::string &typeName,
                                 const PropertyMap &properties) const
{
    for (const auto &[name, value] : properties)
    {
        SetProperty(entity, typeName, name, value);
    }
}

// --- Queries ---

bool TypeRegistry::IsTypeRegistered(const std::string &typeName) const
{
    return m_types.find(typeName) != m_types.end();
}

std::vector<std::string> TypeRegistry::GetRegisteredTypes() const
{
    std::vector<std::string> names;
    for (const auto &[name, desc] : m_types)
        names.push_back(name);
    return names;
}

}
