#pragma once

#include "pch.h"

namespace AdaptiveSharedNamespace
{
    template <typename T>
    class BaseElementFallback
    {
    public:
        BaseElementFallback() : m_requires(0), m_fallbackIds(0), m_fallbackContent(nullptr), m_fallbackType(FallbackType::None)
        {
        }

        virtual FallbackType GetFallbackType() const
        {
            return m_fallbackType;
        }

        virtual bool MeetsRequirements(const std::unordered_map<std::string, std::string>& /*hostProvides*/) const
        {
            return true;
        }

        virtual std::shared_ptr<T> GetFallbackContent() const
        {
            return m_fallbackContent;
        }

        virtual std::unordered_set<std::string> GetFallbackIds() const
        {
            return m_fallbackIds;
        }

        virtual void SerializeFallbackAndRequires(Json::Value& root) const
        {
            SerializeFallback(root);
            SerializeRequires(root);
        }

        static void ParseFallbackAndRequires(ParseContext& context, const Json::Value& json, T& baseElement)
        {
            ParseFallback(context, json, baseElement);
            ParseRequires(context, json, baseElement);
        }

    private:

        void SerializeFallback(Json::Value& root) const
        {
            if (m_fallbackType == FallbackType::Drop)
            {
                root[AdaptiveCardSchemaKeyToString(AdaptiveCardSchemaKey::Fallback)] = "drop";
            }
            else if (m_fallbackType == FallbackType::Content)
            {
                root[AdaptiveCardSchemaKeyToString(AdaptiveCardSchemaKey::Fallback)] = m_fallbackContent->SerializeToJsonValue();
            }
        }

        void SerializeRequires(Json::Value& /*root*/) const
        {
        }

        static void ParseFallback(ParseContext& context, const Json::Value& json, T& baseElement)
        {
            const auto fallbackValue = ParseUtil::ExtractJsonValue(json, AdaptiveCardSchemaKey::Fallback, false);
            if (!fallbackValue.empty())
            {
                if (fallbackValue.isString())
                {
                    auto fallbackStringValue = ParseUtil::ToLowercase(fallbackValue.asString());
                    if (fallbackStringValue == "drop")
                    {
                        baseElement.m_fallbackType = FallbackType::Drop;
                        return;
                    }
                }
                else if (fallbackValue.isObject())
                {
                    // fallback value is a JSON object. parse it and add it as fallback content.
                    context.EnterFallback();
                    std::shared_ptr<T> fallbackElement;
                    T::ParseJsonObject(context, fallbackValue, fallbackElement);
                    context.ExitFallback();

                    if (fallbackElement)
                    {
                        baseElement.m_fallbackType = FallbackType::Content;
                        baseElement.m_fallbackContent = fallbackElement;
                        baseElement.m_fallbackIds = fallbackElement->GetChildIds();
                        baseElement.m_fallbackIds.emplace(fallbackElement->GetId());
                        return;
                    }
                }
                throw AdaptiveCardParseException(ErrorStatusCode::InvalidPropertyValue, "Invalid value for fallback");
            }
        }

        static void ParseRequires(ParseContext& /*context*/, const Json::Value& /*json*/, T& /*baseElement*/)
        {
        }

        std::unordered_map<std::string, std::string> m_requires;
        std::unordered_set<std::string> m_fallbackIds;
        std::shared_ptr<T> m_fallbackContent;
        FallbackType m_fallbackType;
    };
}
