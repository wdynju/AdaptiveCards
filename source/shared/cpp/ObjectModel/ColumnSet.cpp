#include "pch.h"
#include "ColumnSet.h"
#include "Column.h"
#include "ParseUtil.h"
#include "Image.h"
#include "TextBlock.h"

using namespace AdaptiveSharedNamespace;

ColumnSet::ColumnSet() : BaseCardElement(CardElementType::ColumnSet)
{
    PopulateKnownPropertiesSet();
}

const std::vector<std::shared_ptr<Column>>& ColumnSet::GetColumns() const
{
    return m_columns;
}

std::vector<std::shared_ptr<Column>>& ColumnSet::GetColumns()
{
    return m_columns;
}

std::shared_ptr<BaseActionElement> ColumnSet::GetSelectAction() const
{
    return m_selectAction;
}

void ColumnSet::SetSelectAction(const std::shared_ptr<BaseActionElement> action)
{
    m_selectAction = action;
}

void ColumnSet::SetLanguage(const std::string& language)
{
    for (auto& column : m_columns)
    {
        column->SetLanguage(language);
    }
}

Json::Value ColumnSet::SerializeToJsonValue() const
{
    Json::Value root = BaseCardElement::SerializeToJsonValue();

    std::string const& propertyName = AdaptiveCardSchemaKeyToString(AdaptiveCardSchemaKey::Columns);
    root[propertyName] = Json::Value(Json::arrayValue);
    for (const auto& column : m_columns)
    {
        root[propertyName].append(column->SerializeToJsonValue());
    }

    if (m_selectAction != nullptr)
    {
        root[AdaptiveCardSchemaKeyToString(AdaptiveCardSchemaKey::SelectAction)] =
            BaseCardElement::SerializeSelectAction(m_selectAction);
    }

    return root;
}

std::shared_ptr<BaseCardElement> ColumnSetParser::Deserialize(ParseContext& context, const Json::Value& value)
{
    ParseUtil::ExpectTypeString(value, CardElementType::ColumnSet);

    auto container = BaseCardElement::Deserialize<ColumnSet>(context, value);

    // Parse Columns
    context.PushElement({ container->GetId(), container->GetInternalId(), false});
    auto cardElements = ParseUtil::GetElementCollectionOfSingleType<Column>(context, value, AdaptiveCardSchemaKey::Columns, Column::Deserialize, false);
    context.PopElement();
    container->m_columns = std::move(cardElements);

    // Parse optional selectAction
    container->SetSelectAction(ParseUtil::GetAction(context, value, AdaptiveCardSchemaKey::SelectAction, false));

    return container;
}

std::shared_ptr<BaseCardElement> ColumnSetParser::DeserializeFromString(ParseContext& context, const std::string& jsonString)
{
    return ColumnSetParser::Deserialize(context, ParseUtil::GetJsonValueFromString(jsonString));
}

void ColumnSet::PopulateKnownPropertiesSet()
{
    m_knownProperties.insert({AdaptiveCardSchemaKeyToString(AdaptiveCardSchemaKey::Columns),
                              AdaptiveCardSchemaKeyToString(AdaptiveCardSchemaKey::SelectAction)});
}

void ColumnSet::GetResourceInformation(std::vector<RemoteResourceInformation>& resourceInfo)
{
    auto columns = GetColumns();
    for (auto column : columns)
    {
        column->GetResourceInformation(resourceInfo);
    }
    return;
}
