#pragma once

#include "pch.h"
#include "Enums.h"
#include "json/json.h"
#include "BaseActionElement.h"
#include "BaseElement.h"
#include "ParseUtil.h"
#include "Separator.h"
#include "RemoteResourceInformation.h"

namespace AdaptiveSharedNamespace
{
    class BaseCardElement : public BaseElement<BaseCardElement>
    {
    public:
        BaseCardElement(CardElementType type, Spacing spacing, bool separator, HeightType height);
        BaseCardElement(CardElementType type);

        BaseCardElement(const BaseCardElement&) = default;
        BaseCardElement(BaseCardElement&&) = default;
        BaseCardElement& operator=(const BaseCardElement&) = default;
        BaseCardElement& operator=(BaseCardElement&&) = default;
        ~BaseCardElement() = default;

        virtual std::string GetElementTypeString() const;
        virtual void SetElementTypeString(const std::string& value);

        virtual bool GetSeparator() const;
        virtual void SetSeparator(const bool value);

        HeightType GetHeight() const;
        void SetHeight(const HeightType value);

        virtual Spacing GetSpacing() const;
        virtual void SetSpacing(const Spacing value);

        virtual std::string GetId() const;
        virtual std::unordered_set<std::string> GetChildIds() const;
        virtual void SetId(const std::string& value);

        virtual bool GetIsVisible() const;
        virtual void SetIsVisible(const bool value);

        virtual const CardElementType GetElementType() const;

        virtual std::string Serialize() const;
        virtual Json::Value SerializeToJsonValue() const;

        template<typename T> static std::shared_ptr<T> Deserialize(ParseContext& context, const Json::Value& json);

        Json::Value GetAdditionalProperties() const;
        void SetAdditionalProperties(const Json::Value& additionalProperties);

        virtual void GetResourceInformation(std::vector<RemoteResourceInformation>& resourceUris);

        static void ParseJsonObject(ParseContext& context, const Json::Value& json, std::shared_ptr<BaseCardElement>& element);

    protected:
        static Json::Value SerializeSelectAction(const std::shared_ptr<BaseActionElement> selectAction);

        std::unordered_set<std::string> m_knownProperties;

    private:
        virtual void PopulateKnownPropertiesSet();

        CardElementType m_type;
        Spacing m_spacing;
        std::string m_id;
        std::string m_typeString;
        Json::Value m_additionalProperties;
        HeightType m_height;
        bool m_separator;
        bool m_isVisible;
    };

    template<typename T> std::shared_ptr<T> BaseCardElement::Deserialize(ParseContext& context, const Json::Value& json)
    {
        std::shared_ptr<T> cardElement = std::make_shared<T>();
        std::shared_ptr<BaseCardElement> baseCardElement = cardElement;

        ParseUtil::ThrowIfNotJsonObject(json);

        baseCardElement->SetId(ParseUtil::GetString(json, AdaptiveCardSchemaKey::Id));

        context.idStack.push_back({ baseCardElement->GetId(), ""});

        baseCardElement->SetHeight(
            ParseUtil::GetEnumValue<HeightType>(json, AdaptiveCardSchemaKey::Height, HeightType::Auto, HeightTypeFromString));
        baseCardElement->SetIsVisible(ParseUtil::GetBool(json, AdaptiveCardSchemaKey::IsVisible, true));
        baseCardElement->SetSeparator(ParseUtil::GetBool(json, AdaptiveCardSchemaKey::Separator, false));
        baseCardElement->SetSpacing(
            ParseUtil::GetEnumValue<Spacing>(json, AdaptiveCardSchemaKey::Spacing, Spacing::Default, SpacingFromString));

        ParseFallbackAndRequires(context, json, *baseCardElement);

        // Walk all properties and put any unknown ones in the additional properties json
        for (auto it = json.begin(); it != json.end(); ++it)
        {
            const std::string key { it.key().asCString() };
            if (baseCardElement->m_knownProperties.find(key) == baseCardElement->m_knownProperties.end())
            {
                baseCardElement->m_additionalProperties[key] = *it;
            }
        }

        return cardElement;
    }
}
