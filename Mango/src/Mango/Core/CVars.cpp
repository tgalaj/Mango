#include "mgpch.h"

#include "CVars.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "misc/cpp/imgui_stdlib.h"

#include <array>
#include <algorithm>
#include <execution>
#include <shared_mutex>
#include <unordered_map>

namespace mango
{
    template<typename T>
    struct CVarStorage
    {
        T initial;
        T current;
        CVarParameter* parameter;
    };

    template<typename T>
    struct CVarArray
    {
        CVarStorage<T>* cvars{ nullptr };
        int32_t lastCVar{ 0 };

        CVarArray(size_t size)
        {
            cvars = new CVarStorage<T>[size]();
        }

        ~CVarArray()
        {
            delete[] cvars;
        }

        CVarStorage<T>* getCurrentStorage(int32_t index)
        {
            return &cvars[index];
        }

        T* getCurrentPtr(int32_t index)
        {
            return &cvars[index].current;
        };

        T getCurrent(int32_t index)
        {
            return cvars[index].current;
        };

        void setCurrent(const T& val, int32_t index)
        {
            cvars[index].current = val;
        }

        int add(const T& value, CVarParameter* param)
        {
            int index = lastCVar;

            cvars[index].current = value;
            cvars[index].initial = value;
            cvars[index].parameter = param;

            param->arrayIndex = index;
            lastCVar++;
            return index;
        }

        int add(const T& initialValue, const T& currentValue, CVarParameter* param)
        {
            int index = lastCVar;

            cvars[index].current = currentValue;
            cvars[index].initial = initialValue;
            cvars[index].parameter = param;

            param->arrayIndex = index;
            lastCVar++;

            return index;
        }
    };

    uint32_t hash(const char* str)
    {
        return mango::fnvHash1a32(str, strlen(str));
    }

    class CVarSystemImpl : public CVarSystem
    {
    public:
        CVarParameter* getCVar(StringHash32 hash) override final;
        
        CVarParameter* createFloatCVar (const char* name, const char* description, double      defaultValue, double      currentValue) override final;
        CVarParameter* createIntCVar   (const char* name, const char* description, int32_t     defaultValue, int32_t     currentValue) override final;
        CVarParameter* createStringCVar(const char* name, const char* description, const char* defaultValue, const char* currentValue) override final;

        double*     getFloatCVar (StringHash32 hash) override final;
        int32_t*    getIntCVar   (StringHash32 hash) override final;
        const char* getStringCVar(StringHash32 hash) override final;

        void setFloatCVar (StringHash32 hash, double             value) override final;
        void setIntCVar   (StringHash32 hash, int32_t            value) override final;
        void setStringCVar(StringHash32 hash, const char*        value) override final;
        void setStringCVar(StringHash32 hash, const std::string& value) override final;

        void drawImguiEditor() override final;

        void editParameter(CVarParameter* p, float textWidth);

        constexpr static int MAX_INT_CVARS = 1000;
        CVarArray<int32_t> intCVars2{ MAX_INT_CVARS };

        constexpr static int MAX_FLOAT_CVARS = 1000;
        CVarArray<double> floatCVars{ MAX_FLOAT_CVARS };

        constexpr static int MAX_STRING_CVARS = 200;
        CVarArray<std::string> stringCVars{ MAX_STRING_CVARS };


        // Using templates with specializations to get the cvar arrays for each type.
        // If you try to use a type that doesn't have specialization, it will trigger a linker error
        template<typename T>
        CVarArray<T>* getCVarArray();

        template<>
        CVarArray<int32_t>* getCVarArray()
        {
            return &intCVars2;
        }

        template<>
        CVarArray<double>* getCVarArray()
        {
            return &floatCVars;
        }

        template<>
        CVarArray<std::string>* getCVarArray()
        {
            return &stringCVars;
        }

        // Templated get-set cvar versions for syntax sugar
        template<typename T>
        T* getCVarCurrent(uint32_t namehash) 
        {
            CVarParameter* par = getCVar(namehash);
            if (!par) 
            {
                return nullptr;
            }
            else 
            {
                return getCVarArray<T>()->getCurrentPtr(par->arrayIndex);
            }
        }

        template<typename T>
        void setCVarCurrent(uint32_t namehash, const T& value)
        {
            CVarParameter* cvar = getCVar(namehash);
            if (cvar)
            {
                getCVarArray<T>()->setCurrent(value, cvar->arrayIndex);
            }
        }

