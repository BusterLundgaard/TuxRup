import tinycss2


# *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
# GPT4o was used to help construct parsethecss.py
# It was used to generate a dummy parser in a public python API, to test if it was a viable design route
# It was not a viable design route.
# *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
def getTheCss(css, widget_css_name, widget_css_classes):
    rules = tinycss2.parse_stylesheet(css)
    matched_rules = []

    # Possible selectors to match against (name and classes)
    selectors_to_match = [widget_css_name] + [f".{cls}" for cls in widget_css_classes]

    for rule in rules:
        if rule.type != 'qualified-rule':
            continue

        # Get individual selectors from a potentially comma-separated list
        selectors = tinycss2.serialize(rule.prelude).strip().split(',')
        selectors = [sel.strip() for sel in selectors]

        # Check if any selector matches the widget's css-name or css-classes
        if any(sel in selectors_to_match for sel in selectors):
            declarations_raw = tinycss2.parse_declaration_list(rule.content)
            declarations = [
                f"    {decl.lower_name}: {tinycss2.serialize(decl.value).strip()};"
                for decl in declarations_raw
                if decl.type == 'declaration' and not decl.name.startswith('--')
            ]

            if declarations:
                selector_text = ", ".join(selectors)
                rule_block = f"{selector_text} {{\n" + "\n".join(declarations) + "\n}"
                matched_rules.append(rule_block)

    return "\n".join(matched_rules)


def get_css_properties(css, widget_css_name, widget_css_classes):
    rules = tinycss2.parse_stylesheet(css)
    properties = []

    # Selectors to match (name and classes)
    selectors_to_match = [widget_css_name] + [f".{cls}" for cls in widget_css_classes]

    for rule in rules:
        if rule.type != 'qualified-rule':
            continue

        # Parse selectors
        selectors = tinycss2.serialize(rule.prelude).strip().split(',')
        selectors = [sel.strip() for sel in selectors]

        # Check if the rule matches any of the widget selectors
        if any(sel in selectors_to_match for sel in selectors):
            declarations_raw = tinycss2.parse_declaration_list(rule.content)
            for decl in declarations_raw:
                if decl.type == 'declaration' and not decl.name.startswith('--'):
                    prop_str = f"{decl.lower_name}: {tinycss2.serialize(decl.value).strip()};"
                    properties.append(prop_str)

    return "\n".join(properties)


# Example usage clearly demonstrating your case:
if __name__ == "__main__":
    css_input = """
    button {
        background: red;
        color: white;
    }

    .text-button {
        padding: 5px;
        margin: 10px;
    }

    label {
        font-weight: normal;
    }
    """

    css_name = "button"
    css_classes = ["text-button"]

    print(getTheCss(css_input, css_name, css_classes))
