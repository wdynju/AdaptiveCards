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
        const bool isFallback {std::get<2>(idsToPop)};

        // if (elementInternalId == ParseContext::NonFallbackId)
        // {
        //     //throw AdaptiveCardParseException(ErrorStatusCode::InvalidPropertyValue, "Unexpected state when popping element");
        // }

        // check element ids list to see if item is already in list.
        if (!elementId.empty())
        {
            const unsigned int nearestFallbackId = GetNearestFallbackId();
            bool foundEntry = false;
            bool haveSeenFallbackEntry = false;
            for (auto currentEntry = m_elementIds.find(elementId); currentEntry != m_elementIds.end(); ++currentEntry)
            {
                foundEntry = true;

                const unsigned int entryInternalId = currentEntry->second.first;
                const unsigned int entryFallbackId = currentEntry->second.second;

                // three possibilities:

                // 1. We're seeing this element for the second time. This can happen if we're parsing an element
                // that has children AND fallback. If this is the case, there's no collision.
                if (entryInternalId == elementInternalId)
                {
                    break;
                }

                if (entryFallbackId == elementInternalId)
                {
                    break;
                }

                try
                {
                    const auto& previousInStack = m_idStack.at(m_idStack.size()-2);
                    if (std::get<1>(previousInStack) == entryFallbackId)
                    {
                        // we're looking at a fallback entry for our parent
                        break;
                    }
                }
                catch (const std::out_of_range&)
                {
                    throw AdaptiveCardParseException(ErrorStatusCode::IdCollision, "something important");
                }

                // 2. This element is fallback content, and is allowed to share an ID with its fallback parent. To check
                // if this is the case, first see if the nearest fallback element has the same ID. If the nearest
                // fallback element's ID is the same, there's no collision iff m_elementIds doesn't have an existing
                // entry for this fallback ID.
                if (isFallback && nearestFallbackId != ParseContext::NonFallbackId && nearestFallbackId == entryFallbackId)
                {
                    if (haveSeenFallbackEntry)
                    {
                        throw AdaptiveCardParseException(ErrorStatusCode::IdCollision, "something important");
                    }
                    else
                    {
                        haveSeenFallbackEntry = true;
                    }
                    continue;
                }

                // 3. This element is actually colliding with an existing id.
                throw AdaptiveCardParseException(ErrorStatusCode::IdCollision, "something important");
            }

            if (!foundEntry)
            {
                m_elementIds.emplace(std::make_pair(elementId, std::make_pair(elementInternalId, nearestFallbackId)));
            }
        }

        m_idStack.pop_back();
    }
}