        static CVarSystemImpl* get()
        {
            return static_cast<CVarSystemImpl*>(CVarSystem::get());
        }

    private:
        CVarParameter* initCVar(const char* name, const char* description);

    private:
        std::shared_mutex                           m_mutex;
        std::unordered_map<uint32_t, CVarParameter> m_savedCVars;
        std::vector<CVarParameter*>                 m_cachedEditParameters;
    };

    CVarSystem* CVarSystem::get()
    {
        static CVarSystemImpl cvarSys{};
        return &cvarSys;
    }

    double* CVarSystemImpl::getFloatCVar(StringHash32 hash)
    {
        return getCVarCurrent<double>(hash);
    }

    int32_t* CVarSystemImpl::getIntCVar(StringHash32 hash)
    {
        return getCVarCurrent<int32_t>(hash);
    }

    const char* CVarSystemImpl::getStringCVar(StringHash32 hash)
    {
        return getCVarCurrent<std::string>(hash)->c_str();
    }

    CVarParameter* CVarSystemImpl::getCVar(StringHash32 hash)
    {
        std::shared_lock lock(m_mutex);
        auto it = m_savedCVars.find(hash);

        if (it != m_savedCVars.end())
        {
            return &(*it).second;
        }

        return nullptr;
    }

    void CVarSystemImpl::setFloatCVar(StringHash32 hash, double value)
    {
        setCVarCurrent<double>(hash, value);
    }

    void CVarSystemImpl::setIntCVar(StringHash32 hash, int32_t value)
    {
        setCVarCurrent<int32_t>(hash, value);
    }

    void CVarSystemImpl::setStringCVar(StringHash32 hash, const char* value)
    {
        setCVarCurrent<std::string>(hash, value);
    }

    void CVarSystemImpl::setStringCVar(StringHash32 hash, const std::string & value)
    {
        setCVarCurrent<std::string>(hash, value);
    }

    CVarParameter* CVarSystemImpl::createFloatCVar(const char* name, const char* description, double defaultValue, double currentValue)
    {
        std::unique_lock lock(m_mutex);
        CVarParameter* param = initCVar(name, description);
        if (!param) return nullptr;

        param->type = CVarType::FLOAT;

        getCVarArray<double>()->add(defaultValue, currentValue, param);

        return param;
    }

    CVarParameter* CVarSystemImpl::createIntCVar(const char* name, const char* description, int32_t defaultValue, int32_t currentValue)
    {
        std::unique_lock lock(m_mutex);
        CVarParameter* param = initCVar(name, description);
        if (!param) return nullptr;

        param->type = CVarType::INT;

        getCVarArray<int32_t>()->add(defaultValue, currentValue, param);

        return param;
    }

    CVarParameter* CVarSystemImpl::createStringCVar(const char* name, const char* description, const char* defaultValue, const char* currentValue)
    {
        std::unique_lock lock(m_mutex);
        CVarParameter* param = initCVar(name, description);
        if (!param) return nullptr;

        param->type = CVarType::STRING;

        getCVarArray<std::string>()->add(defaultValue, currentValue, param);

        return param;
    }

    CVarParameter* CVarSystemImpl::initCVar(const char* name, const char* description)
    {

        uint32_t namehash = StringHash32{ name };
        m_savedCVars[namehash] = CVarParameter{};

        CVarParameter& newParam = m_savedCVars[namehash];

        newParam.name = name;
        newParam.description = description;

        return &newParam;
    }

    CVarFloat::CVarFloat(const char* name, const char* description, double defaultValue, CVarFlags flags)
    {
        CVarParameter* cvar = CVarSystem::get()->createFloatCVar(name, description, defaultValue, defaultValue);
        cvar->flags = flags;
        index = cvar->arrayIndex;
    }

    template<typename T>
    T getCVarCurrentByIndex(int32_t index) {
        return CVarSystemImpl::get()->getCVarArray<T>()->getCurrent(index);
    }
    template<typename T>
    T* ptrGetCVarCurrentByIndex(int32_t index) {
        return CVarSystemImpl::get()->getCVarArray<T>()->getCurrentPtr(index);
    }


    template<typename T>
    void setCVarCurrentByIndex(int32_t index, const T& data) {
        CVarSystemImpl::get()->getCVarArray<T>()->setCurrent(data, index);
    }


    double CVarFloat::get()
    {
        return getCVarCurrentByIndex<CVarType>(index);
    }

    double* CVarFloat::getPtr()
    {
        return ptrGetCVarCurrentByIndex<CVarType>(index);
    }

