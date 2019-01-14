#pragma once

#include "pch.h"
#include "BaseElement.h"
#include "ElementParserRegistration.h"
#include "ActionParserRegistration.h"
#include "AdaptiveCardParseWarning.h"

namespace AdaptiveSharedNamespace
{
    class ParseContext
    {
    public:
        ParseContext();
        ParseContext(std::shared_ptr<ElementParserRegistration> elementRegistration,
                     std::shared_ptr<ActionParserRegistration> actionRegistration);

        std::shared_ptr<ElementParserRegistration> elementParserRegistration;
        std::shared_ptr<ActionParserRegistration> actionParserRegistration;
        std::vector<std::shared_ptr<AdaptiveCardParseWarning>> warnings;

        static const unsigned int NonFallbackId = 0U;

        void Reset();
        void PushElement(const std::tuple<std::string, unsigned int, bool>& ids);
        void PopElement();

    private:
        unsigned int GetNearestFallbackId() const;

        // ID, internal ID, isFallback
        std::vector<std::tuple<std::string, unsigned int, bool>> m_idStack;

        // ID -> internal ID, fallback ID
        std::unordered_multimap<std::string, std::pair<unsigned int, unsigned int>> m_elementIds;
    };
}
