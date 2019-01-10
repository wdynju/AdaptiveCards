#include "pch.h"
#include "ParseContext.h"
#include "AdaptiveCardParseException.h"

namespace AdaptiveSharedNamespace
{
    ParseContext::ParseContext() :
        elementParserRegistration{std::make_shared<ElementParserRegistration>()},
        actionParserRegistration{std::make_shared<ActionParserRegistration>()},
        elementIds{std::make_shared<std::unordered_set<std::string>>()},
        warnings{},
        m_fallbackCount(0)
    {
    }

    ParseContext::ParseContext(std::shared_ptr<ElementParserRegistration> elementRegistration, std::shared_ptr<ActionParserRegistration> actionRegistration, std::shared_ptr<std::unordered_set<std::string>> ids) : warnings{}, m_fallbackCount(0)
    {
        elementParserRegistration = (elementRegistration) ? elementRegistration : std::make_shared<ElementParserRegistration>();
        actionParserRegistration = (actionRegistration) ? actionRegistration : std::make_shared<ActionParserRegistration>();
        elementIds = (ids) ? ids : std::make_shared<std::unordered_set<std::string>>();
    }

    void ParseContext::EnterFallback()
    {
        m_fallbackCount++;
    }

    void ParseContext::ExitFallback()
    {
        if (m_fallbackCount == 0)
        {
            throw AdaptiveCardParseException(ErrorStatusCode::InvalidPropertyValue, "Unbalanced calls to Enter/ExitFallback");
        }

        m_fallbackCount--;
    }

    bool ParseContext::InFallback() const
    {
        return m_fallbackCount > 0;
    }
}
