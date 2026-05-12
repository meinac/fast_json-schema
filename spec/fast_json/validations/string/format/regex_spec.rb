# frozen_string_literal: true

RSpec.describe "FastJSON::Schema String" do
  describe "format regex" do
    it_behaves_like "string format", "regex", -> {
      using RSpec::Parameterized::TableSyntax

      where(:data, :valid?) do
        # Valid - engine-agnostic patterns
        ""                  | true
        "^abc$"             | true
        "a.b"               | true
        "a+"                | true
        "a*"                | true
        "a?"                | true
        "a|b"               | true
        "[a-z]+"            | true
        "[^abc]"            | true
        "\\d+"              | true
        "(group)"           | true
        "(?:non-capturing)" | true
        "(?<name>named)"    | true
        "\\w+@\\w+\\.\\w+"  | true
        "a{2,5}"            | true
        "\\b\\w+\\b"        | true
        "^.*$"              | true
        "a{,5}"             | true  # Onigmo accepts; strict ECMA-262 rejects

        # Valid (Onigmo-only constructs; strict ECMA-262 rejects)
        "a*+"               | true  # possessive quantifier
        "(?>atomic)"        | true  # atomic group
        "\\Aabc\\z"         | true  # \A and \z anchors

        # Invalid - rejected by both engines
        "("                 | false # unmatched open paren
        ")"                 | false # unmatched close paren
        "[abc"              | false # unterminated character class
        "\\"                | false # lone backslash
        "*"                 | false # quantifier with no operand
        "+"                 | false # quantifier with no operand
        "?"                 | false # quantifier with no operand
        "(?P<bad>foo)"      | false # Python named-group syntax
        "(?<>x)"            | false # empty named group name
        "a{2,1}"            | false # upper < lower in quantifier
      end
    }
  end
end
