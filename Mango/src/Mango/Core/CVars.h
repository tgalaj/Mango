#pragma once

// Based on https://vkguide.dev/docs/extra-chapter/cvar_system/

#include "Mango/Utils/Hash.h"

namespace mango
{
    enum class CVarFlags : uint32_t
    {
        None          = 0,
        Noedit        = 1 << 1,
        EditReadOnly  = 1 << 2,
        Advanced      = 1 << 3,

        EditCheckbox  = 1 << 8,
        EditFloatDrag = 1 << 9,
    };

    enum class CVarType : char
    {
        INT,
        FLOAT,
        STRING,
    };

    class CVarParameter
    {
    public:
        friend class CVarSystemImpl;

        int32_t arrayIndex;

        CVarType    type;
        CVarFlags   flags;
        std::string name;
        std::string description;
    };

    class CVarSystem
    {
    public:
        static CVarSystem* get();

        //pimpl
        virtual CVarParameter* getCVar      (StringHash32 hash)                           = 0;
        virtual double*        getFloatCVar (StringHash32 hash)                           = 0;
        virtual int32_t*       getIntCVar   (StringHash32 hash)                           = 0;
        virtual const char*    getStringCVar(StringHash32 hash)                           = 0;
        virtual void           setFloatCVar (StringHash32 hash, double value)             = 0;
        virtual void           setIntCVar   (StringHash32 hash, int32_t value)            = 0;
        virtual void           setStringCVar(StringHash32 hash, const char* value)        = 0;
        virtual void           setStringCVar(StringHash32 hash, const std::string& value) = 0;

        virtual CVarParameter* createFloatCVar (const char* name, const char* description, double      defaultValue, double      currentValue) = 0;
        virtual CVarParameter* createIntCVar   (const char* name, const char* description, int32_t     defaultValue, int32_t     currentValue) = 0;
        virtual CVarParameter* createStringCVar(const char* name, const char* description, const char* defaultValue, const char* currentValue) = 0;
    
        virtual void drawImguiEditor() = 0;
    };

    template<typename T>
    struct CVar
    {
    protected:
        int index;
        using CVarType = T;
    };

    struct CVarFloat : CVar<double>
    {
        CVarFloat(const char* name, const char* description, double defaultValue, CVarFlags flags = CVarFlags::None);

        double  get();
        double* getPtr();
        float   getFloat();
        float*  getFloatPtr();
        void    set(double val);
    };

    struct CVarInt : CVar<int32_t>
    {
        CVarInt(const char* name, const char* description, int32_t defaultValue, CVarFlags flags = CVarFlags::None);
        
        int32_t  get();
        int32_t* getPtr();
        void     set(int32_t val);
        void     toggle();
    };

    struct CVarString : CVar<std::string>
    {
        CVarString(const char* name, const char* description, const char* defaultValue, CVarFlags flags = CVarFlags::None);

        const char* get();
        void        set(std::string&& val);
    };
}
