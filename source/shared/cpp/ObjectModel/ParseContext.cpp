#include "pch.h"
#include "ParseContext.h"
#include "AdaptiveCardParseException.h"

namespace AdaptiveSharedNamespace
{
    ParseContext::ParseContext() :
        elementParserRegistration{std::make_shared<ElementParserRegistration>()},
        actionParserRegistration{std::make_shared<ActionParserRegistration>()}, warnings{}, m_idStack{}, m_elementIds{}
    {
    }

    ParseContext::ParseContext(std::shared_ptr<ElementParserRegistration> elementRegistration, std::shared_ptr<ActionParserRegistration> actionRegistration) :
        warnings{}, m_idStack{}, m_elementIds{}
    {
        elementParserRegistration = (elementRegistration) ? elementRegistration : std::make_shared<ElementParserRegistration>();
        actionParserRegistration = (actionRegistration) ? actionRegistration : std::make_shared<ActionParserRegistration>();
    }

    void ParseContext::Reset()
    {
        m_idStack.clear();
        m_elementIds.clear();
    }

    unsigned int ParseContext::GetNearestFallbackId() const
    {
        for (auto curElement = m_idStack.crbegin(); curElement != m_idStack.crend(); ++curElement)
        {
            if (std::get<2>(*curElement))
            {
                return std::get<1>(*curElement);
            }
        }
        return ParseContext::NonFallbackId;
    }

    void ParseContext::PushElement(const std::tuple<std::string, unsigned int, bool>& ids) { m_idStack.push_back(ids); }

    void ParseContext::PopElement()
    {
        // about to pop an element off the stack. perform collision list maintenance and detection.
        const auto& idsToPop = m_idStack.back();
        const std::string& elementId {std::get<0>(idsToPop)};
        const unsigned int elementInternalId {std::get<1>(idsToPop)};
        const unsigned int elementFallbackId {std::get<2>(idsToPop)};

        if (elementInternalId == ParseContext::NonFallbackId)
        {
            //throw AdaptiveCardParseException(ErrorStatusCode::InvalidPropertyValue, "Unexpected state when popping element");
        }

        // check element ids list to see if item is already in list.
        if (!elementId.empty())
        {
            bool fFoundEntry = false;
            for (const auto& elementIdsFromList : m_elementIds)
            {
                if (elementIdsFromList.first == elementId)
                {
                    fFoundEntry = true;
                    const auto elementFallbackIdFromList = elementIdsFromList.second;
                    if (elementFallbackIdFromList == ParseContext::NonFallbackId)
                    {
                        //throw AdaptiveCardParseException(ErrorStatusCode::IdCollision, "Id collision detected: " + elementId);
                    }
                }
            }

            if (!fFoundEntry)
            {
                m_elementIds.push_back({ elementId, elementFallbackId });
            }
        }

        // if not, add to list with fallback id.
        // if item is in list with no fallback id, throw exception.
        // if item is in list and has fallback id, throw if list's fallback id is not in parent chain(??)

        m_idStack.pop_back();
    }
}