    float CVarFloat::getFloat()
    {
        return static_cast<float>(get());
    }

    float* CVarFloat::getFloatPtr()
    {
        float* result = reinterpret_cast<float*>(getPtr());
        return result;
    }

    void CVarFloat::set(double f)
    {
        setCVarCurrentByIndex<CVarType>(index, f);
    }

    CVarInt::CVarInt(const char* name, const char* description, int32_t defaultValue, CVarFlags flags)
    {
        CVarParameter* cvar = CVarSystem::get()->createIntCVar(name, description, defaultValue, defaultValue);
        cvar->flags = flags;
        index = cvar->arrayIndex;
    }

    int32_t CVarInt::get()
    {
        return getCVarCurrentByIndex<CVarType>(index);
    }

    int32_t* CVarInt::getPtr()
    {
        return ptrGetCVarCurrentByIndex<CVarType>(index);
    }

    void CVarInt::set(int32_t val)
    {
        setCVarCurrentByIndex<CVarType>(index, val);
    }

    void CVarInt::toggle()
    {
        bool enabled = get() != 0;

        set(enabled ? 0 : 1);
    }

    CVarString::CVarString(const char* name, const char* description, const char* defaultValue, CVarFlags flags)
    {
        CVarParameter* cvar = CVarSystem::get()->createStringCVar(name, description, defaultValue, defaultValue);
        cvar->flags = flags;
        index = cvar->arrayIndex;
    }

    const char* CVarString::get()
    {
        return getCVarCurrentByIndex<CVarType>(index).c_str();
    };

    void CVarString::set(std::string&& val)
    {
        setCVarCurrentByIndex<CVarType>(index, val);
    }


    void CVarSystemImpl::drawImguiEditor()
    {
        static std::string searchText = "";

        ImGui::InputText("Filter", &searchText);
        static bool bShowAdvanced = false;
        ImGui::Checkbox("Advanced", &bShowAdvanced);
        ImGui::Separator();
        m_cachedEditParameters.clear();

        auto addToEditList = [&](auto parameter)
            {
                bool bHidden = ((uint32_t)parameter->flags & (uint32_t)CVarFlags::Noedit);
                bool bIsAdvanced = ((uint32_t)parameter->flags & (uint32_t)CVarFlags::Advanced);

                if (!bHidden)
                {
                    if (!(!bShowAdvanced && bIsAdvanced) && parameter->name.find(searchText) != std::string::npos)
                    {
                        m_cachedEditParameters.push_back(parameter);
                    };
                }
            };

        for (int i = 0; i < getCVarArray<int32_t>()->lastCVar; i++)
        {
            addToEditList(getCVarArray<int32_t>()->cvars[i].parameter);
        }
        for (int i = 0; i < getCVarArray<double>()->lastCVar; i++)
        {
            addToEditList(getCVarArray<double>()->cvars[i].parameter);
        }
        for (int i = 0; i < getCVarArray<std::string>()->lastCVar; i++)
        {
            addToEditList(getCVarArray<std::string>()->cvars[i].parameter);
        }

        if (m_cachedEditParameters.size() > 10)
        {
            std::unordered_map<std::string, std::vector<CVarParameter*>> categorizedParams;

            //insert all the edit parameters into the hashmap by category
            for (auto p : m_cachedEditParameters)
            {
                int dotPos = -1;
                //find where the first dot is to categorize
                for (int i = 0; i < p->name.length(); i++)
                {
                    if (p->name[i] == '.')
                    {
                        dotPos = i;
                        break;
                    }
                }
                std::string category = "";
                if (dotPos != -1)
                {
                    category = p->name.substr(0, dotPos);
                }

                auto it = categorizedParams.find(category);
                if (it == categorizedParams.end())
                {
                    categorizedParams[category] = std::vector<CVarParameter*>();
                    it = categorizedParams.find(category);
                }
                it->second.push_back(p);
            }

            for (auto [category, parameters] : categorizedParams)
            {
                // Alphabetical sort
                std::sort(std::execution::par, parameters.begin(), parameters.end(), [](CVarParameter* A, CVarParameter* B)
                {
                    return A->name < B->name;
                });

                if (ImGui::BeginMenu(category.c_str()))
                {
                    float maxTextWidth = 0;

                    for (auto p : parameters)
                    {
                        maxTextWidth = std::max(maxTextWidth, ImGui::CalcTextSize(p->name.c_str()).x);
                    }
                    for (auto p : parameters)
                    {
                        editParameter(p, maxTextWidth);
                    }

                    ImGui::EndMenu();
                }
            }
        }
        else
        {
            // Alphabetical sort
            std::sort(std::execution::par, m_cachedEditParameters.begin(), m_cachedEditParameters.end(), [](CVarParameter* A, CVarParameter* B)
            {
                return A->name < B->name;
            });

            float maxTextWidth = 0;
            for (auto p : m_cachedEditParameters)
            {
                maxTextWidth = std::max(maxTextWidth, ImGui::CalcTextSize(p->name.c_str()).x);
            }
            for (auto p : m_cachedEditParameters)
            {
                editParameter(p, maxTextWidth);
            }
        }
    }

