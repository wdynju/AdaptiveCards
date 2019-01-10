#pragma once

#include "pch.h"
#include "Enums.h"
#include "json/json.h"
#include "ParseUtil.h"
#include "RemoteResourceInformation.h"
#include "BaseElement.h"

namespace AdaptiveSharedNamespace
{
    class BaseActionElement : public BaseElement<BaseActionElement>
    {
    public:
        BaseActionElement(ActionType type);

        BaseActionElement(const BaseActionElement&) = default;
        BaseActionElement(BaseActionElement&&) = default;
        BaseActionElement& operator=(const BaseActionElement&) = default;
        BaseActionElement& operator=(BaseActionElement&&) = default;
        ~BaseActionElement() = default;

        virtual std::string GetElementTypeString() const;
        virtual void SetElementTypeString(const std::string& value);

        virtual std::string GetTitle() const;
        virtual void SetTitle(const std::string& value);

        virtual std::string GetId() const;
        virtual void SetId(const std::string& value);
        virtual std::unordered_set<std::string> GetChildIds() const;

        virtual std::string GetIconUrl() const;
        virtual void SetIconUrl(const std::string& value);

        virtual Sentiment GetSentiment() const;
        virtual void SetSentiment(const Sentiment& value);

        virtual const ActionType GetElementType() const;

        std::string Serialize() const;
        virtual Json::Value SerializeToJsonValue() const;

        template<typename T> static std::shared_ptr<T> Deserialize(ParseContext& context, const Json::Value& json);

        Json::Value GetAdditionalProperties() const;
        void SetAdditionalProperties(Json::Value const& additionalProperties);

        virtual void GetResourceInformation(std::vector<RemoteResourceInformation>& resourceUris);

        static void ParseJsonObject(ParseContext& context, const Json::Value& json, std::shared_ptr<BaseActionElement>& element);

    protected:
        std::unordered_set<std::string> m_knownProperties;

    private:
        virtual void PopulateKnownPropertiesSet();

        std::string m_typeString;
        std::string m_title;
        std::string m_id;
        std::string m_iconUrl;
        Json::Value m_additionalProperties;
        ActionType m_type;
        Sentiment m_sentiment;
    };

    template<typename T> std::shared_ptr<T> BaseActionElement::Deserialize(ParseContext& context, const Json::Value& json)
    {
        std::shared_ptr<T> cardElement = std::make_shared<T>();
        std::shared_ptr<BaseActionElement> baseActionElement = cardElement;

        ParseUtil::ThrowIfNotJsonObject(json);

        baseActionElement->SetIconUrl(ParseUtil::GetString(json, AdaptiveCardSchemaKey::IconUrl));
        baseActionElement->SetId(ParseUtil::GetString(json, AdaptiveCardSchemaKey::Id));
        // TODO: Support requires
        baseActionElement->SetSentiment(
            ParseUtil::GetEnumValue<Sentiment>(json, AdaptiveCardSchemaKey::Sentiment, Sentiment::Default, SentimentFromString));
        baseActionElement->SetTitle(ParseUtil::GetString(json, AdaptiveCardSchemaKey::Title));

        ParseFallbackAndRequires(context, json, *baseActionElement);

        // Walk all properties and put any unknown ones in the additional properties json
        for (auto it = json.begin(); it != json.end(); ++it)
        {
            std::string key = it.key().asCString();
            if (baseActionElement->m_knownProperties.find(key) == baseActionElement->m_knownProperties.end())
            {
                baseActionElement->m_additionalProperties[key] = *it;
            }
        }
        return cardElement;
    }
}
