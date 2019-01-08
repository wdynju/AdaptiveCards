#include "pch.h"
#include "BaseCardElement.h"
#include "BaseElementFallback.h"
#include "ShowCardAction.h"
#include "OpenUrlAction.h"
#include "ParseUtil.h"
#include "SubmitAction.h"

using namespace AdaptiveSharedNamespace;

BaseCardElement::BaseCardElement(CardElementType type, Spacing spacing, bool separator, HeightType height) :
    m_type(type), m_spacing(spacing), m_typeString(CardElementTypeToString(type)), m_additionalProperties(),
    m_height(height), m_separator(separator), m_isVisible(true)
{
    PopulateKnownPropertiesSet();
}

BaseCardElement::BaseCardElement(CardElementType type) :
    m_type(type), m_spacing(Spacing::Default), m_typeString(CardElementTypeToString(type)), m_separator(false),
    m_height(HeightType::Auto), m_isVisible(true)
{
    PopulateKnownPropertiesSet();
}

void BaseCardElement::PopulateKnownPropertiesSet()
{
    m_knownProperties.insert({AdaptiveCardSchemaKeyToString(AdaptiveCardSchemaKey::Fallback),
                              AdaptiveCardSchemaKeyToString(AdaptiveCardSchemaKey::Height),
                              AdaptiveCardSchemaKeyToString(AdaptiveCardSchemaKey::IsVisible),
                              AdaptiveCardSchemaKeyToString(AdaptiveCardSchemaKey::Separator),
                              AdaptiveCardSchemaKeyToString(AdaptiveCardSchemaKey::Spacing),
                              AdaptiveCardSchemaKeyToString(AdaptiveCardSchemaKey::Requires),
                              AdaptiveCardSchemaKeyToString(AdaptiveCardSchemaKey::Type)});
}

std::string BaseCardElement::GetElementTypeString() const
{
    return m_typeString;
}

void BaseCardElement::SetElementTypeString(const std::string& value)
{
    m_typeString = value;
}

bool BaseCardElement::GetSeparator() const
{
    return m_separator;
}

void BaseCardElement::SetSeparator(const bool value)
{
    m_separator = value;
}

Spacing BaseCardElement::GetSpacing() const
{
    return m_spacing;
}

void BaseCardElement::SetSpacing(const Spacing value)
{
    m_spacing = value;
}

HeightType BaseCardElement::GetHeight() const
{
    return m_height;
}

void BaseCardElement::SetHeight(const HeightType value)
{
    m_height = value;
}

std::string BaseCardElement::GetId() const
{
    return m_id;
}

void BaseCardElement::SetId(const std::string& value)
{
    m_id = value;
}

bool BaseCardElement::GetIsVisible() const
{
    return m_isVisible;
}

void BaseCardElement::SetIsVisible(const bool value)
{
    m_isVisible = value;
}

const CardElementType BaseCardElement::GetElementType() const
{
    return m_type;
}

std::string BaseCardElement::Serialize() const
{
    Json::FastWriter writer;
    return writer.write(SerializeToJsonValue());
}

Json::Value BaseCardElement::SerializeToJsonValue() const
{
    Json::Value root = GetAdditionalProperties();
    root[AdaptiveCardSchemaKeyToString(AdaptiveCardSchemaKey::Type)] = CardElementTypeToString(GetElementType());

    if (m_height != HeightType::Auto)
    {
        root[AdaptiveCardSchemaKeyToString(AdaptiveCardSchemaKey::Height)] = HeightTypeToString(GetHeight());
    }

    if (m_spacing != Spacing::Default)
    {
        root[AdaptiveCardSchemaKeyToString(AdaptiveCardSchemaKey::Spacing)] = SpacingToString(m_spacing);
    }

    if (m_separator)
    {
        root[AdaptiveCardSchemaKeyToString(AdaptiveCardSchemaKey::Separator)] = true;
    }

    if (!m_id.empty())
    {
        root[AdaptiveCardSchemaKeyToString(AdaptiveCardSchemaKey::Id)] = m_id;
    }

    if (!m_isVisible)
    {
        root[AdaptiveCardSchemaKeyToString(AdaptiveCardSchemaKey::IsVisible)] = false;
    }

    return root;
}

Json::Value BaseCardElement::SerializeSelectAction(const std::shared_ptr<BaseActionElement> selectAction)
{
    if (selectAction != nullptr)
    {
        return selectAction->SerializeToJsonValue();
    }
    return Json::Value();
}

Json::Value BaseCardElement::GetAdditionalProperties() const
{
    return m_additionalProperties;
}

void BaseCardElement::SetAdditionalProperties(Json::Value const& value)
{
    m_additionalProperties = value;
}

void BaseCardElement::GetResourceInformation(std::vector<RemoteResourceInformation>&)
{
    return;
}

void BaseCardElement::ParseJsonObject(ParseContext& context, const Json::Value& json, std::shared_ptr<BaseCardElement>& element)
{
    std::string typeString = ParseUtil::GetTypeAsString(json);
    std::shared_ptr<BaseCardElementParser> parser = context.elementParserRegistration->GetParser(typeString);

    if (parser == nullptr)
    {
        parser = context.elementParserRegistration->GetParser("Unknown");
    }

    auto parsedElement = parser->Deserialize(context, json);
    if (parsedElement != nullptr)
    {
        // TODO: reconcile IDs for fallback content
        // AddId(*element, *(context.elementIds));
        element = parsedElement;
        return;
    }

    throw AdaptiveCardParseException(ErrorStatusCode::InvalidPropertyValue, "Unable to parse element of type " + typeString);
}
