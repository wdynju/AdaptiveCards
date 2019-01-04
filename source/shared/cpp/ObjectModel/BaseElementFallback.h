#pragma once

#include "pch.h"

namespace AdaptiveSharedNamespace
{
    template <typename T>
    class BaseElementFallback
    {
    public:
        BaseElementFallback() : m_requires(0), m_fallbackContent(nullptr), m_fallbackType(FallbackType::None)
        {
        }

        virtual ~BaseElementFallback()
        {
        }

        virtual void SetFallbackType(FallbackType fallbackType)
        {
            m_fallbackType = fallbackType;
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

        virtual void SetFallbackContent(std::shared_ptr<T> content)
        {
            // TODO: asserts?
            SetFallbackType(FallbackType::Content);
            m_fallbackContent = content;
        }

    private:
        std::unordered_map<std::string, std::string> m_requires;
        std::shared_ptr<T> m_fallbackContent;
        FallbackType m_fallbackType;
    };
}