    void label(const char* label, float textWidth)
    {
        constexpr float Slack = 50;
        constexpr float EditorWidth = 100;

        ImGuiWindow* window = ImGui::GetCurrentWindow();
        const ImVec2 lineStart = ImGui::GetCursorScreenPos();
        const ImGuiStyle& style = ImGui::GetStyle();
        float fullWidth = textWidth + Slack;

        ImVec2 textSize = ImGui::CalcTextSize(label);

        ImVec2 startPos = ImGui::GetCursorScreenPos();

        ImGui::Text(label);

        ImVec2 finalPos = { startPos.x + fullWidth, startPos.y };

        ImGui::SameLine();
        ImGui::SetCursorScreenPos(finalPos);

        ImGui::SetNextItemWidth(EditorWidth);
    }

    void CVarSystemImpl::editParameter(CVarParameter* p, float textWidth)
    {
        const bool readonlyFlag = ((uint32_t)p->flags & (uint32_t)CVarFlags::EditReadOnly);
        const bool checkboxFlag = ((uint32_t)p->flags & (uint32_t)CVarFlags::EditCheckbox);
        const bool dragFlag     = ((uint32_t)p->flags & (uint32_t)CVarFlags::EditFloatDrag);

        switch (p->type)
        {
        case CVarType::INT:
            if (readonlyFlag)
            {
                std::string displayFormat = p->name + "= %i";
                ImGui::Text(displayFormat.c_str(), getCVarArray<int32_t>()->getCurrent(p->arrayIndex));
            }
            else
            {
                if (checkboxFlag)
                {
                    bool bCheckbox = getCVarArray<int32_t>()->getCurrent(p->arrayIndex) != 0;
                    label(p->name.c_str(), textWidth);

                    ImGui::PushID(p->name.c_str());

                    if (ImGui::Checkbox("", &bCheckbox))
                    {
                        getCVarArray<int32_t>()->setCurrent(bCheckbox ? 1 : 0, p->arrayIndex);
                    }
                    ImGui::PopID();
                }
                else
                {
                    label(p->name.c_str(), textWidth);
                    ImGui::PushID(p->name.c_str());
                    ImGui::InputInt("", getCVarArray<int32_t>()->getCurrentPtr(p->arrayIndex));
                    ImGui::PopID();
                }
            }
            break;

        case CVarType::FLOAT:
            if (readonlyFlag)
            {
                std::string displayFormat = p->name + "= %f";
                ImGui::Text(displayFormat.c_str(), getCVarArray<double>()->getCurrent(p->arrayIndex));
            }
            else
            {
                label(p->name.c_str(), textWidth);
                ImGui::PushID(p->name.c_str());
                if (dragFlag)
                {
                    ImGui::InputDouble("", getCVarArray<double>()->getCurrentPtr(p->arrayIndex), 0, 0, "%.3f");
                }
                else
                {
                    ImGui::InputDouble("", getCVarArray<double>()->getCurrentPtr(p->arrayIndex), 0, 0, "%.3f");
                }
                ImGui::PopID();
            }
            break;

        case CVarType::STRING:
            if (readonlyFlag)
            {
                std::string displayFormat = p->name + "= %s";
                ImGui::PushID(p->name.c_str());
                ImGui::Text(displayFormat.c_str(), getCVarArray<std::string>()->getCurrent(p->arrayIndex));

                ImGui::PopID();
            }
            else
            {
                ImGui::PushID(p->name.c_str());
                label(p->name.c_str(), textWidth);
                ImGui::InputText("", getCVarArray<std::string>()->getCurrentPtr(p->arrayIndex));

                ImGui::PopID();
            }
            break;

        default:
            break;
        }

        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip(p->description.c_str());
        }
    }
}