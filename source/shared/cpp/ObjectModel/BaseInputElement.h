#pragma once

#include "pch.h"
#include "Enums.h"
#include "json/json.h"
#include "ParseUtil.h"
#include "BaseCardElement.h"

namespace AdaptiveSharedNamespace
{
    class BaseInputElement : public BaseCardElement
    {
    public:
        BaseInputElement(CardElementType elementType);
        BaseInputElement(CardElementType type, Spacing spacing, bool separator, HeightType height);

        std::string GetId() const override;
        void SetId(const std::string& value) override;

        template<typename T> static std::shared_ptr<T> Deserialize(ParseContext& context, const Json::Value& json);

        bool GetIsRequired() const;
        void SetIsRequired(const bool isRequired);

        Json::Value SerializeToJsonValue() const override;

    private:
        std::string m_id;
        bool m_isRequired;
    };

    template<typename T> std::shared_ptr<T> BaseInputElement::Deserialize(ParseContext &context, const Json::Value& json)
    {
        std::shared_ptr<T> baseInputElement = BaseCardElement::Deserialize<T>(context, json);

        ParseUtil::ThrowIfNotJsonObject(json);

        baseInputElement->SetId(ParseUtil::GetString(json, AdaptiveCardSchemaKey::Id, true));
        baseInputElement->SetIsRequired(ParseUtil::GetBool(json, AdaptiveCardSchemaKey::IsRequired, false));

        return baseInputElement;
    }
}
