#pragma once

// ============================================================================
// Property System Macros
// ============================================================================
//
// A split header/cpp approach for registering entity types and properties.
// Header: DECLARE_TYPE declares type identity + friends the registration struct.
// Cpp:    BEGIN_TYPE_REGISTER / REGISTER_PROPERTY / END_TYPE_REGISTER
//         build the type descriptor using pointer-to-member.
//
//
// Header example:
//   class EnemyEntity : public Entity {
//       DECLARE_TYPE(EnemyEntity, Entity)
//   private:
//       Weapon* m_weapon{nullptr};
//       Vec2  m_startPosition{};
//       int  m_attackPower{10};
//   public:
//       // ... methods ...
//   };
//
// Cpp example:
//   BEGIN_TYPE_REGISTER(EnemyEntity)
//       REGISTER_PROPERTY(Weapon*, Weapon, &EnemyEntity::m_weapon)
//       REGISTER_PROPERTY(Vec2, StartPosition, &EnemyEntity::m_startPosition)
//       REGISTER_PROPERTY(int, AttackPower, &EnemyEntity::m_attackPower)
//   END_TYPE_REGISTER()
//
// Properties are inherited via the TypeRegistry parent chain.
//
// ============================================================================

#include "TypeRegistry.h"
#include <type_traits>

// Factory helper: returns nullptr for abstract types, real factory otherwise
template <typename T>
std::function<Engine::Entity *()> _EngineEntityFactory()
{
    if constexpr (std::is_abstract_v<T>)
        return nullptr;
    else
        return []() -> Engine::Entity * { return new T(); };
}

// --- Header macro ---
// Place inside the class body. Declares type name, parent, and friends the
// registration struct so it can access private members via pointer-to-member.
#define DECLARE_TYPE(ClassName, ParentClass)                                        \
    friend struct ClassName##_TypeRegistration;                                     \
public:                                                                             \
    static constexpr const char *GetStaticTypeName()                                \
    {                                                                               \
        static_assert(std::is_base_of_v<Engine::Entity, ClassName>,                 \
            #ClassName " must derive from Engine::Entity");                         \
        return #ClassName;                                                          \
    }                                                                               \
    static constexpr const char *GetParentTypeName()                                \
    {                                                                               \
        static_assert(std::is_base_of_v<ParentClass, ClassName>,                    \
            #ParentClass " must be a base of " #ClassName);                         \
        return #ParentClass;                                                        \
    }                                                                               \
private:

#define DECLARE_TYPE_NO_BASE(ClassName)                                             \
    friend struct ClassName##_TypeRegistration;                                     \
public:                                                                             \
    static constexpr const char *GetStaticTypeName()                                \
    {                                                                               \
        static_assert(std::is_base_of_v<Engine::Entity, ClassName>,                 \
            #ClassName " must derive from Engine::Entity");                         \
        return #ClassName;                                                          \
    }                                                                               \
    static constexpr const char *GetParentTypeName() { return ""; }                 \
private:

// --- Cpp macros ---
// Place in the .cpp file. Builds a TypeDescriptor using pointer-to-member,
// then registers it with the TypeRegistry at static init time.

#define BEGIN_TYPE_REGISTER(ClassName)                                              \
    struct ClassName##_TypeRegistration                                             \
    {                                                                               \
        using _Type = ClassName;                                                    \
        static Engine::TypeDescriptor _BuildDescriptor()                            \
        {                                                                           \
            Engine::TypeDescriptor desc;                                            \
            desc.name = ClassName::GetStaticTypeName();                             \
            desc.parentName = ClassName::GetParentTypeName();                       \
            desc.factory = _EngineEntityFactory<ClassName>();

#define REGISTER_PROPERTY(Type, PropName, MemberPtr)                                \
            desc.properties.push_back(Engine::PropertyDescriptor{                   \
                #PropName,                                                          \
                [](Engine::Entity *entity, const std::any &value)                   \
                {                                                                   \
                    static_cast<_Type *>(entity)->*MemberPtr =                      \
                        std::any_cast<Type>(value);                                 \
                }                                                                   \
            });

#define END_TYPE_REGISTER()                                                         \
            Engine::TypeRegistry::Get().RegisterType(desc);                         \
            return desc;                                                            \
        }                                                                           \
        static inline Engine::TypeDescriptor _desc = _BuildDescriptor();            \
    };
